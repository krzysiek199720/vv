#ifndef VV_WIN32_H
#define VV_WIN32_H

#include <memory>

#include "main.h"
#include "palette/palette.h"
#include <windows.h>

std::unique_ptr<palette::Palette> defPalette;
BITMAPINFO bitmapInfo;

const std::string saveFileEnding = std::string{".vv"};

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

#define CTRL    0b000000001
#define SHIFT   0b000000010
#define ALT     0b000000100
#define TAB     0b000001000
#define IMGMOVE 0b000010000
#define LAYERED 0b000100000
#define FOCUS   0b001000000
#define HALPHA  0b010000000 // has alpha
#define RESIMG  0b100000000 // image resize

Vector2 moveStartPoint = {0}; // used for both palette move and img resize
Vector2 movePrePoint = {0};

// hotkeys
#define GETFOCUSHK 1
#define ALPHATOGGLE 2

// resize numbers - in percentage per pixel moved with mouse
#define SMALLSTEP 0.0005
#define LARGESTEP 0.0010

#endif //VV_WIN32_H
