#pragma once
#include "SDK/OsirisSDK/UserCmd.h"

class NET_Tick;
class SVC_PacketEntities;
#define TICK_INTERVAL			(memory->globalVars->intervalPerTick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#include <mutex>
#include <deque>
namespace Timing {
	void __cdecl _Host_RunFrame_Input_hk(void*, void*, float accumulated_extra_samples, bool bFinalTick);
	bool __fastcall CClientState_NETMSG_Tick(void*, void*, const NET_Tick* msg);
	bool __fastcall CClientState_SVCMsg_PacketEntities(void* ecx, void*, SVC_PacketEntities* msg);
	bool __fastcall CBaseClientState_SVCMsg_PacketEntities(void* ecx, void*, SVC_PacketEntities* msg);
	void __cdecl CL_PreprocessEntities(void);
	int __fastcall CL_CopyNewEntity(void* uIn, int iClass, int iSerialNum);
	void __cdecl CL_CopyExistingEntity(void* uIn);
	void DebugDrawTimingInfo(void);
	void Reset(void);
	struct TimingInfo {
		float m_fLastNetTickUpdate = 0.f;
		int m_nLastTickValueRecieved = 0;
	};
	inline TimingInfo TimingInfo_s;

	struct TimingSet {
		int m_nOutDeltaTick = -1;
		int m_nLastValidDeltaTick = -1;
		bool m_bInPredictionMode = 0;
		bool m_bShouldPredictMore = 0;
		bool bActivatedDueToPingSpike = false;
		bool m_bDontSendAnyCmds = false;
	};

	struct TimingStats_s{
		float m_fLongestNetTickTime = -1.f;
		float m_fLastNetTickUpdateAlwaysRecord = -1.f;
		float m_fAverageTickDeltaTime = -1.f;
		float m_fPreviousPauseMax = -1.f;
		int m_nPreviousPauseMaxticks = -1;
		struct ServerTickStats {
			float m_fServerFrametime = 0.f;
			float m_fServerComputationTime = 0.f;
		};
		float m_flAverageServerFrametime = 0.f;
		float m_fServerComputationTime = 0.f;
		std::deque<ServerTickStats> m_vTickStats;
	};

	struct ExploitTimings_s {
		float m_fFreezeTime{ 0.f };
		float m_fFreezeTimeLeft{ 0.f };
		bool m_bForceDoubleSpeed{ false };
		bool m_bForceReturnDeltaFix{ false };
		bool m_bDontSendNetTick{ false };
		int m_nMaxNetTickSent{ false };
		bool m_bDoneSendingBlip{ true };
		bool m_bStopAllDatagramsIn{ false };
		bool m_bNetworkedFreeze{ false };
		int m_nActivateTick{ 0 };
		int nExtraTicksToSimulate{ 0 };
		int nNetworkedDeltaTick{ 0 };
		bool m_bDidBlip{ false };
		int m_nTicksLeftToDrop{ 0 };
		UserCmd m_LastCmd;
	};

	inline ExploitTimings_s ExploitTiming;
	inline TimingStats_s TimingStats;
	inline TimingSet TimingSet_s;
	inline bool newFrame = false;
	inline std::mutex newFrameMutex;
}