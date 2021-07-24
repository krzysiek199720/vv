#include "palette.h"

#include <stdexcept>

#include "../libraries/stb_image_resize.h"

void * palette::Palette::getImage()
{
    DebugPrint("getImage()");

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
    return paletteMemory.address;
}

void palette::Palette::setImage(const char * filename)
{
    image.setImage(filename);
}

palette::Palette::Palette(uint32 width, uint32 height)
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
    if(newSize.width < 1 && newSize.height < 1)
    {
        DebugPrint("SetPaletteSize wrong newSize");
        throw std::bad_alloc();
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
}
