#include "COMPILERDEFINITIONS.h"
#include "Timing.h"
#include "Memory.h"
#include "Interfaces.h"
#include "Hacks/OTHER/Debug.h"
#include "SDK/OsirisSDK/NetworkChannel.h"
#include "SDK/OsirisSDK/LocalPlayer.h"
#include "SDK/OsirisSDK/Entity.h"
#include "Hacks/TickbaseManager.h"
#include "Other/AutoQueuer.h"
#include "EngineHooks.h"
#include "ClientHooks.h"
#include "SDK/OsirisSDK/GlobalVars.h"
//#include "Hacks/SteamWorks/SteamWorks.h"
/*
   It's not really "correct" to bind deltaTick to time as it is dependant on server tick & getting packetentities, but alas this is the best
   I could come up with.
																																				*/

/*

  You may refer to this section as "ANTI-SHARKLASER"

*/
static bool g_bRecievedNonDelta{ true };
int tickAt = 0;
static bool g_bWasSet{ false };
static void* netChannel = nullptr;


void Timing::Reset() { /* RESET EVERYTHING */
	Debug::QuickPrint("Timing::Reset");
	if (localPlayer.get())
		localPlayer->flags() &= ~FROZEN;

	static auto drawotherVar{ interfaces->cvar->findVar("cl_flushentitypacket") };
	drawotherVar->onChangeCallbacks.size = 0;
	drawotherVar->setValue(1);
	static auto rate{ interfaces->cvar->findVar("rate") };
	rate->onChangeCallbacks.size = 0;
	rate->setValue(9999999);


	config->mmlagger.enabled = false;
	config->mmlagger.shouldLag = false;
	config->mmcrasher.enabled = false;
	config->mmcrasher.annoyance = false;
	config->mmcrasher.cooldownMaker = false;
	ExploitTiming.m_nActivateTick = 0;
	ExploitTiming.m_fFreezeTime = 0.f;
	ExploitTiming.m_fFreezeTimeLeft = 0.f;
	ExploitTiming.m_bForceDoubleSpeed = false;
	ExploitTiming.m_bForceReturnDeltaFix = false;
	ExploitTiming.m_bStopAllDatagramsIn = false;
	ExploitTiming.m_bNetworkedFreeze = false;
	TimingSet_s.m_bInPredictionMode = false;
	ExploitTiming.nNetworkedDeltaTick = false;
	TimingInfo_s.m_fLastNetTickUpdate = -1.f;
	TimingInfo_s.m_nLastTickValueRecieved = -1;
	TimingSet_s.m_nOutDeltaTick = -1;
	TimingSet_s.m_nLastValidDeltaTick = -1;
	TimingStats.m_fAverageTickDeltaTime = -1.f;
	TimingStats.m_flAverageServerFrametime = -1.f;
	TimingStats.m_fLongestNetTickTime = -1.f;
	TimingStats.m_fPreviousPauseMax = -1.f;
	TimingStats.m_fServerComputationTime = -1.f;
	TimingStats.m_nPreviousPauseMaxticks = -1;
	TimingStats.m_fAverageTickDeltaTime = -1;
	ExploitTiming.m_nTicksLeftToDrop = 1;
	TimingStats.m_vTickStats.clear();
	g_bRecievedNonDelta = true;
	g_bWasSet = false;
	nHighestDeltaTick = -1;
	tickAt = 0;
}



#include "Other/CrossCheatTalk.h"
#include "Other/EnginePrediction.h"
#include "SDK/OsirisSDK/Prediction.h"
#include "SDK/OsirisSDK/GameMovement.h"


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

#include "Hacks/SteamWorks/SteamWorks.h"
#include <steam/isteammatchmaking.h>
#include <steam/isteamnetworking.h>

template <class T, class ... Types>
extern __forceinline T CallVirtualFunction(void* _this, int nIndex, Types ... vals);




void __fastcall OnEvent(void* ecx, void* edx, KeyValues* pEvent) {

	//pEvent->findKey()

}



extern bool g_bAntiIP;
extern bool g_bRenderFrame;

