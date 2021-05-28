#include <d3d9.h>
#include <vector>
#include <array>
#include <string>

#pragma once
#include "../Interfaces.h"
#include "../SDK/OsirisSDK/Surface.h"

class PendelumTimer { /* Scuffed Way of Doing This */
public:
    void Init_Timer() {
        if (!init) {
            Pen_Time = 0;
            init = true;
        }
    }

    void Inc_Timer() {
        Pen_Time += .1f;
    }

    float Get_Time() {
        return Pen_Time;
    }

    void Reset_Timer() {
        Pen_Time = 0;
    }
private:
    float Pen_Time;
    bool init = false;
};

#include "../ConfigStructs.h"
#include "../Config.h"

namespace Pendelum {
    extern std::pair<int, int> ScreenSizes;
    struct color {
        std::array<int, 4> Color;
    };

    class ScreenVec {
    public:
        int x, y;
        int width, height;
        int padx, pady;
        bool isPad;
    };

    struct AsyncKey {
        bool toggled = false;
        int Key;
        float last_press_time = 0.0f;
        float time_to_wait = 0.0f;
    };

    extern AsyncKey Enter;
    extern AsyncKey Up;
    extern AsyncKey Down;
    extern AsyncKey Left;
    extern AsyncKey Right;
    extern AsyncKey Escape;

    extern AsyncKey QuickLeft;
    extern AsyncKey QuickRight;


    class Window;
    class MenuItem {
    public:
        virtual void Draw(bool focus, color* col = nullptr, bool colorOveride = 0) = 0;
        virtual void SetText(wchar_t* inText) = 0;
        virtual void SetOwnerWindow(Window* window) = 0;
        virtual void SetPosition(int x, int y) = 0;
        virtual std::pair<int,int> CalcSize() = 0;
        // Specific Use Case
        virtual Window* ExposeWindow() = 0; 
        int ID = NULL;
        bool isWrapped;
        //bool drawState = false;
    private:
    };


    MenuItem* CreateCheckBox(const wchar_t* text, bool* value);
    class Window {
    public:
        Window() {
            Text_Color = new Config::ColorToggle;
            Text_Color->color = { 0.f,.9f,0.f};
        }
        struct Member {
            MenuItem* ItemPtr;
            Member* Controller;
            bool isWrapped = false;
            bool* Wrapper = nullptr;
            int wrapLevel = 0;
            bool drawState = true;
        };

        ScreenVec getScreenInfo() {
            return Position;
        }

        void SetWindowInfo(int x, int y, int w, int h, int padx, int pady, bool pad) {
            Position.x = x;
            Position.y = y;
            Position.height = h;
            Position.width = w;
            Position.isPad = pad;
            Position.padx = padx;
            Position.pady = pady;
            /*
            if (pad) {
                int padw = (int)((float)ScreenSizes.first * (float)((float)padx / 100.f));
                int padh = (int)((float)ScreenSizes.second * (float)((float)pady / 100.f));
                Position.width = x + (ScreenSizes.first - padw);
                Position.height = y + (ScreenSizes.second - padh);
            }
            */
        }

        void SetColor(int r, int g,int b,int a) {
            Window_Color.Color[0] = r;
            Window_Color.Color[1] = g;
            Window_Color.Color[2] = b;
            Window_Color.Color[3] = a;
        }

        color GetColor() {
            return Window_Color;
        }

        void Add_Item(MenuItem* member) {
            member->SetOwnerWindow(this);
            member->ID = Members.size();
            Member mem;
            mem.ItemPtr = member;
            Members.push_back(mem);
        }


        void Add_Wrapped_Item(MenuItem* member, bool* wrapControl, int wraplevel = 0) {
            member->SetOwnerWindow(this);
            member->ID = Members.size();
            Member mem;
            mem.Wrapper = wrapControl;
            mem.isWrapped = true;
            mem.ItemPtr = member;
            mem.wrapLevel = (wraplevel);
            Members.push_back(mem);


        }

        std::vector<Member> Get_MemberList() {
            return Members;
        }

        Member Get_Member(int Index){
            return Members.at(Index);
        }

        int GetFocusedItem() {
            return FocusedItem;
        }
        void SetFocusedItem(int item) {
            if (item > (int)Members.size()-1) {
                item = 0;
            }
            if (item < 0) {
                item = (int)Members.size()-1;
            }
            //std::clamp(item, 0, (int)Members.size());
            FocusedItem = item;
        }

