#ifndef VV_SAVEMANAGER_H
#define VV_SAVEMANAGER_H

#include "palette/palette.h"

namespace save
{
    palette::PaletteData loadSave(const std::string& filename);
    palette::PaletteData loadSave(const char * filename);

    void createSave(const std::string& filename, palette::PaletteData);
    void createSave(const char * filename, palette::PaletteData);
}

#endif //VV_SAVEMANAGER_H
