
#include <Windows.h>
#include "Pendelum.h"

#include <d3d9helper.h>
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\d3dx9.h>
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\lib\\x86\\d3dx9.lib")
#include <cassert>
#include <memory>
#include "../SDK/OsirisSDK/Surface.h"
#include "../Interfaces.h"


/* Fuck Directx9. */
bool Initalized = false;
static IDirect3DDevice9* g_pd3dDevice;
static IDirect3DSurface9* surface;

struct Screen {
    int width, height;
} screen;


std::pair<int,int> Pendelum::ScreenSizes;

/*
RECT WindowRect;

struct CustomVertex
{
    float x, y, z, rhw;
    D3DCOLOR color;
};

static const unsigned long CustomVertexFormat = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
struct TopologyType
{
    D3DPRIMITIVETYPE m_type;
    std::uint32_t m_div;
    std::uint32_t m_sub;
};

static const TopologyType TopologyTypes[] =
{
    {},
    { D3DPRIMITIVETYPE::D3DPT_POINTLIST		, 1, 0 },
    { D3DPRIMITIVETYPE::D3DPT_LINELIST		, 2, 0 },
    { D3DPRIMITIVETYPE::D3DPT_LINESTRIP		, 1, 1 },
    { D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST	, 3, 0 },
    { D3DPRIMITIVETYPE::D3DPT_TRIANGLESTRIP	, 1, 2 },
    { D3DPRIMITIVETYPE::D3DPT_TRIANGLEFAN	, 1, 2 }
};
IDirect3DVertexBuffer9* m_vertexBuffer;
CustomVertex* m_vertex;
std::size_t m_vertexCount;
std::size_t m_maxVertices;

D3DPRIMITIVETYPE m_topology;
D3DCOLOR m_color;

struct Rect {
    int x, y, w, h;
};

void drawFilledRect(const Rect& rect, D3DCOLOR color);
*/
/*
void DrawBox(coords start, coords end) {
    if ((start.y > end.y) || (end.y > Screen.Height) || (start.y < 0))
        return;
    if ((start.x > end.x) || (end.x > Screen.Width) || (start.x < 0))
        return;

    interfaces->surface->setDrawColor(config->debug.box.color[0] * 255, config->debug.box.color[1] * 255, config->debug.box.color[2] * 255, 180);
    interfaces->surface->drawFilledRect(start.x, start.y, end.x, end.y);


}
*/
#include "../Hacks/OTHER/Debug.h"
bool Pendelum::Pendelum_Init(IDirect3DDevice9* device)
{
    // Setup back-end capabilities flags
   // ImGuiIO& io = ImGui::GetIO();
   //io.BackendRendererName = "imgui_impl_dx9";
   // io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
    if (!Initalized) {
        //g_pd3dDevice = device;
       // g_pd3dDevice->AddRef();
        //D3DDEVICE_CREATION_PARAMETERS cparams;
        //device->GetCreationParameters(&cparams);
        //GetWindowRect(cparams.hFocusWindow, &WindowRect);
        //IDirect3DDevice9_CreateOffscreenPlainSurface

        const auto [screenWidth, screenHeight] = interfaces->surface->getScreenSize();

        Debug::QuickPrint(("Width: " + std::to_string(screenWidth) + " Height: " + std::to_string(screenHeight)).c_str());

        screen.width = screenWidth;
        screen.height = screenHeight;
        ScreenSizes.first = screenWidth;
        ScreenSizes.second = screenHeight;
        Initalized = true;
        
    }
   
    return true;
}
Pendelum::AsyncKey Pendelum::Down{ false, VK_DOWN, 0.f,.09f };
Pendelum::AsyncKey Pendelum::Up{ false, VK_UP, 0.f,0.09f };
Pendelum::AsyncKey Pendelum::Enter{ false, VK_RETURN, 0.f,0.2f };

Pendelum::AsyncKey Pendelum::Left{ false, VK_LEFT, 0.f,.2f };
Pendelum::AsyncKey Pendelum::Right{ false, VK_RIGHT, 0.f,0.2f };
Pendelum::AsyncKey Pendelum::Escape{ false, VK_ESCAPE, 0.f,0.2f };


Pendelum::AsyncKey Pendelum::QuickLeft{ false, VK_LEFT, 0.f,.1f };
Pendelum::AsyncKey Pendelum::QuickRight{ false, VK_RIGHT, 0.f,0.1f };


int whichKeyPressed()
{
    while (true)
    {
        for (int i = 1; i < 255; i++) {
            if (GetAsyncKeyState(i) & 0x8000) {
                return i;
            }
        }
    }
}


static void ScrollDown(PendelumTimer timer){
    interfaces->surface->drawFilledRect(0, 0, screen.width, (int)((float)screen.height*(timer.Get_Time()/7)));
}


static void ScrollUp(PendelumTimer timer){
    interfaces->surface->drawFilledRect(0, 0, screen.width, (int)((float)screen.height*(7-(timer.Get_Time() / 7))));
}



/*

        interfaces->surface->setDrawColor(60, 60, 60, 200);
        int padw = screen.width / 10;
        int padh = screen.height / 10;
        interfaces->surface->drawFilledRect(padw, padh, screen.width-padw, screen.height-padh);

*/


/*

    std::vector<Window*> Windows;
    Window CreateNewWindow();
    void Set_Focus(Window window);
    void Set_Focus(int window_id);
    void Draw();

*/



std::vector<Pendelum::Window*> Pendelum::Windows(0);
int Pendelum::Window_Of_Focus = 0;


#include "../Memory.h"
#include "../SDK/OsirisSDK/GlobalVars.h"


