#include "palette.h"

#include <stdexcept>

#include "../libraries/stb_image_resize.h"

#define RGB2BGR(a_ulColor) (a_ulColor & 0xFF000000) | ((a_ulColor & 0xFF0000) >> 16) | (a_ulColor & 0x00FF00) | ((a_ulColor & 0x0000FF) << 16)

void * palette::Palette::getImage()
{
    DebugPrint("getImage()");

    if(processed)
        return paletteMemory.address;

    memset(paletteMemory.address, 0x0, (size.width*size.height*CHANNELS) );

    if(image.image != 0)
    {
        int res = stbir_resize_uint8(
                (const unsigned char*) image.image, image.size.width, image.size.height, 0,
                (unsigned char  *)paletteMemory.address, size.width, size.height, 0,
                CHANNELS);
        if(res == 0)
        {
            DebugPrint("Resize error");
            return 0;
        }
        DebugPrint("Resize success");

        // FIXME library doesnt support BGR, and windows RGB
        // What to do?  Can i make windows use RGB?
        uint32* pixel;
        for(uint32 i = 0; i < (size.width*size.height); ++i)
        {
            pixel = &((uint32*)paletteMemory.address)[i];
            *pixel = (uint32)RGB2BGR(*pixel);
        }

        processed = true;
    }

    return paletteMemory.address;
}

void palette::Palette::setImage(const char * filename)
{
    image.setImage(filename);
    processed = false;
}

palette::Palette::Palette(int32 width, int32 height)
    :image(Image{}), size(Size{width, height})
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

Size palette::Palette::getSize() {
    return size;
}

void palette::Palette::setSize(Size newSize) {
    if(newSize.width == size.width
       && newSize.height == size.height)
        return;

    if(newSize.width < 1 && newSize.height < 1)
    {
        DebugPrint("SetPaletteSize wrong newSize");
    }
    this->size = newSize;
    memoryFree(&this->paletteMemory);
    Memory newMemory = memoryAlloc(newSize.width * newSize.height * CHANNELS);
    if(newMemory.address == 0)
    {
        DebugPrint("setSizePalette alloc failed");
        throw std::bad_alloc();
    }
    this->paletteMemory = newMemory;
    processed = false;
}
