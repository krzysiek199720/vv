#ifndef VV_PALETTE_H
#define VV_PALETTE_H

#include "../main.h"
#include "../archdef.h"
#include "image.h"

#define CHANNELS 4

namespace palette
{
    class Palette
    {
    public:
        Palette(uint32 width, uint32 height);
        ~Palette();
    public:
        Image image;
    protected:
        Size size;
        Memory paletteMemory;
    public:
        void* getImage();
        void setImage(const char *);

        void setSize(Size);
        Size getSize();
    };
}

#endif //VV_PALETTE_H
