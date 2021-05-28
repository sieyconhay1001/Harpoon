#pragma once


// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/tier1/interface.h

#include <memory>
#include <type_traits>
#include <string>
#include <Windows.h>

#include <locale>
#include <codecvt>


//#include "SDK/OsirisSDK/Client.h"
//#include "SDK/OsirisSDK/Cvar.h"
//#include "SDK/Engine"
#include "SDK/SDK/IMatchFramework.h"
class Client;
class Cvar;

class Engine;
class EngineTrace;
class EntityList;
class GameEventManager;
class GameMovement;
class GameUI;
class InputSystem;
class Localize;
class MaterialSystem;
class ModelInfo;
class ModelRender;
class NetworkStringTableContainer;
class Panel;
class PhysicsSurfaceProps;
class Prediction;
class RenderView;
class Surface;
class Sound;
class SoundEmitter;
class StudioRender;
class IVEffects;
class IEffects;
class IAPPSystem;
class NetworkStringTableContainer;
class IPanoramaUIEngine;
class IVDebugOverlay;
class IVPhysicsDebugOverlay;
class INetSupport;

typedef void* (*CreateInterfaceFn)(const char* pName, int* pReturnCode);
typedef void* (*InstantiateInterfaceFn)();

class IFileSystem;
#include "SDK/SDKAddition/SoundSystem.h"
class GameInterface {
public:

    //Engine
    Engine* engine = reinterpret_cast<Engine*>(Get_Game_Interface(L"engine", "VEngineClient014"));
    EngineTrace* engineTrace = reinterpret_cast<EngineTrace*>(Get_Game_Interface(L"engine", "EngineTraceClient004"));
    GameEventManager* gameEventManager = reinterpret_cast<GameEventManager*>(Get_Game_Interface(L"engine", "GAMEEVENTSMANAGER002"));
    ModelInfo* modelInfo = reinterpret_cast<ModelInfo*>(Get_Game_Interface(L"engine","VModelInfoClient004"));
    ModelRender* modelRender = reinterpret_cast<ModelRender*>(Get_Game_Interface(L"engine", "VEngineModel016"));
    IVEffects* iveffects = reinterpret_cast<IVEffects*>(Get_Game_Interface(L"engine", "VEngineEffects001"));
    NetworkStringTableContainer* networkStringTableContainer = reinterpret_cast<NetworkStringTableContainer*>(Get_Game_Interface(L"engine", "VEngineClientStringTable001"));
    RenderView* renderView = reinterpret_cast<RenderView*>(Get_Game_Interface(L"engine", "VEngineRenderView014"));;
    IVDebugOverlay* debugOverlay = reinterpret_cast<IVDebugOverlay*>(Get_Game_Interface(L"engine", "VDebugOverlay004"));
    IVPhysicsDebugOverlay* physicsDebugOverlay = reinterpret_cast<IVPhysicsDebugOverlay*>(Get_Game_Interface(L"engine", "VPhysicsDebugOverlay001"));
    INetSupport* netSupport = reinterpret_cast<INetSupport*>(Get_Game_Interface(L"engine", "INETSUPPORT_003"));

    //Client
    Client* client = reinterpret_cast<Client*>(Get_Game_Interface(L"client", "VClient018"));
    EntityList* entityList = reinterpret_cast<EntityList*>(Get_Game_Interface(L"client", "VClientEntityList003"));
    GameMovement* gameMovement = reinterpret_cast<GameMovement*>(Get_Game_Interface(L"client", "GameMovement001"));
    GameUI* gameUI = reinterpret_cast<GameUI*>(Get_Game_Interface(L"client", "GameUI011"));
    IEffects* ieffects = reinterpret_cast<IEffects*>(Get_Game_Interface(L"client", "IEffects001"));
    Prediction* prediction = reinterpret_cast<Prediction*>(Get_Game_Interface(L"client", "VClientPrediction001"));
    // GUI
    Panel* panel = reinterpret_cast<Panel*>(Get_Game_Interface(L"vgui2", "VGUI_Panel009"));
    Sound* sound = reinterpret_cast<Sound*>(Get_Game_Interface(L"engine", "IEngineSoundClient003"));
    Surface* surface = reinterpret_cast<Surface*>(Get_Game_Interface(L"vguimatsurface", "VGUI_Surface031"));

