#pragma once
#include "COMPILERDEFINITIONS.h"
#include <array>
#include <memory>
#include <string>
#include <type_traits>
#include <Windows.h>
#include <Psapi.h>
#include "SDK/OsirisSDK/ModelInfo.h"

class ClientMode;
class ClientState;

class Entity;
class GameEventDescriptor;
class GameEventManager;
class Input;
class ItemSystem;
class KeyValues;
class MoveHelper;
class MoveData;
class ViewRender;
class WeaponSystem;
class MemAlloc;
class StaticPropMgr;
class IViewRenderBeams;

struct ActiveChannels;
struct Channel;
struct GlobalVars;
struct GlowObjectManager;
struct Trace;
struct Vector;

inline LPVOID oCL_Move;
inline LPVOID oCL_SendMove;
typedef void(__cdecl* CL_MoveFn)(float&, bool&);

inline LPVOID oRenderModels;
inline LPVOID oModelRenderDrawModels;


inline LPVOID oDrawWorld;
inline LPVOID oDrawWorldLists;

inline LPVOID oCheckForSequenceChange;
inline LPVOID oProceduralFootPlant;
inline LPVOID oShouldSkipAnimationFrame;
inline LPVOID oIsHLTV;

inline LPVOID oNET_BufferToBufferCompress;
inline LPVOID oCNET_CompressFragments;
inline LPVOID oCNET_CreateFragmentsFromBuffer;
inline LPVOID oCNET_SendSubChannelData;
inline LPVOID oCNET_SendTCPData;
inline LPVOID oCNET_SendNetMessage;
inline LPVOID oCBaseClient_GetSendFrame;
inline LPVOID oNET_SendToImpl;
inline LPVOID oSendClientMessages;
inline LPVOID oHOST_ERROR;
inline LPVOID oGetPredFrame;
inline LPVOID oHOSTRUNFRAMEINPUT;


#define FUNCTION_HOLDER(NAME) uintptr_t NAME = NULL; LPVOID o##NAME = NULL;


int g_bHasSentLagData = 0;
bool g_InLag = false;
bool g_LagSetTick = false;
inline bool HITPAUSE = 0;
inline int TC = 0;
inline int DTC = 0;
inline bool NEWTICK = false;
inline float g_NormalLatency = NULL;

inline  int nHighestDeltaTick{ -1 }; /* HEY IF YOU SEE THIS YOU FORGOT THE MAKE THIS A MEMBER OF TIMING, DO SO YOU NUNCE!!!! */
typedef void* (__thiscall* GetPredictedFrameFn)(void*, int);
typedef void (__thiscall* SaveDataFn)(void*, const char*, int, int);
typedef bool(__cdecl* Voice_RecordStartFn)(const char*, const char*, const char*);
typedef int(__thiscall* RequestFileFn)(void*, const char*, bool);
typedef int(__thiscall* EnqueAsyncFun)(void*, void*);

// Matchmaking Shit
typedef bool(__thiscall* HandleMatchStartFn)(void*, const char*, char*, char*, char*);
typedef void* (__stdcall* CreateMatchSessionFn)(void);
//typedef void(__thiscall* ModelRenderDrawModels)(void*, void*, int nCount, int renderMode, char unknown);


namespace MemoryTools {

    inline void PlaceJMP(BYTE* bt_DetourAddress, DWORD dw_FunctionAddress, DWORD dw_Size)
    {
        DWORD dw_OldProtection, dw_Distance;
        VirtualProtect(bt_DetourAddress, dw_Size, PAGE_EXECUTE_READWRITE, &dw_OldProtection);
        dw_Distance = (DWORD)(dw_FunctionAddress - (DWORD)bt_DetourAddress) - 5;
        *bt_DetourAddress = 0xE9;
        *(DWORD*)(bt_DetourAddress + 0x1) = dw_Distance;
        for (int i = 0x5; i < dw_Size; i++) *(bt_DetourAddress + i) = 0x90;
        VirtualProtect(bt_DetourAddress, dw_Size, dw_OldProtection, NULL);
        return;
    }


};


//typdef void(__cdecl* RenderModelsFn)()
bool g_bDontProcessConnectionless = false;
class Memory {
public:
    Memory() noexcept;


    uintptr_t HOST_RUN_FRAME_INPUT;

    uintptr_t present;
    uintptr_t reset;

    ClientMode* clientMode;
    StaticPropMgr* staticPropManager;

    Input* input;
    GlobalVars* globalVars;
    GlowObjectManager* glowObjectManager;

    bool* disablePostProcessing;

