#ifndef VV_PALETTE_H
#define VV_PALETTE_H

#include <memory>
#include <list>

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
        std::list<std::shared_ptr<Image>> images;
    protected:
        Vector2 offset;
        Vector2 size;
        Memory paletteMemory;
        bool processed = false;

        int32 selectedImageId = -1;
        std::list<std::shared_ptr<Image>>::iterator selectedImage;

        uint32 nextId = 0;
    public:
        void* getImage();
        void addImage(const char *);
        void deleteImage();

        void setSize(Vector2);
        Vector2 getSize();

        void moveImage(Vector2);
        void movePalette(Vector2);

        void selectImage(Vector2*); // nullptr means deselect
        bool isImageSelected();

        bool setSelectedRatio(float);
        bool changeSelectedRatio(float);
        bool resetSelectedRatio();
        void changeZindex(int32);
    protected:
        static bool zindexSortCmp(const std::shared_ptr<Image>, const std::shared_ptr<Image>);
        bool isPointInImage(std::list<std::shared_ptr<Image>>::iterator, Vector2);
        void selectByVector2(Vector2);
    };
}

#endif //VV_PALETTE_H
