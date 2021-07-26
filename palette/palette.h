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
        Vector2 offset;
    protected:
        Vector2 size;
        Memory paletteMemory;
        bool processed = false;
        Image* selectedImage = 0;
    public:
        void* getImage();
        void setImage(const char *);

        void setSize(Vector2);
        Vector2 getSize();

        void moveImage(Vector2);
        void movePalette(Vector2);

        void selectImage(Vector2*); // nullptr means deselect

        bool setSelectedRatio(float);
        bool changeSelectedRatio(float);
        bool resetSelectedRatio();
    };
}

#endif //VV_PALETTE_H