    std::add_pointer_t<void __fastcall(const char*)> loadSky;
    std::add_pointer_t<void __fastcall(const char*, const char*)> setClanTag;
    uintptr_t cameraThink;
    std::add_pointer_t<bool __stdcall(const char*)> acceptMatch;
    std::add_pointer_t<bool __cdecl(Vector, Vector, short)> lineGoesThroughSmoke;
    int(__fastcall* getSequenceActivity)(void*, StudioHdr*, int);
    bool(__thiscall* isOtherEnemy)(Entity*, Entity*);
    uintptr_t hud;
    int*(__thiscall* findHudElement)(uintptr_t, const char*);
    int(__thiscall* clearHudWeapon)(int*, int);
    std::add_pointer_t<ItemSystem* __cdecl()> itemSystem;
    void(__thiscall* setAbsOrigin)(Entity*, const Vector&);
    uintptr_t listLeaves;
    int* dispatchSound;
    std::add_pointer_t<bool __cdecl(float, float, float, float, float, float, Trace&)> traceToExit;
    ViewRender* viewRender;
    uintptr_t drawScreenEffectMaterial;
    std::add_pointer_t<bool __stdcall(const char*, const char*)> submitReport;
    uint8_t* fakePrime;
    std::add_pointer_t<void __cdecl(const char* msg, ...)> debugMsg;
    std::add_pointer_t<void __cdecl(const std::array<std::uint8_t, 4>& color, const char* msg, ...)> conColorMsg;
    float* vignette;
    int(__thiscall* equipWearable)(void* wearable, void* player);
    int* predictionRandomSeed;
    MoveData* moveData;
    MoveHelper* moveHelper;
    std::uintptr_t keyValuesFromString;
    KeyValues*(__thiscall* keyValuesFindKey)(KeyValues* keyValues, const char* keyName, bool create);
    void(__thiscall* keyValuesSetString)(KeyValues* keyValues, const char* value);
    WeaponSystem* weaponSystem;
    std::add_pointer_t<const char** __fastcall(const char* playerModelName)> getPlayerViewmodelArmConfigForPlayerModel;
    GameEventDescriptor* (__thiscall* getEventDescriptor)(GameEventManager* _this, const char* name, int* cookie);
    void(__thiscall* setAbsAngle)(Entity*, const Vector&);
    uintptr_t UpdateState;
    
    uintptr_t CreateState;
    uintptr_t InvalidateBoneCache;
    uintptr_t DrawWorld;
    ClientState* clientState;
    void* WriteUsercmdDeltaToBufferReturn;
    uintptr_t WriteUsercmd;
    uintptr_t RenderModels;
    uintptr_t DrawWorldLists;
    uintptr_t ModelRenderDrawModels;
    uintptr_t CheckForSequenceChange;
    uintptr_t ProceduralFootPlant;
    uintptr_t ShouldSkipAnimationFrame;
    uintptr_t IsHLTV;
    uintptr_t NET_BufferToBufferCompress;
    uintptr_t CNetChan_CompressFragments;
    uintptr_t CNetChan_CreateFragmentsFromBuffer;
    uintptr_t CNetChan_SendSubChannelData;
    uintptr_t CNetChan_SendTcpData;
    uintptr_t CNetChan_SetTimeout;
    uintptr_t CNetChan_SendNetMessage;
    uintptr_t NET_SendToImpl;
    uintptr_t SendClientMessages;
    uintptr_t CL_SendMove_;
    uintptr_t HOST_ERROR;


    uintptr_t oInternalMinThinkTime;
    uintptr_t CheckConnectionStateOrScheduleWakeUp;
    uintptr_t AssertHeldByCurrentThread;
    uintptr_t PendingBytesTotal;

    ClientState* cclientstate;
    int32_t HOST_VERSION;

    uintptr_t RATLIM_ReturnAddress;
    RequestFileFn RequestFile;
    EnqueAsyncFun EnqueVeryLargeAsyncTransfer;
    LPVOID oEnqueVeryLargeAsyncTransfer;

    GetPredictedFrameFn GetPredFrame;
    SaveDataFn SaveData;
    SaveDataFn RestoreData;
    FUNCTION_HOLDER(NET_TickMsgHandler)
    FUNCTION_HOLDER(CL_FlushEntityPacket)
    FUNCTION_HOLDER(Calls_SendToImpl);
    FUNCTION_HOLDER(NET_SendPacket);
    FUNCTION_HOLDER(SVCMSG_PacketEntities);
    FUNCTION_HOLDER(CL_PreprocessEntities);
    FUNCTION_HOLDER(CBaseClientState_SVCMsg_PacketEntities);
    FUNCTION_HOLDER(CL_CopyExistingEntity);
    FUNCTION_HOLDER(CL_CopyNewEntity);
    FUNCTION_HOLDER(NET_SendLong);
    FUNCTION_HOLDER(SNP_GetNextThinkTime);
    FUNCTION_HOLDER(ConnectionProblemLocally);
    FUNCTION_HOLDER(NET_CreateNetChannel);
    FUNCTION_HOLDER(HostRunFrameRender);
// E8 ? ? ? ? 3B D3 7C 0E
    FUNCTION_HOLDER(CSteamNetworkConnectionBase__SNP_ClampSendRate);
    FUNCTION_HOLDER(ProcessConnectionless);
    FUNCTION_HOLDER(CNetChan_SendDatagram);
    FUNCTION_HOLDER(IDK1);
    FUNCTION_HOLDER(IDK2);
    FUNCTION_HOLDER(CBaseClientState_HandleDeferredConnection);
    FUNCTION_HOLDER(CBaseClientState_SendConnectPacket);
    FUNCTION_HOLDER(CSteam3Client_OnGameServerChangeRequested);
    FUNCTION_HOLDER(SNP_SendMessage);
    FUNCTION_HOLDER(CClientModeShared_LevelInit);
    FUNCTION_HOLDER(DrawViewModels);
    FUNCTION_HOLDER(RenderView);
    FUNCTION_HOLDER(DrawUnderWaterOverlay);
    FUNCTION_HOLDER(UnknownViewRenderFunc);
    FUNCTION_HOLDER(UnknownViewRenderFunc2);
    FUNCTION_HOLDER(SysSession);
    FUNCTION_HOLDER(AntiIPGrabFool)
    FUNCTION_HOLDER(InternalRecievedP2PSignal);
    FUNCTION_HOLDER(SteamP2PHandler);
    FUNCTION_HOLDER(PerformScreenOverlay);
    FUNCTION_HOLDER(UpdateToneMapScalar);