    //Other
    Cvar* cvar = reinterpret_cast<Cvar*>(Get_Game_Interface(L"vstdlib", "VEngineCvar007"));
    InputSystem* inputSystem = reinterpret_cast<InputSystem*>(Get_Game_Interface(L"inputsystem", "InputSystemVersion001"));
    Localize* localize = reinterpret_cast<Localize*>(Get_Game_Interface(L"localize","Localize_001"));
    MaterialSystem* materialSystem = reinterpret_cast<MaterialSystem*>(Get_Game_Interface(L"materialsystem","VMaterialSystem080"));
    PhysicsSurfaceProps* physicsSurfaceProps = reinterpret_cast<PhysicsSurfaceProps*>(Get_Game_Interface(L"vphysics", "VPhysicsSurfaceProps001"));
    StudioRender* studioRender = reinterpret_cast<StudioRender*>(Get_Game_Interface(L"studiorender", "VStudioRender026"));
    IFileSystem* fileSystem = reinterpret_cast<IFileSystem*>(Get_Game_Interface(L"filesystem_stdio", "VFileSystem017"));
    SoundEmitter* soundEmitter = reinterpret_cast<SoundEmitter*>(Get_Game_Interface(L"soundemittersystem", "VSoundEmitter003"));
    IMatchFramework* matchFramework = reinterpret_cast<IMatchFramework*>(Get_Game_Interface(L"matchmaking", "MATCHFRAMEWORK_001"));
    IPanoramaUIEngine* panoramaEngine = reinterpret_cast<IPanoramaUIEngine*>(Get_Game_Interface(L"panorama", "PanoramaUIEngine001"));

    void* KeyValuesSystem = GetProcAddress(GetModuleHandleA("vstdlib"), "KeyValuesSystem");


    /* My Interfaces */
private:

    static CreateInterfaceFn Get_Interface_Factory(const wchar_t* wmodule) {

        return reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleW(wmodule), "CreateInterface"));

    }

    static void* Get_Game_Interface(const wchar_t* wmodule, const char* interfacename) noexcept 
    {
        CreateInterfaceFn CreateInterface = Get_Interface_Factory(wmodule);

        if (!CreateInterface) {

            std::wstring wmodule(wmodule);
            std::string str(wmodule.begin(), wmodule.end());

            MessageBoxA(nullptr, ("Failed to create module factory for  " + str).c_str(), "Harpoon", MB_OK | MB_ICONERROR);
            std::exit(EXIT_FAILURE);
        }

        void* Interface = CreateInterface(interfacename, nullptr);

        if (!Interface) {
            MessageBoxA(nullptr, ("Failed to find " + std::string{ interfacename } + " interface!").c_str(), "Harpoon", MB_OK | MB_ICONERROR);
            std::exit(EXIT_FAILURE);
        }

        return Interface;
    }
};

typedef GameInterface Interfaces;
inline std::unique_ptr<const GameInterface> interfaces;