        bool isSlave() {
            return Slave;
        }
        void ColorOverride(Config::ColorToggle* color) {
            delete Text_Color;
            Text_Color = color;
        }

        void ColorOverride(Config::Glow* color) {
            bColorOverride = true;
            Gcolor = color;
        }

        void ColorOverride(std::array<float, 4> color) {
            Text_Color->color[0] = color[0];
            Text_Color->color[1] = color[1];
            Text_Color->color[2] = color[2];
        }


        bool isColorOverride() {
            return bColorOverride;
        }
        //std::array<float, 4> GetTextColor() {
         //   return Text_Color->color;
        //}

        std::array<float, 3> GetTextColorThree() {
            if (!bColorOverride) {
                return std::array<float, 3>{ Text_Color->color[0], Text_Color->color[1], Text_Color->color[2] };
            }
            else {
                return std::array<float, 3>{ Gcolor->color[0], Gcolor->color[1], Gcolor->color[2] };
            }
        }          
        void setAsSlave(Window* Master_Window, bool* Dispatcher = nullptr) {
            Dispatch = Dispatcher;
            Slave = true;
            MasterWindow = Master_Window;
        }
        Window* getMaster() {
            return MasterWindow;
        }

        void endDispatcher() {
            *Dispatch = false;
        }
        void setDispatcher(bool in) {
            *Dispatch = in;
        }
        Window* ExposeWindow() {
            return MasterWindow;
        }

        bool MuteBackground() {
            return m_bMuteBackground;
        }

        void SetMuteBackground(bool bSet) {
            m_bMuteBackground = bSet;
        }


        int WindowID = 0;
        std::vector<Member> Members = {};
    private:
        color Window_Color;
        ScreenVec Position;
        bool m_bMuteBackground = false;
        int FocusedItem = 0;
        Window* MasterWindow = nullptr;
        bool Slave = false;
        bool* Dispatch;
        bool bColorOverride = false;
        Config::ColorToggle* Text_Color;
        Config::Glow* Gcolor;
    };
    bool GetKey(AsyncKey& key, float currtime, bool reset = false);
    void DrawWindow(Pendelum::Window* window, bool FrameOnly = false, bool TextAndFrameOnly = false, bool bDontDrawMaster = false);

    class CheckBox : public MenuItem {
    public:
        CheckBox(wchar_t* inText, bool* inValue) {
            text = inText;
            value = inValue;
        }

        void Draw(bool Focus, color* col = nullptr, bool colorOveride = 0);
        void SetText(wchar_t* inText) {
            text = inText;
        }
        void SetOwnerWindow(Window* window) {
            Master_Window = window;
        }
        void SetValue(bool* inValue) {
            value = inValue;
        }
        void SetPosition(int x, int y) {
            Pos.x = x;
            Pos.y = y;
        }
        std::pair<int, int> CalcSize() {
            return interfaces->surface->getTextSize(5, text.c_str());
        }
        Window* ExposeWindow() {
            return Master_Window;
        }
    private:
        std::wstring text;
        ScreenVec Pos;
        Window* Master_Window;
        bool* value;
    };

    class HotKey : public MenuItem {
    public:
        HotKey(wchar_t* inText, int* inValue) {
            text = inText;
            value = inValue;
        }

        void Draw(bool Focus, color* col = nullptr, bool colorOveride = 0);
        void SetText(wchar_t* inText) {
            text = inText;
        }
        void SetOwnerWindow(Window* window) {
            Master_Window = window;
        }
        void SetValue(int* inValue) {
            value = inValue;
        }
        void SetPosition(int x, int y) {
            Pos.x = x;
            Pos.y = y;
        }
        std::pair<int, int> CalcSize() {
            return interfaces->surface->getTextSize(5, text.c_str());
        }
        Window* ExposeWindow() {
            return Master_Window;
        }
    private:
        std::wstring text;
        ScreenVec Pos;
        Window* Master_Window;
        int* value;
        int m_nLastValue = 0;
        bool keySet = false;
        std::wstring KeyStr{ L" " };
        bool blink = false;
        float timeSinceBlink = 0.0f;
    };

    class IntSlider : public MenuItem {
    public:
        IntSlider(wchar_t* inText, int* inValue, int imin, int imax, int istep) {
            text = inText;
            value = inValue;
            min = imin;
            max = imax;
            step = istep;
        }

