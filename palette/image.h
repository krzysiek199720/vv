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
        uint32 imageChannels;
        Vector2 offset;
    protected:
        Vector2 sizeRaw;
        void* imageRaw;
    public:
        void setImage(const char *);
        void setImageOffset(Vector2);
        void* getImageRaw();
        void* getImageRaw(Vector2*); // it returns both address and sizeRaw
    };
}

#endif //VV_IMAGE_H
