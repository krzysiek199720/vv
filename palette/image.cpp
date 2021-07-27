#include "image.h"

#include "../libraries/stb_image.h"
#include "../libraries/stb_image_resize.h"

palette::Image::Image()
{
}

palette::Image::~Image() {
    stbi_image_free(this->imageRaw);
    if(image.address)
        memoryFree(&image);
}

void palette::Image::setImage(const char * filename) {
    stbi_image_free(this->imageRaw);
    int x,y,n;
    unsigned char *data = stbi_load(filename, &x, &y, &n, CHANNELS);
    if(data)
    {
        imageRaw = data;
        sizeRaw.x = x;
        sizeRaw.y = y;
        imageChannels = CHANNELS;
    }
    else
    {
        DebugPrint("Image get error");
    }

    resizeRatio = 1.0;
    size = {0};
    if(image.address)
        memoryFree(&image);
    image = {0};
    offset = {0};
}

void palette::Image::setImageOffset(Vector2 newOffset) {
    offset = newOffset;
}

void *palette::Image::getImageRaw() {
    return imageRaw;
}

void *palette::Image::getImageRaw(Vector2* sizeOut) {
    *sizeOut = sizeRaw;
    return imageRaw;
}

Vector2 palette::Image::getImageRawSize() {
    return sizeRaw;
}

void *palette::Image::getImage() {
    if(!image.address)
        return getImageRaw();
    return image.address;
}

void *palette::Image::getImage(Vector2 *sizeOut) {
    if(!image.address)
        return getImageRaw(sizeOut);

    *sizeOut = size;
    return image.address;
}

Vector2 palette::Image::getImageSize() {
    if(!image.address)
        return sizeRaw;
    return size;
}

bool palette::Image::setImageRatio(float newRatio)
{
    if(!imageRaw)
        return false;
    if(newRatio == 1.0)
    {
        resetImageRatio();
        return true;
    }

    Vector2 newSize = {(int32)(sizeRaw.x * newRatio), (int32)(sizeRaw.y * newRatio)};

    printf("%d %d\n", newSize.x, newSize.y);

    if(image.address)
        memoryFree(&image);

    image = memoryAlloc(newSize.x * newSize.y * CHANNELS);
    if(!image.address)
        throw std::bad_alloc();

    int res = stbir_resize_uint8(
            (const unsigned char*) imageRaw, sizeRaw.x, sizeRaw.y, 0,
            (unsigned char  *)image.address, newSize.x, newSize.y, 0,
            CHANNELS);
    if(res == 0)
    {
        DebugPrint("Resize error");
        return false;
    }
    resizeRatio = newRatio;
    size = newSize;
    return true;
}

bool palette::Image::changeImageRatio(float ratioChange) {
    float newRatio = resizeRatio + ratioChange;
    if(newRatio <= 0.0)
        return false;
    return setImageRatio(newRatio);
}

bool palette::Image::resetImageRatio() {
    if(image.address)
        if(!memoryFree(&image))
            return false;

    resizeRatio = 1.0;
    size = {0};
    return true;
}
