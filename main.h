#ifndef VV_MAIN_H
#define VV_MAIN_H

#include <iostream>
#include <windows.h>

#include "archdef.h"

inline void DebugPrint(const char* str)
{
#if DEBUG
    std::cout << str << '\n';
#endif
}
inline void DebugPrint(uint32 number)
{
#if DEBUG
    printf("%d\n", number);
#endif
}
inline void DebugPrint(void* address)
{
#if DEBUG
    printf("%p\n", address);
#endif
}

struct Memory
{
    uint32 size;
    void* address;
};

Memory memoryAlloc(uint32 size);
bool memoryFree(Memory*);

struct Size
{
    uint32 width;
    uint32 height;
};

#define CTRL    0b00000001
#define SHIFT   0b00000010
#define TAB     0b00000100



#endif //VV_MAIN_H
