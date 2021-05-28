#include <Windows.h>
#include "Pendelum2.h"

#include <d3d9helper.h>
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\d3dx9.h>
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\lib\\x86\\d3dx9.lib")
#include <cassert>
#include <memory>




// Thanks IMGUI!!!
LRESULT WndProcHandler(void* hwnd, unsigned int msg, Pend_UINT_PTR wParam, Pend_LONG_PTR lParam) {
    // https://docs.microsoft.com/en-us/windows/win32/winmsg/about-messages-and-message-queues?redirectedfrom=MSDN

    switch (msg)
    {
    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
    case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
    {
        int button = 0;
        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { button = 0; }
        if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { button = 1; }
        if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { button = 2; }
        if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
       // if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
        //    ::SetCapture(hwnd);
       // io.MouseDown[button] = true;
        return 0;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
    {
        int button = 0;
        if (msg == WM_LBUTTONUP) { button = 0; }
        if (msg == WM_RBUTTONUP) { button = 1; }
        if (msg == WM_MBUTTONUP) { button = 2; }
        if (msg == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
        //io.MouseDown[button] = false;
        //if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
         //   ::ReleaseCapture();
        return 0;
    }
    case WM_MOUSEWHEEL:
        //io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
        return 0;
    case WM_MOUSEHWHEEL:
        //io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
        return 0;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (wParam < 256)   
            //io.KeysDown[wParam] = 1;
        return 0;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (wParam < 256)
            //io.KeysDown[wParam] = 0;
        return 0;
    case WM_CHAR:
        // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
        if (wParam > 0 && wParam < 0x10000)
           // io.AddInputCharacterUTF16((unsigned short)wParam);
        return 0;
    case WM_SETCURSOR:
        //if (LOWORD(lParam) == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor())
           // return 1;
        return 0;
    case WM_DEVICECHANGE:
        //if ((UINT)wParam == DBT_DEVNODES_CHANGED)
            //g_WantUpdateHasGamepad = true;
        return 0;
    }
    return 0;




}
