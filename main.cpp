#include <memory>

#include "main.h"
#include "palette/palette.h"

// FIXME global
std::unique_ptr<palette::Palette> defPalette;
BITMAPINFO bitmapInfo;

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

void paintToScreen(HDC hdc, void* address, Size size)
{
    int stretchResult = StretchDIBits(
            hdc,
            0, 0, size.width, size.height,
            0, 0, size.width, size.height,
            address,
            &bitmapInfo,
            DIB_RGB_COLORS,
            SRCCOPY
    );
    if(stretchResult == 0)
    {
        DebugPrint("paintToScreen: No lines scanned to screen");
        return;
    }
}

void resizePaletteAndHdc(Size size)
{
    defPalette->setSize(size);

    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = size.width;
    bitmapInfo.bmiHeader.biHeight = -size.height; //negative is top-down, positive is bottom-up
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

}

LRESULT CALLBACK MainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch(message) {
        case WM_ACTIVATEAPP: {
            if(wParam) DebugPrint("Activate app");
            else DebugPrint("Deactivate app");
//            if(defPalette->image.image != 0)
//                paintToScreen(GetDC(0), defPalette->getImage(), defPalette->getSize());
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
                    case VK_RETURN: {
                        InvalidateRect(window, NULL, FALSE);
                    } break;
                }
            }
        } break;

        case WM_PAINT: {
            DebugPrint("WM_PAINT");

            PAINTSTRUCT paintStruct = {};
            HDC deviceContext = BeginPaint(window, &paintStruct);
//            if(defPalette->image.image != 0)
                paintToScreen(deviceContext, defPalette->getImage(), defPalette->getSize());
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

            InvalidateRect(window, NULL, FALSE);
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
    bitmapInfo = BITMAPINFO{0};

    WNDCLASSA windowClass = {};
    windowClass.style = 0;//CS_HREDRAW|CS_VREDRAW;
    windowClass.lpfnWndProc = MainWindowCallback;
    windowClass.hInstance = Instance;
    //windowClass.hIcon = ; //TODO add an icon
    //windowClass.hCursor = ; // TODO maybe add a different cursor
    windowClass.lpszClassName = "vvMainWindowClass";

    if(RegisterClassA(&windowClass))
    {
        HWND windowHandle = CreateWindowExA(
                WS_EX_ACCEPTFILES|WS_EX_APPWINDOW|WS_EX_TOPMOST, // TODO WS_EX_LAYERED|WS_EX_NOACTIVATE -- ??
                windowClass.lpszClassName,
                "vv",
                WS_VISIBLE|WS_POPUPWINDOW, // TODO -- WS_POPUP, WS_POPUPWINDOW ---- ???
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
            resizePaletteAndHdc({WINDOW_WIDTH, WINDOW_HEIGHT});

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
