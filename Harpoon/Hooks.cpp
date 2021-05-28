#include <functional>
#include <intrin.h>
#include <string>
#include <Windows.h>
#include <Psapi.h>

#//include "Antaeus/Hooks/UserMode/MinHook/MinHook.h"

// Osiris Code

#include "Memory.h"
#include "Netvars.h"


#include "SDK/OsirisSDK/Engine.h"
#include "SDK/OsirisSDK/Sound.h"
#include "SDK/OsirisSDK/Surface.h"
#include "SDK/OsirisSDK/Cvar.h"
#include "SDK/OsirisSDK/LocalPlayer.h"
#include "SDK/OsirisSDK/Entity.h"

// Rewritten Code Based off of Original Osiris Code
#include "Hooks.h"
#include "Hooks/Antaeus/Hooks/UserMode/MinHook/MinHook.h"
#include "Interfaces.h"
#include "Config.h"
#include "Other/EventListener.h"
// Personal Code

#include "SDK/SDKAddition/EntityListCull.hpp"
#include "Hacks/TickbaseManager.h"
#include <thread>

#include "Pendelum/D3D9Render.h"
#include "Hacks/Chams.h"
#include "Other/ChatConsole.h"
static LRESULT __stdcall wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
   
    static const auto once = [](HWND window) noexcept {
        netvars = std::make_unique<Netvars>();
        eventListener = std::make_unique<EventListener>();
        config = std::make_unique<Config>();
        renderer = new D3D9Renderer;
        g_pChamsRenderer = new Chams();
        g_pChatConsole = new ChatConsole();
        g_pConsole = new Console();
        entitylistculled = std::make_unique<EntityListCull>(); // Call this last, requires config exists
        //TickbaseManager::tick = std::make_unique<TickbaseManager::Tick>();
        SoundSystem::soundPlayer = std::make_unique<SoundSystem::SoundPlayer>();
        hooks->install();

        return true;
    }(window);



    //Tickbase::tick = std::make_unique<Tickbase::Tick>();
    return CallWindowProcW(hooks->originalWndProc, window, msg, wParam, lParam);




    
}

#include "Pendelum/Pendelum.h"
#include "Menu.h"
#include "Hacks/ESP.h"

#include "EngineHooks.h"
#include "Hacks/Renderables.h"
#include "SDK/OsirisSDK/RenderView.h"
#include "SDK/SDK/ViewRender.h"
bool init = false;
bool init_lag = true;
static HRESULT __stdcall present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion) noexcept
{

    if (!init) {
        
        D3D9Initializer InitSettings;
        InitSettings.SetDevice(device);
        InitSettings.SetScreenFromDevice();
        RendererUtils::RendererInitializer* Init = reinterpret_cast<RendererUtils::RendererInitializer*>(&InitSettings);
        renderer->Init(Init);
               
        
    }


    if (!init_lag) {
        //EngineHooks::InitLaggers();
        init_lag = true;
    }

    if (!init) {
        init = true;
        init_lag = false;
    }
        





    Pendelum::Pendelum_Init(device);
    
    
    RendererStructs::coordQuad NewPair;



    NewPair.lowerLeft.x = 5;
    NewPair.lowerLeft.y = 0;
    NewPair.lowerRight.x = 10;
    NewPair.lowerRight.y = 0;



    NewPair.upperLeft.x = 5;
    NewPair.upperLeft.y = 50;
    NewPair.upperRight.x = 100;
    NewPair.upperRight.y = 50;
    
    


    renderer->Begin();   

    if (!init_lag) {
        renderer->drawText({ 500,500 }, 1, "Initializing Data....", { 0,255,0,255 });
    }

    Renderables::NavDraw::DrawWholeMesh();
    Renderables::NavDraw::Run();
    Renderables::NavDraw::DrawWalkbotPath();

    //renderer->drawMultiLine({ {1,1}, {200,400} }, { 0,255,0,200 }, 10);

    //renderer->drawFilledRectangle2D({ 100,100 }, { 200,200 }, { 50,50,50,100 });
    entitylistculled->quickcullEntities();
    ESP::run();
    //renderer->drawFilledRectangle2D(NewPair, { 255,255,255,150 });  

    Menu::NewMenu();

    renderer->Draw();
    

    if (GetAsyncKeyState(VK_END)) {
        hooks->uninstall();
    }

    return hooks->originalPresent(device, src, dest, windowOverride, dirtyRegion);
}

static HRESULT __stdcall reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
    return hooks->originalReset(device, params);
}



/* We Hook Our Functions */
#include "OtherHooks.h"
#include "ClientHooks.h"
//#include "Hooks/Antaeus/Hooks/UserMode/MinHook.h"
#include <future>




unsigned int __stdcall forceSpectate(void* config) {
    Config* conf = *((Config**)config);

    while (true) {
        if (conf->debug.Lagger) {
            memory->setClanTag("Tryina Set Clan Tag ", "Tryina Set Clan Tag");
        }
    }

    //while (true) {
    //    while (conf->debug.spectateForce) {
     //       if (ClientModeHooks::newTick) {
     //           ClientModeHooks::newTickMut.lock();
     //           ClientModeHooks::newTick = false;
     //           ClientModeHooks::newTickMut.unlock();
     //           memory->setClanTag("Tryina Set Clan Tag ", "Tryina Set Clan Tag");
     //           Debug::QuickPrint("Setting Clean Tag In forceSpectate");
     //       }
    //
     //   }
    //}
}
HANDLE myHandle;
#include "Hacks/Walkbot/WalkbotNetworking.h"
static void _cdecl WalkbotNetworkThread(LPVOID pParam) {

#ifdef SAFE_BUILD
    return;
#endif

    memory->conColorMsg({ 100,100,255,255 }, "[Harpoon] Walkbot-Network Spawning Walkbot Network Connection Thread\n");
    WalkbotNetworking::NetworkContainer.WalkbotNetwork = std::make_unique<WalkbotNetworking::WalkbotNetworkListener>(std::string("127.0.0.1").c_str(), 1592, &WalkbotNetworking::NetworkContainer);
    while (true) {
        WalkbotNetworking::NetworkContainer.WalkbotNetwork->MainLoop();
    }
}




//void 
//CL_MoveFn originalCLMove;

/*

static void __cdecl CL_Move(void* ecx, void* edx, float accumulated_extra_samples, bool bFinalTick)
{


    //memory->conColorMsg({ 0,255,0,255 }, "[Harpoon] Hooked CL_MOVE [/Harpoon]");
    
    
    if (Globals::teleport)
    {
        Debug::QuickPrint("In CL_MOVE, Executing Normally..");
        Globals::teleport = false;
        return;
    }


    //originalCLMove(ecx, edx, accumulated_extra_samples, bFinalTick);

    //auto original = hooks->clMove.getOriginal<CL_MoveFn, 0>(accumulated_extra_samples, bFinalTick);

    //if (true) {
     //   original(ecx, accumulated_extra_samples, bFinalTick);
    //    return;
    //}

    //hooks->clMove.callOriginal<CL_MoveFn,0>(accumulated_extra_samples, bFinalTick);
    //return;
    // static bool __stdcall
    Debug::QuickPrint("Attempting to Call Original CL_MOVE");
    ((CL_MoveFn)oCL_Move)(accumulated_extra_samples, bFinalTick);
    Debug::QuickPrint("Called Original Successfully");
    return;
}
*/

/* In the middle of figuring out animfixing, this got posted. So i added this so my shit*/
/* All credits to https://www.unknowncheats.me/forum/counterstrike-global-offensive/426383-paste-favourite-public-animation-fix.html */
/* This is an egregous example of pasting*/


//sig: "57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02"
#include "Hacks/OTHER/Debug.h"
bool __fastcall shouldSkipAnimationFrame(void* _this, void* edx) {
    return false;
}


//sig: "55 8B EC 83 E4 F0 83 EC 78 56 8B F1 57 8B 56"

void __fastcall doProceduralFootPlant(void* _this, void* edx, void* boneToWorld, void* leftFootChain, void* rightFootChain, void* pos) {
    return;
}

//sig: "55 8B EC 56 8B 75 18 57"
/* // I actually already have this
is_jiggle_bones_enabled : offset of "DT_CSPlayer=>m_hLightingOrigin" - 0x18

void __fastcall build_transformations(void* this_pointer, void* edx, void* hdr, void* pos, void* q, const void* camera_transform, int bone_mask, void* bone_computed) {

    static auto original_build_transformations = m_build_transformations.get_original_function< decltype(&build_transformations) >();

    // the function is apart of the C_CSPlayer virtual table so the "this" pointer is a player entity
    const auto player = reinterpret_cast<sdk::cs_player*>(this_pointer);

    // disable the games jiggle physics
    // there is no need to backup and restore m_isJiggleBonesEnabled
    player->is_jiggle_bones_enabled() = false;

    return original_build_transformations(this_pointer, edx, hdr, pos, q, camera_transform, bone_mask, bone_computed);

}
*/

//sig: "55 8B EC 51 53 8B 5D 08 56 8B F1 57 85"

typedef void(__thiscall* CheckForSeqFunc)(void*, void*, int, bool, bool);
void __fastcall  checkForSequenceChange(void* _this, void* edx, void* hdr, int currrentSequence, bool forceNewSequence, bool Interpolate) {
    ((CheckForSeqFunc)oCheckForSequenceChange)(_this, hdr, currrentSequence, forceNewSequence, false);
}



// interface : VEngineClient
//    index : 93


typedef void(__thiscall* isHltvFunc)(void*);
bool __fastcall isHltv(void* _this, void* edx) {
    if (!interfaces->engine->isInGame())
        return false;

    if (config->visuals.fullHLTV && localPlayer.get() && !localPlayer->isAlive())
        return true;


    
    if(config->debug.alwaystrueforhltv)
        return true;

   // LPVOID rEBP;
    //_asm pop ebp
    //_asm mov rEBP, ebp
   // _asm push ebp


    //shared::stack stack(shared::utilities::get_ebp(_AddressOfReturnAddress()));

    static const auto setup_velocity =  Memory::findPattern_ex(L"client.dll","\x84\xC0\x75\x38\x8B\x0D????\x8B\x01\x8B\x80");

    // forces SetupVelocity to use GetAbsVelocity and not EstimateAbeVelocity

    if (reinterpret_cast<uintptr_t>(_ReturnAddress()) == setup_velocity) {
       // Debug::QuickPrint("Returning True From isHLTV");
        return true;
    }

    static const auto accumulate_layers = Memory::findPattern_ex(L"client.dll", "\x84\xC0\x75\x0D\xF6\x87");
    if (reinterpret_cast<uintptr_t>(_ReturnAddress()) == accumulate_layers) {
        //Debug::QuickPrint("Returning True From isHLTV");
        return true;
    }
    // PVS fix


    return hooks->engine.callOriginal<bool, 93>();
    //return original_is_hltv(this_pointer, edx);

}

