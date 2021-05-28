#include <d3d9.h>
#include <vector>
#include <array>
#include <string>
#include <cstdint>
#include <unordered_map>
//#include <chrono>

#include "Renderer.h"
#define DX9
#ifdef DX9
#include "D3D9Render.h"
#endif


#define PENDELUM2_DEBUG






#define KeyState GetKeyState
//#define KeyState GetAsyncKeyState

// I like to keep windows.h declarations hidden from libs. 
#if defined(_WIN64)
typedef unsigned __int64 Pend_UINT_PTR;
#else
typedef unsigned int Pend_UINT_PTR;
#endif

#if defined(_WIN64)
typedef __int64 Pend_LONG_PTR;
#else
typedef long Pend_LONG_PTR;
#endif

// Define Custom Output Parameters for Debugging



#ifndef DEBUGOUT
#ifdef PENDELUM2_DEBUG
#define DEBUGOUT(STRING) OutputDebugStringW(L##STRING);
#endif
#ifndef PENDELUM2_DEBUG
#define DEBUGOUT(STRING) ;
#endif
#endif


//#define TEXTPOPABOVE

// Easy Constructor for Single Value Member Items
#define PVMConstructor(NAME, TYPE) NAME(TYPE** in_value, const char* text) { m_pValue = reinterpret_cast<void**>(in_value);  m_csTitle = text; }
// Converts PendColor to rCOLOR
#define PCOLORTORCOLOR(NAME) {NAME.r, NAME.g, NAME.b, NAME.a}
// Splits out Coords
#define POINTSTORCOORD(NAME) {{(float)NAME.x1, (float)NAME.y1}, {(float)NAME.x2, (float)NAME.y1}, {(float)NAME.x1, (float)NAME.y2}, {(float)NAME.x2, (float)NAME.y2}}

namespace Pendelum2 {
    inline Renderer::Renderer* g_pRenderer;

    struct Points {
        int x1, x2, y1, y2;
    };

    struct PendColor {
        int r, g, b, a;
    };


    namespace Animation {

    }

    namespace Input {
        class MouseInput {
        public:
            bool IsMouseWithinBounds(int x, int y, int x2, int y2) {
                POINT p;
                if (GetCursorPos(&p))
                {
                    if (ScreenToClient(GetActiveWindow(), &p))
                    {
                        g_pRenderer->drawFilledRectangle2D(p.x, p.y, 10, 10, { 255,0,0,255 });
                            if ((p.x > x) && (p.x < x2)) {
                                if ((p.y > y) && (p.y < y2)) {

                                    return true;
                                }
                            }                       
                    }
                }
                return false;
            }


            bool IsMouseDown() {
                if ((GetKeyState(VK_LBUTTON) & 0x8000) != 0) {
                    m_bCursorClick = true;
                    return true;
                }
                else {
                    m_bCursorClick = false;
                    POINT p;
                    if (GetCursorPos(&p))
                    {
                        if (ScreenToClient(GetActiveWindow(), &p))
                        {
                            m_ipCursorEngagePosition = { p.x,p.y };
                            
                        }

                    }
                    return false;
                }
            }


            bool GetPositionalDifference(int& out_x, int& out_y) {
                POINT p;
                if (GetCursorPos(&p))
                {
                    if (ScreenToClient(GetActiveWindow(), &p))
                    {
                            out_x = m_ipCursorEngagePosition.first - p.x;
                            out_y = m_ipCursorEngagePosition.second - p.y;
                            return true;
                    } 
                    //p.x and p.y are now relative to hwnd's client area
                }
                //cursor position now in p.x and p.y          
            }




        private:
            std::pair<int, int> m_ipCursorEngagePosition;
            bool m_bCursorClick = false;


        };
        class KeyListener {
        public:
            KeyListener() {}
            KeyListener(int Key) {
                m_iKeyState = Key;
            }
            void BindToKey(int key) {
                m_iKey = key;
            }
            int HasKeyStateChanged() {
                short tempState = KeyState(m_iKey);
                if (tempState != m_iKeyState) {
                    UpdateKeyData(tempState);
                    return true;
                }
                UpdateKeyData(tempState);
                return false;
            }
            bool isKeyDown() {
                m_iKeyState = KeyState(m_iKey);
            }
            bool hasKeyToggled() {
                short State = KeyState(m_iKey);
                if (State & 0x8000) {
                    if (!m_bWasDown) {
                        m_bWasDown = true;
                        UpdateKeyData(State);
                        return true;
                    }
                }
                UpdateKeyData(State);
                return false;
            }
        private:
            void UpdateKeyData(short State) {
                m_iKeyState = State;
                m_bWasDown = (State & 0x8000) ? true : false;
            }
            int m_iKey;
            short m_iKeyState;
            bool m_bWasDown;
        };

