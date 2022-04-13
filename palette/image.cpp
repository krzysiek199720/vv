#include "image.h"

#include "../libraries/stb_image.h"
#include "../libraries/stb_image_resize.h"

palette::Image::Image(uint32 newId)
    :id(newId), imagePath(std::string())
{
}

palette::Image::~Image() {
    stbi_image_free(this->imageRaw);
    memoryFree(&image);
}

void palette::Image::setImage(palette::ImageData* imageData)
{
    stbi_image_free(this->imageRaw);
    int x,y,n;
    unsigned char *data = stbi_load(imageData->imagePath.c_str(), &x, &y, &n, CHANNELS);
    if(data)
    {
        imageRaw = data;
        sizeRaw = {x, y};
        imageChannels = CHANNELS;

        imagePath = imageData->imagePath;
        setZindex(imageData->zIndex);
        offset = imageData->offset;
        setImageRatio(imageData->resizeRatio);
    }
    else
    {
        DebugPrint("Image get error");

        resizeRatio = 1.0;
        size = {};

        if(image) memoryFree(&image);
        image = {};
        offset = {};
        imagePath = std::string{};
    }
}

void palette::Image::setImage(const char * filename)
{
    auto imgD = ImageData{0, std::string(filename), {0,0}, 0, 1.0};
    setImage(&imgD);
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
    DebugPrint("setImageRatio");
    if(!imageRaw)
        return false;
    if(newRatio == 1.0)
    {
        resetImageRatio();
        return true;
    }

    Vector2 newSize = {(int32)(sizeRaw.x * newRatio), (int32)(sizeRaw.y * newRatio)};

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
    DebugPrint("changeImageRatio");
    float newRatio = resizeRatio + ratioChange;
    if(newRatio <= 0.0)
        return false;
    return setImageRatio(newRatio);
}

bool palette::Image::resetImageRatio() {
    if(!memoryFree(&image))
        return false;

    resizeRatio = 1.0;
    size = {};
    return true;
}

void palette::Image::setZindex(int32 newZindex) {
    zindex = newZindex;
    DebugPrint("SetZindex image");
    DebugPrint(newZindex);
}

int32 palette::Image::getZindex() const {
    return zindex;
}

float palette::Image::getImageRatio() {
    return resizeRatio;
}

palette::ImageData palette::Image::getImageData()
{
    auto result = palette::ImageData{
        id,
        imagePath,
        offset,
        zindex,
        resizeRatio
    };

    return result;
}