typedef void(__cdecl* HRFIOrg)(float accumulated_extra_samples, bool bFinalTick);
void __cdecl Timing::_Host_RunFrame_Input_hk(void*, void*, float accumulated_extra_samples, bool bFinalTick) {

	if (config->debug.HSpam) {
		for (int i = 0; i < 500; i++) {
			interfaces->engine->clientCmdUnrestricted("say \".hs Fuck You!\"");
		}
	}


	static char* g_pNumTimesSentAddr{ ((char*)Memory::findPattern_ex(L"client", "\x0F\x85????\xFF\x05????") + int(8)) };
	int* pNumTimesSent = *(int**)(g_pNumTimesSentAddr);
	if (*pNumTimesSent > 0)
	{
		VCON("\ng_pNumTimesSent Increased To : %d\n", *pNumTimesSent);
		*pNumTimesSent = 0;
	}


	static char* g_pNumDLLsInjectedAddr{ ((char*)Memory::findPattern_ex(L"client", "\x83\xC4\x0C\xFF\x05????\x5F") + int(5)) };
	int* g_pNumDLLsInjected = *(int**)(g_pNumDLLsInjectedAddr);
	if (*g_pNumDLLsInjected > 0)
	{
		VCON("\ng_pNumDLLsInject Increased To : %d\n", *g_pNumDLLsInjected);
		*g_pNumDLLsInjected = 0;
	}




	//
	g_bAntiIP = config->debug.allowcalloriginal;
	g_bRenderFrame = !config->debug.bNoRender;

	if (!g_bRenderFrame && GetAsyncKeyState(VK_NUMLOCK))
	{
		ConVar* fps_max = interfaces->cvar->findVar("fps_max");
		fps_max->setValue("65");
		g_bRenderFrame = true;
		config->debug.bNoRender = false;
	}
	else if (!g_bRenderFrame)
	{
		ConVar* fps_max = interfaces->cvar->findVar("fps_max");
		fps_max->setValue("8");
	}


#if 0
	static const auto match_framework = **reinterpret_cast<IMatchFramework***>(memory->findPattern_ex(L"client", "\x8B\x0D????\x8B\x01\xFF\x50\x2C\x8D\x4B\x18") + 0x2);

	if (match_framework)
	{
		IMatchEventsSubscription* eventsSub = interfaces->matchFramework->GetEventsSubscription();
		static IMatchEventsSubscription* pEventsSub{ nullptr };
		if (eventsSub && (eventsSub != pEventsSub))
		{
			pEventsSub = eventsSub;

	
		}

	}


	if (g_pSteamNetworking)
		g_pSteamNetworking->AllowP2PPacketRelay(false);

	if (GetAsyncKeyState(VK_NUMLOCK))
	{
		static const auto match_framework = **reinterpret_cast<IMatchFramework***>(memory->findPattern_ex(L"client", "\x8B\x0D????\x8B\x01\xFF\x50\x2C\x8D\x4B\x18") + 0x2);
		if (match_framework)
		{
			//const auto match_session = CallVirtualFunction<uintptr_t*>(match_framework, 13);
			IMatchSession* match_session = interfaces->matchFramework->GetMatchSession();



			if (!match_session) {
				CON("No Match Session!\n");
			}
			else {

				const uint64_t my_lobby_id = CallVirtualFunction<uint64_t>(match_session, 4);


				int nNumMembers = g_pSteamMatchmaking->GetNumLobbyMembers(my_lobby_id);

				VCON("ISteamMatchmaking::GetLobbyMemberData GetNumLobbyMembers Returned %d Members\n", nNumMembers);

				for (int i = 0; i < nNumMembers - 1; i++)
				{
					CSteamID PlayerID = g_pSteamMatchmaking->GetLobbyMemberByIndex(my_lobby_id, i);

					const char* IP = g_pSteamMatchmaking->GetLobbyMemberData(my_lobby_id, PlayerID, nullptr);

					if (IP)
					{
						VCON("ISteamMatchmaking::GetLobbyMemberData Return : %s for ID : %d\n", IP, PlayerID.GetAccountID());
					}

					if (i > 10)
						break;

				}

			}
		}
		else {
			CON("No MatchFramework\n");
		}
	}
#endif

	if (memory->globalVars->tickCount % 5)
		OurSteamWorks::InviteSpam();


	if (Tickbase::OnHostRunFrameInput(accumulated_extra_samples, bFinalTick))
		return;
	//

#if 1
	int cmd_index = memory->globalVars->tickCount % config->mmlagger.desyncTimer;
	int server_drop_count = config->mmlagger.desyncTimer - 8;

	//don't predict any commands the server will drop
	if (config->mmlagger.turnoffDesync)
	{
		if (cmd_index >= (config->mmlagger.desyncTimer - server_drop_count))
		{
			static int nLastRanTickCount{ 0 };
			if (nLastRanTickCount < memory->globalVars->tickCount)
			{
				nLastRanTickCount = memory->globalVars->tickCount;
				ExploitTiming.m_LastCmd.tickCount++;
				ExploitTiming.m_LastCmd.commandNumber++;
				ExploitTiming.m_LastCmd.hasbeenpredicted = false;
				EnginePrediction::run(&ExploitTiming.m_LastCmd);
				
				//MoveData moveData;
				//interfaces->prediction->RunCommand(localPlayer.get(), &ExploitTiming.m_LastCmd, memory->moveData);
			}
			return;
		}
	}
#endif



	CrossCheatTalk::g_pSteamNetSocketsNetChannel->OnNewFrame();

	if(newFrameMutex.try_lock()){
		newFrame = true;
		newFrameMutex.unlock();
	}

	AutoQueuer::Run();




#ifdef LOCAL_TIMING
#if 1
#ifdef DEV_SECRET_BUILD

	if (Timing::ExploitTiming.m_bNetworkedFreeze) {
		Timing::TimingSet_s.m_bInPredictionMode = true;
	}


	static int nLastRan{ 0 };
	static float fLastRunTime{ 0.f };
	if (config->mmlagger.teleport) {
		if (17 <  TIME_TO_TICKS(memory->globalVars->realtime - fLastRunTime)) {
			TimingSet_s.m_bInPredictionMode = true;
			ExploitTiming.m_bStopAllDatagramsIn = true;
			nLastRan = memory->clientState->m_clock_drift_mgr.m_server_tick;
			fLastRunTime = memory->globalVars->realtime;
			bool shifted = true;
			for (int i = 0; i < 7; i++) {
				memory->globalVars->tickCount++; // sv_max_usercmd_future_ticks
				((HRFIOrg)oHOSTRUNFRAMEINPUT)(accumulated_extra_samples, shifted);
			}
			for (int i = 0; i < 7 + 6; i++) {
				ClientHooks::SendZeroSizedPacket(ClientHooks::g_pNetChannel);
			}
			for (int i = 0; i < 6; i++) {
				memory->globalVars->tickCount++; // sv_maxuserprocesscmd
				((HRFIOrg)oHOSTRUNFRAMEINPUT)(accumulated_extra_samples, shifted);
			}
			TimingSet_s.m_bInPredictionMode = false;
			return; 
		}
		else {
			ExploitTiming.m_bStopAllDatagramsIn = false;
			return;
		}
	}

	static bool wasOff{ false };
	if (GetAsyncKeyState(config->TestShit.fuckKey9) || Timing::ExploitTiming.m_bNetworkedFreeze) {
		if (!config->debug.LocalTiming) {
			wasOff = true;
			config->debug.LocalTiming = true;
		}
	}
	else {
		if (wasOff && (ExploitTiming.m_fFreezeTimeLeft <= 0.f)) {
			wasOff = false;
			config->debug.LocalTiming = false;
		}
	}

	if (GetAsyncKeyState(config->TestShit.fuckKey9) || Timing::ExploitTiming.m_bNetworkedFreeze || (config->mmlagger.crimWalk && ExploitTiming.m_bForceDoubleSpeed) ) {
		NetworkChannel* netchann = interfaces->engine->getNetworkChannel();
		if (netchann && (Timing::ExploitTiming.m_fFreezeTime > 1.5f)) {
			memory->globalVars->intervalPerTick = (1.f / (64.f * (float)(config->mmlagger.ticksToSimulate)));
		}
	}
#endif


	static int nPreviousDeltaTick{ -1 };
	static float fPreviousDeltaTickUpdate{ 0.f };
	static bool init{ false };
	if (!netChannel || config->mmcrasher.enabled ||  (netChannel != interfaces->engine->getNetworkChannel()) || !config->debug.LocalTiming || (memory->clientState->deltaTick == -1) || (memory->clientState->signonState < SIGNONSTATE_FULL) || !localPlayer.get() || !localPlayer->isAlive()) {
		init = false;
		if (interfaces->engine->getNetworkChannel() || (memory->clientState->deltaTick == -1)) {
			fPreviousDeltaTickUpdate = memory->globalVars->currenttime;
			nHighestDeltaTick = -1;
			nPreviousDeltaTick = memory->clientState->deltaTick;
			TimingSet_s.m_nOutDeltaTick = nPreviousDeltaTick;
			TimingSet_s.m_nLastValidDeltaTick = nPreviousDeltaTick;
			TimingSet_s.m_bInPredictionMode = false;		
			netChannel = interfaces->engine->getNetworkChannel();
			nHighestDeltaTick = -1;
			init = true;
		}
		else if (!interfaces->engine->getNetworkChannel()) {
			fPreviousDeltaTickUpdate = 0.f;
			nPreviousDeltaTick = -1;
			Reset();
			init = false;
		}
		TimingSet_s.m_bInPredictionMode = false;
	}

	// Last thing added 4/18/2020
	if (!init || (memory->clientState->signonState < SIGNONSTATE_FULL) || !localPlayer.get() || !localPlayer->isAlive() || !config->debug.LocalTiming) {
		((HRFIOrg)oHOSTRUNFRAMEINPUT)(accumulated_extra_samples, bFinalTick);
		if (init) {	
			fPreviousDeltaTickUpdate = memory->globalVars->currenttime;
			nPreviousDeltaTick = memory->clientState->deltaTick;
		}
		return;
	}

	if ((memory->clientState->deltaTick != nPreviousDeltaTick) && !TimingSet_s.m_bInPredictionMode) {
		fPreviousDeltaTickUpdate = memory->globalVars->currenttime;
		nPreviousDeltaTick = memory->clientState->deltaTick;
		TimingSet_s.m_nLastValidDeltaTick = nPreviousDeltaTick;
	}

	TimingSet_s.m_nOutDeltaTick = TIME_TO_TICKS(memory->globalVars->currenttime - fPreviousDeltaTickUpdate) + TimingSet_s.m_nLastValidDeltaTick;

	if (((memory->globalVars->currenttime - TimingInfo_s.m_fLastNetTickUpdate) > 1.5f) || 
			((memory->globalVars->currenttime - fPreviousDeltaTickUpdate) > 1.5f) || 
			(memory->clientState->deltaTick < (TimingSet_s.m_nOutDeltaTick - 100))){

		//TimingSet_s.m_bInPredictionMode = true;
		// Don't autokick in our if a pal turns on server freeze we are  F U C K E D !!!
	} 

	memory->globalVars->intervalPerTick = (1.f / 64.f);

#if 0
	if ((memory->globalVars->currenttime - TimingInfo_s.m_fLastNetTickUpdate) < memory->globalVars->intervalPerTick) {
		if (TimingSet_s.m_bInPredictionMode) {
			TimingSet_s.m_bInPredictionMode = false;
			memory->globalVars->tickCount = -1;
		}
	}
#endif
	if (TimingSet_s.m_bInPredictionMode) {
		memory->clientState->deltaTick = TimingSet_s.m_nOutDeltaTick;
	}


	if (memory->clientState->deltaTick < (TimingSet_s.m_nOutDeltaTick - 7)) { /* TODO: Run Calculation as if we didnt send USERCMD's and freeze player temporaily*/
		memory->clientState->deltaTick = TimingSet_s.m_nOutDeltaTick - 7;
	}

	//if (config->debug.Lagger && (memory->clientState->deltaTick < TimingSet_s.m_nOutDeltaTick)) {
	//	memory->clientState->deltaTick = TimingSet_s.m_nOutDeltaTick;
	//	TimingSet_s.m_bInPredictionMode = true;
	//}

	//if (config->debug.LocalTiming && GetAsyncKeyState(config->TestShit.fuckKey2)) {
	//	TimingSet_s.m_bInPredictionMode = true;
	//}
	//if (GetAsyncKeyState(config->TestShit.fuckKey10)) {
	//	memory->globalVars->intervalPerTick = (1.f / 128.f);
	//}
	//else if (memory->globalVars->intervalPerTick == (1.f / 128.f)) {
	//	memory->globalVars->intervalPerTick = (1.f / 64.f);
	//}



	if (TimingSet_s.m_nOutDeltaTick < memory->clientState->deltaTick) {
		TimingSet_s.m_nOutDeltaTick = memory->clientState->deltaTick;
	}
#endif
#endif
#ifdef DEV_SECRET_BUILD
	if (GetAsyncKeyState(config->TestShit.fuckKey9) || config->mmlagger.crimWalk || Timing::ExploitTiming.m_bNetworkedFreeze) {
		NetworkChannel* netchann = interfaces->engine->getNetworkChannel();
		if (netchann && (((memory->globalVars->realtime - Timing::ExploitTiming.m_fFreezeTime) > memory->globalVars->intervalPerTick) || ExploitTiming.m_bForceDoubleSpeed)) {
			int ticks = config->mmlagger.ticksToSimulate - 1;
			float latency = netchann->getLatency(FLOW_OUTGOING);
			//ticks += static_cast<int>(((latency - (40.f/1000.f)) * memory->globalVars->intervalPerTick)); /* Calculate How Much Time Elapses Between Each ::recvfrom; 40 is a guessed number that seems to work*/
			//ticks = std::clamp(ticks, 1, 64);
			for (int i = 0; i < ticks; i++) {
				if (!config->mmlagger.bNewPrediction) {
					((HRFIOrg)oHOSTRUNFRAMEINPUT)(accumulated_extra_samples, bFinalTick);
				}
				memory->globalVars->tickCount++;
			}
		}
	}
#endif



	((HRFIOrg)oHOSTRUNFRAMEINPUT)(accumulated_extra_samples, bFinalTick);
}