int __cdecl HOST_ERROR_HK(const char* in, ...) { // No way to stop this from getting called so we gotta hook it

    Debug::QuickWarning(std::string("HOST ERROR: " + std::string(in)));



    /* So this error occurs when our simulated clock gets too far ahead and the frame requested with a delta is the current frame */
    if (strstr(in, "referencing")) { // HOT_FIX!!!!
       //static auto drawotherVar{ interfaces->cvar->findVar("cl_flushentitypacket") };
       //drawotherVar->onChangeCallbacks.size = 0;
       //drawotherVar->setValue(1);  
       memory->clientState->deltaTick = -1;
    }
    return 0;
}


int __cdecl ConnectionState_ProblemDetectedLocally(int Reason, signed int a2, const char* pszFmt, ...) {
    Debug::QuickWarning(std::string("Steam Networking Sockets Local Error: " + std::string(pszFmt)));
    return 0;
}




#include "Timing.h"

//using _fnChat = char* (*)(void* packet, char const* errorString, ...);
typedef int(_cdecl* flep)(void*, const char*, int yo);
int _cdecl CL_FlushEntityPacket(void* packet, char const* errorString, int yo) {
    static auto drawotherVar{ interfaces->cvar->findVar("cl_flushentitypacket") };
    //drawotherVar->onChangeCallbacks.size = 0;
    //drawotherVar->setValue(1);  
    if (drawotherVar->getInt() <= 1) {
       // memory->clientState->deltaTick = -1;
    }
    if (strstr(errorString, "delta")) { /* This occurs when we lost/skipped frames while in prediction mode. */
        //drawotherVar->onChangeCallbacks.size = 0;
        //drawotherVar->setValue(1);  
        //if (!Timing::TimingSet_s.m_bInPredictionMode) {
        //    memory->clientState->deltaTick = -1;
        //}
        Timing::ExploitTiming.m_bForceReturnDeltaFix = true;
    }
    return ((flep)memory->oCL_FlushEntityPacket)(packet, errorString, yo); 
}


char __fastcall IDK2_HK(DWORD* a1, const char* a2, DWORD* a3, WORD* a4)
{


}


//char __usercall EncodeMatch(__int16 a1@<dx>, _DWORD* a2@<ecx>, __int64 tvBytes, unsigned __int64 a4)/
//{
//
//}


/* Nothing better than Tribe's midnight maruaders */

typedef void(__thiscall* hdco)(void*);
void __fastcall HandleDeferredConnection(void* cbaseclientstate, void* edx) {
    //Debug::QuickPrint("HandleDeferredConnection");
    ((hdco)memory->oCBaseClientState_HandleDeferredConnection)(cbaseclientstate);
    return;
}

//EngineHooks::ns_address;
struct SoundData {
    std::byte pad[4];
    int entityIndex;
    int channel;
    const char* soundEntry;
    std::byte pad1[8];
    float volume;
    std::byte pad2[44];
};

static void __stdcall emitSound(SoundData data) noexcept
{
    auto modulateVolume = [&data](int(*get)(int)) {
        if (const auto entity = interfaces->entityList->getEntity(data.entityIndex); localPlayer && entity && entity->isPlayer()) {
            if (data.entityIndex == localPlayer->index())
                data.volume *= get(0) / 100.0f;
            else if (!entity->isOtherEnemy(localPlayer.get()))
                data.volume *= get(1) / 100.0f;
            else
                data.volume *= get(2) / 100.0f;
        }
    };

    modulateVolume([](int index) { return config->sound.players[index].masterVolume; });

    if (strstr(data.soundEntry, "Weapon") && strstr(data.soundEntry, "Single")) {
        modulateVolume([](int index) { return config->sound.players[index].weaponVolume; });
    }
    else if (config->misc.autoAccept && !strcmp(data.soundEntry, "UIPanorama.popup_accept_match_beep")) {
        memory->acceptMatch("");
        auto window = FindWindowW(L"Valve001", NULL);
        FLASHWINFO flash{ sizeof(FLASHWINFO), window, FLASHW_TRAY | FLASHW_TIMERNOFG, 0, 0 };
        FlashWindowEx(&flash);
        ShowWindow(window, SW_RESTORE);
    }
    data.volume = std::clamp(data.volume, 0.0f, 1.0f);
    hooks->sound.callOriginal<void, 5>(data);
}

#include "SDK/OsirisSDK/SoundEmitter.h"

static int __fastcall dispatchSound(SoundInfo& soundInfo) noexcept
{
    if (const char* soundName = interfaces->soundEmitter->getSoundName(soundInfo.soundIndex)) {
        auto modulateVolume = [&soundInfo](int(*get)(int)) {
            if (auto entity{ interfaces->entityList->getEntity(soundInfo.entityIndex) }; entity && entity->isPlayer()) {
                if (localPlayer && soundInfo.entityIndex == localPlayer->index())
                    soundInfo.volume *= get(0) / 100.0f;
                else if (!entity->isOtherEnemy(localPlayer.get()))
                    soundInfo.volume *= get(1) / 100.0f;
                else
                    soundInfo.volume *= get(2) / 100.0f;
            }
        };

        modulateVolume([](int index) { return config->sound.players[index].masterVolume; });

        if (!strcmp(soundName, "Player.DamageHelmetFeedback"))
            modulateVolume([](int index) { return config->sound.players[index].headshotVolume; });
        else if (strstr(soundName, "Step"))
            modulateVolume([](int index) { return config->sound.players[index].footstepVolume; });
        else if (strstr(soundName, "Chicken"))
            soundInfo.volume *= config->sound.chickenVolume / 100.0f;
    }
    soundInfo.volume = std::clamp(soundInfo.volume, 0.0f, 1.0f);
    return hooks->originalDispatchSound(soundInfo);
}


#pragma once
#include "Hacks/Other/Debug.h"
#include "EngineHooks.h"
#include "SDK/SDK/ModelRenderSystem.h"
#include "Hacks/SteamWorks/SteamWorks.h"

//#define LIGHT_HOOKS



using _BYTE = unsigned char;
using _DWORD = uint32_t;
int __fastcall GetPredictedFrame(_DWORD* __this, unsigned int a2)
{
    int result; // eax
    //

    if (__this[587])
        result = __this[a2 % 0x96 + 286];
    else
        result = 0;
    return result;
}



#define HOOKFUNCTION(NAME, HookFunction) if (MH_CreateHook((LPVOID*)(memory->##NAME), &HookFunction, (LPVOID*)(&memory->o##NAME)) != MH_OK) {   Debug::QuickPrint(#NAME, false);    MH_DisableHook((LPVOID*)memory->##NAME);}



#include "Timing.h"


bool __fastcall net_routable_int(void* _this) noexcept
{
    if (!GetAsyncKeyState(config->TestShit.fuckKey2) || config->debug.Lagger || config->lagger.FULLONBOGANFUCKERY)
        return 13;
    return 13;
}
bool __fastcall sv_netroutable_int(void* _this) noexcept
{
    if (!GetAsyncKeyState(config->TestShit.fuckKey2) || config->debug.Lagger || config->lagger.FULLONBOGANFUCKERY)
        return 13; //1200
    return 13;
}



void* __fastcall GetClientNetworkable(void* thisEntityList, void*,  int Index) {

    if (localPlayer.get() && (localPlayer->index() == Index) && Timing::TimingSet_s.bActivatedDueToPingSpike)
        return 0; /* Causes a HOST_ERROR in CL_CopyExistingEntity and CL_CopyNewEntity so LP doesn't update but all other ents do */

    return hooks->entityList.callOriginal<void*, 0, int>(Index);
}


void __fastcall CPrediction__PostNetworkDataReceived(void* thisPrediction, void* edx, int commands_acknowledged) {

    if (Timing::TimingSet_s.m_bInPredictionMode) {
        return;
    }
    else {
        hooks->Predicition.callOriginal<void, 6, int>(commands_acknowledged);
    }
}

void __fastcall CPrediction_PreEntityPacketReceived(void*, void*, int commAck, int currWorldUpdate, int serverTicks) {
    if (Timing::TimingSet_s.m_bInPredictionMode) {
        return;
    }
    else {
        hooks->Predicition.callOriginal<void, 4, int,int,int>(commAck, currWorldUpdate, serverTicks);
    }
}

#include <sstream>
#include <iomanip>

std::string hexStr(BYTE* data, int len)
{
    std::stringstream ss;
    ss << std::hex;

    for (int i(0); i < len; ++i)
        ss << std::setw(2) << std::setfill('0') << (int)data[i] << " ";

    return ss.str();
}
#include "Hacks/OTHER/GOTVConnect.h"
#include "Other/CrossCheatTalk.h"


void PlaceJMP(BYTE* bt_DetourAddress, DWORD dw_FunctionAddress, DWORD dw_Size)
{
    DWORD dw_OldProtection, dw_Distance;
    VirtualProtect(bt_DetourAddress, dw_Size, PAGE_EXECUTE_READWRITE, &dw_OldProtection);
    dw_Distance = (DWORD)(dw_FunctionAddress - (DWORD)bt_DetourAddress) - 5;
    *bt_DetourAddress = 0xE9;
    *(DWORD*)(bt_DetourAddress + 0x1) = dw_Distance;
    for (DWORD i = 0x5; i < dw_Size; i++) *(bt_DetourAddress + i) = 0x90;
    VirtualProtect(bt_DetourAddress, dw_Size, dw_OldProtection, NULL);
    return;
}

__declspec(naked) int NET_ReceiveRawPacket()
{
    __asm
    {
        push ebx //save ebx
        push ecx //save ecx
        push edx //save edx

        mov eax, [esp + (8 + 12)] //from
        mov ebx, [esp + (4 + 12)] //len
        push eax //from
        push ebx //len
        push edx //buf
        push ecx //sock
        call EngineHooks::Hooked_NetReceiveRawPacket
        test al, al
        je dontrun
        //run it!
        pop edx
        pop ecx
        pop ebx
        PUSH EBP
        MOV EBP, ESP
        SUB ESP, 0x2CC
        jmp EngineHooks::NET_ReceiveRawPacketJMPBackAdr
        dontrun :
        //don't run original
        pop edx
            pop ecx
            pop ebx
            xor eax, eax
            ret
    }
}

#include "Other/EnginePrediction.h"
#include "Other/CrossCheatTalk.h"
#include "SteamNetHooks.h"


// 55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 8B 5D 08 56 57 8B F9 89 7C 24 18
// 55 8B EC 81 EC ? ? ? ? 53 8B 5D 08 56 8B 35 ? ? ? ?
// 55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 56 57 8B F9 85 DB 74 1F FF 15 ? ? ? ? 0F B7 53 12 0F B6 4B 03 C1 E2 08 8B 30 0B D1 52 8B C8 FF 56 10 8B F0 EB 05 BE ? ? ? ? 8B D6 B9 ? ? ? ? E8 ? ? ? ? 85 C0 0F 85 ? ? ? ?

LPVOID* oGenericFunc1 = NULL;
LPVOID* oGenericFunc2 = NULL;
LPVOID* oGenericFunc3 = NULL;


#include "SDK/OsirisSDK/KeyValues.h"