        class KeyboardListener {
        public:
            KeyboardListener() {
                int i = 0;
                for (KeyListener& Key : Keys) {
                    i++;
                    Key.BindToKey(i);
                }
            }
            void WndProcKeyEvent(unsigned int msg, Pend_UINT_PTR wParam, Pend_LONG_PTR lParam) {




            }
        private:
            std::array<KeyListener, 255> Keys;
            
        };

        inline MouseInput Mouse;
    }



    LRESULT WndProcHandler(void* hwnd, unsigned int msg, Pend_UINT_PTR wParam, Pend_LONG_PTR lParam);

    struct Pad {
        float pad1, pad2;
    };

    class Position {
    public:
        Position() { rawPoints = &m_poOurPoints; }
        Position(int x, int y, int width, int height) : m_fHeight(height) , m_fWidth(width), rawPoints(&m_poOurPoints){
            SetPosition(x, y);
        }
        Position( Pad Padding, Position* RelativeObj) {
            rawPoints = &m_poOurPoints;
            SetPosition(Padding, RelativeObj);
        }
        void SetPosition(int x, int y){
            m_poOurPoints.x1 = x;
            m_poOurPoints.y1 = y;
            m_poOurPoints.x2 = x + m_fHeight;
            m_poOurPoints.y2 = y + m_fWidth;
        }
        void SetPosition(Pad Padding, Position* RelativeObj) {
            m_bIsRelative = true;
            m_bIsDynamic = true;
            m_paPadding = Padding;
            Relative = RelativeObj;
            GetPointsRelative();
        }
        void SetRelative(Pad Padding, Position* RelativeObj) {
            rawPoints = &m_poOurPoints;
            SetPosition(Padding, RelativeObj);
        }
        void SetSize(int w, int h) {
            m_fWidth = w;
            m_fHeight = h;
        }
        std::pair<int, int> GetSize() {
            return std::pair<int, int>(m_fWidth, m_fHeight);
        }

        Points GetPoints() {
            if (!m_bIsRelative)
                return m_poOurPoints;
            return GetPointsRelative();
        }
        Points* rawPoints;
    private:
        Points GetPointsRelative() {
            Points Their = Relative->GetPoints();
            if (m_bIsDynamic) {
                m_poOurPoints.x1 = ((Their.x2 - Their.x1) * m_paPadding.pad1) + Their.x1;
                m_poOurPoints.y1 = ((Their.y2 - Their.y1) * m_paPadding.pad1) + Their.y1;
                m_poOurPoints.x2 = ((Their.x2 - Their.x1) * m_paPadding.pad2) + Their.x2;
                m_poOurPoints.y2 = ((Their.y2 - Their.y1) * m_paPadding.pad2) + Their.y2;
            }
            return m_poOurPoints;
        }
        bool m_bIsRelative = false;
        bool m_bIsDynamic = false;
        float m_fWidth, m_fHeight;
        Pad m_paPadding;
        Points m_poOurPoints;
        Position* Relative;
    };


    class PObject {
    public:
        virtual void Draw() = 0;
        virtual void OnClickEvent() {};
        Position GetPosition() { return m_poPos; };
    protected:
        Position m_poPos;
    private:
    };


    class PWindow;
    class PIntergratedWindow;
    class PMember : public PObject {
    public:
        PMember(PWindow* window) : m_pMaster(window) {};
        PMember(PIntergratedWindow* window) : m_pMaster(reinterpret_cast<PWindow*>(window)) {}
        PMember() {};
        void AddMaster(PWindow* window) { m_pMaster = window; };
        void AddMaster(PIntergratedWindow* window) { m_pMaster = reinterpret_cast<PWindow*>(window); };
    protected:
        PWindow* m_pMaster;
        const char* m_csTitle;
    };


    class PValueMember : public PMember {
    public:

    protected:
        void** m_pValue;
    };


