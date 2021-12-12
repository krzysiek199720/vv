#ifndef VV_MAIN_H
#define VV_MAIN_H

#include <iostream>
#include <windows.h>

#include "archdef.h"

template<typename T>
T Min(T a, T b)
{
    return (a<b?a:b);
}
template<typename T>
T Max(T a, T b)
{
    return (a<b?b:a);
}

#define CHANNELS 4 // hwo many bytes per pixel

struct Memory
{
    uint32 size;
    void* address;
};

Memory memoryAlloc(uint32 size);
bool memoryFree(Memory*);

struct Vector2
{
    int32 x;
    int32 y;
    static Vector2 add(Vector2 v1, Vector2 v2){return {v1.x + v2.x, v1.y + v2.y};}
};

inline void DebugPrint(const char* str...)
{
#if DEBUG
    std::cout << str << '\n';std::cout.flush();
#endif
}
inline void DebugPrint(uint32 number...)
{
#if DEBUG
    printf("%d\n", number);
#endif
}
inline void DebugPrint(int32 number...)
{
#if DEBUG
    printf("%d\n", number);
#endif
}
inline void DebugPrint(float number...)
{
#if DEBUG
    printf("%f\n", number);
#endif
}
inline void DebugPrint(void* address...)
{
#if DEBUG
    printf("%p\n", address);
#endif
}
inline void DebugPrint(Vector2 vec...)
{
#if DEBUG
    printf("%d %d\n", vec.x, vec.y);
#endif
}

#endif //VV_MAIN_H
