#ifndef VV_IMAGE_H
#define VV_IMAGE_H

#include "../main.h"
#include "../archdef.h"

namespace palette
{
    class Image
    {
    public:
        Image();
        ~Image();
    public:
        uint32 imageChannels = 0;
        Vector2 offset = {0};
    protected:
        Vector2 sizeRaw = {0};
        void* imageRaw = 0;

        float resizeRatio = 1.0;
        Vector2 size = {0};
        Memory image;
    public:
        void setImage(const char *);
        void setImageOffset(Vector2);

        void* getImageRaw(); // raw means loaded image, before transformations
        void* getImageRaw(Vector2*); // it returns both address and sizeRaw
        Vector2 getImageRawSize();

        void* getImage();
        void* getImage(Vector2*); // it returns both address and size
        Vector2 getImageSize();

        bool setImageRatio(float);
        bool changeImageRatio(float);
    };
}

#endif //VV_IMAGE_H
