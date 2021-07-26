#ifndef VV_WIN32_H
#define VV_WIN32_H

#include <memory>

#include "main.h"
#include "palette/palette.h"
#include <windows.h>

std::unique_ptr<palette::Palette> defPalette;
BITMAPINFO bitmapInfo;

uint8 pressedCK = 0; // pressed control keys
#define isPressedCK(key) (pressedCK & (key))

uint8 globalAlpha = 0xFF;

#define RESOLUTIONCOUNT 4
Vector2 resolutions[RESOLUTIONCOUNT] = {
        {848, 480},
        {1280, 720},
        {1600, 900},
        {1920, 1080}
};
uint8 resIndex = 0;

#define CTRL    0b00000001
#define SHIFT   0b00000010
#define TAB     0b00000100

#define IMGMOVE 0b00001000
Vector2 moveStartPoint = {0};

bool treatAsLayered = false;
bool hasFocus = true;
bool noAlpha = false;

#define GETFOCUSHK 1
#define ALPHATOGGLE 2

#endif //VV_WIN32_H
