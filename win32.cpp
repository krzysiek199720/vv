#include <windowsx.h>
#include "win32.h"

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

void paintToScreen(HWND window, HDC hdc, void* address, Vector2 size)
{
    HDC memDc = CreateCompatibleDC(hdc);

    void* DibBits = 0;
    HBITMAP newBitmap = CreateDIBSection(memDc, &bitmapInfo,
                                         DIB_RGB_COLORS, &DibBits, 0, 0);

    memcpy(DibBits, address, (size.x * size.y * CHANNELS));

    SelectObject(memDc, newBitmap);

    if(isSetting(LAYERED))
    {
        BLENDFUNCTION blend =  {
        AC_SRC_OVER,
        0,
        isSetting(HALPHA) ? (uint8)0 : globalAlpha,
        AC_SRC_ALPHA
        };
        UpdateLayeredWindow(
                window,
                hdc,
                0,
                0,
                memDc,
                0,
                RGB(0, 0, 0),
                &blend,
                ULW_ALPHA
        );
    }
    else
    {
        BitBlt(
                hdc,
                0, 0, size.x, size.y,
                memDc,
                0, 0,
                SRCCOPY
        );
    }

    DeleteDC(memDc);
}

void resizePaletteAndHdc(Vector2 size)
{
    defPalette->setSize(size);

    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = size.x;
    bitmapInfo.bmiHeader.biHeight = -size.y; //negative is top-down, positive is bottom-up
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    bitmapInfo.bmiHeader.biSizeImage  = size.x*size.y*CHANNELS;
}

void forceUpdate(HWND window)
{
    InvalidateRect(window, 0, FALSE);
}

void resizeWindow(HWND window, Vector2 resolution)
{
    MoveWindow(
            window,
            0, 0,
            resolution.x, resolution.y,
            false
    );
    resizePaletteAndHdc(resolution);
    forceUpdate(window);
}


void processKeys(HWND window, WPARAM wParam, LPARAM lParam)
{
    if(!isSetting(FOCUS))
        return;

    bool isDown = (lParam & (1 << 31)) == 0;
    bool wasDown = (lParam & (1 << 30)) != 0;

    // key valid to process
    switch(wParam)
    {
        case VK_ESCAPE:
        {
            SendMessageA(window, WM_CLOSE, 0, 0);
        } break;

        //TODO DEBUG - remove
        case VK_RETURN:
        {
            if(isDown && !wasDown)
                forceUpdate(window);
        } break;
        case VK_CONTROL:
        {
            if(isDown)
                setSetting(CTRL);
            else
                unsetSetting(CTRL);
        }break;
        case VK_SHIFT:
        {
            if(isDown)
                setSetting(SHIFT);
            else
                unsetSetting(SHIFT);
        }break;
        case VK_TAB:
        {
            if(isDown)
                setSetting(TAB);
            else
                unsetSetting(TAB);
        }break;

        case VK_ADD:
        case VK_OEM_PLUS:
        {
            if(isDown && !wasDown && isSetting(CTRL))
            {
                DebugPrint("Alpha up");
                globalAlpha = Min(0xFF, globalAlpha + 0xF);
                forceUpdate(window);
            }
            if(isDown && !wasDown && isSetting(SHIFT))
            {
                uint8 newIndex = Min(RESOLUTIONCOUNT - 1, resIndex + 1);
                if(newIndex != resIndex)
                {
                    resIndex = newIndex;
                    resizeWindow(window, resolutions[resIndex]);
                }
            }
        }break;
        case VK_SUBTRACT:
        case VK_OEM_MINUS:
        {
            if (isDown && !wasDown && isSetting(CTRL))
            {
                DebugPrint("Alpha down");
                globalAlpha = Max(0x0, globalAlpha - 0xF);
                forceUpdate(window);
            }
            if(isDown && !wasDown && isSetting(SHIFT))
            {
                uint8 newIndex = Max(0, resIndex - 1);
                if(newIndex != resIndex)
                {
                    resIndex = newIndex;
                    resizeWindow(window, resolutions[resIndex]);
                }
            }
        }break;

        case VK_F1:
        {
            if (isDown && !wasDown)
            {
                if(isSetting(LAYERED))
                {
                    DebugPrint("Change treatAsLayerd false");
                    unsetSetting(LAYERED);
                    SetWindowLong(window, GWL_EXSTYLE,
                                  GetWindowLong(window, GWL_EXSTYLE) & (~WS_EX_LAYERED));
                }
                else
                {
                    DebugPrint("Change treatAsLayerd true");
                    setSetting(LAYERED);
                    SetWindowLong(window, GWL_EXSTYLE,
                                  GetWindowLong(window, GWL_EXSTYLE) | WS_EX_LAYERED);
                }
                unsetSetting(HALPHA);
                forceUpdate(window);
            }
        }
    }

}