    class PWindow : public PObject {
    public:
        PWindow(int x, int y, int w, int h, PendColor WindowColor, const char* ititle, bool bordered = false, PendColor BorderColor = { 255,255,255,255 })
        : m_Color(WindowColor) , title(ititle), m_bIsBordered(bordered), m_BorderColor(BorderColor) {
            SetSize(w, h);
            SetPosition(x, y);
        }


        void OnClickEvent() {

        }


        void Draw() {
            Points point = m_poPos.GetPoints();
            g_pRenderer->drawFilledRectangle2D(POINTSTORCOORD(point), PCOLORTORCOLOR(m_Color));
            if (m_bIsBordered) {
                RendererStructs::coordinate coords[] = { { point.x1, point.y1 }, { point.x2, point.y1 }, { point.x2, point.y2 }, { point.x1, point.y2 }, { point.x1, point.y1 } };
                g_pRenderer->drawMultiLine(coords, 5, PCOLORTORCOLOR(m_BorderColor));
            }
            DrawTitleBar();

            for (auto Member : m_umMembers) {
                Member.second->Draw();
            }


        }
        void SetSize(int w, int h) {
            m_poPos.SetSize(w, h);
        }
      
        void SetPosition(int x, int y) {
            m_poPos.SetPosition(x, y);
        }

        Position GetDrawPosition() {
            Position pos = m_poPos;
            Points points = pos.GetPoints();
            pos.SetPosition(points.x1 + 5, points.y1 - titlebarsize.second + 2);
            return pos;
        }

        void IncrementPosition(int x, int y) {
            titlebarsize.second += y;
        }

        void SetColor(PendColor WindowColor) {
            m_Color = WindowColor;
        }

        PendColor GetColor() {
            return m_Color;
        }

        void SetBorder(bool settings, PendColor BorderColor = { 255,255,255,255 }) {
            m_bIsBordered = settings;
            if (m_bIsBordered) {
                m_BorderColor = BorderColor;
            }
        }
        void AddItem(PMember* member, const char* Identifier) {
            auto member_search = m_umMemberAccessList.find(Identifier);
            if (member_search != m_umMemberAccessList.end()) {
                DEBUGOUT("Item Inserted Already Exists")
                return;
            }
            member->AddMaster(this);
            m_umMembers.insert({ m_iItemCount, member });
            m_umMemberAccessList.insert({ Identifier,m_iItemCount });
            m_iItemCount++;
        }
        void DeleteItem(const char* Identifier) {
            auto member = m_umMemberAccessList.find(Identifier);
            if (member == m_umMemberAccessList.end())
                return;
            int InternalMemberID = m_umMemberAccessList.at(Identifier);
            delete m_umMembers.at(InternalMemberID);
            m_umMembers.erase(InternalMemberID);
            m_umMemberAccessList.erase(Identifier);
        }
    protected:
        void DrawTitleBar() {
            Points point = m_poPos.GetPoints();
            titlebarsize = g_pRenderer->getTextSize(1, title);
            Points titleBar;
            titleBar = point;
            titleBar.x2 += 1;
            titleBar.y2 = point.y1 - titlebarsize.second;
#ifndef TEXTPOPABOVE
            g_pRenderer->drawFilledRectangle2D(POINTSTORCOORD(titleBar), PCOLORTORCOLOR(m_Color));
#endif
            //RendererStructs::coordinate ncoords[] = { { point.x1, titleBar.y2 }, { point.x2,titleBar.y2 } };
            //g_pRenderer->drawMultiLine(ncoords, 2, PCOLORTORCOLOR(m_BorderColor));
            float textx = (titleBar.x1 + (((titleBar.x2 - titleBar.x1) / 2.f) - (titlebarsize.first / 2.f)));
            float texty = (float)point.y1;
#ifdef TEXTPOPABOVE
            texty = (titleBar.y1 + (titlebarsize.second / 2.f));
#endif
            g_pRenderer->drawText({textx, texty}, 1, title, { 255,255,255,255 });          
            POINT p;
            if (GetCursorPos(&p))
            {
                if (ScreenToClient(GetActiveWindow(), &p))
                {
                    g_pRenderer->drawFilledRectangle2D(p.x, p.y, 10, 10, { 255,0,0,255 });
                    if (Input::Mouse.IsMouseDown()) {
                        if ((p.x > point.x1) && (p.x < titleBar.x2)) {
                            if ((p.y > point.y1) && (p.y < titleBar.y2)) {

                                if (!m_bClicked)
                                    m_ipCursorEngagePosition = { point.x1 - p.x,point.y1 - p.y };
                                m_bClicked = true;

                            }
                        }
                    }
                    else {
                        m_bClicked = false;
                    }
                }
            }
            if (m_bClicked) {
                int x_diff, y_diff;
                x_diff = point.x1 - (p.x + m_ipCursorEngagePosition.first);
                y_diff = point.y1 - (p.y + m_ipCursorEngagePosition.second);
                {
                    m_poPos.SetPosition(point.x1 - x_diff, point.y1 - y_diff);
                }
            }  
            return;
        }
    protected:
        std::pair<int, int> titlebarsize;
        std::pair<int, int> m_ipCursorEngagePosition;
        bool m_bClicked = false;
        std::unordered_map<std::string, int> m_umMemberAccessList; 
        std::unordered_map<int, PMember*> m_umMembers; 
        int m_iItemCount = 0;
        PendColor m_Color;
        PendColor m_BorderColor;
        bool m_bIsBordered;
        const char* title;
    };


