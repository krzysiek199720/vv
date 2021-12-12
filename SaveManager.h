#ifndef VV_SAVEMANAGER_H
#define VV_SAVEMANAGER_H

#include "palette/palette.h"

namespace save
{
    palette::PaletteData loadSave(std::string filename);
    palette::PaletteData loadSave(const char * filename);

    void createSave(std::string filename);
    void createSave(const char * filename);
}

#endif //VV_SAVEMANAGER_H