typedef bool(__thiscall* NMTOrg)(void*, const NET_Tick* msg);
bool __fastcall Timing::CClientState_NETMSG_Tick(void* ecx, void*, const NET_Tick* msg) {

	if(TimingStats.m_fLastNetTickUpdateAlwaysRecord < 0.f)
		TimingStats.m_fLastNetTickUpdateAlwaysRecord = memory->globalVars->realtime;


	float fElapsedTime = (memory->globalVars->realtime - TimingStats.m_fLastNetTickUpdateAlwaysRecord);
	if (TimingStats.m_fLongestNetTickTime < fElapsedTime) {
		TimingStats.m_fLongestNetTickTime = fElapsedTime;
	}
	TimingStats.m_fLastNetTickUpdateAlwaysRecord = memory->globalVars->realtime;

	TimingStats_s::ServerTickStats stsNewEntry;
	stsNewEntry.m_fServerComputationTime = msg->host_computationtime;
	stsNewEntry.m_fServerFrametime = msg->host_framestarttime_std_deviation;

	TimingStats.m_vTickStats.push_front(stsNewEntry);
	while(TimingStats.m_vTickStats.size() > 128) {
		TimingStats.m_vTickStats.pop_back();
	}

	float count = 0;
	float comptimetotal = 0.f;
	float frametimetotal = 0.f;
	for (TimingStats_s::ServerTickStats stat : TimingStats.m_vTickStats) {
		comptimetotal += stat.m_fServerComputationTime;
		frametimetotal += stat.m_fServerFrametime;
		count += 1.f;
	}

	TimingStats.m_fServerComputationTime = comptimetotal / count;
	TimingStats.m_flAverageServerFrametime = frametimetotal / count;

	if (msg) {
		TimingInfo_s.m_fLastNetTickUpdate = memory->globalVars->currenttime;
		TimingInfo_s.m_nLastTickValueRecieved = msg->tick;
	}

	if (Timing::ExploitTiming.m_bNetworkedFreeze)
		return true;

	static bool wasOn{ false };
	if (GetAsyncKeyState(config->TestShit.fuckKey9) || Timing::ExploitTiming.m_bNetworkedFreeze) {
		//if (!g_bWasSet) {
		// Flush Entity Packets?
		//}
		TimingSet_s.m_bInPredictionMode = true;
		g_bWasSet = true;
		wasOn = true;
		return true;

	}
	else if (wasOn) {
		wasOn = false;
		TimingSet_s.m_bInPredictionMode = false;
		//Timing::Reset();
		if((config->mmlagger.cleanup || !config->mmlagger.bNewPrediction) && !Timing::ExploitTiming.m_bNetworkedFreeze)
			memory->clientState->deltaTick = -1;
	}


	TimingSet_s.m_bInPredictionMode = false;


	//if (ExploitTiming.m_fFreezeTimeLeft > memory->globalVars->intervalPerTick)
	//	return true;


	if (
#ifdef DEV_SECRET_BUILD
		config->mmlagger.crimWalk && 
#endif
		ExploitTiming.m_bForceDoubleSpeed)
		return true;


	//if ((TimingSet_s.m_nOutDeltaTick > msg->tick) && g_bWasSet) {
	//	TimingSet_s.m_nOutDeltaTick = -1;
	//	g_bWasSet = false;
	//	return true;
	//}




	return ((NMTOrg)memory->oNET_TickMsgHandler)(ecx, msg);

#if 1
	if (g_bDontProcessConnectionless) {
		tickAt = 0;
	}
	if (tickAt > 200) {
		g_bDontProcessConnectionless = false;
	}

	if (GetAsyncKeyState(config->TestShit.fuckKey5)) {
		return true;
	}
#endif
#ifdef ANTI_SHARKLASER_PROTECTION

	if (TimingSet_s.bActivatedDueToPingSpike && config->mmlagger.enabled)
		return true;


	int nLatencyFlowOut = (interfaces->engine->getNetworkChannel()->getLatency(FLOW_OUTGOING) * 1000);
	static bool bActivatedDueToPingSpike = TimingSet_s.bActivatedDueToPingSpike;
	if (bActivatedDueToPingSpike && (350 > nLatencyFlowOut)){
		TimingSet_s.m_bInPredictionMode = false;
		memory->clientState->deltaTick = -1;
		bActivatedDueToPingSpike = false;
		TimingSet_s.bActivatedDueToPingSpike = false;
	}


	if ((nLatencyFlowOut > 300) && TimingSet_s.m_bInPredictionMode) {
		TimingInfo_s.m_fLastNetTickUpdate = memory->globalVars->currenttime; /* Store Data But Don't Use Currently */
		TimingInfo_s.m_nLastTickValueRecieved = msg->tick;
		return true;
	}

	if ((nLatencyFlowOut > 400)) { /* Kick In at 400 */
		TimingSet_s.m_bInPredictionMode = true;
		TimingSet_s.bActivatedDueToPingSpike = true;
		return true;
	}

	if (TimingSet_s.m_bInPredictionMode)
		memory->clientState->deltaTick = -1;


#endif


	if (TimingSet_s.m_bInPredictionMode) {
		memory->clientState->deltaTick = -1;
	}


	TimingSet_s.m_bInPredictionMode = false;

	return ((NMTOrg)memory->oNET_TickMsgHandler)(ecx, msg);
}