    class PIntergratedWindow : public PMember,  PWindow{
    public:
        void Draw() {
            Points point = m_poPos.GetPoints();
            g_pRenderer->drawFilledRectangle2D(POINTSTORCOORD(point), PCOLORTORCOLOR(m_Color));
            if (m_bIsBordered) {
                RendererStructs::coordinate coords[] = { { point.x1, point.y1 }, { point.x2, point.y1 }, { point.x2, point.y2 }, { point.x1, point.y2 }, { point.x1, point.y1 } };
                g_pRenderer->drawMultiLine(coords, 5, PCOLORTORCOLOR(m_BorderColor));
            }
        }
    protected:

    private:
        Position m_poPos;
        //bool m_bIsBordered;
    };



    class PCheckbox : PValueMember {
    public:
        PVMConstructor(PCheckbox, bool)

        
            void Draw() {
            m_poPos = m_pMaster->GetDrawPosition();
            Points point = m_poPos.GetPoints();

            auto [w, h] = g_pRenderer->getTextSize(1, m_csTitle);


            point.x2 = point.x1 + 200;
            m_Color = m_pMaster->GetColor();
            point.y2 = (point.y1 - h) + 20;

            auto [boxw, boxh] = g_pRenderer->getTextSize(1, "XX");

            PendColor col;

            if (*(bool*)*m_pValue) {
                col = { 0,0,0,255 };
            }
            else {
                col = { 255,255,255,255 };
            }


            Points Check = point;
            Check.x1 = point.x1;
            Check.x2 = point.x1 + boxw;
            Check.y1 = point.y1 + 10;
            Check.y2 = (point.y1 - boxh) + 10;

            g_pRenderer->drawFilledRectangle2D(POINTSTORCOORD(Check), PCOLORTORCOLOR(col));


            if (Input::Mouse.IsMouseDown()) {


                if (!m_bIsDown) {
                    if (Input::Mouse.IsMouseWithinBounds(Check.x1, Check.y1, Check.x2, Check.y2)) {
                        *(bool*)m_pValue = !(*(bool*)m_pValue);
                    }
                    m_bIsDown = true;
                }
            }
            else {
                m_bIsDown = false;
            }


            //g_pRenderer->drawFilledRectangle2D(POINTSTORCOORD(point), PCOLORTORCOLOR(m_Color));
            point.y1 += 10;// + (h/2);
            g_pRenderer->drawText( {(float)point.x1 + (boxw * 1.5f), (float)point.y1 }, 1, m_csTitle, { 255,255,255,255 });


            m_pMaster->IncrementPosition(200, 40 + h);

            //if (m_bIsBordered) {
            //    RendererStructs::coordinate coords[] = { { point.x1, point.y1 }, { point.x2, point.y1 }, { point.x2, point.y2 }, { point.x1, point.y2 }, { point.x1, point.y1 } };
            //    g_pRenderer->drawMultiLine(coords, 5, PCOLORTORCOLOR(m_BorderColor));
            //}


        }
    private:
        PendColor m_Color;
        bool m_bIsDown = false;
    };


    template<class T>
    class PSlider : public PValueMember {
    public:
        PVMConstructor(PSlider, T)

    private:
    };


    


















}