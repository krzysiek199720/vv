#ifndef VV_IMAGE_H
#define VV_IMAGE_H

#include "../main.h"
#include "../archdef.h"
#include "../libraries/json.hpp"

namespace palette
{
    struct ImageData
    {
        uint32 id;
        std::string imagePath;
        Vector2 offset;
        int32 zIndex;
        float resizeRatio;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(ImageData, id, imagePath, offset, zIndex, resizeRatio)
    };

    class Image
    {
    public:
        Image(uint32);
        ~Image();
    public:
        uint32 imageChannels = 0;
        Vector2 offset = {0};
        const uint32 id;
    protected:
        int32 zindex = 0;
        Vector2 sizeRaw = {0};
        void* imageRaw = 0;

        float resizeRatio = 1.0;
        Vector2 size = {0};
        Memory image = {0};

        std::string imagePath;
    public:
        ImageData getImageData();

        void setImage(const char *);
        void setImage(palette::ImageData* imageData);
        void setImageOffset(Vector2);

        void* getImageRaw(); // raw means loaded image, before transformations
        void* getImageRaw(Vector2*); // it returns both address and sizeRaw
        Vector2 getImageRawSize();

        void* getImage();
        void* getImage(Vector2*); // it returns both address and size
        Vector2 getImageSize();

        bool setImageRatio(float);
        bool changeImageRatio(float);
        bool resetImageRatio();
        float getImageRatio();

        void setZindex(int32);
        int32 getZindex() const;
    };
}

#endif //VV_IMAGE_H
