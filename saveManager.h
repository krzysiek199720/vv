#ifndef VV_SAVEMANAGER_H
#define VV_SAVEMANAGER_H

#include "palette/palette.h"

namespace save
{
    bool loadSave(const char *, palette::PaletteData*);

    void createSave(const char *, palette::PaletteData);
}

#endif //VV_SAVEMANAGER_H