typedef bool(__thiscall* IsPausedFunc)(void*);
bool __fastcall CL_IsPaused(void* ecx, void* edx)
{
    static uintptr_t CL_MoveRetAddr = Memory::findPattern_ex(L"engine", "\xF3\x0F\x10\x05????\x84\xC0");
    if (CL_MoveRetAddr == (uintptr_t)_ReturnAddress())
    {
        if (Timing::ExploitTiming.m_nTicksLeftToDrop > 0)
        {
            Timing::ExploitTiming.m_nTicksLeftToDrop--;
            Timing::ExploitTiming.m_LastCmd.hasbeenpredicted = false;
            Timing::ExploitTiming.m_LastCmd.tickCount++;
            Timing::ExploitTiming.m_LastCmd.commandNumber++;
            EnginePrediction::run(&Timing::ExploitTiming.m_LastCmd);
            CON("We Are Paused LOL");
            return true;
        }
    }

    return ((IsPausedFunc)memory->oCL_IsPaused)(ecx);
}


typedef void(__thiscall* pGenericFunction_t)(void*, KeyValues*);
void __fastcall GenericHook(void* ecx, void* edx, KeyValues* a2)
{

    const char* szEventName = a2->GetName();
    if (strstr(szEventName, "OnNetLanConnectionlessPacket"))
    {
        return ((pGenericFunction_t)oGenericFunc1)(ecx, a2);
        VCON("OnNetLanConnectionLessPacket Called From %s\n", (a2->GetString("from")));
    }
    else if (strstr(szEventName, "OnMatchPlayerMgrUpdate"))
    {
        return ((pGenericFunction_t)oGenericFunc1)(ecx, a2);
        VCON("OnMatchPlayerMgrUpdate Called With Update %s from %d\n", a2->GetString("update"), a2->GetInt("xuid"));
    }
    else if (strstr(szEventName, "OnMatchSessionUpdate"))
    {
        return ((pGenericFunction_t)oGenericFunc1)(ecx, a2);
        VCON("OnMatchSessionUpdate Called With State %s\n", a2->GetString("state"));
    }
    else {
        VCON("GenericHook Called With KeyValue %s\n", a2->GetName());
    }




    return ((pGenericFunction_t)oGenericFunc1)(ecx, a2);
   

    return;
}
#include "SDK/OsirisSDK/KeyValues.h"
void __fastcall GenericHook2(void* ecx, void* edx, KeyValues* a2)
{

    //VCON("GenericHook2 Called With KeyValue %s\n", a2->GetName());



        return ((pGenericFunction_t)oGenericFunc2)(ecx, (KeyValues * )a2);

    return;
}
void __fastcall GenericHook3(void* ecx, void* edx, KeyValues* a2)
{
    VCON("GenericHook3 Called With KeyValue %s\n", a2->GetName());

        return ((pGenericFunction_t)oGenericFunc3)(ecx, (KeyValues * )a2);

    return;
}

// 55 8B EC 83 E4 F8 83 EC 44 53 8B 5D 08 
char __stdcall sub_10011610(KeyValues* a2)
{
    VCON("sub_10011610 Called With KeyValue %s\n", a2->GetName());
    return 0;
}


// 55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 57 8B 7D 08 8B D9 89 5C 24 14
int __fastcall sub_10024160(int a1, int a2, KeyValues* a3)
{

    VCON("sub_10024160 Called With KeyValue %s\n", a3->GetName());

    return 0;
}

// 55 8B EC 83 E4 F8 83 EC 14 53 56 8B F1 57 89 74 24 1C
LPVOID* oSub_10027010 = NULL;
typedef int(__thiscall* pGenericFunction)(void*);
int __fastcall sub_10027010(DWORD* _this, void*)
{

    //CON("sub_10027010\n");

        return ((pGenericFunction)oSub_10027010)(_this);


    return 0;
}



#define QUICKHOOK(HookFunction, Addr) LPVOID* fn = MemoryTooks::findPat; 

void QuickHook(const wchar_t* dll, const char* pattern, void* pFunction, LPVOID** ppOriginal)
{
    LPVOID* HookFunction = (LPVOID*)Memory::findPattern_ex(dll, pattern);
    //LPVOID* oHookFunction;
    if (MH_CreateHook((LPVOID*)(HookFunction), pFunction, (LPVOID*)(ppOriginal)) != MH_OK) { MH_DisableHook((LPVOID*)HookFunction); }


    

}


char* __fastcall sub_10028A10(void** _this, void*, int a2, int a3, int a4, int a5)
{
    CON("sub_10028A10\n");

    return 0;
}

void InstallTMPHooks()
{
#if 1 // IP Grab Stuff
    LPVOID* oFunc;
    QuickHook(L"matchmaking", "\x55\x8B\xEC\x83\xE4\xF8\x81\xEC????\x53\x8B\x5D\x08\x56\x57\x8B\xF9\x89\x7C\x24\x18", &GenericHook, (LPVOID**)&oGenericFunc1);
    QuickHook(L"matchmaking", "\x55\x8B\xEC\x81\xEC????\x53\x8B\x5D\x08\x56\x8B\x35????", &GenericHook2, (LPVOID**)&oGenericFunc2);
    QuickHook(L"matchmaking", "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x0C\x53\x8B\x5D\x08\x56\x57\x8B\xF9\x85\xDB\x74\x1F\xFF\x15????\x0F\xB7\x53\x12\x0F\xB6\x4B\x03\xC1\xE2\x08\x8B\x30\x0B\xD1\x52\x8B\xC8\xFF\x56\x10\x8B\xF0\xEB\x05\xBE????\x8B\xD6\xB9????\xE8????\x85\xC0\x0F\x85????", &GenericHook3, (LPVOID**)&oGenericFunc3);
    //QuickHook(L"matchmaking", "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x44\x53\x8B\x5D\x08", &sub_10011610, &oFunc);
    //QuickHook(L"matchmaking", "\x55\x8B\xEC\x83\xE4\xF8\x81\xEC????\x53\x56\x57\x8B\x7D\x08\x8B\xD9\x89\x5C\x24\x14", &sub_10024160, &oFunc);
    //QuickHook(L"matchmaking", "\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x57\x8B\x7D\x08\x8B\xD9", &sub_10028A10, &oFunc);
    //QuickHook(L"matchmaking", "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x14\x53\x56\x8B\xF1\x57\x89\x74\x24\x1C", &sub_10027010, &oSub_10027010);
#endif
}

void* __fastcall sub_100260B0(void** _this, void*, int a2, int a3, int a4, int a5)
{
    CON("sub_100260B0");
    return nullptr;
}

typedef int(__thiscall* pGenericFunction_tt)(void*, int);
int __fastcall sub_10025E30(DWORD* _this, void*, int a2)
{

    CON("sub_10025E30");


    return 0;
}
//
bool g_bAntiIP = false;
void* g_pSendToJumpBackAddress = NULL; // Write 8 bytes NOP, then place jump
void* g_pSendToJumpBackAddress2 = NULL; // Write 5 bytes NOP, then place jump
void* g_pFindAddressFunc = NULL;
__declspec(naked) void Anti_IP_Grabber(void)
{
    __asm {
        cmp g_bAntiIP, 1
        je return_to_function
        call ds:sendto
return_to_function:
        test    eax, eax
        jmp g_pSendToJumpBackAddress
    };
}

__declspec(naked) void Anti_IP_Grabber2(void)
{
    __asm {
        cmp g_bAntiIP, 1
        je set_zero
        call g_pFindAddressFunc
        jmp return_to_function
set_zero:
        mov eax, 0
return_to_function:
        jmp g_pSendToJumpBackAddress2
    };
}
/*
uintptr_t pSendToCall = MemoryTools::FindPattern("engine", "\xFF\x15????\x85\xC0\x79\x63", "pSendToCall");
g_pSendToJumpBackAddress = (void*)MemoryTools::FindPattern("engine", "\x79\x63\xFF\x15????", "g_pSendToJumpBackAddress");
uintptr_t pFindSteamAddressCall = MemoryTools::FindPattern("engine", "\x56\xE8????\x8B\xF8\x85\xFF\x0F\x84????", "pFindSteamAddressCall");
g_pSendToJumpBackAddress2 = (void*)MemoryTools::FindPattern("engine", "\x8B\xF8\x85\xFF\x0F\x84????\x8B\x15????", "g_pSendToJumpBackAddress2");
g_pFindAddressFunc = (void*)MemoryTools::FindPattern("engine", "\x56\xE8????\x8B\xF8\x85\xFF\x0F\x84????", "g_pFindAddressFunc");
*/
bool g_bRenderFrame = true;
void* g_poHostRunFrameRender = NULL;
void __declspec(naked) _Host_RunFrame_Render(/*ecx, xmm0*/)
{
    _asm {
        cmp g_bRenderFrame, 1
        jne return_back
        call g_poHostRunFrameRender
return_back:
        retn
    }
}

typedef LPVOID(__stdcall* LoadLibraryExWFunc_t)(wchar_t*, HANDLE, DWORD);
LPVOID __stdcall ValveHookedLoadLibraryExW(wchar_t* Src, HANDLE hFile, DWORD dwFlags)
{
    // 55 8B EC 51 56 8B 75 08 8D 45 FC
    static LoadLibraryExWFunc_t** oLoadLibraryExW = reinterpret_cast<LoadLibraryExWFunc_t**>((char*)Memory::findPattern_ex(L"csgo.exe", "\x56\xFF\x15????\x5E\x8B\xE5") + int(3));
    if (Src)
    {
        std::wstring str(Src);
        std::string cstr(str.begin(), str.end());
        VCON("The Following DLL Was Loaded Through LoadLibraryExW : %s\n", cstr.c_str());
    }
    static LoadLibraryExWFunc_t FuncToCall = **oLoadLibraryExW;
    return (**oLoadLibraryExW)(Src, hFile, dwFlags);
}

LPVOID oValveAntiCheatFunc = NULL;
typedef char(__cdecl* ValveACFuc)(char, int, int, int , int);
char __cdecl ValveAntiCheatFuc(char a1, int a2, int a3, int a4, int a5)
{
    // E8 ? ? ? ? 8B 4D FC 83 C4 18 C7 45 ? ? ? ? ?
    VCON("Called Valve AC Function!\n");
    static char* g_pNumTimesSentAddr{ ((char*)Memory::findPattern_ex(L"client", "\x0F\x85????\xFF\x05????") + int(8)) };
    int* pNumTimesSent = *(int**)(g_pNumTimesSentAddr);
    (*pNumTimesSent)++;

    return ((ValveACFuc)oValveAntiCheatFunc)(a1, a2, a3, a4, a5);
}


LPVOID oClientReportValidationDeconstructor = NULL;
typedef int(__thiscall* ClientReportValidationDeconstructor)(void*);
int __fastcall ClientReportValidationDecontructor(void* _this, void*)
{
    // E8 ? ? ? ? 83 7C 24 ? ? 7C 15 8B 54 24 58
    VCON("ClientReportValidationDecontructor\n");

    return ((ClientReportValidationDeconstructor)oClientReportValidationDeconstructor)(_this);
}


template <typename T>
static constexpr auto _relativeToAbsolute(uintptr_t address) noexcept
{
    return (T)(address + 4 + *reinterpret_cast<std::int32_t*>(address));
}



