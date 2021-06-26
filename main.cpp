#include <windows.h>
#include <iostream>

#include "archdef.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

inline void DebugPrint(const char* str)
{
#if DEBUG
    std::cout << str << '\0';
#endif
}

LRESULT CALLBACK MainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch(message) {
        case WM_ACTIVATEAPP: {
            if(wParam) DebugPrint("Activate app\n");
            else DebugPrint("Deactivate app\n");
        } break;

        case WM_CLOSE: {
            DebugPrint("WM_CLOSE\n");
            // TODO do some on close saving

            DestroyWindow(window);
        } break;

        case WM_DESTROY: {
            DebugPrint("WM_DESTROY\n");
        } break;

        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:{
            DebugPrint("KEYBOARD KEY\n");
        } break;

        case WM_KEYDOWN: {
            DebugPrint("KEYBOARD KEY DOWN\n");
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
            PAINTSTRUCT paintStruct = {};
            HDC deviceContext = BeginPaint(window, &paintStruct);
            PatBlt(deviceContext, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, BLACKNESS);
            EndPaint(window, &paintStruct);
        } break;

        case WM_SIZE: {
            DebugPrint("WM_SIZE\n");
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
                WS_EX_ACCEPTFILES,
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
            DebugPrint("window created\n");
            //window created successfully
            MSG message;
            while(GetMessage( &message, windowHandle, 0, 0 ) > 0)
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        } else
        {
            DebugPrint("no window created\n");
        }
    } else
    {
        DebugPrint("no class registered\n");
    }

    return 0;
}