bool IsMouseClicked(bool SetFalse = false) {

    static bool m_bCursorClick{ false };
    if (((GetKeyState(VK_LBUTTON) & 0x8000) != 0)) {
        if (m_bCursorClick) {
            return false;
        }
        m_bCursorClick = true;
        if (SetFalse)
            m_bCursorClick = false;

        return true;
    }
    else {
        m_bCursorClick = false;
        return false;
    }

    

    

}

bool IsMouseWithinBounds(int x, int y, int x2, int y2) {
    POINT p;
    if (GetCursorPos(&p))
    {
        if (ScreenToClient(GetActiveWindow(), &p))
        {
            if ((p.x > x) && (p.x < x2)) {
                if ((p.y > y) && (p.y < y2)) {

                    return true;
                }
            }
        }
    }
    return false;
}

void Pendelum::CheckBox::Draw(bool Focus, color* col, bool colorOveride) {
        ScreenVec Vec = Master_Window->getScreenInfo();
        int x, y;
        x = Vec.x + Pos.x;
        y = Vec.y + Pos.y;
        interfaces->surface->setTextPosition(x, y);
        //interfaces->surface->setTextColor(0, 250, 0, 255);
        if (Focus) {
            interfaces->surface->setTextColor(200, 200, 255, 255);

            auto [memberWidth, memberheight] = CalcSize();

            if (Pendelum::GetKey(Enter, memory->globalVars->currenttime) || IsMouseClicked() && IsMouseWithinBounds(x, y, x + (memberWidth * 2), y + memberheight + 2)) {
                *value = !*value;
            }
        }
        std::wstring xstr = *value ? L"[X]" : L"[ ]";
        std::wstring stext = {  xstr + L" " + text  };
        interfaces->surface->printText(stext);  
}




void Pendelum::HotKey::Draw(bool Focus, color* col, bool colorOveride) {
    ScreenVec Vec = Master_Window->getScreenInfo();
    int x, y;
    x = Vec.x + Pos.x;
    y = Vec.y + Pos.y;
    interfaces->surface->setTextPosition(x, y);

    bool Clicked = false;
    if (Focus) {
        auto [memberWidth, memberheight] = CalcSize();
        bool InBounds = IsMouseWithinBounds(x, y, x + (memberWidth * 2), y + memberheight + 2);
        Clicked = IsMouseClicked();     
        interfaces->surface->setTextColor(200, 200, 255, 255);
        if (Pendelum::GetKey(Enter, memory->globalVars->currenttime) || (Clicked && InBounds)) {
            keySet = !keySet;
        }
    }
    else {
        keySet = false;
    }




    if (*value == 0) {
        KeyStr = L" ";
    }


    if (*value != m_nLastValue) {
        UINT FakeMessage = MapVirtualKeyW(*value, MAPVK_VK_TO_VSC);
        LONG fakeLParam = FakeMessage << 16;

        char buffer[1024]; // Microsofts website recommends 1024 buffer length, but why? What kinda fuckin key name...???
        if (GetKeyNameTextA(fakeLParam, buffer, 1024)) {
            KeyStr = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(buffer);
        }
    }


    if (keySet) {

        interfaces->surface->setTextColor(255, 200, 200, 255);
        int KEY = 0;

        if (GetKey(Pendelum::Escape, memory->globalVars->currenttime) && !Clicked) {
            KEY = 0;
            *value = KEY;
            KeyStr = L" ";
            keySet = false;
        }
        else {
            for (int i = 1; i < 254; i++) {
                if (GetAsyncKeyState(i) & 0x8000) {
                    KEY = i;
                    break;
                }
            }

            if (KEY && (KEY != VK_RETURN) && (KEY != VK_LBUTTON)) {
                *value = KEY;
                UINT FakeMessage = MapVirtualKeyW(KEY, MAPVK_VK_TO_VSC);
                LONG fakeLParam = FakeMessage << 16;

                char buffer[1024]; // Microsofts website recommends 1024 buffer length, but why? What kinda fuckin key name...???
                if (GetKeyNameTextA(fakeLParam, buffer, 1024)) {
                    KeyStr = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(buffer);
                }
                keySet = false;
            }
            else{
                if (timeSinceBlink < (memory->globalVars->currenttime)) {
                    timeSinceBlink = memory->globalVars->currenttime + 0.25f;
                    blink = !blink;
                }

                if (blink) {
                    KeyStr = L" ";
                } else {
                    //KeyStr = L"█";
                    KeyStr = L"■";
                }
            }
        }
    }

    




    std::wstring stext = {L"[" + KeyStr + L"]" + L" " + text };
    interfaces->surface->printText(stext);
}


void Pendelum::Combo::Draw(bool Focus, color* col, bool colorOveride) {
    ScreenVec Vec = Master_Window->getScreenInfo();
    int x, y;
    x = Vec.x + Pos.x;
    y = Vec.y + Pos.y;
    interfaces->surface->setTextPosition(x, y);
    //interfaces->surface->setTextColor(0, 250, 0, 255);
    if (Focus) {
        bool Clicked = false;
        auto [memberWidth, memberheight] = CalcSize();
        bool InBounds = IsMouseWithinBounds(x, y, x + (memberWidth * 2), y + memberheight + 2);
        static bool wasRDown{false};
        Clicked = IsMouseClicked();
        interfaces->surface->setTextColor(200, 200, 255, 255);
        if (Pendelum::GetKey(Enter, memory->globalVars->currenttime)) {
            *value = 0;
        } else if (Pendelum::GetKey(QuickLeft, memory->globalVars->currenttime) || (InBounds && Clicked)) {
             *value = (*value)-1;
        } else if (Pendelum::GetKey(QuickRight, memory->globalVars->currenttime) || (!wasRDown && (InBounds && ((GetKeyState(VK_RBUTTON) & 0x8000) != 0)))){
            *value = (*value) + 1;
            wasRDown = true;
        } else if (InBounds && !((GetKeyState(VK_RBUTTON) & 0x8000) != 0)) {
            wasRDown = false;
        }

        


    }

    if (!ComboText) {
        std::wstring stext = { text + L": INVALID PTR, REPORT TO TURION"};
        interfaces->surface->printText(stext);
        return;
    }

    if ((*value) > (ComboText->size() - 1)) {
        (*value) = 0;
    }

    if ((*value) < 0) {
        (*value) = (ComboText->size() - 1);
    }

    (*value) = std::clamp((*value), 0, (int)(ComboText->size()-1));

    std::wstring xstr = ComboText->at(*value);
    std::wstring stext = { text + L": " + xstr };
    interfaces->surface->printText(stext);
}



