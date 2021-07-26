#ifndef VV_WIN32_H
#define VV_WIN32_H

#include <memory>

#include "main.h"
#include "palette/palette.h"
#include <windows.h>

std::unique_ptr<palette::Palette> defPalette;
BITMAPINFO bitmapInfo;

uint8 globalAlpha = 0xFF;

#define RESOLUTIONCOUNT 4
Vector2 resolutions[RESOLUTIONCOUNT] = {
        {848, 480},
        {1280, 720},
        {1600, 900},
        {1920, 1080}
};
uint8 resIndex = 0;


uint32 setting = 0; // pressed control keys
#define isSetting(key) (setting & (key))
void setSetting(uint32 key){setting |= (key);}
void unsetSetting(uint32 key){setting &= ~(key);}

#define CTRL    0b00000001
#define SHIFT   0b00000010
#define TAB     0b00000100
#define IMGMOVE 0b00001000
#define LAYERED 0b00010000
#define FOCUS   0b00100000
#define HALPHA  0b01000000 // has alpha

Vector2 moveStartPoint = {0};

// hotkeys
#define GETFOCUSHK 1
#define ALPHATOGGLE 2

#endif //VV_WIN32_H