        void Draw(bool Focus, color* col = nullptr, bool colorOveride = 0);
        void SetText(wchar_t* inText) {
            text = inText;
        }
        void SetOwnerWindow(Window* window) {
            Master_Window = window;
        }
        void SetValue(int* inValue) {
            value = inValue;
        }
        void SetPosition(int x, int y) {
            Pos.x = x;
            Pos.y = y;
        }
        std::pair<int, int> CalcSize() {
            return interfaces->surface->getTextSize(5, text.c_str());
        }
        Window* ExposeWindow() {
            return Master_Window;
        }
    private:
        std::wstring text;
        ScreenVec Pos;
        Window* Master_Window;
        int* value;
        int min;
        int max;
        int step = 1;
    };

    class FloatSlider : public MenuItem {
    public:
        FloatSlider(wchar_t* inText, float* inValue, float imin, float imax, float istep) {
            text = inText;
            value = inValue;
            min = imin;
            max = imax;
            step = istep;
        }

        void Draw(bool Focus, color* col = nullptr, bool colorOveride = 0);
        void SetText(wchar_t* inText) {
            text = inText;
        }
        void SetOwnerWindow(Window* window) {
            Master_Window = window;
        }
        void SetValue(float* inValue) {
            value = inValue;
        }
        void SetPosition(int x, int y) {
            Pos.x = x;
            Pos.y = y;
        }
        std::pair<int, int> CalcSize() {
            return interfaces->surface->getTextSize(5, text.c_str());
        }
        Window* ExposeWindow() {
            return Master_Window;
        }
    private:
        std::wstring text;
        ScreenVec Pos;
        Window* Master_Window;
        float* value;
        float min;
        float max;
        float step = .05;
    };


    class Combo : public MenuItem {
    public:
        Combo(const wchar_t* inText, std::vector<std::wstring>* inComboText, int* inValue) {
            ComboText = inComboText;
            value = inValue;
            text = inText;
        }

        void Draw(bool Focus, color* col = nullptr, bool colorOveride = 0);
        void SetText(wchar_t* inText) {
            text = inText;
        }
        void SetOwnerWindow(Window* window) {
            Master_Window = window;
        }
        void SetValue(int inValue) {
            *value = inValue;
        }
        void SetPosition(int x, int y) {
            Pos.x = x;
            Pos.y = y;
        }
        std::pair<int, int> CalcSize() {
            return interfaces->surface->getTextSize(5, text.c_str());
        }
        Window* ExposeWindow() {
            return Master_Window;
        }
    private:
        std::wstring text;
        std::vector<std::wstring>* ComboText;
        ScreenVec Pos;
        Window* Master_Window;
        int* value;
    };

    class CStr_Combo : public MenuItem {
    public:
        CStr_Combo(const char* inText, std::vector<std::string>* inComboText, int* inValue) {
            ComboText = inComboText;
            value = inValue;
            text = inText;
        }

        void Draw(bool Focus, color* col = nullptr, bool colorOveride = 0);
        void SetText(wchar_t* inText) {
            std::wstring wText = inText;
            std::string txt(wText.begin(), wText.end());
            text = txt;
        }
        void SetOwnerWindow(Window* window) {
            Master_Window = window;
        }
        void SetValue(int inValue) {
            *value = inValue;
        }
        void SetPosition(int x, int y) {
            Pos.x = x;
            Pos.y = y;
        }
        std::pair<int, int> CalcSize() {

            std::wstring str(text.begin(), text.end());


            return interfaces->surface->getTextSize(5, str.c_str());
        }
        Window* ExposeWindow() {
            return Master_Window;
        }
    private:
        std::string text;
        std::vector<std::string>* ComboText;
        ScreenVec Pos;
        Window* Master_Window;
        int* value;
    };



    class MapCombo : public MenuItem {
    public:
        MapCombo(const char* inText, std::vector<std::string>* inComboText, std::string* inValue) {
            ComboText = inComboText;
            value = inValue;
            text = inText;
        }

