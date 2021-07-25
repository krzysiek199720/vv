#include "image.h"

#include "../libraries/stb_image.h"

palette::Image::Image()
    : sizeRaw(Vector2{0}), imageChannels(0), imageRaw(0),
      offset(Vector2{0})
{
}

palette::Image::~Image() {
    stbi_image_free(this->imageRaw);
}

void palette::Image::setImage(const char * filename) {
    stbi_image_free(this->imageRaw);
    int x,y,n;
    unsigned char *data = stbi_load(filename, &x, &y, &n, 4);
    if(data)
    {
        imageRaw = data;
        sizeRaw.x = x;
        sizeRaw.y = y;
        imageChannels = 4; // Because i always want to work with RGBA
        DebugPrint("Image get successful");
    }
    else
    {
        DebugPrint("Image get error");
    }
}

void palette::Image::setImageOffset(Vector2 newOffset) {
    offset = newOffset;
}

void *palette::Image::getImageRaw() {
    return imageRaw;
}

void *palette::Image::getImageRaw(Vector2* outSize) {
    *outSize = sizeRaw;
    return imageRaw;
}