    FUNCTION_HOLDER(CheckInitICE); // E8 ? ? ? ? 83 BB ? ? ? ? ? 75 4D
    FUNCTION_HOLDER(ConnectionP2P_ProcessSignal); // E8 ? ? ? ? 8A D8 E9 ? ? ? ? 68 ? ? ? ? 
    FUNCTION_HOLDER(CL_IsPaused);

    uintptr_t KeyValues_GetName;
    uintptr_t KeyValues_GetFloat;
    uintptr_t KeyValues_GetString;
    uintptr_t KeyValues_GetInt;
    uintptr_t oHandleSessionRequest;

    uintptr_t** m_pPlayerResource;



    HandleMatchStartFn HandleMatchStart;
    CreateMatchSessionFn CreateMatchSession; 

    uint8_t* MatchSingleton;

    uintptr_t CBaseClient_GetSendFrame;

    uintptr_t bf_write_WriteUBitLong;
    uintptr_t bf_write_WriteBits;
    uintptr_t bf_write_String;
    uintptr_t GetSingleton;
    uintptr_t ChatPrintf;
    //uintptr_t WatchUserInfoProcess;
    uintptr_t PossibleCallerForWatchUser;
    uintptr_t CBaseAnimatingOverlayConstructor;
    uintptr_t CBaseAnimatingConstructor;

    FUNCTION_HOLDER(WatchUserInfoProcess);
    FUNCTION_HOLDER(UnknownGOTVFunction2);
    FUNCTION_HOLDER(SVCMsg_VoiceData);
    FUNCTION_HOLDER(CNetChan_ProcessMessages);
    FUNCTION_HOLDER(CPrediction_RunSimulation);
    uintptr_t watchInfoGlobal;

   //uintptr_t UIComponentMatchInfoCreator;

    Entity** gameRules;

    uintptr_t CUtlVector_AddToTail;

    //CL_MoveFn* CL_MoveCall;
    Voice_RecordStartFn Voice_RecordStart;
    MemAlloc* memalloc;

    IViewRenderBeams* renderBeams;

    ActiveChannels* activeChannels;
    Channel* channels;

    void* globalMatchInfo;
    uintptr_t CreateMatchInfoUIComponent;
 
    std::uintptr_t returnSequenceLocation();
    static std::uintptr_t findPattern_ex(const wchar_t* wmodule, const char* pattern, std::string funcName = "") noexcept {
        return findPattern(wmodule, pattern, funcName);
    }






private:
    static std::uintptr_t findPattern(const wchar_t* wmodule, const char* pattern, std::string funcName = "") noexcept
    {
        static auto id = 0;
        ++id;

        if (HMODULE moduleHandle = GetModuleHandleW(wmodule)) {
            if (MODULEINFO moduleInfo; GetModuleInformation(GetCurrentProcess(), moduleHandle, &moduleInfo, sizeof(moduleInfo))) {
                auto start = static_cast<const char*>(moduleInfo.lpBaseOfDll);
                const auto end = start + moduleInfo.SizeOfImage;

                auto first = start;
                auto second = pattern;

                while (first < end && *second) {
                    if (*first == *second || *second == '?') {
                        ++first;
                        ++second;
                    } else {
                        first = ++start;
                        second = pattern;
                    }
                }

                if (!*second)
                    return reinterpret_cast<std::uintptr_t>(start);
            }
        }
        std::string pat{ pattern };
        MessageBoxA(NULL, ("Failed to find pattern #" + std::to_string(id) + '!' + " " + funcName + " " + pat).c_str(), "Harpoon", MB_OK | MB_ICONWARNING);
        return 0;
    }

    static std::uintptr_t GetCallOffset(DWORD offset)
    {
        auto call = *(std::uintptr_t*)((std::uintptr_t)offset + 1);
        return (std::uintptr_t)((std::uintptr_t)offset + call + 5);
    }

    static std::uintptr_t PatternScanFromCall(const wchar_t* wmodule, const char* signature, std::uintptr_t offset = 0)
    {
        auto pat_offset = (std::uintptr_t)findPattern(wmodule, signature);
        return GetCallOffset(pat_offset) + offset;
    }


};

inline std::unique_ptr<const Memory> memory;
