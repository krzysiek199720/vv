#include "palette.h"

#include <stdexcept>

#include "../libraries/stb_image_resize.h"

#define RGB2BGR(a_ulColor) ((a_ulColor) & 0xFF000000) | (((a_ulColor) & 0xFF0000) >> 16) | ((a_ulColor) & 0x00FF00) | (((a_ulColor) & 0x0000FF) << 16)

void * palette::Palette::getImage()
{
    DebugPrint("getImage()");

    if(processed)
        return paletteMemory.address;

//    zeroing imageRaw so uncovered areas are redrawn and transparent
    memset(paletteMemory.address, 0x0, (size.x * size.y * CHANNELS) );

        if(image.getImageRaw() == 0)
        return paletteMemory.address;

//    actual imageRaw making
    {
        Vector2 imageSize = {0};
        void* imageAddress = image.getImageRaw(&imageSize);

        Vector2 finalOffset = {offset.x + image.offset.x, offset.y + image.offset.y};

        bool fitsX = (finalOffset.x < size.x && (finalOffset.x + imageSize.x) >= 0);
        bool fitsY = (finalOffset.y < size.y && (finalOffset.y + imageSize.y) >= 0);

        if(fitsX && fitsY)
        {
            Vector2 palette_write = {0};
            Vector2 image_read_start = {0};
            Vector2 image_read_end = {0};

            palette_write.x = (finalOffset.x < 0) ? 0 : finalOffset.x;
            palette_write.y = (finalOffset.y < 0) ? 0 : finalOffset.y;

            image_read_start.x = (finalOffset.x < 0) ?  -(finalOffset.x) : 0;
            image_read_start.y = (finalOffset.y < 0) ?  -(finalOffset.y) : 0;


            image_read_end.x = ((finalOffset.x + imageSize.x) >= size.x) ?
                    imageSize.x - ((finalOffset.x + imageSize.x) - size.x) :
                    imageSize.x;
            image_read_end.y = ((finalOffset.y + imageSize.y) >= size.y) ?
                    imageSize.y - ((finalOffset.y + imageSize.y) - size.y) :
                    imageSize.y;

            int32 pixels_count_x = image_read_end.x - image_read_start.x;
            int32 pixels_count_y = image_read_end.y - image_read_start.y;

            uint32* pixelPalette = ((uint32*) paletteMemory.address) +
                    ((palette_write.y * size.x) + palette_write.x);

            uint32* pixelImage = ((uint32*) imageAddress) +
                                 ((image_read_start.y * imageSize.x) + image_read_start.x);

            for(int32 i = 0; i < pixels_count_y; ++i)
            {
                for(int32 j = 0; j < pixels_count_x; ++j)
                {
                    *pixelPalette++ = *pixelImage++;
                }
                pixelPalette += (size.x - pixels_count_x);
                pixelImage += (imageSize.x - pixels_count_x);
            }
        }
        else
            DebugPrint("No image-screen overlap, skipping draw");
    }
//    end:- actual imageRaw making
    // FIXME library doesnt support BGR, and windows RGB
    // What to do?  Can i make windows use RGB?
    uint32* pixel;
    for(uint32 i = 0; i < (size.x * size.y); ++i)
    {
        pixel = &((uint32*)paletteMemory.address)[i];
        *pixel = (uint32)RGB2BGR(*pixel);
    }

    processed = true;
    return paletteMemory.address;
}

void palette::Palette::setImage(const char * filename)
{
    image.setImage(filename);
    processed = false;
}

palette::Palette::Palette(int32 width, int32 height)
    :image(Image{}), size(Vector2{width, height})
{
    Memory memory = memoryAlloc(width*height*CHANNELS);
    if(memory.address == 0)
    {
        DebugPrint("Constructor palette fail");
        throw std::bad_alloc();
    }
    paletteMemory = memory;
}

palette::Palette::~Palette()
{
    memoryFree(&paletteMemory);
}

Vector2 palette::Palette::getSize() {
    return size;
}

void palette::Palette::setSize(Vector2 newSize) {
    if(newSize.x == size.x
       && newSize.y == size.y)
        return;

    if(newSize.x < 1 && newSize.y < 1)
    {
        DebugPrint("SetPaletteSize wrong newSize");
    }
    this->size = newSize;
    memoryFree(&this->paletteMemory);
    Memory newMemory = memoryAlloc(newSize.x * newSize.y * CHANNELS);
    if(newMemory.address == 0)
    {
        DebugPrint("setSizePalette alloc failed");
        throw std::bad_alloc();
    }
    this->paletteMemory = newMemory;
    processed = false;
}

void palette::Palette::moveImage(Vector2 change) {
    Vector2 newImageOffset = {image.offset.x + change.x, image.offset.y + change.y};
    image.setImageOffset(newImageOffset);
    processed = false;
}

void palette::Palette::movePalette(Vector2 change) {
    offset.x += change.x;
    offset.y += change.y;
    processed = false;
}




//int res = stbir_resize_uint8(
//        (const unsigned char*) image.imageRaw, image.sizeRaw.x, image.sizeRaw.y, 0,
//        (unsigned char  *)paletteMemory.address, size.x, size.y, 0,
//        CHANNELS);
//if(res == 0)
//{
//DebugPrint("Resize error");
//return 0;
//}
//DebugPrint("Resize success");