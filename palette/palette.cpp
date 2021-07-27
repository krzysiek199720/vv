#include "palette.h"

#include <stdexcept>

#define RGB2BGR(a_ulColor) ((a_ulColor) & 0xFF000000) | (((a_ulColor) & 0xFF0000) >> 16) | ((a_ulColor) & 0x00FF00) | (((a_ulColor) & 0x0000FF) << 16)

void * palette::Palette::getImage()
{
    if(processed)
    {
        return paletteMemory.address;
    }

//    zeroing image so uncovered areas are redrawn and transparent
    memset(paletteMemory.address, 0x0, (size.x * size.y * CHANNELS) );

    if(images.empty())
    {
        processed = true;
        return paletteMemory.address;
    }

    std::list<std::shared_ptr<Image>>::iterator imageIt;
//    actual image making
    for (imageIt = images.begin(); imageIt != images.end(); ++imageIt)
    {
        std::shared_ptr<Image> image = (*imageIt);
        Vector2 imageSize = {0};
        void* imageAddress = (*imageIt)->getImage(&imageSize);

        Vector2 finalOffset = {offset.x + image->offset.x, offset.y + image->offset.y};

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
            uint32* pixelPaletteStart = pixelPalette; // for select border
            uint32* pixelImage = ((uint32*) imageAddress) +
                                 ((image_read_start.y * imageSize.x) + image_read_start.x);

//            FIXME might be able to do a rowwise memcpy
            for(int32 i = 0; i < pixels_count_y; ++i)
            {
                for(int32 j = 0; j < pixels_count_x; ++j)
                {
                    *pixelPalette++ = *pixelImage++;
                }
                pixelPalette += (size.x - pixels_count_x);
                pixelImage += (imageSize.x - pixels_count_x);
            }


            // if this is selected image do a rectangle border
            if(image->id == selectedImageId)
            {
                bool borderTop = (finalOffset.y >= 0);
                bool borderBottom = (image_read_end.y >= imageSize.y);
                bool borderLeft = (finalOffset.x >= 0);
                bool borderRight = (image_read_end.x >= imageSize.x);

                uint32 color = 0xFF0000FF;
                uint32* borderPixel;
                if(borderTop)
                {
                    borderPixel = pixelPaletteStart;
                    for(uint32 i = 0; i < pixels_count_x; ++i)
                    {
                        *borderPixel++ = color;
                    }
                }
                if(borderBottom)
                {
                    borderPixel = pixelPaletteStart + (pixels_count_y * size.x);
                    for(uint32 i = 0; i < pixels_count_x; ++i)
                    {
                        *borderPixel++ = color;
                    }
                }
                if(borderLeft)
                {
                    borderPixel = pixelPaletteStart;
                    for(uint32 i = 0; i < pixels_count_y; ++i)
                    {
                        *borderPixel = color;
                        borderPixel += size.x;
                    }
                }
                if(borderRight)
                {
                    borderPixel = pixelPaletteStart + pixels_count_x;
                    for(uint32 i = 0; i < pixels_count_y; ++i)
                    {
                        *borderPixel = color;
                        borderPixel += size.x;
                    }
                }
            }
        }
        else
            DebugPrint("No image-screen overlap, skipping draw");
    }
//    end:- actual image making

    // FIXME library doesnt support BGR, and windows RGB
    // What to do?  Can i make windows use RGB?
    uint32* pixel = (uint32*)paletteMemory.address;
    for(; pixel < ((uint32*)paletteMemory.address + (size.x * size.y)); pixel++)
    {
        *pixel = (uint32)RGB2BGR(*pixel);
    }

    processed = true;
    return paletteMemory.address;
}

bool palette::Palette::zindexSortCmp(const std::shared_ptr<Image> a, const std::shared_ptr<Image> b)
{
    return  (*a).getZindex() <= (*b).getZindex();
}

void palette::Palette::addImage(const char * filename)
{
    std::shared_ptr<Image> image = std::make_shared<Image>(nextId++);
    image->setImage(filename);

    images.push_front(image);
//    FIXME maybe? change sort for a smart insertion to reduce workload
//    not sure if it is  worthwhile, maybe sort if fast enough
    images.sort(zindexSortCmp);

    processed = false;
}

palette::Palette::Palette(int32 width, int32 height)
    :images(std::list<std::shared_ptr<Image>>{}), size(Vector2{width, height})
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

