#include "image.h"

#include "../libraries/stb_image.h"

palette::Image::Image()
    :size(Size{0}), imageChannels(0), image(0)
{
}

palette::Image::~Image() {
    stbi_image_free(this->image);
}

void palette::Image::setImage(const char * filename) {
    int x,y,n;
    unsigned char *data = stbi_load(filename, &x, &y, &n, 4);
    if(data)
    {
        image = data;
        size.width = x;
        size.height = y;
        imageChannels = 4; // Because i always want to work with RGBA
        DebugPrint("Image get successful");
    }
    else
    {
        DebugPrint("Image get error");
    }
}