void Pendelum::CStr_Combo::Draw(bool Focus, color* col, bool colorOveride) {
    ScreenVec Vec = Master_Window->getScreenInfo();
    int x, y;
    x = Vec.x + Pos.x;
    y = Vec.y + Pos.y;
    interfaces->surface->setTextPosition(x, y);
    //interfaces->surface->setTextColor(0, 250, 0, 255);
    if (Focus) {
        bool Clicked = false;
        auto [memberWidth, memberheight] = CalcSize();
        bool InBounds = IsMouseWithinBounds(x, y, x + (memberWidth * 2), y + memberheight + 2);
        static bool wasRDown{ false };
        Clicked = IsMouseClicked();
        interfaces->surface->setTextColor(200, 200, 255, 255);
        if (Pendelum::GetKey(Enter, memory->globalVars->currenttime)) {
            *value = 0;
        }
        else if (Pendelum::GetKey(QuickLeft, memory->globalVars->currenttime) || (InBounds && Clicked)) {
            *value = (*value) - 1;
        }
        else if (Pendelum::GetKey(QuickRight, memory->globalVars->currenttime) || (!wasRDown && (InBounds && ((GetKeyState(VK_RBUTTON) & 0x8000) != 0)))) {
            *value = (*value) + 1;
            wasRDown = true;
        }
        else if (InBounds && !((GetKeyState(VK_RBUTTON) & 0x8000) != 0)) {
            wasRDown = false;
        }




    }


    std::wstring wText(text.begin(), text.end());

    if (!ComboText) {
        std::wstring stext = { wText + L": INVALID PTR, REPORT TO TURION" };
        interfaces->surface->printText(stext);
        return;
    }

    if ((*value) > (ComboText->size() - 1)) {
        (*value) = 0;
    }

    if ((*value) < 0) {
        (*value) = (ComboText->size() - 1);
    }

    (*value) = std::clamp((*value), 0, (int)(ComboText->size() - 1));

    std::wstring xstr(ComboText->at(*value).begin(), ComboText->at(*value).end());
    std::wstring stext = { wText + L": " + xstr };
    interfaces->surface->printText(stext);
}



void Pendelum::MapCombo::Draw(bool Focus, color* col, bool colorOveride) {
    /* This menu design was not designed with... how do i say.... usefullness in mind. It's shit. All of this code is S H I T!*/

    /* Super Ghetto Fix */

    if (strcmp((*value).c_str(), ComboText->at(m_nIndex).c_str())) {
        int nIndex = 0; // Why didnt I just go for a normal for loop? Alcohol
        for (std::string sText : *ComboText)
        {
            if (!strcmp((*value).c_str(), sText.c_str()))
            {
                *value = sText;
                m_nIndex = nIndex;
                break;
            }
            nIndex++;
        }
    }




    ScreenVec Vec = Master_Window->getScreenInfo();
    int x, y;
    x = Vec.x + Pos.x;
    y = Vec.y + Pos.y;
    interfaces->surface->setTextPosition(x, y);
    //interfaces->surface->setTextColor(0, 250, 0, 255);
    if (Focus) {
        bool Clicked = false;
        auto [memberWidth, memberheight] = CalcSize();
        bool InBounds = IsMouseWithinBounds(x, y, x + (memberWidth * 2), y + memberheight + 2);
        static bool wasRDown{ false };
        Clicked = IsMouseClicked();
        interfaces->surface->setTextColor(200, 200, 255, 255);
        if (Pendelum::GetKey(Enter, memory->globalVars->currenttime)) {
            m_nIndex = 0;
        }
        else if (Pendelum::GetKey(QuickLeft, memory->globalVars->currenttime) || (InBounds && Clicked)) {
            m_nIndex = (m_nIndex) - 1;
        }
        else if (Pendelum::GetKey(QuickRight, memory->globalVars->currenttime) || (!wasRDown && (InBounds && ((GetKeyState(VK_RBUTTON) & 0x8000) != 0)))) {
            m_nIndex = (m_nIndex) + 1;
            wasRDown = true;
        }
        else if (InBounds && !((GetKeyState(VK_RBUTTON) & 0x8000) != 0)) {
            wasRDown = false;
        }

    }


    std::wstring wText(text.begin(), text.end());

    if (!ComboText) {
        std::wstring stext = { wText + L": INVALID PTR, REPORT TO TURION" };
        interfaces->surface->printText(stext);
        return;
    }

    if ((m_nIndex) > (ComboText->size() - 1)) {
        (m_nIndex) = 0;
    }

    if ((m_nIndex) < 0) {
        (m_nIndex) = (ComboText->size() - 1);
    }

    (m_nIndex) = std::clamp((m_nIndex), 0, (int)(ComboText->size() - 1));

    *value = ComboText->at(m_nIndex);

    std::wstring xstr(ComboText->at(m_nIndex).begin(), ComboText->at(m_nIndex).end());
    std::wstring stext = { wText + L": " + xstr };
    interfaces->surface->printText(stext);
}









