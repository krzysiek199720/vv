#include "saveManager.h"

#include "main.h"
#include "libraries/json.hpp"
#include <windows.h>

using json = nlohmann::json;

void to_json(json& j, const Vector2& p)
{
    j = {{"x", p.x}, {"y", p.y}};
}
void from_json(const json& j, Vector2& p) {
    j.at("x").get_to(p.x);
    j.at("y").get_to(p.y);
}

namespace palette{
    void to_json(json& j, const ImageData& p)
    {
        j = {
            {"id", p.id},
            {"imagePath", p.imagePath},
            {"offset", p.offset},
            {"zIndex", p.zIndex},
            {"resizeRatio", p.resizeRatio}
        };
    }
    void from_json(const json& j, ImageData& p) {
        j.at("id").get_to(p.id);
        j.at("imagePath").get_to(p.imagePath);
        j.at("offset").get_to(p.offset);
        j.at("zIndex").get_to(p.zIndex);
        j.at("resizeRatio").get_to(p.resizeRatio);
    }

    void from_json(const json& j, PaletteData& p) {
        j.at("size").get_to(p.size);
        j.at("offset").get_to(p.offset);
        j.at("imagesData").get_to(p.imagesData);
    }
}

void save::createSave(const char * filename, palette::PaletteData pd)
{
    // fixme temp
    const char* fn = "D:\\krzych\\Desktop\\sav.vv";

    json root = {
        {"size", pd.size},
        {"offset", pd.offset},
        {"imagesData", pd.imagesData}
    };

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
    palette::PaletteData res = palette::PaletteData{0};
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

    DebugPrint(fileValue.c_str());

    return true;
}