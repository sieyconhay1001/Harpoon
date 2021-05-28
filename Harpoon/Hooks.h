#pragma once

#include <d3d9.h>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <Windows.h>


//#include "Hooks/Antaeus/Hooks.h"
#include "Hooks/Antaeus/Hooks/UserMode/MinHook.h"
#include "Hooks/Antaeus/Hooks/UserMode/VmtSwap.h"
#include "Pendelum/D3D9Render.h"
#include "SDK/OsirisSDK/SoundInfo.h"
//#include "Hooks/Antaeus/Hooks/UserMode/MinHook/MinHook.h"


using HookType = MinHook; // Osiris had a good idea, so I stole it

class Hooks {
public:
    Hooks(HMODULE module) noexcept;

    void install() noexcept;
    void uninstall() noexcept;

    WNDPROC originalWndProc;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)> originalPresent;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)> originalReset;
    std::add_pointer_t<int __fastcall(SoundInfo&)> originalDispatchSound;




    /*



    //

    HookingType 
    */



    MinHook bspQuery;


    

    
    //HookingType* clMove = hooks.UseDetourHook();
    MinHook modelRender;

    MinHook sound;
    MinHook surface;
    MinHook viewRender;
    MinHook gameEventManager;
    MinHook gamemovement;
    VmtSwap networkChannel;
    //MinHook networkChannel;
    MinHook engine;
    MinHook client;
    MinHook panel;
    MinHook clientMode;
    MinHook svCheats;
    MinHook net_maxroutable;
    MinHook sv_netMaxRoutable;
    MinHook renderView;
    MinHook studioRender;
    MinHook materialSystem;
    MinHook fileSystem; 
    MinHook gameCoordinator;
    MinHook steamNetworkingSockets;
    MinHook steamNetworkingUtils;
    MinHook steamNetworkingMessages;
    MinHook steamNetworking;
    MinHook entityList;
    MinHook Predicition;
    MinHook clientState;
    MinHook steamFriends;
    MinHook hclientstate;
    MinHook steamMatchmaking;
    MinHook IMatchSink;
    bool netchanhooked = false;
private:
    HMODULE module;
    HWND window;
};

inline std::unique_ptr<Hooks> hooks;
inline Renderer::Renderer* renderer;
class Entity; 

struct Container {
    Container() : isHooked(false) { }

    VmtSwap vmt;
    bool isHooked = false;
    Entity* OldEntity = nullptr;
};

class extraHooks
{
public:
    Container player;
    void hookEntity(Entity* ent);
    bool init();
    bool init(Entity* ent);
    void restore();
};

extern extraHooks extraHook;