void Pendelum::IntSlider::Draw(bool Focus, color* col, bool colorOveride) {
    ScreenVec Vec = Master_Window->getScreenInfo();
    int x, y;
    x = Vec.x + Pos.x;
    y = Vec.y + Pos.y;
    interfaces->surface->setTextPosition(x, y);
    //interfaces->surface->setTextColor(0, 250, 0, 255);
    if (Focus) {
        interfaces->surface->setTextColor(200, 200, 255, 255);
        if (Pendelum::GetKey(Enter, memory->globalVars->currenttime)) {
            if ((max - *value) < (*value - min))
                *value = min;
            else
                *value = max;
        }

        if (Pendelum::GetKey(QuickRight, memory->globalVars->currenttime)) {
            (*value)+=step;
            *value = std::clamp(*(value), min, max);
        }

        if (Pendelum::GetKey(QuickLeft, memory->globalVars->currenttime)) {
            (*value)-=step;
            *value = std::clamp(*(value), min, max);
        }
    }

   

    std::wstring stext = { text + L" "};

    //interfaces->surface->setDrawColor(0, 250, 0, 255);
    interfaces->surface->printText(stext);

    auto [width, height] = interfaces->surface->getTextSize(5, stext.c_str());

    int y_save = y;
    y += ((float)height / 2.f);
    x += 3 + width;


    if (Focus) {
        auto [memberWidth, memberheight] = CalcSize();
        bool InBounds = IsMouseWithinBounds(x + 30, y, x + (memberWidth * 2), y + memberheight + 2);

        POINT p;
        if ((GetKeyState(VK_LBUTTON) & 0x8000) != 0) {
            if (GetCursorPos(&p))
            {
                if (ScreenToClient(GetActiveWindow(), &p))
                {
                    //*value = (float)((float)max / ((float)(30.f) / ((float)((float)p.x - (float)x + 30.f))));
                    *value = (int)(((float)((float)p.x - (float)x) / (30.f)) * (float)max);
                    *value = std::clamp(*(value), min, max);
                }
            }
        }
    }

    interfaces->surface->drawLine(x, y, x + 30, y);
    int x_save = x + 30.f;
    x += (int)((30.f) * (((*value) / (float)max)));
    interfaces->surface->drawLine(x, (int)((float)y - (int)((float)height / 2.f)), x, (int)((float)y + ((float)height / 2.f)));

    auto[nwidth, nheight] = interfaces->surface->getTextSize(5, L" ");

    interfaces->surface->setTextPosition(x_save + nwidth, y_save);
    std::wstring stext2 = { std::to_wstring(*value) + L" " };
    interfaces->surface->printText(stext2);

    

}

void Pendelum::FloatSlider::Draw(bool Focus, color* col, bool colorOveride) {
    ScreenVec Vec = Master_Window->getScreenInfo();

    int x, y;
    x = Vec.x + Pos.x;
    y = Vec.y + Pos.y;
    interfaces->surface->setTextPosition(x, y);
    //interfaces->surface->setTextColor(0, 250, 0, 255);
    if (Focus) {
        interfaces->surface->setTextColor(200, 200, 255, 255);
        if (Pendelum::GetKey(Enter, memory->globalVars->currenttime)) {

            if ((max - *value) < (*value - min))
                *value = min;
            else
                *value = max;

            //*value = !*value;
        }

        if (Pendelum::GetKey(QuickRight, memory->globalVars->currenttime)) {
            (*value)+= step;
            *value = std::clamp(*(value), min, max);
        }

        if (Pendelum::GetKey(QuickLeft, memory->globalVars->currenttime)) {
            (*value)-= step;
            *value = std::clamp(*(value), min, max);
        }

    }
    std::wstring stext = { text + L" " };

    //interfaces->surface->setDrawColor(0, 250, 0, 255);
    interfaces->surface->printText(stext);

    auto [width, height] = interfaces->surface->getTextSize(5, stext.c_str());

    int y_save = y;
    y += ((float)height / 2.f);
    x += 3 + width;

    if (Focus) {
        auto [memberWidth, memberheight] = CalcSize();
        bool InBounds = IsMouseWithinBounds(x + 30, y, x + memberWidth, y + memberheight + 2);

        POINT p;
        if ((GetKeyState(VK_LBUTTON) & 0x8000) != 0) {
            if (GetCursorPos(&p))
            {
                if (ScreenToClient(GetActiveWindow(), &p))
                {
                    //*value = (float)((float)max / ((float)(30.f) / ((float)((float)p.x - (float)x + 30.f))));
                    *value = ((float)((float)p.x - (float)x) / (30.f)) * (float)max;
                    *value = std::clamp(*(value), min, max);
                }
            }
        }
    }


    interfaces->surface->drawLine(x, y, x + 30, y);
    int x_save = x + 30.f;

    x += (int)((30.f) * (((*value) / (float)max)));
    interfaces->surface->drawLine(x, (int)((float)y - (int)((float)height / 2.f)), x, (int)((float)y + ((float)height / 2.f)));


    auto [nwidth, nheight] = interfaces->surface->getTextSize(5, L" ");

    interfaces->surface->setTextPosition(x_save + nwidth, y_save);
    std::wstring stext2 = { std::to_wstring(*value) + L" " };
    interfaces->surface->printText(stext2);


}


Pendelum::Window* Pendelum::CreateNewWindow() {
    Window* Window = new Pendelum::Window();
    int WindowID = Windows.size();
    Window->WindowID = WindowID;
    Windows.push_back(Window);
    return Window;
}

void Pendelum::Set_Focus(Window window) {
    Window_Of_Focus = window.WindowID;
}

void Pendelum::Set_Focus(Window* window) {
    Window_Of_Focus = window->WindowID;
}

void Pendelum::Set_Focus(int window_id) {
    Window_Of_Focus = window_id;
}


