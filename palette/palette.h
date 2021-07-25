#ifndef VV_PALETTE_H
#define VV_PALETTE_H

#include "../main.h"
#include "../archdef.h"
#include "image.h"

namespace palette
{
    class Palette
    {
    public:
        Palette(int32 width, int32 height);
        ~Palette();
    public:
        Image image;
    protected:
        Size size;
        Memory paletteMemory;
        bool processed = false;
    public:
        void* getImage();
        void setImage(const char *);

        void setSize(Size);
        Size getSize();
    };
}

#endif //VV_PALETTE_H
