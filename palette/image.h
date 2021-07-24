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
        Size size;
        uint32 imageChannels;
        void* image;
    public:
        void setImage(const char *);
    };
}

#endif //VV_IMAGE_H