Pendelum::MenuItem* Pendelum::CreateColorSettings(const wchar_t* Text, Config::ColorToggle* col, Window* Master_Window) {
    MenuItem* checkbox = new ColorSettings(const_cast<wchar_t*>(Text), col, Master_Window);
    MenuItem* item = checkbox;
    return item;
}

Pendelum::MenuItem* Pendelum::CreateColorSettings(const wchar_t* Text, std::array<float,4>* col, Window* Master_Window) {
    MenuItem* checkbox = new ColorSettings(const_cast<wchar_t*>(Text), col, Master_Window);
    MenuItem* item = checkbox;
    return item;
}

Pendelum::MenuItem* Pendelum::CreateColorSettings(const wchar_t* Text, Config::Glow* col, Window* Master_Window) {
    MenuItem* checkbox = new ColorSettings(const_cast<wchar_t*>(Text), col, Master_Window);
    MenuItem* item = checkbox;
    return item;
}

Pendelum::MenuItem* Pendelum::CreateHitBoxCombo(const wchar_t* Text, std::array<bool, 19>* hit, Window* Master_Window) {
    MenuItem* checkbox = new HitboxCombo(const_cast<wchar_t*>(Text), Master_Window, hit);
    MenuItem* item = checkbox;
    return item;
}

Pendelum::MenuItem* Pendelum::CreateCombo(const wchar_t* Text, int* value,std::vector<std::wstring>* ComboText ) {
    MenuItem* checkbox = new Combo(const_cast<wchar_t*>(Text), ComboText, value);
    MenuItem* item = checkbox;
    return item;
}

Pendelum::MenuItem* Pendelum::CreateCStrCombo(const char* Text, int* value, std::vector<std::string>* ComboText) {
    MenuItem* checkbox = new CStr_Combo(const_cast<char*>(Text), ComboText, value);
    MenuItem* item = checkbox;
    return item;
}


Pendelum::MenuItem* Pendelum::CreateMapCombo(const char* Text, std::string* value, std::vector<std::string>* ComboText) {
    MenuItem* checkbox = new MapCombo(const_cast<char*>(Text), ComboText, value);
    MenuItem* item = checkbox;
    return item;
}




