#ifndef VV_PALETTE_H
#define VV_PALETTE_H

#include <memory>
#include <vector>
#include <list>

#include "../main.h"
#include "../archdef.h"
#include "image.h"

namespace palette
{
    struct PaletteData{
        Vector2 size;
        Vector2 offset;
        std::vector<ImageData> imagesData;
    };

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

        uint32 nextId = 1;
        float resizePreviewRatio = 1.0;
        bool showPreviewRatio = false;
    public:
        PaletteData getPaletteData();
        void setPalette(PaletteData);

        void* getImage();
        void addImage(const char *);
        void addImage(std::string);
        void addImage(ImageData*);
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
        bool setResizePreview(float);
        bool changeResizePreview(float);

        void changeZindex(int32);
    protected:
        static bool zindexSortCmp(const std::shared_ptr<Image>, const std::shared_ptr<Image>);
        bool isPointInImage(std::list<std::shared_ptr<Image>>::iterator, Vector2);
        void selectByVector2(Vector2);
        void calculateWriteRegion(Vector2, Vector2, Vector2, Vector2*, Vector2*, Vector2*, Vector2*);
    };
}

#endif //VV_PALETTE_H