static void __fastcall updateColorCorrectionWeights(void* thisptr, void*) noexcept
{
    hooks->clientMode.callOriginal<void,58>();

    const auto& cfg = config->visuals.colorCorrection;
    if (cfg.enabled) {
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + ( 0x498 )) = cfg.blue;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + ( 0x4A0 )) = cfg.red;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + ( 0x4A8 )) = cfg.mono;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + ( 0x4B0 )) = cfg.saturation;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + ( 0x4C0 )) = cfg.ghost;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + ( 0x4C8 )) = cfg.green;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + ( 0x4D0 )) = cfg.yellow;
    }
}

LPVOID oCNetChanProcessPacket;
typedef void(__thiscall* CNetChan_ProcessPacketFunc_t)(NetworkChannel*, netpacket_s*, bool);
void __fastcall hk_CNetChan_ProcessPacket(NetworkChannel* pChan, void*, netpacket_s* packet, bool bHasHeader)
{
    static CNetChan_ProcessPacketFunc_t ProcessPacket{ reinterpret_cast<CNetChan_ProcessPacketFunc_t>(oCNetChanProcessPacket) };
    ProcessPacket(pChan, packet, bHasHeader);

    EventInfo* pEvent = memory->clientState->pEvents;
    while (pEvent != nullptr) {
        if (!pEvent->m_client_class)
            continue;
        pEvent->fire_delay = 0.f;
        pEvent = pEvent->m_next;
    }

    interfaces->engine->FireEvents();
}