void Pendelum::DrawWindow(Pendelum::Window* window, bool FrameOnly /*= false*/, bool TextAndFrameOnly /* = false*/, bool bDontDrawMaster /* = false*/) {


    Pendelum::color Color = window->GetColor();
    Pendelum::ScreenVec screenInfo = window->getScreenInfo();
    interfaces->surface->setDrawColor(Color.Color[0], Color.Color[1], Color.Color[2], Color.Color[3]);
    if (screenInfo.isPad) {
        int padw = (int)((float)screen.width * (float)((float)screenInfo.padx / 100.f));
        int padh = (int)((float)screen.height * (float)((float)screenInfo.pady / 100.f));
        window->SetWindowInfo(padw, padh, screen.width - padw, screen.height - padh, screenInfo.padx, screenInfo.pady, screenInfo.isPad);
        screenInfo = window->getScreenInfo();
        interfaces->surface->drawFilledRect(padw, padh, screen.width - padw, screen.height - padh);
    }
    else {
        interfaces->surface->drawFilledRect(screenInfo.x, screenInfo.y, screenInfo.width + screenInfo.x, screenInfo.height + screenInfo.y);
    }

    if (window->isSlave() && !bDontDrawMaster) {
        if (!FrameOnly && !TextAndFrameOnly && !window->MuteBackground()) {
            DrawWindow(window->getMaster(), false, true);
        }
        else {
            DrawWindow(window->getMaster(), true);
        }


        bool Clicked = false;
        Pendelum::ScreenVec Screen = window->getScreenInfo();
        bool InBounds = IsMouseWithinBounds(Screen.x, Screen.y, Screen.x + Screen.height, Screen.y + Screen.height);
        Clicked = IsMouseClicked(true);
        if (Clicked && !InBounds) {
            Pendelum::Set_Focus(window->getMaster()->WindowID);
            window->endDispatcher();
        }


    }

    if (FrameOnly)
        return;

    interfaces->surface->setDrawColor(window->GetTextColorThree());
    interfaces->surface->setTextColor(window->GetTextColorThree());

    interfaces->surface->setTextFont(5);
    int x = 8;
    int y = 2;
    int prev_height = 0;
    int prev_width = 0;
    int wrapped = 0;
    int wraplevel = 0;
    bool lastWrap = false;
    int largestx = 0;
    bool screenWrap = false;

    std::array<std::pair<int,int>, 50> wraps;
    wraps.fill(std::pair<int,int>(0,0));

    for (int j = 0; j < window->Members.size(); j++) {   
        bool kill = false;
        Pendelum::Window::Member* member = &(window->Members.at(j));
        int temp_x = x;
        member->drawState = false;
        if (member->isWrapped) {
            if (!(*(member->Wrapper))) {
                continue;
            }
            if ((member->wrapLevel - wraplevel) > 1) /* Took me 6hours of madness until i figured this very very simple solution out.....*/
                continue;
            if (kill) {
                continue;
            }

            temp_x += (20 * (member->wrapLevel+1));
        }
        else {
            wrapped = 0;         
        }


        member->drawState = true;
        interfaces->surface->setTextColor(window->GetTextColorThree());
        interfaces->surface->setDrawColor(window->GetTextColorThree());
        member->ItemPtr->SetPosition(temp_x, y);
        auto [memberWidth, memberheight] = member->ItemPtr->CalcSize();





        if (IsMouseWithinBounds(screenInfo.x + temp_x, screenInfo.y + y, screenInfo.x + temp_x + (memberWidth * 2), screenInfo.y + y + memberheight)) {
            window->SetFocusedItem(member->ItemPtr->ID);
        }

        //if (wraplevel > wrapped)
        //    screenWrap = false;

        if ((screenInfo.y + y + (memberheight*2) + 5) > screenInfo.height){
            x += (largestx + 12);
            largestx = 0;
            wraps.fill(std::pair<int, int>(0, 0));
            screenWrap = true;
            y = 2;
        }

        if ((wrapped > 0) && (wraplevel == member->wrapLevel)) {
            if (!screenWrap) {
                interfaces->surface->setDrawColor((window->GetTextColorThree()));
                interfaces->surface->drawLine(screenInfo.x + ((temp_x - 8)), screenInfo.y + prev_height, screenInfo.x + ((temp_x - 8)), screenInfo.y + y + (memberheight - 2));
            }

            wrapped++;
        }
        else if ((wrapped > 0) && (wraplevel != member->wrapLevel) && wraps.at(member->wrapLevel).first){
            //temp_x = wraps.at(member.wrapLevel).first;
            int temp_y = wraps.at(member->wrapLevel).second;
            if (!screenWrap) {
                interfaces->surface->setDrawColor((window->GetTextColorThree()));
                interfaces->surface->drawLine(screenInfo.x + ((temp_x - 8)), (screenInfo.y + temp_y) - memberheight, screenInfo.x + ((temp_x - 8)), screenInfo.y + y + (memberheight - 2));
            }
            wrapped++;
            wraps.at(wraplevel) = std::pair<int, int>(0, 0);

        } 
        else if (member->isWrapped) { 
            wrapped++; 
            if (wrapped > 0) {
                if (!screenWrap) {
                    interfaces->surface->setDrawColor((window->GetTextColorThree()));
                    interfaces->surface->drawLine(screenInfo.x + ((temp_x - 8)), screenInfo.y + y, screenInfo.x + ((temp_x - 8)), screenInfo.y + y + (memberheight - 2));
                }
                wraps.at(wraplevel).first = x;
                wraps.at(wraplevel).second = y;
            }
        }

        prev_height = y;

        if(!screenWrap) {
            y += memberheight + 2;
        }
        else {
            screenWrap = false;
        }



        if (!FrameOnly && !TextAndFrameOnly) {
            interfaces->surface->setDrawColor(window->GetTextColorThree());
            member->ItemPtr->Draw(window->GetFocusedItem() == member->ItemPtr->ID);
        }
        else {
            interfaces->surface->setDrawColor(window->GetTextColorThree());
            member->ItemPtr->Draw(false);
        }


        if (member->isWrapped) {
            wraplevel = member->wrapLevel;
        }
        else {
            if (wraplevel > 0)
                wraps.at(wraplevel) = std::pair<int, int>(0, 0);
            wraplevel = 0;
        }

        if ((memberWidth + temp_x + x) > largestx) {
            largestx = memberWidth + temp_x + x;
        }

        //lastWrap = member->isWrapped;
        
    }

    interfaces->surface->setDrawColor(Color.Color[0], Color.Color[1], Color.Color[2], Color.Color[3]);

    if (TextAndFrameOnly)
        return;

    if (Pendelum::GetKey(Pendelum::Down, memory->globalVars->currenttime)) {
        do {
            window->SetFocusedItem(window->GetFocusedItem() + 1);
        } while (!window->Get_Member(window->GetFocusedItem()).drawState);//!(window->Get_Member(window->GetFocusedItem()).isWrapped && (*window->Get_Member(window->GetFocusedItem()).Wrapper) || !window->Get_Member(window->GetFocusedItem()).isWrapped));
    }
    else if (Pendelum::GetKey(Pendelum::Up, memory->globalVars->currenttime)) {
        do {
            window->SetFocusedItem(window->GetFocusedItem() - 1);
        } while (!window->Get_Member(window->GetFocusedItem()).drawState);//!(window->Get_Member(window->GetFocusedItem()).isWrapped && (*window->Get_Member(window->GetFocusedItem()).Wrapper) || !window->Get_Member(window->GetFocusedItem()).isWrapped));
    }

    //IsMouseClicked(IsMouseClicked());
    return;
}

Pendelum::MenuItem* Pendelum::CreateCheckBox(const wchar_t* Text, bool* value) {
    MenuItem* checkbox = new CheckBox(const_cast<wchar_t*>(Text), value);
    MenuItem* item = checkbox;
    return item;
}

Pendelum::MenuItem* Pendelum::CreateHotKey(const wchar_t* Text, int* value) {
    MenuItem* checkbox = new HotKey(const_cast<wchar_t*>(Text), value);
    MenuItem* item = checkbox;
    return item;
}

Pendelum::MenuItem* Pendelum::CreateIntSlider(const wchar_t* Text, int* value, int min, int max, int step) {
    MenuItem* checkbox = new IntSlider(const_cast<wchar_t*>(Text), value, min, max, step);
    MenuItem* item = checkbox;
    return item;
}

Pendelum::MenuItem* Pendelum::CreateFloatSlider(const wchar_t* Text, float* value, float min, float max, float step) {
    MenuItem* checkbox = new FloatSlider(const_cast<wchar_t*>(Text), value, min, max, step);
    MenuItem* item = checkbox;
    return item;
}

int j = 0;
void Pendelum::Draw() {
    int i = 0;
    for (Window* window : Windows) {
        i++;
        j++;
        if ((window->WindowID != Window_Of_Focus)) {
            continue;
        }


        //Debug::QuickPrint(("Drawing Window: " + std::to_string(window->WindowID) + " WOF : " + std::to_string(Window_Of_Focus) + " I : " + std::to_string(i) + " J : " + std::to_string(j)).c_str());
        DrawWindow(window);

        if (GetKey(Escape, memory->globalVars->currenttime) && window->isSlave()) {
           // Debug::QuickPrint("Exiting Window");



            Set_Focus(window->getMaster());
            window->endDispatcher();
        }
        return;
    }

}


