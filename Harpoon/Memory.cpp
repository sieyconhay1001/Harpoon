#include "Interfaces.h"
#include "Memory.h"
#include "SDK/OsirisSDK/LocalPlayer.h"


#include "MemAlloc.h"



template <typename T>
static constexpr auto relativeToAbsolute(uintptr_t address) noexcept
{
    return (T)(address + 4 + *reinterpret_cast<std::int32_t*>(address));
}
std::uintptr_t Memory::returnSequenceLocation() {
    return findPattern(L"client", "\x55\x8B\xEC\x53\x8B\x5D\x08\x56\x8B\xF1\x83");
}


#define ACTUAL_ADDRESS_IN_FIRST_OPCODE(VAR, DLL, STRPATTERN) VAR = relativeToAbsolute<uintptr_t>(findPattern(L##DLL, STRPATTERN, #VAR) + 1);

Memory::Memory() noexcept
{
    present = findPattern(L"gameoverlayrenderer", "\xFF\x15????\x8B\xF8\x85\xDB") + 2;
    reset = findPattern(L"gameoverlayrenderer", "\xC7\x45?????\xFF\x15????\x8B\xF8") + 9;

    clientMode = **reinterpret_cast<ClientMode***>((*reinterpret_cast<uintptr_t**>(interfaces->client))[10] + 5);
    input = *reinterpret_cast<Input**>((*reinterpret_cast<uintptr_t**>(interfaces->client))[16] + 1);
    globalVars = **reinterpret_cast<GlobalVars***>((*reinterpret_cast<uintptr_t**>(interfaces->client))[11] + 10);
    glowObjectManager = *reinterpret_cast<GlowObjectManager**>(findPattern(L"client", "\x0F\x11\x05????\x83\xC8\x01") + 3);
    disablePostProcessing = *reinterpret_cast<bool**>(findPattern(L"client", "\x83\xEC\x4C\x80\x3D") + 5);
    loadSky = relativeToAbsolute<decltype(loadSky)>(findPattern(L"engine", "\xE8????\x84\xC0\x74\x2D\xA1") + 1);
    setClanTag = reinterpret_cast<decltype(setClanTag)>(findPattern(L"engine", "\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15"));
    lineGoesThroughSmoke = relativeToAbsolute<decltype(lineGoesThroughSmoke)>(findPattern(L"client", "\xE8????\x8B\x4C\x24\x30\x33\xD2") + 1);
    cameraThink = findPattern(L"client", "\x85\xC0\x75\x30\x38\x86");
    acceptMatch = reinterpret_cast<decltype(acceptMatch)>(findPattern(L"client", "\x55\x8B\xEC\x83\xE4\xF8\x8B\x4D\x08\xBA????\xE8????\x85\xC0\x75\x12"));
    getSequenceActivity = reinterpret_cast<decltype(getSequenceActivity)>(findPattern(L"client", "\x55\x8B\xEC\x53\x8B\x5D\x08\x56\x8B\xF1\x83"));
    isOtherEnemy = relativeToAbsolute<decltype(isOtherEnemy)>(findPattern(L"client", "\x8B\xCE\xE8????\x02\xC0") + 3);
    auto temp = reinterpret_cast<std::uintptr_t*>(findPattern(L"client", "\xB9????\xE8????\x8B\x5D\x08") + 1);
    hud = *temp;
    findHudElement = relativeToAbsolute<decltype(findHudElement)>(reinterpret_cast<uintptr_t>(temp) + 5);
    clearHudWeapon = reinterpret_cast<decltype(clearHudWeapon)>(findPattern(L"client", "\x55\x8B\xEC\x51\x53\x56\x8B\x75\x08\x8B\xD9\x57\x6B\xFE\x2C"));
    itemSystem = relativeToAbsolute<decltype(itemSystem)>(findPattern(L"client", "\xE8????\x0F\xB7\x0F") + 1);
    setAbsOrigin = relativeToAbsolute<decltype(setAbsOrigin)>(findPattern(L"client", "\xE8????\xEB\x19\x8B\x07") + 1);
    listLeaves = findPattern(L"client", "\x56\x52\xFF\x50\x18") + 5;
    dispatchSound = reinterpret_cast<int*>(findPattern(L"engine", "\x74\x0B\xE8????\x8B\x3D") + 3);
    traceToExit = reinterpret_cast<decltype(traceToExit)>(findPattern(L"client", "\x55\x8B\xEC\x83\xEC\x30\xF3\x0F\x10\x75"));
    viewRender = **reinterpret_cast<ViewRender***>(findPattern(L"client", "\x8B\x0D????\xFF\x75\x0C\x8B\x45\x08") + 2);
    drawScreenEffectMaterial = relativeToAbsolute<uintptr_t>(findPattern(L"client", "\xE8????\x83\xC4\x0C\x8D\x4D\xF8") + 1);
    submitReport = reinterpret_cast<decltype(submitReport)>(findPattern(L"client", "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x28\x8B\x4D\x08"));
    fakePrime = reinterpret_cast<std::uint8_t*>(findPattern(L"client", "\x17\xF6\x40\x14\x10") - 1);
    const auto tier0 = GetModuleHandleW(L"tier0");
    debugMsg = reinterpret_cast<decltype(debugMsg)>(GetProcAddress(tier0, "Msg"));
    conColorMsg = reinterpret_cast<decltype(conColorMsg)>(GetProcAddress(tier0, "?ConColorMsg@@YAXABVColor@@PBDZZ"));
    vignette = *reinterpret_cast<float**>(findPattern(L"client", "\x0F\x11\x05????\xF3\x0F\x7E\x87") + 3) + 1;
    equipWearable = reinterpret_cast<decltype(equipWearable)>(findPattern(L"client", "\x55\x8B\xEC\x83\xEC\x10\x53\x8B\x5D\x08\x57\x8B\xF9"));
    predictionRandomSeed = *reinterpret_cast<int**>(findPattern(L"client", "\x8B\x0D????\xBA????\xE8????\x83\xC4\x04") + 2);
    moveData = **reinterpret_cast<MoveData***>(findPattern(L"client", "\xA1????\xF3\x0F\x59\xCD") + 1);
    moveHelper = **reinterpret_cast<MoveHelper***>(findPattern(L"client", "\x8B\x0D????\x8B\x45?\x51\x8B\xD4\x89\x02\x8B\x01") + 2);
    keyValuesFromString = relativeToAbsolute<decltype(keyValuesFromString)>(findPattern(L"client", "\xE8????\x83\xC4\x04\x89\x45\xD8") + 1);
    keyValuesFindKey = relativeToAbsolute<decltype(keyValuesFindKey)>(findPattern(L"client", "\xE8????\xF7\x45") + 1);
    keyValuesSetString = relativeToAbsolute<decltype(keyValuesSetString)>(findPattern(L"client", "\xE8????\x89\x77\x38") + 1);
    PerformScreenOverlay = findPattern(L"client", "\x55\x8B\xEC\x51\xA1????\x53\x56\x8B\xD9");


    ACTUAL_ADDRESS_IN_FIRST_OPCODE(KeyValues_GetFloat, "matchmaking", "\xE8????\x8B\x06\x8D\x4D\x08");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(KeyValues_GetName, "engine", "\xE8????\x8D\x50\x01")
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(KeyValues_GetString, "matchmaking", "\xE8????\x89\x45\x18");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(KeyValues_GetInt, "matchmaking", "\xE8????\x6A\x04");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(HostRunFrameRender, "engine", "\xE8????\xE8????\x80\x3D?????\x74\x05");

    weaponSystem = *reinterpret_cast<WeaponSystem**>(findPattern(L"client", "\x8B\x35????\xFF\x10\x0F\xB7\xC0") + 2);
    getPlayerViewmodelArmConfigForPlayerModel = relativeToAbsolute<decltype(getPlayerViewmodelArmConfigForPlayerModel)>(findPattern(L"client", "\xE8????\x89\x87????\x6A\x00") + 1);
    getEventDescriptor = relativeToAbsolute<decltype(getEventDescriptor)>(findPattern(L"engine", "\xE8????\x8B\xD8\x85\xDB\x75\x27") + 1);
    activeChannels = *reinterpret_cast<ActiveChannels**>(findPattern(L"engine", "\x8B\x1D????\x89\x5C\x24\x48") + 2);
    channels = *reinterpret_cast<Channel**>(findPattern(L"engine", "\x81\xC2????\x8B\x72\x54") + 2);
    setAbsAngle = reinterpret_cast<decltype(setAbsAngle)>(reinterpret_cast<DWORD*>(findPattern(L"client", "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x64\x53\x56\x57\x8B\xF1")));
    CreateState = findPattern(L"client", "\x55\x8B\xEC\x56\x8B\xF1\xB9????\xC7\x46");
    UpdateState = findPattern(L"client", "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x18\x56\x57\x8B\xF9\xF3\x0F\x11\x54\x24");
    InvalidateBoneCache = findPattern(L"client", "\x80\x3D?????\x74\x16\xA1????\x48\xC7\x81");

    memalloc = *reinterpret_cast<MemAlloc**>(GetProcAddress(GetModuleHandleA("tier0.dll"), "g_pMemAlloc"));


    staticPropManager = reinterpret_cast<StaticPropMgr*>(findPattern(L"engine", "\xA1????\xB9????\x56\x8B\x40\x30"));
    renderBeams = *reinterpret_cast<IViewRenderBeams**>(findPattern(L"client", "\xB9????\xA1????\xFF\x10\xA1????\xB9" + 1));
    clientState = **reinterpret_cast<ClientState***>(findPattern(L"engine", "\xA1????\x8B\x80????\xC3") + 1);
    WriteUsercmdDeltaToBufferReturn = *(reinterpret_cast<void**>(findPattern(L"engine", "\x84\xC0\x74\x04\xB0\x01\xEB\x02\x32\xC0\x8B\xFE\x46\x3B\xF3\x7E\xC9\x84\xC0\x0F\x84????")));


    WriteUsercmd = findPattern(L"client", "\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x8B\xD9\x8B\x0D");
    RenderModels = findPattern(L"client", "\x55\x8B\xEC\x83\xE4\xF8\x51\x8B\x45\x18");

    ModelRenderDrawModels = findPattern(L"client", "\x55\x8B\xEC\x83\xEC\x4C\x83\x7D\x0C\x00");

    NET_TickMsgHandler = findPattern(L"engine", "\x55\x8B\xEC\x53\x56\x8B\xF1\x8B\x0D????\x57");
    CL_FlushEntityPacket = relativeToAbsolute<std::uintptr_t>(findPattern(L"engine", "\xE8????\x8B\x0D????\x83\xC4\x08\x81\xF9????") + 1);


        
    //DrawWorld = reinterpret_cast<std::uintptr_t>(*((LPVOID**)findPattern(L"client", "\xE8????\x8B\x45\xF4\x89\x83????" + 1)));

    DrawWorldLists = findPattern(L"engine","\x55\x8B\xEC\x80\x3D?????\x75\x5D" ); //"\x55\x8B\xEC\x80\x3D?????\x75\x5D"); // "\x55\x8B\xEC\x53\x8B\x5D\x10\x56\x8B\xF1\x84\xDB"
    localPlayer.init(*reinterpret_cast<Entity***>(findPattern(L"client", "\xA1????\x89\x45\xBC\x85\xC0") + 1));
    //CL_MoveCall = reinterpret_cast<CL_MoveFn*>(findPattern(L"engine", "\x55\x8B\xEC\xA1????\x81\xEC????\xB9????\x53\x8B\x98"));
    Voice_RecordStart = reinterpret_cast<Voice_RecordStartFn>(findPattern(L"engine", "\x55\x8B\xEC\x83\xEC\x0C\x83\x3D?????\x56\x57"));

    NET_BufferToBufferCompress = relativeToAbsolute<std::uintptr_t>(findPattern(L"engine", "\xE8????\x83\xC4\x08\x84\xC0\x74\x3E") + 1);
    CNetChan_CompressFragments = relativeToAbsolute<std::uintptr_t>(findPattern(L"engine", "\xE8????\x8B\xCB\xE8????\x33\xD2") + 1);
    CNetChan_CreateFragmentsFromBuffer = relativeToAbsolute<std::uintptr_t>(findPattern(L"engine", "\xE8????\xC7\x46?????\xC6\x46\x10\x00\x6A\x20") + 1);
    CNetChan_SendSubChannelData = relativeToAbsolute<std::uintptr_t>(findPattern(L"engine", "\xE8????\x84\xC0\x74\x05\x80\x4C\x24??") + 1);
    CNetChan_SendTcpData = relativeToAbsolute<std::uintptr_t>(findPattern(L"engine", "\xE8????\x33\xD2\x8D\x83????") + 1);
    CNetChan_SetTimeout = findPattern(L"engine", "\x55\x8B\xEC\x80\x7D\x0C\x00\xF3\x0F\x10\x4D?");
    CNetChan_SendNetMessage = findPattern(L"engine", "\x55\x8B\xEC\x83\xEC\x08\x56\x8B\xF1\x8B\x86????\x85\xC0", "CNetChan_SendNetMessage");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(CNetChan_ProcessMessages, "engine", "\xE8????\x8B\x16\x8B\xCE\x8A\xD8\xFF\x92????\x5F");


    NET_SendToImpl = relativeToAbsolute<std::uintptr_t>(findPattern(L"engine", "\xE8????\x83\xC4\x0C\x8D\x4B\x08", "NET_SendToImpl") + 1);
    Calls_SendToImpl = relativeToAbsolute<std::uintptr_t>(findPattern(L"engine", "\xE8????\x83\xC4\x08\xFF\x45\xF8", "Calls_SendToImpl") + 1);
    RequestFile = (RequestFileFn)findPattern(L"engine", "\x55\x8B\xEC\x83\xEC\x3C\x56\x8B\xF1\xFF\x86????\x8B\x0D????", "RequestFile");
    EnqueVeryLargeAsyncTransfer = (EnqueAsyncFun)findPattern(L"engine", "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x4C\x53\x56\x57\x8B\xF9\x8B\x07", "EnqueVeryLargeAsyncTransfer");
        
    

    CBaseClient_GetSendFrame = relativeToAbsolute<std::uintptr_t>(findPattern(L"engine", "\xE8????\x85\xC0\x74\x11\x8B\x0E", "CBaseClient_GetSendFrame") + 1);

    bf_write_WriteUBitLong = relativeToAbsolute<std::uintptr_t>(findPattern(L"engine", "\xE8????\x8D\x43\x04", "bf_write_WriteUBitLong")+1);
    bf_write_WriteBits = relativeToAbsolute<std::uintptr_t>(findPattern(L"engine", "\xE8????\x0F\xB6\x45\x0C", "bf_write_WriteBits") + 1);
    bf_write_String = relativeToAbsolute<std::uintptr_t>(findPattern(L"engine", "\xE8????\x6A\x40\x8D\x55\x88", "bf_write_String") + 1);

    SendClientMessages = relativeToAbsolute<std::uintptr_t>(findPattern(L"engine", "\xE8????\x8B\x3D????\x47", "SendClientMessages") + 1);  
    CL_SendMove_ = relativeToAbsolute<std::uintptr_t>(findPattern(L"engine", "\xE8????\x84\xDB\x0F\x84????\x8B\x8F????", "CL_SendMove_") + 1);

    CL_IsPaused = findPattern(L"engine", "\x80\xB9?????\x75\x62");



    ACTUAL_ADDRESS_IN_FIRST_OPCODE(NET_SendPacket, "engine", "\xE8????\xC7\x87????????\x83\xC4\x0C");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(CBaseClientState_SVCMsg_PacketEntities, "engine", "\xE8????\x8B\x15????\xB9????\xFF\xB4\x24????");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(CL_CopyExistingEntity, "engine", "\xE8????\x8B\x4F\x04\x85\xC9\x74\xE5");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(CL_CopyNewEntity, "engine", "\xE8????\x83\xC4\x04\xBE????\x39\x5F\x10");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(NET_SendLong, "engine", "\xE8????\x83\xC4\x08\x8B\xF0\x83\xBD?????");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(SNP_GetNextThinkTime, "steamnetworkingsockets", "\xE8????\x3B\xD3\x7C\x0E");

    SNP_SendMessage = findPattern(L"steamnetworkingsockets", "\x55\x8B\xEC\x83\xEC\x10\x53\x56\x57\x8B\xF9\x6A\x00", "CSteamNetworkConnectionBase__SNP_SendMessage");

   // ACTUAL_ADDRESS_IN_FIRST_OPCODE(CSteamNetworkConnectionBase__SNP_ClampSendRate, "steamnetworkingsockets", "\xE8????\x99\x52");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(ConnectionProblemLocally, "steamnetworkingsockets", "\xE8????\x83\xC4\x0C\xEB\x3C");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(NET_CreateNetChannel, "engine", "\xE8????\x83\xC4\x10\xB9????");

    ACTUAL_ADDRESS_IN_FIRST_OPCODE(IDK1, "client", "\xE8????\x8B\x07\x83\xC4\x10\x8B\x5D\x0C");
    //ACTUAL_ADDRESS_IN_FIRST_OPCODE(IDK2, "client", "\xE8????\x83\xC4\x18\x84\xC0\x75\x36");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(CBaseClientState_HandleDeferredConnection, "engine", "\xE8????\x5F\x5E\x8B\xE5\x5D\xC2\x04\x00\x8D\x86????");

    ACTUAL_ADDRESS_IN_FIRST_OPCODE(GetSingleton, "client", "\xE8????\x8B\xC8\xE8????\xEB\x22");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(WatchUserInfoProcess, "client", "\xE8????\x8D\x4C\x24\x14\xE8????\xFF\x15????");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(oHandleSessionRequest, "steamnetworkingsockets", "\xE8????\xFF\x35????\xB9????");


    ACTUAL_ADDRESS_IN_FIRST_OPCODE(CheckConnectionStateOrScheduleWakeUp, "steamnetworkingsockets", "\xE8????\x85\xF6\x74\x0E\x8B\xCE");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(oInternalMinThinkTime, "steamnetworkingsockets", "\xE8????\x83\xFF\x03");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(AssertHeldByCurrentThread, "steamnetworkingsockets", "\xE8????\x8B\x5D\x08\x8B\x43\x04");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(PendingBytesTotal, "steamnetworkingsockets", "\xE8????\x3B\x38");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(InternalRecievedP2PSignal, "steamnetworkingsockets", "\xE8????\x8D\x4D\x8C\xE8????\x8D\xA5????");

    ACTUAL_ADDRESS_IN_FIRST_OPCODE(CBaseAnimatingConstructor, "client", "\xE8????\x80\x7D\x08\x01");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(CBaseAnimatingOverlayConstructor, "client", "\xE8????\xC7\x06????\xB9????");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(UnknownViewRenderFunc, "client", "\xE8????\x8D\x44\x24\x37");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(UnknownViewRenderFunc2, "client", "\xE8????\x80\xBF?????\x74\x50\x8B\x0D????\x8B\x01\xFF\x90????");

    ACTUAL_ADDRESS_IN_FIRST_OPCODE(DrawViewModels, "client", "\xE8????\x8B\x0D????\x8B\x43\x10");


    ACTUAL_ADDRESS_IN_FIRST_OPCODE(SysSession, "matchmaking", "\xE8????\x5F\x5E\x5B\x8B\xE5\x5D\xC2\x10\x00\xBA????\x8B\xCE\xE8????\x85\xC0\x75\x3E");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(AntiIPGrabFool, "matchmaking", "\xE8????\x85\xFF\x74\x11");

    ACTUAL_ADDRESS_IN_FIRST_OPCODE(ConnectionP2P_ProcessSignal, "steamnetworkingsockets", "\xE8????\x8A\xD8\xE9????\x68????");
    ACTUAL_ADDRESS_IN_FIRST_OPCODE(UpdateToneMapScalar, "client", "\xE8????\xFF\x74\x24\x40\x8D\x45\x10");

    CPrediction_RunSimulation = findPattern(L"client", "\x55\x8B\xEC\x83\xEC\x08\x53\x8B\x5D\x10\x56\x57\xFF\x75\x0C\x8B\xF1\xF3\x0F\x11\x55", "CPrediction_RunSimulation");

    //SteamP2PHandler = findPattern(L"client", "\x55\x8B\xEC\x6A\xFF\x68????\x64\xA1????\x50\x64\x89\x25????\x81\xEC????\x8B\xC1\x53\x56", "SteamP2PHandler");

    RenderView = findPattern(L"client", "\x55\x8B\xEC\x83\xE4\xC0\x81\xEC????\x53\x56\x57\x8B\xF9\x89\x7C\x24\x44");
    DrawUnderWaterOverlay = findPattern(L"client", "\x57\xE8????\x8B\xF8\x85\xFF\x74\x34");
    if (CPrediction_RunSimulation)
    {
        OutputDebugStringA(std::to_string(CPrediction_RunSimulation).c_str());
    }
    else {
        throw std::exception();
    }


    CBaseClientState_SendConnectPacket = findPattern(L"engine", "\x55\x8B\xEC\x83\xE4\xF8\x81\xEC????\x53\x56\x57\x68????\x8B\xF9\xFF\x15????\x8B\x5D\x08", "CBaseClientState_SendConnectPacket");
    CSteam3Client_OnGameServerChangeRequested = findPattern(L"engine", "\x55\x8B\xEC\x8B\x0D????\x85\xC9\x74\x05", "CSteam3Client_OnGameServerChangeRequested");

    gameRules = *reinterpret_cast<Entity***>(findPattern(L"client", "\x8B\xEC\x8B\x0D????\x85\xC9\x74\x07", "gameRules") + 4);
    PossibleCallerForWatchUser = findPattern(L"client", "\x55\x8B\xEC\x83\xEC\x14\xC7\x45?????\xC7\x45?????\xC7\x45?????\xC7\x45?????\xC7\x45?????\xE8????\xFF\x75\x08\x8D\x4D\xEC\x89\x45\xFC\xE8????\x8B\x45\xFC\xFF\x70\x0C", "PossibleCallerForWatchUser");
    //ACTUAL_ADDRESS_IN_FIRST_OPCODE(RATLIM_ReturnAddress, "steamnetworkingsockets","\xE8????\x8B\x4D\x10\x83\xBF?????");
    RATLIM_ReturnAddress = findPattern(L"steamnetworkingsockets", "\x8B\xCA\x8B\xF0\x8B\x55\x10", "RATLIM_ReturnAddress");



    SVCMSG_PacketEntities = findPattern(L"engine", "\x55\x8B\xEC\x56\x57\x8B\xF1\xE8????\x8B\x7D\x08", "SVCMSG_PacketEntities");

    watchInfoGlobal = (uintptr_t)(findPattern(L"client", "\x83\xC4\x0C\xB9????\xFF\x75\xFC", "watchInfoGlobal") + 1);

    HOST_ERROR = relativeToAbsolute<std::uintptr_t>(findPattern(L"engine", "\xE8????\x83\xC4\x04\xEB\x7A", "HOST_ERROR") + 1);

    GetPredFrame = relativeToAbsolute<GetPredictedFrameFn>(findPattern(L"client", "\xE8????\x8B\xC8\x89\x4C\x24\x18\x85\xC9\x0F\x84????\x80\xBF?????", "GetPredFrame") + 1);
    SaveData = relativeToAbsolute<SaveDataFn>(findPattern(L"client", "\xE8????\x6B\x45\x08\x34", "SaveData") + 1);
    RestoreData = relativeToAbsolute<SaveDataFn>(findPattern(L"client", "\xE8????\x6A\x01\xEB\x02\x6A\x02", "RestoreData") + 1);
    ProcessConnectionless = findPattern(L"engine", "\x55\x8B\xEC\x83\xE4\xF0\x81\xEC????\x56\x57\x8B\xF9\x8B\x4D\x08", "ProcessConnectionless");
      
    CL_PreprocessEntities = findPattern(L"engine", "\x80\x3D?????\x56\x75\x14", "CL_PreprocessEntities");
    MatchSingleton = *(uint8_t**)(findPattern(L"client.dll", "\xA1????\x85\xC0\x75\x2F\xA1????\x68????\x8B\x08\x8B\x01\xFF\x50\x04\x85\xC0\x74\x13\x8B\xC8\xE8????\xA3????\x8B\xC8\xE9????\x33\xC0\xA3????\x8B\xC8\xE9????\xCC\x55\x8B\xEC\x8B\x4D\x08", "MatchSingleton") + 1);
    HandleMatchStart = (HandleMatchStartFn)findPattern(L"client", "\x55\x8B\xEC\x51\x53\x56\x8B\xF1\x8B\x0D????\x57\x8B\x01", "HandleMatchStart");
    CreateMatchSession = relativeToAbsolute<CreateMatchSessionFn>(findPattern(L"client", "\xE8????\x83\xEC\x14\xE8????", "CreateMatchSession") + 1);

    HOST_RUN_FRAME_INPUT = relativeToAbsolute<uintptr_t>(findPattern(L"engine", "\xE8????\xF3\x0F\x10\x45?\xF2\x0F\x10\x4D?", "HOST_RUN_FRAME_INPUT") + 1);
    CNetChan_SendDatagram = findPattern(L"engine", "\x55\x8B\xEC\x83\xE4\xF0\xB8????\xE8????\x56\x57\x8B\xF9\x89\x7C\x24\x18", "CNetChan_SendDatagram");



    ChatPrintf = findPattern(L"client", "\x55\x8B\xEC\xB8????\xE8????\x8B\x0D????\x8B\x01\xFF\x90????\x85\xC0\x74\x0A\x80\x78\x10\x00\x0F\x85????\x8D\x45\x18");
    
   
    HOST_VERSION = *(int32_t*)findPattern(L"engine", "\xA1????\x50\xFF\x77\x08", "HOST_VERSION");

    CUtlVector_AddToTail = relativeToAbsolute<std::uintptr_t>(findPattern(L"engine", "\xE8????\x8D\x3C\x1E", "CUtlVector_AddToTail") + 1);


    UnknownGOTVFunction2 = findPattern(L"client", "\x55\x8B\xEC\x81\xEC????\x53\x56\x57\x8B\x7D\x08\x89\x4D\xFC", "UnknownGOTVFunction2");
    globalMatchInfo = (void*)(findPattern(L"client", "\xE8????\x6A\x1A", "globalMatchInfo") + 0x13B2);
    CreateMatchInfoUIComponent = findPattern(L"client", "\xA1????\x85\xC0\x75\x52", "CreateMatchInfoUIComponent");

    CClientModeShared_LevelInit = findPattern(L"client", "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x20\x56\x57\x8B\xF9\x8B\x4F\x18", "CClientModeShared_LevelInit");

    //std::uintptr_t NetDCCall = findPattern(L"engine", "\xE8????\x83\xC4\x08\x84\xC0\x74\x3E");
    //NET_BufferToBufferCompress = (NetDCCall + 1) + *reinterpret_cast<std::uintptr_t*>(NetDCCall + 1) + sizeof(std::uintptr_t);


    using _fn = void(__thiscall*)(uintptr_t, uint64_t);
    static const auto this_ = *(uintptr_t**)(findPattern(L"client", "\x83\x3D?????\x74\x5E\x8B\x0D????\x68", "this") + 2);
    static const auto fn = (_fn)findPattern(L"client", "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x1C\x53\x56\x57\x8B\xF1\xFF\x15????\x8B", "fn");

    static const auto collection = *(uint32_t**)(findPattern(L"client", "\x8B\x35????\x66\x3B\xD0\x74\x07","collection") + 2);

    
    CheckForSequenceChange = findPattern(L"client", "\x55\x8B\xEC\x51\x53\x8B\x5D\x08\x56\x8B\xF1\x57\x85", "CheckForSequenceChange");
    ProceduralFootPlant = findPattern(L"client", "\x55\x8B\xEC\x83\xE4\xF0\x83\xEC\x78\x56\x8B\xF1\x57\x8B\x56", "ProceduralFootPlant");;
    ShouldSkipAnimationFrame = findPattern(L"client", "\x57\x8B\xF9\x8B\x07\x8B\x80????\xFF\xD0\x84\xC0\x75\x02", "ShouldSkipAnimationFrame");


    m_pPlayerResource = *(uintptr_t***)(findPattern(L"client", "\x74\x30\x8B\x35????\x85\xF6") + 4);

    if (!interfaces->engine) {
        std::exception("No Engine Interface!");
    }
    cclientstate = **reinterpret_cast<ClientState***>((*reinterpret_cast<uintptr_t**>(interfaces->engine))[12] + 0x10);

}
