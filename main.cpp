#include <windows.h>
#include <shellapi.h>
#include <iostream>
#include <memory>

#include "main.h"
#include "palette/palette.h"

// FIXME global
std::unique_ptr<palette::Palette> defPalette;

Memory memoryAlloc(uint32 size)
{
    Memory memory = {0};

    void* resAdd = VirtualAlloc(
            0,
            size,
            MEM_COMMIT,
            PAGE_READWRITE
    );

    if(resAdd)
    {
        memory.address = resAdd;
        memory.size = size;
    }
    else
        DebugPrint("VirtualAlloc failed");

    return memory;
}
bool memoryFree(Memory* memory)
{
    if(memory->address)
        return VirtualFree(memory->address, memory->size, MEM_RELEASE);
    return true;
}

LRESULT CALLBACK MainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch(message) {
        case WM_ACTIVATEAPP: {
            if(wParam) DebugPrint("Activate app");
            else DebugPrint("Deactivate app");
        } break;

        case WM_CLOSE: {
            DebugPrint("WM_CLOSE");
            // TODO do some on close saving

            DestroyWindow(window);
        } break;

        case WM_DESTROY: {
            DebugPrint("WM_DESTROY");
        } break;

        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:{
            DebugPrint("KEYBOARD KEY");
        } break;

        case WM_KEYDOWN: {
            DebugPrint("KEYBOARD KEY DOWN");
            bool isDown = (lParam & (1 << 31)) == 0;
            bool wasDown = (lParam & (1 << 30)) != 0;
            if(!wasDown)
            {
                // key valid to process
                switch(wParam)
                {
                    case VK_ESCAPE: {
                        SendMessageA(window, WM_CLOSE, 0, 0);
                    } break;
                }
            }
        } break;

        case WM_PAINT: {
            //TODO repaint window - eg after resize
            DebugPrint("WM_PAINT");
            PAINTSTRUCT paintStruct = {};
            HDC deviceContext = BeginPaint(window, &paintStruct);
            PatBlt(deviceContext, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BLACKNESS);
            EndPaint(window, &paintStruct);
        } break;

        case WM_SIZE: {
            DebugPrint("WM_SIZE");

        } break;

        case WM_DROPFILES: {
            DebugPrint("WM_DROPFILES");
            char filename[1024];
            UINT nCount = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, 0, 0);
            if(nCount > 1)
            {
                DebugPrint("Too many files");
                break;
            }
            DragQueryFile((HDROP)wParam, 0, filename, sizeof(filename));
            DebugPrint(filename);
            defPalette->setImage(filename);
            //TODO tmp
            defPalette->getImage();
            // tmp
        } break;

        default: {
            result = DefWindowProcA(window, message, wParam, lParam);
        } break;
    }

    return result;
}


INT WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance,
                   PSTR CommandLine, INT ShowCode)
{
    defPalette = std::make_unique<palette::Palette>(WINDOW_WIDTH, WINDOW_HEIGHT);

    WNDCLASSA windowClass = {};
    windowClass.style = CS_HREDRAW|CS_VREDRAW;
    windowClass.lpfnWndProc = MainWindowCallback;
    windowClass.hInstance = Instance;
    //windowClass.hIcon = ; //TODO add an icon
    //windowClass.hCursor = ; // TODO maybe add a different cursor
    windowClass.lpszClassName = "vvMainWindowClass";

    if(RegisterClassA(&windowClass))
    {
        HWND windowHandle = CreateWindowExA(
                WS_EX_ACCEPTFILES|WS_EX_APPWINDOW,//|WS_EX_TOPMOST, // TODO WS_EX_LAYERED|WS_EX_NOACTIVATE -- ??
                windowClass.lpszClassName,
                "vv",
                WS_VISIBLE|WS_POPUP, // TODO -- WS_POPUP, WS_POPUPWINDOW ---- ???
                0,
                0,
                WINDOW_WIDTH,
                WINDOW_HEIGHT,
                0,
                0,
                Instance,
                0 // can use a structure to pass values to WM_CREATE callback
        );
        if(windowHandle)
        {
            DebugPrint("window created");
            //window created successfully
            MSG message;
            while(GetMessage( &message, windowHandle, 0, 0 ) > 0)
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        } else
        {
            DebugPrint("no window created");
        }
    } else
    {
        DebugPrint("no class registered");
    }

    return 0;
}