void Pendelum::ColorSettings::Draw(bool Focus, color* col, bool colorOveride) {
    ScreenVec Vec = Master_Window->getScreenInfo();
    int x, y;
    x = Vec.x + Pos.x;
    y = Vec.y + Pos.y;

    auto [width, height] = interfaces->surface->getTextSize(5, L"H");
    
    interfaces->surface->setDrawColor(GetColor());
    interfaces->surface->drawFilledRect(x + 5, y, width + x + height, y + height);
    x = width + x + height + 5;

    interfaces->surface->setTextPosition(x, y);
    //interfaces->surface->setTextColor((int)(value->color[0] * 255.f), (int)(value->color[1] * 255.f), (int)(value->color[2] * 255.f), 255);
    if (Focus) {
        interfaces->surface->setTextColor(200, 200, 255, 255);

        auto [memberWidth, memberheight] = CalcSize();
        bool InBounds = IsMouseWithinBounds((x - 20), y, x + (memberWidth * 2), y + memberheight + 2);
        bool Clicked = IsMouseClicked();

        if (Pendelum::GetKey(Enter, memory->globalVars->currenttime) || (InBounds && Clicked)) {
            open = !open;
        }
    }
    std::wstring stext = { text };
    //std::string dstext = { "Window_Of_Focus: " + std::to_string(Window_Of_Focus) + " Color Window ID: " + std::to_string(ColorWindow->WindowID) + " Master Window ID: " + std::to_string(ColorWindow->getMaster()->WindowID) + " Size of Windows: " + std::to_string(Windows.size())  };
    if (open) {
#ifndef _DEBUG
        Set_Focus(*ColorWindow);
#endif
    }

    interfaces->surface->printText(stext);
   // Debug::QuickPrint(dstext.c_str());

}

void Pendelum::HitboxCombo::Draw(bool Focus, color* col, bool colorOveride) {
    PreDraw();
    ScreenVec Vec = Master_Window->getScreenInfo();
    int x, y;
    x = Vec.x + Pos.x;
    y = Vec.y + Pos.y;

    auto [width, height] = interfaces->surface->getTextSize(5, L"H");

    interfaces->surface->setDrawColor(0,250,0);
    interfaces->surface->drawFilledRect(x + 5, y, width + x + height, y + height);
    x = width + x + height + 5;

    interfaces->surface->setTextPosition(x, y);
    //interfaces->surface->setTextColor((int)(value->color[0] * 255.f), (int)(value->color[1] * 255.f), (int)(value->color[2] * 255.f), 255);
    if (Focus) {
        interfaces->surface->setTextColor(200, 200, 255, 255);
        if (Pendelum::GetKey(Enter, memory->globalVars->currenttime)) {
            open = !open;
        }
    }
    std::wstring stext = { text };
    //stext = { L"Window_Of_Focus: " + std::to_wstring(Window_Of_Focus) + L" Color Window ID: " + std::to_wstring(ColorWindow->WindowID) + L" Master Window ID: " + std::to_wstring(ColorWindow->getMaster()->WindowID) + L" Size of Windows: " + std::to_wstring(Windows.size()) };
    if (open) {
        Set_Focus(*ColorWindow);
    }

    interfaces->surface->printText(stext);
    PostDraw();
}

bool Pendelum::DrawMenuAnimationClose(PendelumTimer timer) {
    if (!Initalized)
        return false;

    interfaces->surface->setDrawColor(0, 0, 0, 240);
    if (timer.Get_Time() >= 6.f) {
        return false;
    }
    else {
        /*
        int x1, x2, y1, y2;
        x1 = (int)((float)screen.width / 2) * (timer.Get_Time() / 7);
        y1 = (int)((float)screen.height / 2) * (timer.Get_Time() / 7);

        x2 = (screen.width/2) + (int)(((float)screen.width) * (1 - (timer.Get_Time() / 7)));
        y2 = (screen.height/2) + (int)(((float)screen.height) * (1 - (timer.Get_Time() / 7)));

        interfaces->surface->drawFilledRect(x1,y1,x2,y2);
        */
        interfaces->surface->drawFilledRect(0, 0, screen.width, (int)(((float)screen.height) * (1-(timer.Get_Time() / 6.f))));
        return true;
    }
    return false;
}

bool Pendelum::DrawMenuAnimation(PendelumTimer timer) {
    if (!Initalized)
        return true;
    //using surface = interfaces->surface;
    interfaces->surface->setDrawColor(0, 0, 0, 240);
    if (timer.Get_Time() >= 15.f) {
        interfaces->surface->drawFilledRect(0, 0, screen.width, screen.height);
        return false;

    } 
    else if (timer.Get_Time() < 7.f){
        ScrollDown(timer);
        return true;
    }
    else if (timer.Get_Time() > 8.f){
        interfaces->surface->drawFilledRect(0, 0, screen.width, screen.height);
        interfaces->surface->setTextFont(5);
        interfaces->surface->setTextColor(0, 255, 0, 255);
        const wchar_t* text = L"[Harpoon Version v.003]";
        const auto [textWidth, textHeight] = interfaces->surface->getTextSize(Surface::font, text);
        interfaces->surface->setTextPosition((screen.width / 2) - (textWidth/2), (screen.height / 2) - (textHeight/2));
        interfaces->surface->printText(text);
        return true;
    }
    else {
        interfaces->surface->drawFilledRect(0, 0, screen.width, screen.height);
    }

    return true;
   //Draw_Filled_Rectangle(Start, End, D3DCOLOR_RGBA(0, 0, 0, 130));
    //drawFilledRect(rect, D3DCOLOR_RGBA(0, 0, 0, 130));
    //Draw_Text("Test Of Pendelum Menu", 2.f, 2.f, D3DCOLOR_RGBA(0, 230, 0, 255));
    //Render();
}