LRESULT CALLBACK MainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch(message) {
        case WM_ACTIVATEAPP:
        {
            if(wParam) DebugPrint("Activate app");
            else DebugPrint("Deactivate app");
       } break;

        case WM_SETFOCUS:
        {
            setSetting(FOCUS);
            DebugPrint("Has focus");
        }break;
        case WM_KILLFOCUS:
        {
            DebugPrint("Lost focus");
            unsetSetting(FOCUS);
        }break;

        case WM_HOTKEY:
        {
            if(wParam == IDHOT_SNAPDESKTOP || wParam == IDHOT_SNAPWINDOW)
            {
                result = DefWindowProcA(window, message, wParam, lParam);
                break;
            }
            // switch for future hotkeys
            switch(wParam)
            {
                case GETFOCUSHK:
                {
                    DebugPrint("GETFOCUS Hotkey pressed");
                    SetForegroundWindow(window);
                }break;
                case ALPHATOGGLE:
                {
                    DebugPrint("ALPHATOGGLE Hotkey pressed");
                    if(isSetting(LAYERED))
                    {
                        isSetting(HALPHA) ? unsetSetting(HALPHA) : setSetting(HALPHA);
                        forceUpdate(window);
                    }
                }break;
            }

        }break;

        case WM_CLOSE:
        {
            DebugPrint("WM_CLOSE");
            // TODO do some on close saving

            DestroyWindow(window);
        } break;

        case WM_DESTROY:
        {
            DebugPrint("WM_DESTROY");
        } break;

        case WM_KEYUP:
        case WM_KEYDOWN:
        {
            processKeys(window, wParam, lParam);
        } break;

        case WM_PAINT:
        {
            DebugPrint("WM_PAINT");

            PAINTSTRUCT paintStruct = {};
            HDC deviceContext = BeginPaint(window, &paintStruct);
            paintToScreen(window, deviceContext, defPalette->getImage(), defPalette->getSize());
            EndPaint(window, &paintStruct);
        } break;

        case WM_DROPFILES:
        {
//            TODO add ability to drop multiple files
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

            forceUpdate(window);
        } break;

        case WM_NCHITTEST:
        {
            LRESULT hit = DefWindowProc(window, message, wParam, lParam);
            if (isSetting(TAB) && hit == HTCLIENT) hit = HTCAPTION;
            return hit;
        }break;

        case WM_LBUTTONDOWN:
        {
            DebugPrint("Mouse click");
            Vector2 clickPoint = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if(isSetting(CTRL))
            {
                DebugPrint("Setting IMGMOVE");
                setSetting(IMGMOVE);
                moveStartPoint = clickPoint;
                // start move actions
                break;
            }
            DebugPrint("Select image mouse");
            defPalette->selectImage(&clickPoint);
            forceUpdate(window);
        }break;
        case WM_LBUTTONUP:
        {
            DebugPrint("Mouse click up");

            if(isSetting(IMGMOVE))
            {
                DebugPrint("UnSetting IMGMOVE");
                unsetSetting(IMGMOVE);
                // reset move actions
                moveStartPoint = {0};
            }

        }break;

        case WM_MOUSEMOVE:
        {
            if(isSetting(IMGMOVE))
            {
                Vector2 newPos = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                Vector2 pictureShift = {
                    newPos.x - moveStartPoint.x,
                    newPos.y - moveStartPoint.y,
                };
                moveStartPoint = newPos;

                defPalette->movePalette(pictureShift);
                forceUpdate(window);
            }
        }break;

        default: {
            result = DefWindowProcA(window, message, wParam, lParam);
        } break;
    }

    return result;
}

INT WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance,
                   PSTR CommandLine, INT ShowCode)
{
    defPalette = std::make_unique<palette::Palette>(
            resolutions[resIndex].x, resolutions[resIndex].y);
    bitmapInfo = BITMAPINFO{0};

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
                WS_EX_ACCEPTFILES|WS_EX_APPWINDOW|WS_EX_TOPMOST, // TODO WS_EX_NOACTIVATE -- ??
                windowClass.lpszClassName,
                "vv",
                WS_VISIBLE|WS_POPUP, // WS_POPUP, WS_POPUPWINDOW
                0,
                0,
                resolutions[resIndex].x,
                resolutions[resIndex].y,
                0,
                0,
                Instance,
                0 // can use a structure to pass values to WM_CREATE callback
        );
        if(windowHandle)
        {
            DebugPrint("window created");

//            set hotkeys
            RegisterHotKey(windowHandle, GETFOCUSHK, MOD_WIN, VK_F8);
            RegisterHotKey(windowHandle, ALPHATOGGLE, MOD_CONTROL|MOD_WIN, VK_F8);

            resizePaletteAndHdc(resolutions[resIndex]);

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