        void Draw(bool Focus, color* col = nullptr, bool colorOveride = 0);
        void SetText(wchar_t* inText) {
            std::wstring wText = inText;
            std::string txt(wText.begin(), wText.end());
            text = txt;
        }
        void SetOwnerWindow(Window* window) {
            Master_Window = window;
        }
        void SetValue(int inValue) {
            *value = inValue;
        }
        void SetPosition(int x, int y) {
            Pos.x = x;
            Pos.y = y;
        }
        std::pair<int, int> CalcSize() {

            std::wstring str(text.begin(), text.end());


            return interfaces->surface->getTextSize(5, str.c_str());
        }
        Window* ExposeWindow() {
            return Master_Window;
        }
    private:
        std::string text;
        std::vector<std::string>* ComboText;
        ScreenVec Pos;
        Window* Master_Window;
        std::string* value;
        int m_nIndex = 0;
    };



    Window* CreateNewWindow();
    MenuItem* CreateCheckBox(const wchar_t* text, bool* value);
    MenuItem* CreateIntSlider(const wchar_t* text, int* value, int min, int max, int step = 1);
    MenuItem* CreateFloatSlider(const wchar_t* text, float* value, float min, float max, float step = .05);
    MenuItem* CreateCombo(const wchar_t* text, int* value, std::vector<std::wstring>* ComboText);
    MenuItem* CreateCStrCombo(const char* Text, int* value, std::vector<std::string>* ComboText);
    MenuItem* CreateMapCombo(const char* Text, std::string* value, std::vector<std::string>* ComboText);
    MenuItem* CreateHotKey(const wchar_t* Text, int* value);
    void Set_Focus(Window window);
    void Set_Focus(Window* window);
    void Set_Focus(int window_id);





    class HitboxCombo : public MenuItem {
    public:




        HitboxCombo(wchar_t* inText, Window* Master, std::array<bool,19>* inhitboxes) {
            hitbox_strs = {
                L"Head",
                L"Neck",
                L"Pelvis",
                L"Abdomen",
                L"Kidneys",
                L"Sternum",
                L"Clavicles",
                L"Left Thigh",
                L"Right Thigh",
                L"Left Shin",
                L"Right Shin",
                L"Left Ankle",
                L"Right Ankle",
                L"Left Hand",
                L"Right Hand",
                L"Left Arm",
                L"Left Forearm",
                L"Right Arm",
                L"Right Forearm",
                L"ALL"
            };

            Master_Window = Master;
            hitboxes_ptr = inhitboxes;
            hitboxes = *(hitboxes_ptr);
            ColorWindow = CreateNewWindow();
            ColorWindow->setAsSlave(Master, &open);
            ColorWindow->SetColor(120, 120, 120, 150);
            ColorWindow->SetWindowInfo(NULL, NULL, NULL, NULL, 40, 40, true);

            int i = 0;
            for (std::wstring Hitbox : hitbox_strs) {
                MenuItem* Enabled = CreateCheckBox(Hitbox.c_str(), &(hitboxes.at(i)));
                ColorWindow->Add_Item(Enabled);
                i++;
            }
            ColorWindow->ColorOverride(std::array<float, 4>{0, 250, 0, 255});

            text = inText;
            

        }
        void PreDraw() {
            hitboxes = *hitboxes_ptr;
        }
        void PostDraw() {
            *hitboxes_ptr = hitboxes;
        }
        void Draw(bool Focus, color* col = nullptr, bool colorOveride = 0);
        void SetText(wchar_t* inText) {
            text = inText;
        }
        void SetOwnerWindow(Window* window) {
            Master_Window = window;
        }
        void SetValue() {
        }
        void SetPosition(int x, int y) {
            Pos.x = x;
            Pos.y = y;
        }

        std::pair<int, int> CalcSize() {
            return interfaces->surface->getTextSize(5, text.c_str());
        }

        Window* ExposeWindow() {
            return ColorWindow;
        }

        bool* ExposeOpen() {
            return &open;
        }
    private:
        Window* ColorWindow;
        std::wstring text;
        std::array<bool, 19> hitboxes;
        std::array<bool, 19>* hitboxes_ptr;
        ScreenVec Pos;
        Window* Master_Window;
        bool gStruct = false;
        bool open = false;
        std::vector<std::wstring> hitbox_strs = {
            L"Head",
            L"Neck",
            L"Pelvis",
            L"Abdomen",
            L"Kidneys",
            L"Sternum",
            L"Clavicles",
            L"Left Thigh",
            L"Right Thigh",
            L"Left Shin",
            L"Right Shin",
            L"Left Ankle",
            L"Right Ankle",
            L"Left Hand",
            L"Right Hand",
            L"Left Arm",
            L"Left Forearm",
            L"Right Arm",
            L"Right Forearm",
            L"ALL"
        };
    };