void Pendelum::Render() {
    IDirect3DStateBlock9* d3d9_state_block = NULL;
    if (g_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &d3d9_state_block) < 0)
        return;

    if (d3d9_state_block->Capture() != D3D_OK)
        return;

    // Restore the DX9 state
    d3d9_state_block->Apply();
    d3d9_state_block->Release();

}

bool Pendelum::GetKey(AsyncKey& key, float currtime, bool reset) {
    if (reset)
        key.last_press_time = 0.0f;

    if (abs(currtime- key.last_press_time) > (key.time_to_wait)) {
        if (GetAsyncKeyState(key.Key) & 0x8000) {
            key.toggled = !key.toggled;
            key.last_press_time = currtime;
            return true;
        }
    }
    return false;
}

LRESULT Pendelum::Pendelum_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //if (ImGui::GetCurrentContext() == NULL)
    //    return 0;

   // ImGuiIO& io = ImGui::GetIO();
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
        //if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
        //    ::SetCapture(hwnd);
        //io.MouseDown[button] = true;
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
        return 0;
    }
    case WM_MOUSEWHEEL:
        return 0;
    case WM_MOUSEHWHEEL:
        return 0;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (wParam < 256)
        return 0;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (wParam < 256)
        return 0;
    case WM_CHAR:
        if (wParam > 0 && wParam < 0x10000)
        return 0;
    case WM_SETCURSOR:

        return 0;
    case WM_DEVICECHANGE:

        return 0;
    case WM_ACTIVATEAPP:

        return 0;
    }
    return 0;
}






void Pendelum::Draw_Text(const char* text, float x, float y, D3DCOLOR color) {
    ID3DXFont* g_font = NULL;
    LPD3DXFONT pFont = 0;
    
    RECT rct;
    rct.left = x - 1;
    rct.right = x + 1;
    rct.top = y - 1;
    rct.bottom = y + 1;


    auto hr = D3DXCreateFont(g_pd3dDevice, 
        21,  // font height
        0,                //Font width

        FW_NORMAL,        //Font Weight

        1,                //MipLevels

        false,            //Italic

        DEFAULT_CHARSET,  //CharSet

        OUT_DEFAULT_PRECIS, //OutputPrecision

        ANTIALIASED_QUALITY, //Quality

        DEFAULT_PITCH | FF_DONTCARE,//PitchAndFamily

        "smalle",          //pFacename,

        &g_font);         //ppFont

    g_font->DrawTextA(NULL, text, -1, &rct, DT_NOCLIP, color);
}

struct CUSTOMVERTEX
{
    FLOAT x, y, z; //Position
    DWORD color; //Color
};

void Pendelum::Draw_Filled_Rectangle_Clear(ScreenVec Start, ScreenVec End, D3DCOLOR color){
    D3DRECT Rect;
    Rect.x1 = Start.x;
    Rect.x2 = End.x;
    Rect.y1 = Start.y;
    Rect.y2 = End.y;
    g_pd3dDevice->Clear(1, &Rect, D3DCLEAR_TARGET, color, 1, NULL);

}


/*

This buffer is called a vertex buffer. Simply put, it represents an array of vertices. Direct3D allows you to
create vertex buffers and use them through the IDirect3DVertexBuffer9 interface. Vertex buffers are created with the CreateVertexBuffer method of IDirect3DDevice9. Remember,
IDirect3DDevice9 is the Direct3D device that represents your graphics hardware. The syntax and parameters for CreateVertexBuffer follow.


 NOTE. Pointers to IDirect3DVertexBuffer9 can either be written as
IDirect3DVertexBuffer9*, or you can use the typecast
LPDIRECT3DVERTEXBUFFER9.
HRESULT IDirect3DDevice9::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9 **ppVertexBuffer, HANDLE *pSharedHandle);


UINT Length
Size in bytes of the vertex buffer to create. This will be something like sizeof(CUSTOMVERTEX)*3.


DWORD Usage
Just pass 0.


DWORD FVF
The FVF of your vertex structure. This will tell Direct3D how
your vertices are structured. For this value you can pass your
DWORD FVF constant, as defined earlier.


D3DPOOL Pool
The memory in which the vertex buffer should be created. You
can create this in system memory or on your graphics card. For
this example we will use D3DPOOL_SYSTEMMEM. Possible
values can be:
typedef enum _D3DPOOL {
D3DPOOL_DEFAULT = 0,
D3DPOOL_MANAGED = 1,
D3DPOOL_SYSTEMMEM = 2,
D3DPOOL_SCRATCH = 3,
D3DPOOL_FORCE_DWORD = 0x7fffffff
} D3DPOOL;


IDirect3DVertexBuffer9 **ppVertexBuffer
Address where the created vertex buffer is returned.
96 Chapter 4: Direct3D for 3D Graphics

HANDLE *pSharedHandle
This is a reserved value. Just pass NULL.


*/
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

/*
void Pendelum::Draw_Filled_Rectangle(ScreenVec Start, ScreenVec End, D3DCOLOR color) {
    tagRECT Rect;
    Rect.left = Start.x;
    Rect.right = End.x;
    Rect.top = Start.y;
    Rect.bottom = End.y;

    IDirect3DSurface9* surface;
    auto hResult = g_pd3dDevice->CreateOffscreenPlainSurface(
        WindowRect.right,
        WindowRect.bottom,
        D3DFMT_X8R8G8B8,
        D3DPOOL_DEFAULT,
        &surface,       
        NULL);

    g_pd3dDevice->ColorFill(surface, &Rect, color);

}
*/