#include "saveManager.h"

#include "main.h"
#include "libraries/json.hpp"
#include <windows.h>

#include "palette/palette.h"

using json = nlohmann::json;

void save::createSave(const char * filename, palette::PaletteData pd)
{
    // fixme temp
    const char* fn = "D:\\krzych\\Desktop\\sav.vv";

    json root = pd;

    std::string s = to_string(root);

    HANDLE file = CreateFileA(
        fn,
        GENERIC_WRITE,
        0,
        0,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        0
    );

    std::string result = s;

    int32 success = WriteFile(
        file,
        result.c_str(),
        s.size(),
        0,
        0
    );

    if(!success)
        MessageBoxA(GetWindow(0,0), "Save failed", "Save failed", MB_OK);

    DebugPrint(result.c_str());

    CloseHandle(file);
}

bool save::loadSave(const char * filename, palette::PaletteData* palette)
{
    HANDLE file = CreateFileA(
            filename,
            GENERIC_READ,
            FILE_SHARE_READ,
            0,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            0
    );

    if(!file)
    {
        MessageBoxA(GetWindow(0,0), "Save open failed", "Save open failed", MB_OK);
        return false;
    }

    std::string fileValue = std::string();
    fileValue.reserve(1024);

    char buffer[1024];
    DWORD bytesRead = 0;
    bool shouldRead = true;
    while(shouldRead)
    {
        int32 readStatus = ReadFile(
            file,
            buffer,
            1024,
            &bytesRead,
            0
        );

        if(readStatus)
        {
            shouldRead = !(bytesRead < 1024);
            fileValue.insert(fileValue.size(), buffer, bytesRead);
        }
        else
        {
            DebugPrint("Some weird sht");
            MessageBoxA(GetWindow(0,0), "Save read failed", "Save open failed", MB_OK);
            return false;
        }
    }
    CloseHandle(file);

    json j = json::parse(fileValue);

    palette::PaletteData pd = j.get<palette::PaletteData>();
    *palette = pd;

    DebugPrint(fileValue.c_str());

    return true;
}