#include "Interfaces.h"
#include "SDK/OsirisSDK/Surface.h"


void TimingDrawText(std::wstring wzText, int x, int& y) {
	interfaces->surface->setTextColor(245, 213, 71, 255);
	auto [textx, texty] = interfaces->surface->getTextSize(5, wzText.c_str());
	x = x - textx - 5;
	y = y - texty - 1;
	interfaces->surface->setTextFont(5);
	interfaces->surface->setTextPosition(x, y);
	interfaces->surface->printText(wzText.c_str());
}

void Timing::DebugDrawTimingInfo(void) {

	if (!config->debug.TimingStats) {
		return;
	}

	auto [x, y] = interfaces->surface->getScreenSize();
	int TextPosX = x;
	int TextPosY = y / 2;
	float fDelta = 0.f;
	int nTicks = 0;
	if (interfaces->engine->isConnected() && interfaces->engine->getNetworkChannel() && (Timing::TimingStats.m_fLastNetTickUpdateAlwaysRecord >= 0.f)) {
		fDelta = memory->globalVars->realtime - Timing::TimingStats.m_fLastNetTickUpdateAlwaysRecord;
		nTicks = TIME_TO_TICKS(fDelta);
		if (fDelta > Timing::TimingStats.m_fPreviousPauseMax) {
			Timing::TimingStats.m_fPreviousPauseMax = fDelta;
			Timing::TimingStats.m_nPreviousPauseMaxticks = nTicks;
		}
	}
	else {
		//Timing::TimingStats.m_fPreviousPauseMax = 0.f;
		//Timing::TimingStats.m_nPreviousPauseMaxticks = 0.f;
	}

#ifdef DEV_SECRET_BUILD
	if (config->debug.TimingStats) {
		std::wstring LastUpdate{ L"Last Tick Update Time = " + std::to_wstring(Timing::TimingStats.m_fLastNetTickUpdateAlwaysRecord) };
		std::wstring TimeSinceLastTick{ L"Time Since Last Tick = " + std::to_wstring(fDelta) };
		std::wstring TicksSinceLastTick{ L"Ticks Since Last Tick = " + std::to_wstring(nTicks) };


		std::wstring MaxTime{ L"Previous Max Time = " + std::to_wstring(fDelta) };
		std::wstring MaxTicks{ L"Previous Max Time (Ticks) = " + std::to_wstring(nTicks) };
		std::wstring LongestNetTickTime{ L"Highest Delta Time = " + std::to_wstring(Timing::TimingStats.m_fLongestNetTickTime) };
		std::wstring FrameTime{ L"Server Frametime = " + std::to_wstring(Timing::TimingStats.m_flAverageServerFrametime) };
		std::wstring ComputationTime{ L"Server ComputationTime = " + std::to_wstring(Timing::TimingStats.m_fServerComputationTime) };
		std::wstring OurFrameTime{ L"Local Client Frametime = " + std::to_wstring(memory->globalVars->frametime) };
		std::wstring OurDeltaTick{ L"Local Client DeltaTick = " + std::to_wstring(memory->clientState->deltaTick) };
		std::wstring OurLastValidDeltaTick{ L"OurLastValidDeltaTick = " + std::to_wstring(TimingSet_s.m_nLastValidDeltaTick) };
		std::wstring OurLastOutDeltaTick{ L"OurOutDeltaTick = " + std::to_wstring(TimingSet_s.m_nOutDeltaTick) };
		std::wstring Highesttick{ L"nHighestDeltaTick  = " + std::to_wstring(TimingSet_s.m_nOutDeltaTick) };
		std::wstring TheyCantHitYou{ L"They Cant Hit Shit Mode On" };
		if (config->mmlagger.shouldLag) {
			TimingDrawText(TheyCantHitYou, TextPosX, TextPosY);
		}
		TimingDrawText(OurFrameTime, TextPosX, TextPosY);
		TimingDrawText(OurDeltaTick, TextPosX, TextPosY);
		TimingDrawText(OurLastValidDeltaTick, TextPosX, TextPosY);
		TimingDrawText(OurLastOutDeltaTick, TextPosX, TextPosY);
		TimingDrawText(Highesttick, TextPosX, TextPosY);
		TimingDrawText(LastUpdate, TextPosX, TextPosY);
		TimingDrawText(LongestNetTickTime, TextPosX, TextPosY);
		TimingDrawText(TimeSinceLastTick, TextPosX, TextPosY);
		TimingDrawText(TicksSinceLastTick, TextPosX, TextPosY);
		TimingDrawText(MaxTime, TextPosX, TextPosY);
		TimingDrawText(MaxTicks, TextPosX, TextPosY);
		TimingDrawText(FrameTime, TextPosX, TextPosY);
		TimingDrawText(ComputationTime, TextPosX, TextPosY);
		TimingDrawText(L"--------------------", TextPosX, TextPosY);
		TimingDrawText(L"    Timing Stats    ", TextPosX, TextPosY);
	}
#endif
}

