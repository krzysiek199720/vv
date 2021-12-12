#include <windowsx.h>
#include <math.h>

#include "win32.h"

Memory memoryAlloc(uint32 size)
{
    Memory memory = {0};

    void* resAdd = VirtualAlloc(
            NULL,
            size,
            MEM_COMMIT|MEM_RESERVE,
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
    bool result;
    if(memory->address)
        result = VirtualFree(memory->address, 0, MEM_RELEASE);
    else
        result = true;
    if(result)
    {
        *memory = {0};
        return true;
    }
    DebugPrint("Failed to release memory");
    throw std::bad_alloc();
}

std::string GetLastErrorAsString()
{
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0) {
        return std::string(); //No error message has been recorded
    }

    LPSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    //Copy the error message into a std::string.
    std::string message(messageBuffer, size);

    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);

    return message;
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
        DebugPrint("paintToScreen layered");
        BLENDFUNCTION blend =  {
        AC_SRC_OVER,
        0,
        isSetting(HALPHA) ? (uint8)0 : globalAlpha,
        AC_SRC_ALPHA
        };
        int32 resUpdate = UpdateLayeredWindow(
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
        if(!resUpdate)
        {
            DebugPrint("Layered print failed.");
            DebugPrint((int32)GetLastError());
            DebugPrint(GetLastErrorAsString().c_str());
        }
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

    DeleteObject(newBitmap);
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
                globalAlpha = Min(0xFF, globalAlpha + 0xF);
                if(!isSetting(LAYERED))
                    SetLayeredWindowAttributes(window, RGB(0,0,0), globalAlpha, LWA_ALPHA);
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
                globalAlpha = Max(0x0, globalAlpha - 0xF);
                if(!isSetting(LAYERED))
                    SetLayeredWindowAttributes(window, RGB(0,0,0), globalAlpha, LWA_ALPHA);
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
                    unsetSetting(LAYERED);
                    // todo change alpha to global ALPHA
                    SetLayeredWindowAttributes(window, RGB(0,0,0), globalAlpha, LWA_ALPHA);
                }
                else
                {
                    setSetting(LAYERED);
                    SetWindowLong(window, GWL_EXSTYLE,
                                  GetWindowLong(window, GWL_EXSTYLE) & (~WS_EX_LAYERED));
                    SetWindowLong(window, GWL_EXSTYLE,
                                  GetWindowLong(window, GWL_EXSTYLE) | WS_EX_LAYERED);
                    defPalette->selectImage(0); // deselect image
                }
                unsetSetting(HALPHA);
                forceUpdate(window);
            }
        }break;

        case VK_BACK:
        {
            if(isDown && !wasDown)
            {
                if(defPalette->resetSelectedRatio())
                    forceUpdate(window);
            }

        }break;

        case VK_MENU:
        {
            if (isDown)
            {
                if(!wasDown)
                {
                    setSetting(ALT);
                }
            }
            else
            {
                unsetSetting(ALT);
            }
        }break;

        case VK_DELETE:
        {
            if(isDown && !wasDown)
            {
                defPalette->deleteImage();
                forceUpdate(window);
            }
        }break;
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
            // TODO do some on close saving

            DestroyWindow(window);
        } break;

        case WM_DESTROY:
        {
        } break;

        case WM_KEYUP:
        case WM_KEYDOWN:
        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
        {
            processKeys(window, wParam, lParam);
        } break;

        case WM_PAINT:
        {
            DebugPrint("The print");
            PAINTSTRUCT paintStruct = {};
            HDC deviceContext = BeginPaint(window, &paintStruct);
            paintToScreen(window, deviceContext, defPalette->getImage(), defPalette->getSize());
            EndPaint(window, &paintStruct);
        } break;

        case WM_DROPFILES:
        {
//            TODO add ability to drop multiple files
            char filename[1024];
            UINT nCount = DragQueryFile((HDROP)wParam, 0xFFFFFFFF, 0, 0);
            if(nCount > 1)
            {
                DebugPrint("Too many files");
                break;
            }
            DragQueryFile((HDROP)wParam, 0, filename, sizeof(filename));
            defPalette->addImage(filename);

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
            Vector2 clickPoint = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if(isSetting(CTRL))
            {
                setSetting(IMGMOVE);
                moveStartPoint = clickPoint;

                break;
            }
            if(isSetting(ALT))
            {
                setSetting(RESIMG);
                moveStartPoint = clickPoint;
                movePrePoint = clickPoint;
                break;
            }
            defPalette->selectImage(&clickPoint);
            forceUpdate(window);
        }break;
        case WM_LBUTTONUP:
        {

            if(isSetting(IMGMOVE))
            {
                unsetSetting(IMGMOVE);
                // reset move actions
                moveStartPoint = {0};
                break;
            }
            if(isSetting(RESIMG))
            {
                // do resizing
                Vector2 newPos = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                Vector2 move = {
                        newPos.x - moveStartPoint.x,
                        newPos.y - moveStartPoint.y,
                };
                moveStartPoint = newPos;
                // FIXME i hate to use sqrt here,
                //  maybe some other method to get a nice move pattern for scaling
                double moveSize = sqrt(move.x * move.x + move.y * move.y);
                int32 moveSizeInt = lround(moveSize);
                if(moveSizeInt <= 0)
                    break;

                float scaledMove = isSetting(SHIFT) ? moveSizeInt * LARGESTEP : moveSizeInt * SMALLSTEP;
                if(move.x < 0 && move.y < 0)
                    scaledMove = -scaledMove;
                bool success = defPalette->changeSelectedRatio(scaledMove);

                if(success)
                    forceUpdate(window);

                unsetSetting(RESIMG);
                break;
            }

        }break;
        case WM_RBUTTONDOWN:
        {
            defPalette->selectImage(0);
            forceUpdate(window);
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

                if(defPalette->isImageSelected())
                {
                    defPalette->moveImage(pictureShift);
                }
                else
                {
                    defPalette->movePalette(pictureShift);
                }
                DebugPrint("img move");
                forceUpdate(window);
                break;
            }
            if(isSetting(RESIMG))
            {
                DebugPrint("no.");
                // not necesary but ill check anyway to prevent a few calculations
                if(defPalette->isImageSelected())
                {
                    DebugPrint("yes.");
                    Vector2 newPos = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                    Vector2 move = {
                            newPos.x - moveStartPoint.x,
                            newPos.y - moveStartPoint.y,
                    };

                    // FIXME i hate to use sqrt here,
                    //  maybe some other method to get a nice move pattern for scaling
                    double moveSize = sqrt(move.x * move.x + move.y * move.y);
                    int32 moveSizeInt = lround(moveSize);
                    if(moveSizeInt <= 0)
                        break;

                    float scaledMove = isSetting(SHIFT) ? moveSizeInt * LARGESTEP : moveSizeInt * SMALLSTEP;
                    if(move.x < 0 && move.y < 0)
                        scaledMove = -scaledMove;
                    defPalette->changeResizePreview(scaledMove);

                    forceUpdate(window);
                }

            }
        }break;
        case WM_MOUSEWHEEL:
        {
//            this is some weird magic with it, im just gonna always act like it
//            was 1 scroll of the wheel
            int32 resultChange = ((int32)wParam < 0) ? -1 : 1;
            DebugPrint("WM_MOUSEWHEEL");
            DebugPrint((int32)wParam);
            DebugPrint(resultChange);
            defPalette->changeZindex(resultChange);
            forceUpdate(window);
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
                WS_EX_ACCEPTFILES|WS_EX_APPWINDOW|WS_EX_TOPMOST|WS_EX_LAYERED, // TODO WS_EX_NOACTIVATE -- ??
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
            SetLayeredWindowAttributes(windowHandle, RGB(0,0,0), globalAlpha, LWA_ALPHA);

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