/*
    Interfaces Dump

     --> launcher.dll <--

     --> vstdlib.dll <--
      VProcessUtils002 ------------------------------------------- [2]
      EventSystem001 --------------------------------------------- [1]
      VEngineCvar007 --------------------------------------------- [7]

     --> filesystem_stdio.dll <--
      QueuedLoaderVersion001 ------------------------------------- [1]
      VNewAsyncFileSystem001 ------------------------------------- [1]
      VBaseFileSystem011 ----------------------------------------- [11]
      VFileSystem017 --------------------------------------------- [17]

     --> engine.dll <--
      VEngineVGui001 --------------------------------------------- [1]
      VSERVERENGINETOOLS001 -------------------------------------- [1]
      VCLIENTENGINETOOLS001 -------------------------------------- [1]
      VTOOLFRAMEWORKVERSION002 ----------------------------------- [2]
      VENGINETOOLFRAMEWORK003 ------------------------------------ [3]
      VENGINETOOL003 --------------------------------------------- [3]
      XboxSystemInterface002 ------------------------------------- [2]
      VProfExport001 --------------------------------------------- [1]
      VoiceServer002 --------------------------------------------- [2]
      VENGINE_GAMEUIFUNCS_VERSION005 ----------------------------- [5]
      VENGINE_HLDS_API_VERSION002 -------------------------------- [2]
      VENGINE_LAUNCHER_API_VERSION004 ---------------------------- [4]
      FileLoggingListener001 ------------------------------------- [1]
      StaticPropMgrServer002 ------------------------------------- [2]
      StaticPropMgrClient005 ------------------------------------- [5]
      SpatialPartition001 ---------------------------------------- [1]
      VEngineRandom001 ------------------------------------------- [1]
      VEngineServerStringTable001 -------------------------------- [1]
      VEngineClientStringTable001 -------------------------------- [1]
      INETSUPPORT_003 -------------------------------------------- [3]
      VModelInfoClient004 ---------------------------------------- [4]
      VModelInfoServer002 ---------------------------------------- [2]
      GAMEEVENTSMANAGER001 --------------------------------------- [1]
      GAMEEVENTSMANAGER002 --------------------------------------- [2]
      EngineTraceClient004 --------------------------------------- [4]
      EngineTraceServer004 --------------------------------------- [4]
      VCvarQuery001 ---------------------------------------------- [1]
      BugReporterUserName001 ------------------------------------- [1]
      BlackBoxVersion001 ----------------------------------------- [1]
      VEngineServer023 ------------------------------------------- [23]
      ServerUploadGameStats001 ----------------------------------- [1]
      GameServerData001 ------------------------------------------ [1]
      ISERVERPLUGINHELPERS001 ------------------------------------ [1]
      VEngineRenderView014 --------------------------------------- [14]
      VEngineShadowMgr002 ---------------------------------------- [2]
      VEngineEffects001 ------------------------------------------ [1]
      VEngineModel016 -------------------------------------------- [16]
      VPhysicsDebugOverlay001 ------------------------------------ [1]
      VDebugOverlay004 ------------------------------------------- [4]
      VEngineClient014 ------------------------------------------- [14]
      IEngineSoundServer003 -------------------------------------- [3]
      IEngineSoundClient003 -------------------------------------- [3]

     --> inputsystem.dll <--
      InputSystemVersion001 -------------------------------------- [1]
      InputStackSystemVersion001 --------------------------------- [1]

     --> vphysics.dll <--
      VPhysicsSurfaceProps001 ------------------------------------ [1]
      VPhysicsCollision007 --------------------------------------- [7]
      VPhysics031 ------------------------------------------------ [31]

     --> materialsystem.dll <--
      ShaderDLL004 ----------------------------------------------- [4]
      ShaderSystem002 -------------------------------------------- [2]
      VMaterialSystemStub001 ------------------------------------- [1]
      VEngineCvar007 --------------------------------------------- [7]
      MaterialSystemHardwareConfig013 ---------------------------- [13]
      VMaterialSystemConfig004 ----------------------------------- [4]
      VMaterialSystem080 ----------------------------------------- [80]
      COLORCORRECTION_VERSION_1 ---------------------------------- [0]

     --> datacache.dll <--
      VResourceAccessControl001 ---------------------------------- [1]
      VPrecacheSystem001 ----------------------------------------- [1]
      VStudioDataCache005 ---------------------------------------- [5]
      MDLCache004 ------------------------------------------------ [4]
      VDataCache003 ---------------------------------------------- [3]

     --> studiorender.dll <--
      VStudioRender026 ------------------------------------------- [26]

     --> soundemittersystem.dll <--
      VSoundEmitter003 ------------------------------------------- [3]

     --> vscript.dll <--
      VScriptManager009 ------------------------------------------ [9]

     --> soundsystem.dll <--
      SoundSystem001 --------------------------------------------- [1]

     --> valve_avi.dll <--
      VAvi001 ---------------------------------------------------- [1]

     --> vguimatsurface.dll <--
      SchemeSurface001 ------------------------------------------- [1]
      VGUI_Surface031 -------------------------------------------- [31]

     --> vgui2.dll <--
      VGUI_Panel009 ---------------------------------------------- [9]
      VGUI_ivgui008 ---------------------------------------------- [8]
      VGUI_System010 --------------------------------------------- [10]
      SchemeSurface001 ------------------------------------------- [1]
      VGUI_Surface031 -------------------------------------------- [31]
      VGUI_Scheme010 --------------------------------------------- [10]
      VGUI_InputInternal001 -------------------------------------- [1]
      VGUI_Input005 ---------------------------------------------- [5]

     --> panoramauiclient.dll <--
      PanoramaUIClient001 ---------------------------------------- [1]

     --> video.dll <--

     --> panorama.dll <--
      PanoramaUIEngine001 ---------------------------------------- [1]

     --> shaderapidx9.dll <--
      ShaderShadow010 -------------------------------------------- [10]
      ShaderDeviceMgr001 ----------------------------------------- [1]
      DebugTextureInfo001 ---------------------------------------- [1]
      ShaderDevice001 -------------------------------------------- [1]
      ShaderApi029 ----------------------------------------------- [29]
      MaterialSystemHardwareConfig013 ---------------------------- [13]
      VBAllocTracker001 ------------------------------------------ [1]

     --> imemanager.dll <--
      IMEManager001 ---------------------------------------------- [1]

     --> localize.dll <--
      Localize_001 ----------------------------------------------- [1]

     --> panorama_text_pango.dll <--
      PanoramaTextServices001 ------------------------------------ [1]

     --> steamclient.dll <--
      IVALIDATE001 ----------------------------------------------- [1]
      CLIENTENGINE_INTERFACE_VERSION005 -------------------------- [5]
      SteamClient018 --------------------------------------------- [18]
      SteamClient017 --------------------------------------------- [17]
      SteamClient016 --------------------------------------------- [16]
      SteamClient015 --------------------------------------------- [15]
      SteamClient014 --------------------------------------------- [14]
      SteamClient013 --------------------------------------------- [13]
      SteamClient012 --------------------------------------------- [12]
      SteamClient011 --------------------------------------------- [11]
      SteamClient010 --------------------------------------------- [10]
      SteamClient009 --------------------------------------------- [9]
      SteamClient008 --------------------------------------------- [8]
      SteamClient007 --------------------------------------------- [7]
      SteamClient006 --------------------------------------------- [6]
      p2pvoice002 ------------------------------------------------ [2]
      p2pvoicesingleton002 --------------------------------------- [2]

     --> vstdlib_s.dll <--
      IVALIDATE001 ----------------------------------------------- [1]
      VEngineCvar002 --------------------------------------------- [2]

     --> stdshader_dbg.dll <--
      ShaderDLL004 ----------------------------------------------- [4]

     --> stdshader_dx9.dll <--
      ShaderDLL004 ----------------------------------------------- [4]

     --> matchmaking.dll <--
      MATCHFRAMEWORK_001 ----------------------------------------- [1]
      VENGINE_GAMETYPES_VERSION002 ------------------------------- [2]

     --> client_panorama.dll <--
      VCLIENTTOOLS001 -------------------------------------------- [1]
      CustomSteamImageOnModel_IMaterialProxy003 ------------------ [3]
      ItemTintColor_IMaterialProxy003 ---------------------------- [3]
      VGuiModuleLoader003 ---------------------------------------- [3]
      RunGameEngine005 ------------------------------------------- [5]
      GameUI011 -------------------------------------------------- [11]
      GameConsole004 --------------------------------------------- [4]
      VENGINE_GAMETYPES_VERSION002 ------------------------------- [2]
      VClientPrediction001 --------------------------------------- [1]
      GameMovement001 -------------------------------------------- [1]
      RenderToRTHelper001 ---------------------------------------- [1]
      VParticleSystemQuery004 ------------------------------------ [4]
      IEffects001 ------------------------------------------------ [1]
      ClientAlphaPropertyMgrV001 --------------------------------- [1]
      ClientLeafSystem002 ---------------------------------------- [2]
      VClientEntityList003 --------------------------------------- [3]
      VCLIENTMATERIALSYSTEM001 ----------------------------------- [1]
      VClient018 ------------------------------------------------- [18]
      VClientDllSharedAppSystems001 ------------------------------ [1]
      GameClientExports001 --------------------------------------- [1]
      ClientRenderTargets001 ------------------------------------- [1]

     --> scenefilecache.dll <--
      SceneFileCache002 ------------------------------------------ [2]

     --> vaudio_miles.dll <--
      vaudio_miles ----------------------------------------------- [0]
      VAudio002 -------------------------------------------------- [2]

     --> serverbrowser.dll <--
      VGuiModuleServerBrowser001 --------------------------------- [1]
      ServerBrowser003 ------------------------------------------- [3]

     --> vaudio_celt.dll <--
      vaudio_celt ------------------------------------------------ [0]

*/