#include "SDK/SDK/Tier0/bitvec.h"
#if 1
enum UpdateType
{
	EnterPVS = 0x0,
	LeavePVS = 0x1,
	DeltaEnt = 0x2,
	PreserveEnt = 0x3,
	Finished = 0x4,
	Failed = 0x5,
};

#define	MAX_EDICT_BITS				11			// # of bits needed to represent max edicts
// Max # of edicts in a level
#define	MAX_EDICTS					(1<<MAX_EDICT_BITS)

#pragma pack(push, 1)
struct CClientFrame
{
	void* vtable;
	int last_entity; //unknown if correct
	int tick_count;//unknown if correct
	DWORD unknown;
	CBitVec<MAX_EDICTS>	transmit_entity;
	CBitVec<MAX_EDICTS>* from_baseline;	// if bit n is set, this entity was send as update from baseline
	CBitVec<MAX_EDICTS>* transmit_always; // if bit is set, don't do PVS checks before sending (HLTV only)

	CClientFrame* m_pNext;

private:

	// Index of snapshot entry that stores the entities that were active and the serial numbers
	// for the frame number this packed entity corresponds to
	// m_pSnapshot MUST be private to force using SetSnapshot(), see reference counters
	void/*CFrameSnapshot*/* m_pSnapshot;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CEntityInfo
{
	void* entityinfo_vtable;
	CClientFrame* m_pFrom;
	CClientFrame* m_pTo;
	int m_nOldEntity;
	int m_nNewEntity;
	int m_nHeaderBase;
	int m_nHeaderCount;
	
	//void NextOldEntity(void)
	//void NextNewEntity(void)
};
#pragma pack(pop)
#pragma pack(push, 1)
struct CPostDataUpdateCall
{
	int m_iEnt;
	int m_UpdateType;
};
#pragma pack(pop)
#pragma pack(push, 1)
struct CEntityReadInfo : CEntityInfo
{
	//CEntityInfo entityinfo;
	UpdateType m_UpdateType;
	bool m_bAsDelta;
	char pad[3];
	void* m_pSerializedEntity;
	bf_read* m_pBuf;
	int m_UpdateFlags;
	bool m_bIsEntity;
	char m_bIsEntityPad[3];
	int m_nBaseline;
	int unknown;
	int m_nLocalPlayerBits;
	int m_nOtherPlayerBits;
	CPostDataUpdateCall m_PostDataUpdateCalls[2048]; //MAX_EDICTS
	int m_nPostDataUpdateCalls;
};
#pragma pack(pop)

#if 0
#include "SDK/SDK/Tier0/bitvec.h"
void CEntityInfo::NextOldEntity(void)
{
	if (m_pFrom)
	{
		m_nOldEntity = FindNextSetBit(&m_pFrom->transmit_entity, m_nOldEntity + 1);

		if (m_nOldEntity < 0)
		{
			// Sentinel/end of list....
			m_nOldEntity = ENTITY_SENTINEL;
		}
	}
	else
	{
		m_nOldEntity = ENTITY_SENTINEL;
	}
}

void CEntityInfo::NextNewEntity(void)
{
	m_nNewEntity = FindNextSetBit(&m_pTo->transmit_entity, m_nNewEntity + 1);

	if (m_nNewEntity < 0)
	{
		// Sentinel/end of list....
		m_nNewEntity = ENTITY_SENTINEL;
	}
}
#endif
enum FHDR {
	FHDR_ZERO = 0x0000,
	FHDR_LEAVEPVS = 0x0001,
	FHDR_DELETE = 0x0002,
	FHDR_ENTERPVS = 0x0004,

};

// "55 8B EC 56 57 8B F1 E8 ? ? ? ? 8B 7D 08"
/* Todo: Use This To Fix Prediction (Store Our Frames, Apply Move info to this offset back */
/* Todo: Also Accept non-localPlayer entities so we arent forced to update but get other players
   locations */



void RemoveLocalPlayerUpdates(CEntityReadInfo& u) {
	int oldEntity = u.m_nOldEntity;
	//oldEntity = u.GetNextOldEntity(u.m_nOldEntity);
	UpdateType updateType = u.m_UpdateType;

	while (updateType < Finished)
	{


	}
}
#endif

#include "Hooks.h"


bool RestrictPreDataUpdate = false;
bool RestrictPostDataUpdate = false;
bool RestrictOnPostRestoreData = false;

MinHook LocalPlayer;
void __fastcall PostDataUpdate(void* ecx, void*, int updatetype) { /* Why Dont Post if we didnt do Pre*/
	if (!Timing::TimingSet_s.m_bInPredictionMode) {
		LocalPlayer.callOriginal<void, 7, int>(updatetype);
		return;
	}		
	Entity* lp = (Entity*)((uintptr_t)ecx - 8);
	if (lp && (lp == localPlayer.get()) && lp->isAlive() && RestrictPostDataUpdate) {
		return;
	}

	LocalPlayer.callOriginal<void, 7, int>(updatetype);
}

void __fastcall PreDataUpdate(void* ecx, void*, int updatetype) { /* Dont Fill Data ! */
	if (!Timing::TimingSet_s.m_bInPredictionMode) {
		LocalPlayer.callOriginal<void, 6, int>(updatetype);
		return;
	}
	Entity* lp = (Entity*)((uintptr_t)ecx - 8);
	if (lp && (lp == localPlayer.get()) && lp->isAlive() && RestrictPreDataUpdate) {
		return;
	}

	LocalPlayer.callOriginal<void, 6, int>(updatetype);
}

void __fastcall OnPostRestoreData(void* ecx, void*) { /* Do Not Recompute That Origin! */
	if (!Timing::TimingSet_s.m_bInPredictionMode) {
		LocalPlayer.callOriginal<void, 15>();
		return;
	}
	Entity* lp = (Entity*)((uintptr_t)ecx - 8);
	if (lp && (lp == localPlayer.get()) && lp->isAlive() && RestrictOnPostRestoreData) {
		Debug::QuickPrint("Restricting Origin Calculation");
		return;
	}

	LocalPlayer.callOriginal<void, 15>();
}


typedef bool(__thiscall* PEOrg)(void*, void*);



bool __fastcall Timing::CClientState_SVCMsg_PacketEntities(void* ecx, void*, SVC_PacketEntities* msg) {

	if (Timing::ExploitTiming.m_bNetworkedFreeze)
		return true;

	if (config->mmlagger.bNewPrediction)
		return ((PEOrg)memory->oSVCMSG_PacketEntities)(ecx, msg);


#if 0
	static float lastRecieveTime{ 0.f };
	static int localPlayerTickBase{ 0 };
	if (localPlayer.get()) {
		int nLocalPlayerTickBaseDelta = 0;
		if (localPlayer->tickBase() > localPlayerTickBase) {
			nLocalPlayerTickBaseDelta = localPlayer->m_nTickBase() - localPlayerTickBase;
		}

		float fLastReceiveTimeDelta = memory->globalVars->realtime - lastRecieveTime;
		int nEstimatedTickBaseDelta = TIME_TO_TICKS(fLastReceiveTimeDelta);
		localPlayerTickBase = localPlayer->m_nTickBase();
		lastRecieveTime = memory->globalVars->realtime;
		char buffer[4096];
		const char* formatter = "SVCMsg_PacketEntities Recieved Time Since Previous Recieve %f, LocalPlayer Tickbase Delta %d, Estimated Delta : %d, tickBase %d, tickCout %d";
		snprintf(buffer, 4096, formatter, fLastReceiveTimeDelta, nLocalPlayerTickBaseDelta, nEstimatedTickBaseDelta, localPlayer->tickBase(), memory->globalVars->tickCount);
		Debug::QuickPrint(buffer);
	}
#endif









	if (GetAsyncKeyState(config->TestShit.fuckKey2)) {
		return true;
	}

	static bool fullOnce{ true };
	if (GetAsyncKeyState(config->TestShit.fuckKey9) || Timing::ExploitTiming.m_bNetworkedFreeze) {
		g_bRecievedNonDelta = false;
		fullOnce = true;
		return true;
	}



	




	if (TimingSet_s.m_bInPredictionMode) { /* Don't Set Delta Tick, thats our Job! */
		Debug::QuickWarning("CClientState::SVCMsgPacketEntities In Pred Mode");
		return true;
	}

	if (!(memory->clientState->deltaTick == -1)) {
		if (msg->m_bIsDelta && (msg->m_nDeltaFrom == ((memory->clientState->deltaTick + 1)))) {
			Debug::QuickWarning("CClientState::SVCMsgPacketEntities Recieved Update that was Self-Referencing. Ignoring and Requesting full update. Error in Predicition Code");
			memory->clientState->deltaTick = -1;
			return true;
		}

		if (Timing::ExploitTiming.m_bForceReturnDeltaFix && msg->m_bIsDelta) {
			Debug::QuickWarning("CClientState::SVCMsgPacketEntities Dropping Entity Packet Due to m_bForceReturnDeltaFix");
			//memory->clientState->deltaTick = -1;
			Timing::ExploitTiming.m_bForceReturnDeltaFix = false;
			return true;
		}
	}



	//return ((PEOrg)memory->oSVCMSG_PacketEntities)(ecx, msg);

	/* Code Below Needs to get fixed and added back. but idc right now */

	if (msg->m_bIsDelta && !g_bRecievedNonDelta) {
		//Debug::QuickWarning("CClientState::SVCMsgPacketEntities Dropping Entity Packet Due to Non-Delta Packet");
		if (fullOnce) {
			memory->clientState->deltaTick = -1;
			fullOnce = false;
		}
		return true;
	}
	//return ((PEOrg)memory->oSVCMSG_PacketEntities)(ecx, msg);
	//if (ExploitTiming.m_fFreezeTimeLeft > memory->globalVars->intervalPerTick) {
	//	Debug::QuickWarning("CClientState::SVCMsgPacketEntities Returing On Condition (ExploitTiming.m_fFreezeTimeLeft > memory->globalVars->intervalPerTick)");
	//	return true;
	//}


	if (!msg->m_bIsDelta) {
		Timing::ExploitTiming.m_bForceReturnDeltaFix = false;
		g_bWasSet = false;
		g_bRecievedNonDelta = true;
		fullOnce = true;
	}


	int nLatencyFlowOut = (interfaces->engine->getNetworkChannel()->getLatency(FLOW_OUTGOING) * 1000);

#ifdef ALLOW_PLAYER_UPDATES
	if (localPlayer.get() && localPlayer->isAlive() && !(LocalPlayer.getThis() != localPlayer.get())) {
		LocalPlayer.init(localPlayer.get() + 8);
		LocalPlayer.hookAt(7, PostDataUpdate);
		LocalPlayer.hookAt(6, PreDataUpdate);
		LocalPlayer.hookAt(15, OnPostRestoreData);
	}

	Vector SaveOrigin, AbsOrigin, eyeAngles, AbsAngles;
	if (TimingSet_s.m_bInPredictionMode && localPlayer.get() && localPlayer->isAlive()) {
		RestrictPreDataUpdate = true;
		RestrictPostDataUpdate = true;
		RestrictOnPostRestoreData = true;
		SaveOrigin = localPlayer->origin();
		AbsOrigin = localPlayer->getAbsOrigin();
		eyeAngles = localPlayer->eyeAngles();
		AbsAngles = localPlayer->getAbsAngle();
	}
#else
#endif


	
	if (msg->m_bIsDelta && (TimingSet_s.m_bInPredictionMode || g_bWasSet || !g_bRecievedNonDelta)) {
		g_bRecievedNonDelta = false;
		if (config->mmlagger.cleanup || !config->mmlagger.bNewPrediction) {
			memory->clientState->deltaTick = -1;
			Timing::TimingSet_s.m_nLastValidDeltaTick = -1;
		}



		//Timing::TimingSet_s.m_nOutDeltaTick = -1;
		Debug::QuickWarning("CClientState::SVCMsgPacketEntities Dropping Entity Packet Due Delta And Set Flag");
		return true;
	}

	




	if (TimingSet_s.bActivatedDueToPingSpike && (350 > nLatencyFlowOut)) {
		TimingSet_s.m_bInPredictionMode = false;
		//memory->clientState->deltaTick = -1;
		TimingSet_s.bActivatedDueToPingSpike = false;
		TimingSet_s.bActivatedDueToPingSpike = false;
	}

#ifndef ALLOW_PLAYER_UPDATES
	if (TimingSet_s.m_bInPredictionMode) {
		return true;
	}
#endif


#ifdef ALLOW_PLAYER_UPDATES
#if 0
	SVC_PacketEntities* entmsg = msg;
	CEntityReadInfo u;
	u.m_pBuf = &entmsg->m_DataIn;
	u.m_bAsDelta = entmsg->m_bIsDelta;
	u.m_nHeaderCount = entmsg->m_nUpdatedEntries;
	u.m_nBaseline = entmsg->m_nBaseline;
	u.m_nBaseline = entmsg->m_bUpdateBaseline;
	RemoveLocalPlayerUpdates(u);
#endif
	if (TimingSet_s.m_bInPredictionMode && localPlayer.get() && localPlayer->isAlive()) {
		RestrictPreDataUpdate = false;
		RestrictPostDataUpdate = false;
		RestrictOnPostRestoreData = false;
		localPlayer->origin() = SaveOrigin;
		localPlayer->setAbsOrigin(AbsOrigin);
		localPlayer->eyeAngles() = eyeAngles;
		localPlayer->setAbsAngle(AbsAngles);
	}
#endif
	


	
	return ((PEOrg)memory->oSVCMSG_PacketEntities)(ecx, msg);
}


// E8 ? ? ? ? 8B 15 ? ? ? ? B9 ? ? ? ? FF B4 24 ? ? ? ?
bool __fastcall Timing::CBaseClientState_SVCMsg_PacketEntities(void* ecx, void*, SVC_PacketEntities* msg) {

	if (Timing::ExploitTiming.m_bNetworkedFreeze)
		return true;


	if (config->mmlagger.bNewPrediction)
		return ((PEOrg)memory->oCBaseClientState_SVCMsg_PacketEntities)(ecx, msg);




#ifndef ALLOW_PLAYER_UPDATES

#endif

	if (GetAsyncKeyState(config->TestShit.fuckKey2))
		return true;

	if (GetAsyncKeyState(config->TestShit.fuckKey9) || Timing::ExploitTiming.m_bNetworkedFreeze) {
		return true;
	}

	if (TimingSet_s.m_bInPredictionMode) { /* Don't Set Delta Tick, thats our Job! */
		return true;
	}
	return ((PEOrg)memory->oCBaseClientState_SVCMsg_PacketEntities)(ecx, msg);
}
/* Stops Delta Tick From Being Set */


// 80 3D ? ? ? ? ? 56 75 14
typedef void(__cdecl* CL_PPEOrg)(void);
void __cdecl Timing::CL_PreprocessEntities(void) {
	if (TimingSet_s.m_bInPredictionMode) { /* Don't Call Preprocess*/
		return;
	}

	if (GetAsyncKeyState(config->TestShit.fuckKey9) || Timing::ExploitTiming.m_bNetworkedFreeze) {
		return;
	}

	((CL_PPEOrg)memory->oCL_PreprocessEntities);
}



typedef int(__fastcall* CL_CNEOrg)(void*, int, int);
int __fastcall Timing::CL_CopyNewEntity(void* uIn, int iClass, /*double trace,*/ int iSerialNum)
{
	CEntityReadInfo* u = (CEntityReadInfo*)uIn;
	const char* formatter = {
		"CL_CopyNewEntity:          \n"
		" uIn        : %d           \n"
		" iClass     : %d           \n"
		" iSerialNum : %d           \n"
		//" trace      : %d"
	};

	char buffer[4096];
	snprintf(buffer, 4096, formatter, uIn, iClass, iSerialNum);
	Debug::QuickPrint(buffer);


	const char* formatter2 = {
		"CL_CopyNewEntity : CEntityReadInfo\n"
			"m_nOldEntity   :  %d \n"
			"m_nNewEntity   :  %d \n"
			"m_nHeaderBase  :  %d \n" 
			"m_nHeaderCount : %d \n"
	};
	snprintf(buffer, 4096, formatter2, u->m_nOldEntity, u->m_nNewEntity, u->m_nHeaderBase, u->m_nHeaderCount);
	Debug::QuickPrint(buffer);

	if (!uIn || !Timing::TimingSet_s.m_bInPredictionMode || !(localPlayer.get() && localPlayer->isAlive()) || (localPlayer.get() && localPlayer->isAlive() && (!(interfaces->entityList->getEntity(u->m_nNewEntity) == (Entity*)localPlayer.get())))) {
		return ((CL_CNEOrg)memory->oCL_CopyNewEntity)(uIn, iClass, iSerialNum);
	}

	if (u && u->m_nNewEntity && u->m_pTo && u->m_pTo->last_entity) {
		Debug::QuickPrint("Adjustment Here");
		u->m_pTo->last_entity = u->m_nNewEntity;
		u->m_pTo->transmit_entity.Set(u->m_nNewEntity);
		u->m_UpdateType = UpdateType::Failed;
		return UpdateType::Failed;
	}

}


/* Depending on each CS:GO Update this changes! But why!? Is a compiler to blame?*/

#if 1
typedef void(__cdecl* CL_CEEOrg)(void*);
void __cdecl Timing::CL_CopyExistingEntity(void* uIn)
{
	//double trace = 0.0;
	//_asm mov trace, xmm0
	// We dont use it, just leave it alone.
	CEntityReadInfo* u = (CEntityReadInfo*)uIn;
	const char* formatter = {
		"CL_CopyExistingEntity:          \n"
		" uIn        : %d           \n"
	};
	char buffer[4096];
	snprintf(buffer, 4096, formatter, uIn);
	Debug::QuickPrint(buffer);


	if (Timing::TimingSet_s.m_bInPredictionMode) {
		bool lol = true;
	}

	if (!uIn || !u->m_nNewEntity || !Timing::TimingSet_s.m_bInPredictionMode || !(localPlayer.get() && localPlayer->isAlive()) || (localPlayer.get() && localPlayer->isAlive() && (!(interfaces->entityList->getEntity(u->m_nNewEntity) == (Entity*)localPlayer.get())))) {
		((CL_CEEOrg)memory->oCL_CopyExistingEntity)(uIn);
		return;
	}
	//++u->m_nPostDataUpdateCalls;
	if (u && u->m_nNewEntity && u->m_pTo && u->m_pTo->last_entity) {
		u->m_pTo->last_entity = u->m_nNewEntity;
		u->m_pTo->transmit_entity.Set(u->m_nNewEntity);
		u->m_UpdateType = UpdateType::Failed;
	}
	//u.m_nLocalPlayerBits += bit_count;
}
#endif


#if 0
typedef void(__cdecl* CL_CEEOrg)(void*, double trace);
void __cdecl Timing::CL_CopyExistingEntity(void* uIn, double trace)
{
	CEntityReadInfo* u = (CEntityReadInfo*)uIn;
	if (!Timing::TimingSet_s.m_bInPredictionMode || (!(interfaces->entityList->getEntity(u->m_nNewEntity) == (Entity*)localPlayer.get()))) {
		((CL_CEEOrg)memory->oCL_CopyExistingEntity)(uIn, trace);
		return;
	}

	//u->m_pFrom->transmit_entity.Get(u->m_nNewEntity); // ASSERT 1

	//u->m_PostDataUpdateCalls[u->m_nPostDataUpdateCalls].m_iEnt = iEnt;
	//u->m_PostDataUpdateCalls[u->m_nPostDataUpdateCalls].m_UpdateType = updateType;
	//++u->m_nPostDataUpdateCalls;


	u->m_pTo->last_entity = u->m_nNewEntity;
	u->m_pTo->transmit_entity.Set(u->m_nNewEntity);
}
#endif