void Hooks::install() noexcept {
    //SkinChanger::initializeKits();

    originalPresent = **reinterpret_cast<decltype(originalPresent)**>(memory->present);
    **reinterpret_cast<decltype(present)***>(memory->present) = present;
    originalReset = **reinterpret_cast<decltype(originalReset)**>(memory->reset);
    **reinterpret_cast<decltype(reset)***>(memory->reset) = reset;

    if constexpr (std::is_same_v<HookType, MinHook>) {
        if (MH_Initialize() != MH_OK)
            throw std::exception();
    }


    panel.init(interfaces->panel);
    clientMode.init(memory->clientMode);
    client.init(interfaces->client);
    svCheats.init(interfaces->cvar->findVar("sv_cheats"));
    hclientstate.init((DWORD**)((DWORD)memory->cclientstate + 8));
    sv_netMaxRoutable.init(interfaces->cvar->findVar("sv_maxroutable"));
    net_maxroutable.init(interfaces->cvar->findVar("net_maxroutable"));

    engine.init(interfaces->engine);
    modelRender.init(interfaces->modelRender);
    bspQuery.init(interfaces->engine->getBSPTreeQuery());
    sound.init(interfaces->sound);

    if (!interfaces->surface) {
        Debug::QuickPrint("No Surface Module");
    }

    surface.init(interfaces->surface);

    viewRender.init(memory->viewRender);
    renderView.init(interfaces->renderView);
    studioRender.init(interfaces->studioRender);
    materialSystem.init(interfaces->materialSystem);
    fileSystem.init(interfaces->fileSystem);
    entityList.init(interfaces->entityList);
    Predicition.init(interfaces->prediction);


    OurSteamWorks::Init();
    CrossCheatTalk::Init();


    //entityList.hookAt(0, GetClientNetworkable);
    Predicition.hookAt(6, CPrediction__PostNetworkDataReceived);
    Predicition.hookAt(4, CPrediction_PreEntityPacketReceived);
    Predicition.hookAt(19, EnginePrediction::CPred_RunCommand);
    /*







   gamemovement.init(interfaces->gameMovement);
   */
   //clMove.init(memory->CL_MoveCall);

   //clientState.init(memory->clientState);
   //clientState.hookAt(63, EngineHooks::SetReservationCookie);

    Debug::QuickPrint("Ok\nHooks::install clientMode   :: VTable :: Hooking createMove (24)...", false);
    clientMode.hookAt(24, ClientModeHooks::createMove);
#ifndef LIGHT_HOOKS
    Debug::QuickPrint(" - Initializing Hooks - ");
    Debug::QuickPrint("Hooks::install bspQuery     :: VTable :: Hooking listLeavesInBox (6)...", false);

    bspQuery.hookAt(6, bspQueryHooks::listLeavesInBox);
    Debug::QuickPrint("Ok\nHooks::install client       :: VTable :: Hooking frameStageNotify (37)...", false);
    client.hookAt(37, ClientHooks::frameStageNotify);
    //clientMode.hookAt(17, shouldDrawFog);
    Debug::QuickPrint("Ok\nHooks::install client       :: VTable :: Hooking writeUsercmdDeltaToBuffer (24)...", false);
    client.hookAt(24, ClientHooks::WriteUsercmdDeltaToBuffer);

    Debug::QuickPrint("Ok\nHooks::install client       :: VTable :: Hooking DispatchUserMessage (36)...", false);
    client.hookAt(38, ClientHooks::DispatchUserMessage);

    Debug::QuickPrint("Ok\nHooks::install clientMode   :: VTable :: Hooking overrideView (18)...", false);
    clientMode.hookAt(18, ClientModeHooks::overrideView);


    engine.hookAt(114, ClientCmdUnRestricted);
    //engine.hookAt(99, ExecuteClientCmd);
    //engine.hookAt(6, ServerCmd);
    engine.hookAt(7, ClientCmd);
    //clientMode.hookAt(7, ClientModeClientCmd);




    viewRender.hookAt(41, viewRenderHooks::RenderSmokeOverlay);

    //clientMode.hookAt(27, shouldDrawViewModel);
    //clientMode.hookAt(35, getViewModelFov);
    Debug::QuickPrint("Ok\nHooks::install clientMode   :: VTable :: Hooking doPostScreenEffects (44)...", false);
    clientMode.hookAt(44, ClientModeHooks::doPostScreenEffects);


    clientMode.hookAt(58, updateColorCorrectionWeights);



    //engine.hookAt(82, isPlayingDemo);
    //engine.hookAt(101, getScreenAspectRatio);
    //engine.hookAt(218, getDemoPlaybackParameters);
    //clMove.hook(CL_Move);
    Debug::QuickPrint("Ok\nHooks::install modelRender  :: VTable :: Hooking drawModelExecute (21)...", false);
    modelRender.hookAt(21, modelRenderHooks::drawModelExecute);
    //#ifdef _DEBUG

        //
        //studioRender.hookAt(29, studioRenderHooks::DrawModel);
        /* Okay apparently this shit doesnt work so i'll do it manually....*/
#if 1
    Debug::QuickPrint("Ok\nHooks::install studioRender :: VTable :: Hooking drawModel (29)...", false);
    if (MH_CreateHook((LPVOID)((*reinterpret_cast<void***>(interfaces->studioRender))[29]), &studioRenderHooks::DrawModel,
        reinterpret_cast<LPVOID*>(&(g_pChamsRenderer->oDrawModel))) != MH_OK) {
        Debug::QuickWarning("Draw Model Failed To Hook!");

    }
#endif





    Debug::QuickPrint("Ok\nHooks::install studioRender :: VTable :: Hooking BeginFrame (8)...", false);
    studioRender.hookAt(8, ESP::BeginFrame);


#endif
    //Debug::QuickPrint("Ok\nHooks::install studioRender :: VTable :: Hooking drawModelStaticProp (30)...", false);
    //studioRender.hookAt(30, studioRenderHooks::drawModelStaticProp);


    //Debug::QuickPrint("Ok\nHooks::install studioRender :: VTable :: Hooking drawModelArrayStaticProp (47)...", false);
    //studioRender.hookAt(47, studioRenderHooks::drawModelArrayStaticProp);

    Debug::QuickPrint("Ok\nHooks::install materialSys  :: VTable :: Hooking findMaterial (84)...", false);
    materialSystem.hookAt(84, materialSystemHooks::findMaterial);

    Debug::QuickPrint("Ok\nHooks::install materialSys  :: VTable :: Hooking reloadMaterials (82)...", false);
    materialSystem.hookAt(82, materialSystemHooks::reloadMaterials);

    //#endif
    Debug::QuickPrint("Ok\nHooks::install Panel        :: VTable :: Hooking paintTraverse (41)...", false);
    panel.hookAt(41, PanelHooks::paintTraverse);

    sound.hookAt(5, emitSound);


    Debug::QuickPrint("Ok\nHooks::install Surface      :: VTable :: Hooking setDrawColor (15)...", false);
    surface.hookAt(15, SurfaceHooks::setDrawColor);

    Debug::QuickPrint("Ok\nHooks::install Surface      :: VTable :: Hooking lockCursor (67)...", false);
    surface.hookAt(67, Menu::hk_lockCursor);
    Debug::QuickPrint("Ok\nHooks::install SvCheats     :: VTable :: Hooking svCheats (13)...", false);
    svCheats.hookAt(13, SvHooks::svCheatsGetBool);

    net_maxroutable.hookAt(13, net_routable_int);
    sv_netMaxRoutable.hookAt(13, sv_netroutable_int);

    Debug::QuickPrint("Ok\nHooks::install fileSystem   :: VTable :: Hooking allowLooseFiles (13)...", false);
    fileSystem.hookAt(13, fileSystemHooks::allowLooseFiles);

    Debug::QuickPrint("Ok\nHooks::install fileSystem   :: VTable :: Hooking canLoadThirdPartyFiles (128)...", false);
    fileSystem.hookAt(128, fileSystemHooks::canLoadThirdPartyFiles);

    Debug::QuickPrint("Ok\nHooks::install fileSystem   :: VTable :: Hooking getUnveridiedFileHashes (101)...", false);
    fileSystem.hookAt(101, fileSystemHooks::getUnveridiedFileHashes);


    Debug::QuickPrint("Ok\nHooks::install engine       :: VTable :: Hooking isHLTV (93)...", false);
    engine.hookAt(93, isHltv);

    hclientstate.hookAt(24, CrossCheatTalk::SVCMsg_VoiceData);


    Debug::QuickPrint("Ok\nHooks::install RenderView   :: VTable :: Hooking SceneEnd (9)...", false);
    renderView.hookAt(9, RenderViewHooks::SceneEnd);

    Debug::QuickPrint("Ok\nHooks::install RenderView   :: VTable :: Hooking DrawLights (21)...", false);
    renderView.hookAt(21, RenderViewHooks::DrawLights);

    //Debug::QuickPrint("Ok\nHooks::install ViewRender   :: VTable :: Hooking Render2DEffectsPostHUD (39)...", false);
    //viewRender.hookAt(39, RenderViewHooks::Render2DEffectsPostHUD);

    Debug::QuickPrint("Ok\nHooks::install ViewRender   :: VTable :: Hooking DisableFog (37)...", false);
    renderView.hookAt(37, viewRenderHooks::DisableFog);

   // gamemovement.hookAt(1, procmovement_hk);

    if (DWORD oldProtection; VirtualProtect(memory->dispatchSound, 4, PAGE_EXECUTE_READWRITE, &oldProtection)) {
        originalDispatchSound = decltype(originalDispatchSound)(uintptr_t(memory->dispatchSound + 1) + *memory->dispatchSound);
        *memory->dispatchSound = uintptr_t(dispatchSound) - uintptr_t(memory->dispatchSound + 1);
        VirtualProtect(memory->dispatchSound, 4, oldProtection, nullptr);
    }

#ifndef LIGHT_HOOKS
    if (MH_CreateHook(reinterpret_cast<LPVOID*>(memory->HOST_RUN_FRAME_INPUT), &Timing::_Host_RunFrame_Input_hk,
        reinterpret_cast<LPVOID*>(&oHOSTRUNFRAMEINPUT)) != MH_OK) {
        Debug::QuickPrint("Error On Creation Of _HOST_RUN_FRAME_INPUT Hook\n", false);
        if (MH_DisableHook(reinterpret_cast<LPVOID*>(memory->HOST_RUN_FRAME_INPUT)) != MH_OK)
        {
            Debug::QuickPrint("Error On Disabling Of _HOST_RUN_FRAME_INPUT Hook\n", false);
        }
        else {
            Debug::QuickPrint("Successfully Disabled _HOST_RUN_FRAME_INPUT Hook\n", false);
        }
    }
    else {
        Debug::QuickPrint("Ok\nSuccessfully Created _HOST_RUN_FRAME_INPUT Hook\n", false);
    }



#if 0
    Debug::QuickPrint("Ok\nHooks::install Engine       :: StaticFunction :: Hooking CL_MOVE...", false);
    if (MH_CreateHook(memory->CL_MoveCall, &EngineHooks::CL_Move,
        reinterpret_cast<LPVOID*>(&oCL_Move)) != MH_OK) {
        Debug::QuickPrint("Error On Creation Of CL_MOVE Hook\n", false);
        if (MH_DisableHook(memory->CL_MoveCall) != MH_OK)
        {
            Debug::QuickPrint("Error On Disabling Of CL_MOVE Hook\n", false);
        }
        else {
            Debug::QuickPrint("Successfully Disabled CL_MOVE Hook\n", false);
        }
    }
    else {
        Debug::QuickPrint("Ok\nSuccessfully Created CL_MOVE Hook\n", false);
    }

#endif


















    /*
    *     Debug::QuickPrint("Ok\nHooks::install Client      :: StaticFunction :: Hooking RenderModels...", false);
    if (MH_CreateHook(reinterpret_cast<LPVOID*>(memory->RenderModels), &ClientHooks::RenderModels,
        reinterpret_cast<LPVOID*>(&oRenderModels)) != MH_OK) {
        Debug::QuickPrint("Error On Creation Of RenderModels Hook\n", false);
        if (MH_DisableHook(reinterpret_cast<LPVOID*>(memory->RenderModels)) != MH_OK)
        {
            Debug::QuickPrint("Error On Disabling Of RenderModels Hook\n", false);
        }
        else {
            Debug::QuickPrint("Successfully Disabled RenderModels Hook\n", false);
        }
    }
    else {
        Debug::QuickPrint("Ok\nSuccessfully Created RenderModels Hook\n", false);
    }

    if (!oRenderModels)
        Debug::QuickPrint("You Gonna Crash Son");
        */

        /*
        Debug::QuickPrint("Ok\nHooks::install Client      :: StaticFunction :: Hooking DrawWorld...", false);

        if (MH_CreateHook(reinterpret_cast<LPVOID*>(memory->DrawWorld), &ClientHooks::DrawWorld,
            reinterpret_cast<LPVOID*>(&oDrawWorld)) != MH_OK) {
            Debug::QuickPrint("Error On Creation Of DrawWorld Hook\n", false);
            if (MH_DisableHook(reinterpret_cast<LPVOID*>(memory->DrawWorld)) != MH_OK)
            {
                Debug::QuickPrint("Error On Disabling Of DrawWorld Hook\n", false);
            }
            else {
                Debug::QuickPrint("Successfully Disabled DrawWorld Hook\n", false);
            }
        }
        else {
            Debug::QuickPrint("Ok\nSuccessfully Created DrawWorld Hook\n", false);
        }
        */
    if (MH_CreateHook(reinterpret_cast<LPVOID*>(memory->ProceduralFootPlant), &doProceduralFootPlant,
        reinterpret_cast<LPVOID*>(&oProceduralFootPlant)) != MH_OK) {
        Debug::QuickPrint("Error On Creation Of ProceduralFootPlant Hook\n", false);
        if (MH_DisableHook(reinterpret_cast<LPVOID*>(memory->ProceduralFootPlant)) != MH_OK)
        {
            Debug::QuickPrint("Error On Disabling Of ProceduralFootPlant Hook\n", false);
        }
        else {
            Debug::QuickPrint("Successfully Disabled ProceduralFootPlant Hook\n", false);
        }
    }
    else {
        Debug::QuickPrint("Ok\nSuccessfully Created ProceduralFootPlant Hook\n", false);
    }

    if (MH_CreateHook(reinterpret_cast<LPVOID*>(memory->ShouldSkipAnimationFrame), &shouldSkipAnimationFrame,
        reinterpret_cast<LPVOID*>(&oShouldSkipAnimationFrame)) != MH_OK) {
        Debug::QuickPrint("Error On Creation Of ShouldSkipAnimationFrame Hook\n", false);
        if (MH_DisableHook(reinterpret_cast<LPVOID*>(memory->ShouldSkipAnimationFrame)) != MH_OK)
        {
            Debug::QuickPrint("Error On Disabling Of ShouldSkipAnimationFrame Hook\n", false);
        }
        else {
            Debug::QuickPrint("Successfully Disabled ShouldSkipAnimationFrame Hook\n", false);
        }
    }
    else {
        Debug::QuickPrint("Ok\nSuccessfully Created ShouldSkipAnimationFrame Hook\n", false);
    }


    if (MH_CreateHook(reinterpret_cast<LPVOID*>(memory->CheckForSequenceChange), &checkForSequenceChange,
        reinterpret_cast<LPVOID*>(&oCheckForSequenceChange)) != MH_OK) {
        Debug::QuickPrint("Error On Creation Of CheckForSequenceChange Hook\n", false);
        if (MH_DisableHook(reinterpret_cast<LPVOID*>(memory->CheckForSequenceChange)) != MH_OK)
        {
            Debug::QuickPrint("Error On Disabling Of CheckForSequenceChange Hook\n", false);
        }
        else {
            Debug::QuickPrint("Successfully Disabled CheckForSequenceChange Hook\n", false);
        }
    }
    else {
        Debug::QuickPrint("Ok\nSuccessfully Created CheckForSequenceChange Hook\n", false);
    }

    /*
    if (MH_CreateHook(reinterpret_cast<LPVOID*>(memory->NET_BufferToBufferCompress), &EngineHooks::NET_BufferToBufferCompress,
        reinterpret_cast<LPVOID*>(&oNET_BufferToBufferCompress)) != MH_OK) {
        Debug::QuickPrint("Error On Creation Of NET_BufferToBufferCompress Hook\n", false);
        if (MH_DisableHook(reinterpret_cast<LPVOID*>(memory->NET_BufferToBufferCompress)) != MH_OK)
        {
            Debug::QuickPrint("Error On Disabling Of NET_BufferToBufferCompress Hook\n", false);
        }
        else {
            Debug::QuickPrint("Successfully Disabled NET_BufferToBufferCompress Hook\n", false);
        }
    }
    else {
        Debug::QuickPrint("Ok\nSuccessfully Created NET_BufferToBufferCompress Hook\n", false);
    }
    */
#if 1
    if (MH_CreateHook(reinterpret_cast<LPVOID*>(memory->CNetChan_CompressFragments), &EngineHooks::CNET_CompressFragments,
        reinterpret_cast<LPVOID*>(&oCNET_CompressFragments)) != MH_OK) {
        Debug::QuickPrint("Error On Creation Of CNetChan_CompressFragments Hook\n", false);
        if (MH_DisableHook(reinterpret_cast<LPVOID*>(memory->CNetChan_CompressFragments)) != MH_OK)
        {
            Debug::QuickPrint("Error On Disabling Of CNetChan_CompressFragments Hook\n", false);
        }
        else {
            Debug::QuickPrint("Successfully Disabled CNetChan_CompressFragments Hook\n", false);
        }
    }
    else {


        Debug::QuickPrint("Ok\nSuccessfully Created CNetChan_CompressFragments Hook\n", false);
    }
#endif

    /*
    if (MH_CreateHook(reinterpret_cast<LPVOID*>(memory->CNetChan_CreateFragmentsFromBuffer), &EngineHooks::CNET_CreateFragmentsFromBuffer,
        reinterpret_cast<LPVOID*>(&oCNET_CreateFragmentsFromBuffer)) != MH_OK) {
        Debug::QuickPrint("Error On Creation Of CNetChan_CreateFragmentsFromBuffer Hook\n", false);
        if (MH_DisableHook(reinterpret_cast<LPVOID*>(memory->CNetChan_CreateFragmentsFromBuffer)) != MH_OK)
        {
            Debug::QuickPrint("Error On Disabling Of CNetChan_CreateFragmentsFromBuffer Hook\n", false);
        }
        else {
            Debug::QuickPrint("Successfully Disabled CNetChan_CreateFragmentsFromBuffer Hook\n", false);
        }
    }
    else {
        Debug::QuickPrint("Ok\nSuccessfully Created CNetChan_CreateFragmentsFromBuffer Hook\n", false);
    }
    */

#if 1
    if (MH_CreateHook(reinterpret_cast<LPVOID*>(memory->CNetChan_SendSubChannelData), &EngineHooks::CNET_SendSubChannelData,
        reinterpret_cast<LPVOID*>(&oCNET_SendSubChannelData)) != MH_OK) {
        Debug::QuickPrint("Error On Creation Of CNetChan_SendSubChannelData Hook\n", false);
        if (MH_DisableHook(reinterpret_cast<LPVOID*>(memory->CNetChan_SendSubChannelData)) != MH_OK)
        {
            Debug::QuickPrint("Error On Disabling Of CNetChan_SendSubChannelData Hook\n", false);
        }
        else {
            Debug::QuickPrint("Successfully Disabled CNetChan_SendSubChannelData Hook\n", false);
        }
    }
    else {
        Debug::QuickPrint("Ok\nSuccessfully Created CNetChan_SendSubChannelData Hook\n", false);
    }
#endif
#endif   
    /*
    if (MH_CreateHook(reinterpret_cast<LPVOID*>(memory->CBaseClient_GetSendFrame), &EngineHooks::CGameClient_GetSendFrame,
        reinterpret_cast<LPVOID*>(&oCBaseClient_GetSendFrame)) != MH_OK) {
        Debug::QuickPrint("Error On Creation Of CBaseClient_GetSendFrame Hook\n", false);
        if (MH_DisableHook(reinterpret_cast<LPVOID*>(memory->CBaseClient_GetSendFrame)) != MH_OK)
        {
            Debug::QuickPrint("Error On Disabling Of CBaseClient_GetSendFrame Hook\n", false);
        }
        else {
            Debug::QuickPrint("Successfully Disabled CBaseClient_GetSendFrame Hook\n", false);
        }
    }
    else {
        Debug::QuickPrint("Ok\nSuccessfully Created CBaseClient_GetSendFrame Hook\n", false);
    }
    */


#ifdef NET_SENDTOIMPHOOK
    if (MH_CreateHook(reinterpret_cast<LPVOID*>(memory->NET_SendToImpl), &EngineHooks::NET_SendToImpl,
        reinterpret_cast<LPVOID*>(&oNET_SendToImpl)) != MH_OK) {
        Debug::QuickPrint("Error On Creation Of NET_SendToImpl Hook\n", false);
        if (MH_DisableHook(reinterpret_cast<LPVOID*>(memory->NET_SendToImpl)) != MH_OK)
        {
            Debug::QuickPrint("Error On Disabling Of NET_SendToImpl Hook\n", false);
        }
        else {
            Debug::QuickPrint("Successfully Disabled NET_SendToImpl Hook\n", false);
        }
    }
    else {
        Debug::QuickPrint("Ok\nSuccessfully Created NET_SendToImpl Hook\n", false);
    }
#endif
    if (MH_CreateHook(reinterpret_cast<LPVOID*>(memory->CNetChan_SendNetMessage), &ClientHooks::SendNetMessage,
        reinterpret_cast<LPVOID*>(&oCNET_SendNetMessage)) != MH_OK) {
        Debug::QuickPrint("Error On Creation Of CNetChan_SendNetMessage Hook\n", false);
        if (MH_DisableHook(reinterpret_cast<LPVOID*>(memory->CNetChan_SendNetMessage)) != MH_OK)
        {
            Debug::QuickPrint("Error On Disabling Of CNetChan_SendNetMessage Hook\n", false);
        }
        else {
            Debug::QuickPrint("Successfully Disabled CNetChan_SendNetMessage Hook\n", false);
        }
    }
    else {
        Debug::QuickPrint("Ok\nSuccessfully Created CNetChan_SendNetMessage Hook\n", false);
    }

    LPVOID CNetChanProcessPackets = (LPVOID)memory->findPattern_ex(L"engine", "\x55\x8B\xEC\x83\xE4\xC0\x81\xEC????\x53\x56\x57\x8B\x7D\x08\x8B\xD9");
    MH_CreateHook(CNetChanProcessPackets, hk_CNetChan_ProcessPacket, &oCNetChanProcessPacket);


    /*
    if (MH_CreateHook(reinterpret_cast<LPVOID*>(memory->SendClientMessages), &EngineHooks::SendClientMessages,
        reinterpret_cast<LPVOID*>(&oSendClientMessages)) != MH_OK) {
        Debug::QuickPrint("Error On Creation Of SendClientMessages Hook\n", false);
        if (MH_DisableHook(reinterpret_cast<LPVOID*>(memory->SendClientMessages)) != MH_OK)
        {
            Debug::QuickPrint("Error On Disabling Of SendClientMessages Hook\n", false);
        }
        else {
            Debug::QuickPrint("Successfully Disabled SendClientMessages Hook\n", false);
        }
    }
    else {
        Debug::QuickPrint("Ok\nSuccessfully Created SendClientMessages Hook\n", false);
    }
    */
    /*
    {
        int Ret = MH_CreateHook(reinterpret_cast<LPVOID*>(memory->CL_SendMove_), &EngineHooks::CL_SendMove,
            reinterpret_cast<LPVOID*>(&oCL_SendMove));
        if (Ret != MH_OK) {
            Debug::QuickPrint("Error On Creation Of CL_SendMove Hook\n", false);
            Debug::QuickPrint(std::to_string(Ret).c_str());
            if (MH_DisableHook(reinterpret_cast<LPVOID*>(memory->CL_SendMove_)) != MH_OK)
            {
                Debug::QuickPrint("Error On Disabling Of CL_SendMove Hook\n", false);
            }
            else {
                Debug::QuickPrint("Successfully Disabled CL_SendMove Hook\n", false);
            }
        }
        else {
            Debug::QuickPrint("Ok\nSuccessfully Created CL_SendMove Hook\n", false);
        }

    }
    */

    {
        int Ret = MH_CreateHook(reinterpret_cast<LPVOID*>(memory->HOST_ERROR), &HOST_ERROR_HK,
            reinterpret_cast<LPVOID*>(&oHOST_ERROR));
        if (Ret != MH_OK) {
            Debug::QuickPrint("Error On Creation Of HOST_ERROR Hook\n", false);
            Debug::QuickPrint(std::to_string(Ret).c_str());
            if (MH_DisableHook(reinterpret_cast<LPVOID*>(memory->HOST_ERROR)) != MH_OK)
            {
                Debug::QuickPrint("Error On Disabling Of HOST_ERROR Hook\n", false);
            }
            else {
                Debug::QuickPrint("Successfully Disabled HOST_ERROR Hook\n", false);
            }
        }
        else {
            Debug::QuickPrint("Ok\nSuccessfully Created HOST_ERROR Hook\n", false);
        }
    }
    HOOKFUNCTION(EnqueVeryLargeAsyncTransfer, EngineHooks::EnqueueVeryLargeAsyncTransfer);
#ifdef LOCAL_TIMING
#ifndef FUCKYSHIT

#endif
#endif 
#ifndef FUCKYSHIT

    HOOKFUNCTION(NET_TickMsgHandler, Timing::CClientState_NETMSG_Tick);
    // HOOKFUNCTION(CL_PreprocessEntities, Timing::CL_PreprocessEntities);

    HOOKFUNCTION(ProcessConnectionless, EngineHooks::ProcessConnectionless);
    HOOKFUNCTION(SVCMSG_PacketEntities, Timing::CClientState_SVCMsg_PacketEntities)
    HOOKFUNCTION(CBaseClientState_SVCMsg_PacketEntities, Timing::CBaseClientState_SVCMsg_PacketEntities);
#endif
#ifdef ALLOW_PLAYER_UPDATES
    HOOKFUNCTION(CL_CopyNewEntity, Timing::CL_CopyNewEntity);
    HOOKFUNCTION(CL_CopyExistingEntity, Timing::CL_CopyExistingEntity);
#endif

#ifdef RCE_EXPLOIT















    // HOOKFUNCTION(ClampDataWriteValue, SteamNetHooks::ClampDataWriteValue);
#endif

    // HOOKFUNCTION(CSteamNetworkConnectionBase__SNP_ClampSendRate, OurSteamWorks::CSteamNetworkConnectionBase__SNP_ClampSendRate);
    HOOKFUNCTION(SNP_GetNextThinkTime, OurSteamWorks::SNP_GetNextThinkTime);
    // HOOKFUNCTION(ConnectionProblemLocally, ConnectionState_ProblemDetectedLocally);
    // HOOKFUNCTION(NET_SendPacket, EngineHooks::NET_SendPacket);

    HOOKFUNCTION(NET_SendLong, EngineHooks::NET_SendLong);
    HOOKFUNCTION(CBaseClientState_HandleDeferredConnection, HandleDeferredConnection);
    HOOKFUNCTION(CSteam3Client_OnGameServerChangeRequested, GOTVConnect::CSteam3Client_OnGameServerChangeRequested);
    // HOOKFUNCTION(CNetChan_SendDatagram, EngineHooks::SendDatagram);
    // HOOKFUNCTION(Calls_SendToImpl, EngineHooks::Calls_SendToImpl)
    // HOOKFUNCTION(NET_CreateNetChannel, EngineHooks::NET_CreateNetChannel);

    HOOKFUNCTION(CL_FlushEntityPacket, CL_FlushEntityPacket);
    HOOKFUNCTION(CBaseClientState_SendConnectPacket, GOTVConnect::CBaseClientState_SendConnectPacket);
    HOOKFUNCTION(WatchUserInfoProcess, GOTVConnect::UnknownFunction);
    HOOKFUNCTION(UnknownGOTVFunction2, GOTVConnect::UnknownFunction2);
    HOOKFUNCTION(CPrediction_RunSimulation, EnginePrediction::CPred_RunSimulation);

    HOOKFUNCTION(SNP_SendMessage, SteamNetHooks::CSteamNetworkConnectionBase__SNP_SendMessage)
    HOOKFUNCTION(RenderView, viewRenderHooks::RenderView);
    HOOKFUNCTION(DrawUnderWaterOverlay, viewRenderHooks::DrawUnderWaterOverlay);
    HOOKFUNCTION(UnknownViewRenderFunc, viewRenderHooks::UnknownViewRenderFunction);
    HOOKFUNCTION(UnknownViewRenderFunc2, viewRenderHooks::UnknownViewRenderFunction2);
    HOOKFUNCTION(HostRunFrameRender, _Host_RunFrame_Render);
    HOOKFUNCTION(PerformScreenOverlay, viewRenderHooks::PerformScreenOverlay);
    HOOKFUNCTION(UpdateToneMapScalar, viewRenderHooks::UpdateToneMapScalar);
    g_poHostRunFrameRender = memory->oHostRunFrameRender;

    LPVOID ValveAntiCheatFuncAddr =_relativeToAbsolute<LPVOID>(Memory::findPattern_ex(L"client", "\xE8????\x8B\x4D\xFC\x83\xC4\x18\xC7\x45?????") + 1);

    MH_CreateHook(ValveAntiCheatFuncAddr, &ValveAntiCheatFuc, &oValveAntiCheatFunc);

    LPVOID ClientReportValidationFuncAddr = _relativeToAbsolute<LPVOID>(Memory::findPattern_ex(L"client", "\xE8????\x83\x7C\x24??\x7C\x15\x8B\x54\x24\x58") + 1);

    MH_CreateHook(ClientReportValidationFuncAddr, &ClientReportValidationDecontructor, &oClientReportValidationDeconstructor);


    // HOOKFUNCTION(CL_IsPaused, CL_IsPaused);

#ifdef RCE_EXPLOIT || IP_PROTECT
    // HOOKFUNCTION(InternalRecievedP2PSignal, SteamNetHooks::InternalRecievedP2PSignal);
    // HOOKFUNCTION(ConnectionP2P_ProcessSignal, SteamNetHooks::ConnectionP2P_ProcessSignal);
#endif


    // HOOKFUNCTION(SteamP2PHandler, SteamNetHooks::SteamP2PHandler);
    // HOOKFUNCTION(SysSession, sub_100260B0);
    // HOOKFUNCTION(AntiIPGrabFool, sub_10025E30);


    // HOOKFUNCTION(DrawViewModels, viewRenderHooks::DrawViewModels)
#define NOT_BOT 1
#if NOT_BOT

    DWORD CL_MoveSig = memory->findPattern_ex(L"engine", "\x55\x8B\xEC\x81\xEC????\x53\x56\x57\x8B\x3D????\x8A");
    EngineHooks::oCL_Move = reinterpret_cast<EngineHooks::CL_MoveFunc>(CL_MoveSig + 9);
    MemoryTools::PlaceJMP((BYTE*)(CL_MoveSig), (DWORD)&EngineHooks::Hooked_CL_Move, 9);


    InstallTMPHooks();


    printf("Hooking NET_ReceiveRawPacket...\n");

    auto adr = memory->findPattern_ex(L"engine", "\x55\x8B\xEC\x81\xEC\xCC\x02\x00\x00\xA1????\x53\x56\x57");
    if (!adr)
    {
        Debug::QuickPrint("ERROR: CAN'T FIND NET_ReceiveRawPacket SIGNATURE, EXITING!\n");
        Sleep(5000);
        exit(EXIT_SUCCESS);
    }

    PlaceJMP((BYTE*)(adr), (DWORD)&NET_ReceiveRawPacket, 9);
    EngineHooks::NET_ReceiveRawPacketJMPBackAdr = (adr + 9);

    const char* vac = "Might fuck around and get VAC'd";
    auto clMoveChokeClamp = Memory::findPattern_ex(L"engine", "\xB8????\x3B\xF0\x0F\x4F\xF0\x89\x5D\xFC") + 1;



#ifdef LAGGER


    //
    auto nMaxRoutableClamp1 = (Memory::findPattern_ex(L"engine", "\xFF\x90????\x3D????\x7D\x07") + 2);
    auto nMaxRoutableClamp2 = Memory::findPattern_ex(L"engine", "\xB8????\xEB\x05\x3B\xC6") + 1;

    auto TooManyQueuedPackets = Memory::findPattern_ex(L"steamnetworkingsockets", "\x8D\x0C\x16") + 3;
    auto TooManyQueuedPackets2 = Memory::findPattern_ex(L"steamnetworkingsockets", "\x3B\x08\x7E\x54") + 2;



    unsigned long protect = 0;
    unsigned long protect1 = 0;
    unsigned long protect2 = 0;
    unsigned long protect3 = 0;

    Debug::QuickPrint(std::to_string(*(std::uint32_t*)clMoveChokeClamp).c_str());
    Debug::QuickPrint(std::to_string(*(std::int32_t*)nMaxRoutableClamp1).c_str());
    Debug::QuickPrint(std::to_string(*(std::int32_t*)nMaxRoutableClamp2).c_str());

    VirtualProtect((void*)clMoveChokeClamp, 4, PAGE_EXECUTE_READWRITE, &protect);
    *(std::uint32_t*)clMoveChokeClamp = 150;
    VirtualProtect((void*)clMoveChokeClamp, 4, protect, &protect);
#endif

#ifdef LAGGER


    /* Remove Clamping From NET_SendPacket (Causes NET_SendLong To Be Called) */

   // VirtualProtect((void*)nMaxRoutableClamp1, 4, PAGE_EXECUTE_READWRITE, &protect1);
   // *(std::uint32_t*)nMaxRoutableClamp1 = 1200;
    //VirtualProtect((void*)nMaxRoutableClamp1, 4, protect1, &protect1);

    VirtualProtect((void*)nMaxRoutableClamp2, 4, PAGE_EXECUTE_READWRITE, &protect1);
    *(std::uint32_t*)nMaxRoutableClamp2 = 36;
    VirtualProtect((void*)nMaxRoutableClamp2, 4, protect1, &protect1);


    memory->conColorMsg({ 255,0,255,255 }, hexStr((BYTE*)TooManyQueuedPackets, 4).c_str());
    memory->conColorMsg({ 255,0,255,255 }, " -> ");


    VirtualProtect((void*)TooManyQueuedPackets, 2, PAGE_EXECUTE_READWRITE, &protect2); /* cause jle to not set*/
    *(std::byte*)TooManyQueuedPackets = (std::byte)0x90;
    *(std::byte*)(TooManyQueuedPackets + sizeof(std::byte)) = (std::byte)0x90; /* Remove Compare */
    VirtualProtect((void*)TooManyQueuedPackets, 2, protect2, &protect2);


    VirtualProtect((void*)TooManyQueuedPackets2, 1, PAGE_EXECUTE_READWRITE, &protect3);
    if (*(std::byte*)(TooManyQueuedPackets2) == (std::byte)0x7E) {
        *(std::byte*)(TooManyQueuedPackets2) = (std::byte)0xEB; /* jle -> jmp */
        //Debug::QuickPrint("JLE -> JMP");
    }
    else {
        Debug::QuickPrint(std::to_string((int)*(std::byte*)(TooManyQueuedPackets2)).c_str());
    }
    VirtualProtect((void*)TooManyQueuedPackets2, 1, protect3, &protect3);

#endif
    memory->conColorMsg({ 255,0,255,255 }, hexStr((BYTE*)TooManyQueuedPackets, 4).c_str());
    memory->conColorMsg({ 255,0,255,255 }, "\n");
    //client->PatternSearch("74 63 8B 76 24").Patch(0xEB)
    auto nInviteCooldown = (Memory::findPattern_ex(L"client", "\x74\x63\x8B\x76\x24"));

    VirtualProtect((void*)nInviteCooldown, 4, PAGE_EXECUTE_READWRITE, &protect1);
    *(std::byte*)(nInviteCooldown) = (std::byte)0xEB;
    VirtualProtect((void*)nInviteCooldown, 4, protect1, &protect1);


    uintptr_t pNTOpenFile = Memory::findPattern_ex(L"csgo.exe", "\x74\x1B\xF6\x45\x0C\x20");

    if (!pNTOpenFile)
        Debug::QuickWarning("No pNTOpenFile!");
    else
    {
        memory->conColorMsg({ 255,0,255,255 }, hexStr((BYTE*)pNTOpenFile, 4).c_str());
        memory->conColorMsg({ 255,0,255,255 }, " -> ");
        VirtualProtect((void*)pNTOpenFile, 4, PAGE_EXECUTE_READWRITE, &protect1);
        *(std::byte*)(pNTOpenFile) = (std::byte)0xEB;
        VirtualProtect((void*)pNTOpenFile, 4, protect1, &protect1);

        memory->conColorMsg({ 255,0,255,255 }, hexStr((BYTE*)pNTOpenFile, 4).c_str());
        memory->conColorMsg({ 255,0,255,255 }, " (Trusted) \n");
    }

    // 33 F6 A1 ? ? ? ? 85 C0 74 25 + 4 <- g_nBlockedCount
    // 74 20 8B 3D ? ? ? ? + 5
    //Dispatch -> 55 8B EC 8B 0D ? ? ? ? 83 EC 0C 8B 01 53 56 57 BF ? ? ? ?
    int* g_pBlockCount = (int*)((char*)Memory::findPattern_ex(L"client", "\x33\xF6\xA1????\x85\xC0\x74\x25") + (int)3); // I think this is actually a strlen and not how many different files were blocked?
    char* g_pBlockedFileList = (char*)Memory::findPattern_ex(L"client", "\x74\x20\x8B\x3D????") + (int)4;
    memory->conColorMsg({ 255,0,255,255 }, hexStr((BYTE*)g_pBlockCount, 8).c_str());
    memory->conColorMsg({ 255,0,255,255 }, " (g_pBlocked Count) \n");
    int* ptr = *(int**)g_pBlockCount;
    memory->conColorMsg({ 255,0,255,255 }, hexStr((BYTE*)&ptr, 4).c_str());
    memory->conColorMsg({ 255,0,255,255 }, " (g_pBlocked Count Val) \n");
    memory->conColorMsg({ 255,0,255,255 }, hexStr((BYTE*)g_pBlockedFileList, 8).c_str());
    memory->conColorMsg({ 255,0,255,255 }, " (g_pBlockedFileList Count) \n");
    ptr = *(int**)g_pBlockedFileList;
    memory->conColorMsg({ 255,0,255,255 }, hexStr((BYTE*)&ptr, 4).c_str());
    memory->conColorMsg({ 255,0,255,255 }, " (g_pBlockedFileList Count Val) \n");

    CON("Calling Internal CS:GO Blocked File Info\n");
    typedef int(__cdecl* BlockedShitFunc_t)();
    BlockedShitFunc_t BlockedShit = (BlockedShitFunc_t)Memory::findPattern_ex(L"client", "\x56\x8B\x35????\xFF\xD6\x68????");
    BlockedShit();

    if (**(int**)g_pBlockCount)
    {
        VCON("CS:GO Has Blocked Files From Loading\n");
        VCON("CS:GO Has Blocked The Following Files  -  %s\n", **(const char***)g_pBlockedFileList);

        if (strstr(**(const char***)g_pBlockedFileList, "obs"))
        {
            CON("CS:GO Has Previously Blocked OBS GameCapture\n");
        }
        else {
            CON("CS:GO Has Not Previously Blocked OBS GameCapture\n");
        }

        VCON("Resetting Blocked List\n");
        **(int**)g_pBlockCount = 0;
        ***(char***)g_pBlockedFileList = '\0';
    }
    CON("Calling Internal CS:GO Blocked File Info\n");
    BlockedShit();
    CON("If That says files were not blocked, exit CS:GO.\n");


    LPVOID oHook = NULL;
    LPVOID ValveHookedLoadLibraryExWAdr = (LPVOID)Memory::findPattern_ex(L"csgo.exe", "\x55\x8B\xEC\x51\x56\x8B\x75\x08\x8D\x45\xFC");
    MH_CreateHook(ValveHookedLoadLibraryExWAdr, &ValveHookedLoadLibraryExW, &oHook);

    char* g_pNumDLLsInjectedAddr = ((char*)Memory::findPattern_ex(L"client", "\x83\xC4\x0C\xFF\x05????\x5F") + int(5));
    int*  g_pNumDLLsInjected = *(int**)(g_pNumDLLsInjectedAddr);
    memory->conColorMsg({ 255,0,255,255 }, hexStr((BYTE*)g_pNumDLLsInjectedAddr, 16).c_str());
    VCON("\ng_pNumDLLsInject : %d\n", *g_pNumDLLsInjected);
    *g_pNumDLLsInjected = 0;


    static char* g_pNumTimesSentAddr{ ((char*)Memory::findPattern_ex(L"client", "\x0F\x85????\xFF\x05????") + int(8)) };
    int* pNumTimesSent = 0;
    pNumTimesSent = *(int**)(g_pNumTimesSentAddr);
    if (pNumTimesSent && (*pNumTimesSent > 0))
    {
        memory->conColorMsg({ 255,0,255,255 }, hexStr((BYTE*)pNumTimesSent, 16).c_str());
        VCON("\ng_pNumTimesSent : %d\n", *pNumTimesSent);
        *pNumTimesSent = 0;
    }



    // 
    uintptr_t FPSMaxClamp = Memory::findPattern_ex(L"engine", "\x0F\x2F\x45\xFC\x76\x75") + 4;

    if (!FPSMaxClamp)
        Debug::QuickWarning("No pThreadCreationBypass!");
    else
    {
        memory->conColorMsg({ 255,0,255,255 }, hexStr((BYTE*)FPSMaxClamp, 4).c_str());
        memory->conColorMsg({ 255,0,255,255 }, " -> ");
        VirtualProtect((void*)FPSMaxClamp, 4, PAGE_EXECUTE_READWRITE, &protect1);
        *(std::byte*)(FPSMaxClamp) = (std::byte)0xEB;
        VirtualProtect((void*)FPSMaxClamp, 4, protect1, &protect1);

        memory->conColorMsg({ 255,0,255,255 }, hexStr((BYTE*)FPSMaxClamp, 4).c_str());
        memory->conColorMsg({ 255,0,255,255 }, " (FPS Max Unlock) \n");
    }






    uintptr_t pSendToCall = Memory::findPattern_ex(L"engine", "\xFF\x15????\x85\xC0\x79\x63", "pSendToCall");
    g_pSendToJumpBackAddress = (void*)Memory::findPattern_ex(L"engine", "\x79\x63\xFF\x15????", "g_pSendToJumpBackAddress");
    uintptr_t pFindSteamAddressCall = Memory::findPattern_ex(L"engine", "\x56\xE8????\x8B\xF8\x85\xFF\x0F\x84????", "pFindSteamAddressCall");
    g_pSendToJumpBackAddress2 = (void*)Memory::findPattern_ex(L"engine", "\x8B\xF8\x85\xFF\x0F\x84????\x8B\x15????", "g_pSendToJumpBackAddress2");
    g_pFindAddressFunc = (void*)Memory::findPattern_ex(L"engine", "\x56\xE8????\x8B\xF8\x85\xFF\x0F\x84????", "g_pFindAddressFunc");
#if 1
    if (!pSendToCall)
        Debug::QuickWarning("No pSendToCall!");
    else if (true)
    {
        memory->conColorMsg({ 255,0,255,255 }, hexStr((BYTE*)pSendToCall, 8).c_str());
        memory->conColorMsg({ 255,0,255,255 }, " -> ");
        VirtualProtect((void*)pSendToCall, 8, PAGE_EXECUTE_READWRITE, &protect1);
        for (int i = 0; i < 8; i++) // Nop everything rq
        {
            *(std::byte*)(pSendToCall + i) = (std::byte)0x90;
        }


        std::uint32_t nDistance = (std::uint32_t)((std::uint32_t)&Anti_IP_Grabber - (std::uint32_t)pSendToCall) - 5;
        *(std::byte*)(pSendToCall) = (std::byte)0xE9; // Jump Far Absolute; Address Given in operand
        *(std::uint32_t*)(pSendToCall + 1) = (std::uint32_t)nDistance; // Write Operand (Distance To Our Function)

        VirtualProtect((void*)pSendToCall, 8, protect1, &protect1);
        // ff 15 10 a4 24 79 85 c0  -> e9 50 b8 86 c0 90 90 90  (Anti IP Grab)
        memory->conColorMsg({ 255,0,255,255 }, hexStr((BYTE*)pSendToCall, 8).c_str());
        memory->conColorMsg({ 255,0,255,255 }, " (Anti IP Grab) \n");
    }

    if (!pFindSteamAddressCall)
        Debug::QuickWarning("No pFindSteamAddressCall!");
    else if (false)
    {
        memory->conColorMsg({ 255,0,255,255 }, hexStr((BYTE*)pFindSteamAddressCall, 8).c_str());
        memory->conColorMsg({ 255,0,255,255 }, " -> ");
        VirtualProtect((void*)pFindSteamAddressCall, 5, PAGE_EXECUTE_READWRITE, &protect1);
        for (int i = 0; i < 5; i++) // Nop everything rq
        {
            *(std::byte*)(pFindSteamAddressCall + i) = (std::byte)0x90;
        }

        std::uint32_t nDistance = (std::uint32_t)((std::uint32_t)&Anti_IP_Grabber2 - (std::uint32_t)pFindSteamAddressCall) - 5;
        *(std::byte*)(pFindSteamAddressCall) = (std::byte)0xE9; // Jump Far Absolute; Address Given in operand
        *(std::uint32_t*)(pFindSteamAddressCall + 1) = (std::uint32_t)nDistance; // Write Operand (Distance To Our Function)

        VirtualProtect((void*)pFindSteamAddressCall, 5, protect1, &protect1);
        memory->conColorMsg({ 255,0,255,255 }, hexStr((BYTE*)pFindSteamAddressCall, 5).c_str());
        memory->conColorMsg({ 255,0,255,255 }, " (Anti IP Grab) \n");
    }
#endif






#endif


    Debug::QuickPrint("Enabling All Hooks...", false);
    if constexpr (std::is_same_v<HookType, MinHook>) {
        if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
            throw std::exception();
    }




    Debug::QuickPrint("Ok\n", false);
    Debug::QuickPrint(" - Hooking Complete - ");


    static unsigned int myCounter = 0;
    static DWORD myThreadID;



    //myHandle = (HANDLE)_beginthreadex(0, 0, forceSpectate, &config, 0, 0);
#ifndef SAFE_BUILD
    myHandle = (HANDLE)_beginthreadex(0, 0, forceSpectate, &config, 0, 0);
    _beginthread(WalkbotNetworkThread, 0, NULL);
#endif


}