Vector2 palette::Palette::getSize()
{
    return size;
}

void palette::Palette::setSize(Vector2 newSize)
{
    if(newSize.x == size.x
       && newSize.y == size.y)
        return;

    if(newSize.x < 1 && newSize.y < 1)
    {
        DebugPrint("SetPaletteSize wrong newSize");
    }
    size = newSize;
    memoryFree(&paletteMemory);
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
    if(!isImageSelected())
    {
        return;
    }

    std::shared_ptr<Image> item = *selectedImage;

    Vector2 newImageOffset = {item->offset.x + change.x, item->offset.y + change.y};
    item->setImageOffset(newImageOffset);
    processed = false;
}

void palette::Palette::movePalette(Vector2 change) {
    offset.x += change.x;
    offset.y += change.y;
    processed = false;
}

bool palette::Palette::isPointInImage(std::list<std::shared_ptr<Image>>::iterator it, Vector2 point)
{
    std::shared_ptr<Image> item = *it;
    Vector2 finalOffset = {offset.x + item->offset.x, offset.y + item->offset.y};

    Vector2 imageEnd = Vector2::add(finalOffset, item->getImageSize());

    return (
            (point.x >= finalOffset.x) &&
            (point.x < imageEnd.x) &&
            (point.y >= finalOffset.y) &&
            (point.y < imageEnd.y)
            );
}

void palette::Palette::selectByVector2(Vector2 point)
{
//    FIXME
//    i wanted to be better at searching next, but had some problems
//    i dont know how to fix roght now


    bool foundFirst = false;
    std::list<std::shared_ptr<Image>>::iterator firstMatch;
    for(auto imageIt = images.begin(); imageIt != images.end(); ++imageIt)
    {
        if(isPointInImage( imageIt, point))
        {
            if(selectedImageId == (*imageIt)->id)
            {
                continue;
            }
            if(!foundFirst)
            {
                foundFirst = true;
                firstMatch = imageIt;
                continue;
            }
            selectedImageId = (*imageIt)->id;
            selectedImage = imageIt;

            return;
        }
    }

    if(foundFirst)
    {
        selectedImageId = (*firstMatch)->id;
        selectedImage = firstMatch;
    }
    else
    {
        selectedImageId = -1;
    }


//    bool isSplit = false; // will the search start at begin() or not - false is starting at begin()
//    if(isImageSelected())
//    {
//        isSplit = (selectedImage != images.begin());
//    }
//    {
//        std::list<std::shared_ptr<Image>>::iterator imageIt = selectedImage;
//        ++imageIt;
//        for(; imageIt != images.end(); ++imageIt)
//        {
//            if(isPointInImage( imageIt, point))
//            {
//                selectedImageId = (*imageIt)->id;
//                selectedImage = imageIt;
//                return;
//            }
//        }
//    }
//    if(isSplit)
//    {
//        std::list<std::shared_ptr<Image>>::iterator endIt = selectedImage;
//        ++endIt; // endIt needs to be one over the end
//        for(std::list<std::shared_ptr<Image>>::iterator imageIt = images.begin(); imageIt != endIt; ++imageIt)
//        {
//            if(isPointInImage(imageIt, point))
//            {
//                selectedImageId = (*imageIt)->id;
//                selectedImage = imageIt;
//                return;
//            }
//        }
//    }
}

void palette::Palette::selectImage(Vector2* position)
{
    if(!position)
    {
        if(isImageSelected())
        {
            processed = false;
        }
        selectedImageId  = -1;
        return;
    }

    selectByVector2(*position);
    processed = false;
}

bool palette::Palette::setSelectedRatio(float newRatio) {
    if(isImageSelected())
    {
        bool success = (*selectedImage)->setImageRatio(newRatio);
        if(!success)
            return false;
        processed = false;
        return true;
    }
    return false;
}

bool palette::Palette::changeSelectedRatio(float ratioChange) {
    if(isImageSelected())
    {
        bool success = (*selectedImage)->changeImageRatio(ratioChange);
        if(!success)
            return false;
        processed = false;
        return true;
    }
    return false;
}

bool palette::Palette::resetSelectedRatio() {
    if(!isImageSelected())
        return false;

    (*selectedImage)->resetImageRatio();
    processed = false;
    return true;
}

bool palette::Palette::isImageSelected() {
    return selectedImageId != -1;
}

void palette::Palette::deleteImage() {
//    TODO remember to image (delete *ptr)
}

