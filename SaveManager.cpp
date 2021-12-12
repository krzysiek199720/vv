#include "SaveManager.h"

#include "main.h"
#include "libraries/json.hpp"
#include <windows.h>

using json = nlohmann::json;


void to_json(json& j, const Vector2& p)
{
    j = {{"x", p.x}, {"y", p.y}};
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
}

void save::createSave(const std::string& filename, palette::PaletteData pd){ createSave(filename.c_str(), pd);}
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

    int32 success = WriteFile(
        file,
        s.c_str(),
        s.size(),
        0,
        0
    );

    if(!success)
        MessageBoxA(GetWindow(0,0), "Save failed", "Save failed", MB_OK);

    CloseHandle(file);
}