//return reinterpret_cast<T(__thiscall*)(void*, Args...)>(originals[Idx]);

extern "C" BOOL WINAPI _CRT_INIT(HMODULE module, DWORD reason, LPVOID reserved);
static DWORD WINAPI unload(HMODULE module) noexcept
{
    Sleep(100);

    //interfaces->inputSystem->enableInput(true);
    //eventListener->remove();

    //ImGui_ImplDX9_Shutdown();
   // ImGui_ImplWin32_Shutdown();
    //ImGui::DestroyContext();

    _CRT_INIT(module, DLL_PROCESS_DETACH, nullptr);

    FreeLibraryAndExitThread(module, 0);
}

#include "Hacks/Glow.h"
#include "SDK/OsirisSDK/InputSystem.h"
#include "Hacks/RageBot/Resolver.h"
void Hooks::uninstall() noexcept
{
    if constexpr (std::is_same_v<HookType, MinHook>) {
        MH_DisableHook(MH_ALL_HOOKS);
        //MH_DisableHook(memory->CL_MoveCall);
        MH_Uninitialize();
    }

    CloseHandle(myHandle);
    //clMove.restore();
    /*
    



    

    */

    bspQuery.restore();
    engine.restore();
    panel.restore();
    client.restore();
    svCheats.restore();
    clientMode.restore();
    netvars->restore();

    if(extraHook.player.isHooked)
        extraHook.restore();



    modelRender.restore();
    viewRender.restore();
    surface.restore();
    sound.restore();
    /*






    
    
    


    
    */
    gameEventManager.restore();
    Glow::clearCustomObjects();
    if(netchanhooked)
        networkChannel.restore();

    SetWindowLongPtrW(window, GWLP_WNDPROC, LONG_PTR(originalWndProc));
    **reinterpret_cast<void***>(memory->present) = originalPresent;
    **reinterpret_cast<void***>(memory->reset) = originalReset;
    
    if (DWORD oldProtection; VirtualProtect(memory->dispatchSound, 4, PAGE_EXECUTE_READWRITE, &oldProtection)) {
        *memory->dispatchSound = uintptr_t(originalDispatchSound) - uintptr_t(memory->dispatchSound + 1);
        VirtualProtect(memory->dispatchSound, 4, oldProtection, nullptr);
    }
    
    interfaces->inputSystem->enableInput(true);
    if (HANDLE thread = CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(unload), module, 0, nullptr))
        CloseHandle(thread);
}
//bool init = false;
Hooks::Hooks(HMODULE module) noexcept
{
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

    this->module = module;

    // interfaces and memory shouldn't be initialized in wndProc because they show MessageBox on error which would cause deadlock
    interfaces = std::make_unique<const Interfaces>();
    memory = std::make_unique<const Memory>();

    window = FindWindowW(L"Valve001", nullptr);
    originalWndProc = WNDPROC(SetWindowLongPtrW(window, GWLP_WNDPROC, LONG_PTR(wndProc)));
}