    class ColorSettings : public MenuItem {
    public:
        ColorSettings(wchar_t* inText, Config::ColorToggle* col, Window* Master) {


            text = inText;
            value = col;

#ifdef _DEBUG
            return;
#endif

            Master_Window = Master;
            ColorWindow = CreateNewWindow();
            ColorWindow->setAsSlave(Master, &open);
            ColorWindow->SetColor(120, 120, 120, 150);
            ColorWindow->SetWindowInfo(NULL, NULL, NULL, NULL, 40, 40, true);
            MenuItem* Enabled = CreateCheckBox(L"Enabled: ", &(col->enabled));
            ColorWindow->Add_Item(Enabled);

            MenuItem* r = CreateFloatSlider(L"r: ", &(col->color[0]), 0.f, 1.f);
            ColorWindow->Add_Wrapped_Item(r, &(col->enabled));

            MenuItem* g = CreateFloatSlider(L"g: ", &(col->color[1]), 0.f, 1.f);
            ColorWindow->Add_Wrapped_Item(g, &(col->enabled));

            MenuItem* b = CreateFloatSlider(L"b: ", &(col->color[2]), 0.f, 1.f);
            ColorWindow->Add_Wrapped_Item(b, &(col->enabled));

            if (col->color.size() > 2) {
               // MenuItem* a = CreateFloatSlider(L"a: ", &(col->color[3]), 0.f, 1.f); // uhhhhh reinterpret_cast<float*>(&(col->color) + (3*sizeof(float))) because debug build is mad at my crimes of using the array subscript to point to memory that never was actually defined as
                //std::array<float, 4>* valcol = reinterpret_cast<std::array<float, 4>*>(value->color.data());    
                float* valcol = reinterpret_cast<float*>(col->color.data());
                MenuItem* a = CreateFloatSlider(L"a: ", &(valcol[3]), 0.f, 1.f);
                                                                                    
                                                                                     // as part of the array.....
                ColorWindow->Add_Wrapped_Item(a, &(col->enabled));
                valcol[3] = 1.f;
            }
            ColorWindow->ColorOverride(col);

        }

        ColorSettings(wchar_t* inText, Config::Glow* col, Window* Master) {

            text = inText;
            gvalue = col;
#ifdef _DEBUG
            return;
#endif
            Master_Window = Master;
            ColorWindow = CreateNewWindow();
            ColorWindow->setAsSlave(Master, &open);
            ColorWindow->SetColor(120, 120, 120, 150);
            ColorWindow->SetWindowInfo(NULL, NULL, NULL, NULL, 40, 40, true);
            MenuItem* Enabled = CreateCheckBox(L"Enabled: ", &(col->enabled));
            ColorWindow->Add_Item(Enabled);

            MenuItem* r = CreateFloatSlider(L"r: ", &(col->color[0]), 0.f, 1.f);
            ColorWindow->Add_Wrapped_Item(r, &(col->enabled));

            MenuItem* g = CreateFloatSlider(L"g: ", &(col->color[1]), 0.f, 1.f);
            ColorWindow->Add_Wrapped_Item(g, &(col->enabled));

            MenuItem* b = CreateFloatSlider(L"b: ", &(col->color[2]), 0.f, 1.f);
            ColorWindow->Add_Wrapped_Item(b, &(col->enabled));

            if (col->color.size() > 2) {
                //MenuItem* a = CreateFloatSlider(L"a: ",&(col->color[3]), 0.f, 1.f); //
                float* valcol = reinterpret_cast<float*>(col->color.data());
                MenuItem* a = CreateFloatSlider(L"a: ", &(valcol[3]), 0.f, 1.f);
                ColorWindow->Add_Wrapped_Item(a, &(col->enabled));
                col->color[3] = 1.f;
                //*reinterpret_cast<float*>(&(col->color) + (3 * sizeof(float))) = 1.f;
            }
            ColorWindow->ColorOverride(col);
            gStruct = true;
            text = inText;
            gvalue = col;
        }