typedef float quaternion[4];

extraHooks extraHook;

void __fastcall DoExtraBoneProcessing(Entity* player, uint32_t, StudioHdr* hdr, Vector* pos, quaternion* q, matrix3x4* matrix, void* bone_list, void* context)
{
    const auto entity = reinterpret_cast<Entity*> (player);;
    auto state = entity->getAnimstate();
    const auto val = &(state->m_bOnGround);//reinterpret_cast<float*> (reinterpret_cast<uintptr_t> (state) + 292);
    const auto backup = *val;
    auto backup_onground = false;

    if (state)
    {
        backup_onground = state->m_bOnGround;
        state->m_bOnGround = false;

        if (entity->velocity().length2D() < 0.1f)
            *val = 0.f;
    }

    extraHook.player.vmt.getOriginal<void>(197, player, hdr, pos, q, matrix, bone_list, context);

    if (state)
    {
        *val = backup;
        state->m_bOnGround = backup_onground;
    }
}



void __fastcall StandardBlendingRules(Entity* player, uint32_t edx, StudioHdr* hdr, Vector* pos, quaternion* q, float curTime, int boneMask)
{
    auto orig = extraHook.player.vmt.getOriginal<void, StudioHdr*, Vector*, quaternion*, float, int>(205, hdr, pos, q, curTime, boneMask);
    uint32_t* effects = (uint32_t*)((uintptr_t)player + 0xF0);
    *effects |= 8;
    orig(player, hdr, pos, q, curTime, boneMask);
    *effects &= ~8;
}

void extraHooks::hookEntity(Entity* ent)
{
    player.vmt.init(ent);
    player.vmt.hookAt(197, DoExtraBoneProcessing);
   // player.vmt.hookAt(205, StandardBlendingRules);
    player.isHooked = true;
}


bool extraHooks::init()
{
    if (interfaces->engine->isInGame())
    {
        if (!localPlayer || !localPlayer.get())
            return false;
        static Entity* oldLocalPlayer = nullptr;
        if (!player.isHooked || oldLocalPlayer != localPlayer.get())
        {
            oldLocalPlayer = localPlayer.get();
            hookEntity(localPlayer.get());
        }
    }
    return true;
}

bool extraHooks::init(Entity* entity)
{
    if (interfaces->engine->isInGame())
    {
        if (!localPlayer || !localPlayer.get())
            return false;
        if (!player.isHooked || player.OldEntity != entity)
        {
            player.OldEntity = entity;
            hookEntity(entity);
        }
    }
    return true;
}

void extraHooks::restore()
{
    if (player.isHooked)
    {
        player.vmt.restore();
        player.isHooked = false;
    }
}




//sig: "55 8B EC 83 EC 14 53 56 57 FF 75 18"
//use_new_animation_state = "88 87 ? ? ? ? 75" + 0x2

void __fastcall calculate_view(Entity* _thisPlayer, void* edx, Vector eyeOrigin, Vector eyeAngles, float& zNear, float& zFar, float& fov) {

   // static auto original_calculate_view = m_calculate_view.get_original_function< decltype(&calculate_view) >();

    //const auto player = reinterpret_cast<sdk::cs_player*>(this_pointer);

    if (_thisPlayer != localPlayer.get()) {
        // return original_calculate_view(this_pointer, edx, eye_origin, eye_angles, z_near, z_far, fov);
    }

  //  const auto old_use_new_animation_state = player->ClientSideAnimation();

    // prevent calls to ModifyEyePosition
    //player->use_new_animation_state() = false;

    //original_calculate_view(this_pointer, edx, eye_origin, eye_angles, z_near, z_far, fov);
//
    //player->use_new_animation_state() = old_use_new_animation_state;

}