        ColorSettings(wchar_t* inText, std::array<float,4>* col, Window* Master) {

            text = inText;
            fvalue = col;
#ifdef _DEBUG
            return;
#endif
            Master_Window = Master;
            ColorWindow = CreateNewWindow();
            ColorWindow->setAsSlave(Master, &open);
            ColorWindow->SetColor(120, 120, 120, 150);
            ColorWindow->SetWindowInfo(NULL, NULL, NULL, NULL, 40, 40, true);
            MenuItem* r = CreateFloatSlider(L"r: ", &((*col)[0]), 0.f, 1.f);
            ColorWindow->Add_Item(r);

            MenuItem* g = CreateFloatSlider(L"g: ", &((*col)[1]), 0.f, 1.f);
            ColorWindow->Add_Item(g);

            MenuItem* b = CreateFloatSlider(L"b: ", &((*col)[2]), 0.f, 1.f);
            ColorWindow->Add_Item(b);

            if (col->size() > 2) {
                MenuItem* a = CreateFloatSlider(L"a: ", &((*col)[3]), 0.f, 1.f);
                ColorWindow->Add_Item(a);
                (*col)[3] = 1.f;
            }
            ColorWindow->ColorOverride(*col);
            value = new Config::ColorToggle();

            float* valcol = reinterpret_cast<float*>(col->data());
            value->color[0] = (*col)[0];
            value->color[1] = (*col)[1];
            value->color[2] = (*col)[2];
            valcol[3] = (*col)[3];
            //*reinterpret_cast<float*>(&(value->color) + (3 * sizeof(float))) = (*col)[3];
            //value->color[3] = (*col)[3];
            fvalue = col;
            floatOver = true;
            text = inText;
        }
        void Draw(bool Focus, color* col = nullptr, bool colorOveride = 0);
        void SetText(wchar_t* inText) {
            text = inText;
        }
        void SetOwnerWindow(Window* window) {
            Master_Window = window;
        }
        void SetValue(Config::ColorToggle* inValue) {
            value = inValue;
        }
        void SetPosition(int x, int y) {
            Pos.x = x;
            Pos.y = y;
        }

        std::array<float, 3> GetColor() {
            if (gStruct) {
                return std::array<float, 3>{gvalue->color[0], gvalue->color[1], gvalue->color[2]};
            }
            else if (floatOver){
                return std::array<float, 3>{(*fvalue)[0], (*fvalue)[1], (*fvalue)[2]};
                ColorWindow->ColorOverride(*fvalue);
            }
            else {
                return std::array<float, 3>{value->color[0], value->color[1], value->color[2]};
            }
        }

        std::pair<int, int> CalcSize() {
            return interfaces->surface->getTextSize(5, text.c_str());
        }

        Window* ExposeWindow() {
            return ColorWindow;
        }

        bool* ExposeOpen() {
            return &open;
        }
    private:
        Window* ColorWindow;
        std::wstring text;
        ScreenVec Pos;
        Window* Master_Window;
        Config::ColorToggle* value;
        Config::Glow* gvalue;
        std::array<float, 4>* fvalue;
        bool gStruct = false;
        bool open = false;
        bool floatOver = false;
    };


    MenuItem* CreateColorSettings(const wchar_t* text, Config::ColorToggle* col, Window* Master_Window);
    MenuItem* CreateColorSettings(const wchar_t* Text, Config::Glow* col, Window* Master_Window);
    MenuItem* CreateColorSettings(const wchar_t* Text, std::array<float, 4>* col, Window* Master_Window);
    MenuItem* CreateHitBoxCombo(const wchar_t* Text, std::array<bool,19>* hitboxes, Window* Master_Window);
    //MenuItem* CreateCheckBox();

    //MenuItem* ContentWrapper(const wchar_t* text, bool* value);
    //MenuItem* CreateMultiItem(const wchar_t* text, bool* value);
    extern std::vector<Window*> Windows;
    extern int Window_Of_Focus;

    void Draw();



	/* Setup */

	bool Pendelum_Init(IDirect3DDevice9* device);
	LRESULT Pendelum_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    /* Utils */
    void Render();

	/* Debug */

	bool DrawMenuAnimation(PendelumTimer timer);
    bool DrawMenuAnimationClose(PendelumTimer timer);

	/* Drawing Functions */

	void Draw_Text(const char* text, float x, float y, D3DCOLOR color);


    void Draw_Filled_Rectangle_Clear(ScreenVec Start, ScreenVec End, D3DCOLOR color = D3DCOLOR_RGBA(255, 255, 255, 255));
    void Draw_Filled_Rectangle(ScreenVec Start, ScreenVec End, D3DCOLOR color = D3DCOLOR_RGBA(255, 255, 255, 255));


    /* Animation Functions */
}

