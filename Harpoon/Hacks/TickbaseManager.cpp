#include "TickbaseManager.h"
#include "../SDK/OsirisSDK/UserCmd.h"
#include "../Interfaces.h"
#include "../SDK/OsirisSDK/Engine.h"
#include "../SDK/OsirisSDK/NetworkChannel.h"
#include "Backtrack.h"
#include "../SDK/OsirisSDK/Entity.h"




#include "../Hacks/TickbaseManager.h"
#include "../EngineHooks.h"
#include "../SDK/OsirisSDK/UserCmd.h"
#include "../SDK/OsirisSDK/LocalPlayer.h"
#include "../SDK/OsirisSDK/Entity.h"
#include "../SDK/OsirisSDK/Engine.h"
#include "../Config.h"
#include "../SDK/OsirisSDK/GlobalVars.h"
#include "../SDK/OsirisSDK/NetworkChannel.h"
#include "../Timing.h"
#include <algorithm>

#include "OTHER/Debug.h"

// TODO: Make last tick out not be shooting so Dummy Commands dont shoot!

#define VERBOSE_DOUBLE_TAP_DEBUG
#define CL_MOVE_DT
#define HOST_RUN_FRAME_DT
int Tickbase::GetMaxProcessCMD() {
	int nCustomForce = config->m_Exploits.m_TickBase.m_DoubleTap.nCustomMaxUserProcess;
	if (nCustomForce) {
		return nCustomForce;
	}

	if (memory->gameRules && *memory->gameRules && (*memory->gameRules)->m_bIsValveDS())
		return 8;

	return 16;
}

bool ShouldAttemptShift(UserCmd* pCmd, int nTickShiftCount) {
	if (localPlayer->nextAttack() > memory->globalVars->serverTime())
		return false;

	float fNextAttack = (localPlayer->nextAttack() + (nTickShiftCount * memory->globalVars->intervalPerTick));
	if (fNextAttack >= memory->globalVars->serverTime())
		return false;

	auto pActiveWeapon = localPlayer->getActiveWeapon();
	if (!pActiveWeapon || !pActiveWeapon->clip() || pActiveWeapon->isGrenade())
		return false;

	float fShiftTime = (localPlayer->tickBase() - nTickShiftCount) * memory->globalVars->intervalPerTick;
	if (fShiftTime < pActiveWeapon->nextPrimaryAttack())
		return false;

	return true;
}
#include "../SDK/SDKAddition/EntityListCull.hpp"
#define MAX_PROCESS_CMDS GetMaxProcessCMD()
void Tickbase::OnCreateMove(UserCmd* pCmd, bool& bSendPacket) {
	if (!localPlayer.get() || localPlayer->isDormant() || !localPlayer->isAlive() || !config->m_Exploits.m_TickBase.m_DoubleTap.bEnabled)
		return;

	TickBaseInfo.nMaxUserProcessCmds = MAX_PROCESS_CMDS;
	TickBaseInfo.nTicksAllowedForProcessing = std::clamp(TickBaseInfo.nTicksAllowedForProcessing, 0, TickBaseInfo.nMaxUserProcessCmds + 1);
	if (!pCmd->commandNumber || !pCmd->tickCount) {
		return;
	}

	if (TickBaseInfo.bInShift) {
		return;
	}


#if 1
	if (pCmd->buttons & UserCmd::IN_ATTACK) {
		Entity* pActiveWeapon = localPlayer->getActiveWeapon();
		if (pActiveWeapon && pActiveWeapon->clip() && !pActiveWeapon->isGrenade()) {

			if (pActiveWeapon->isPistol()) {
				float fShiftTime = (localPlayer->tickBase() + TickBaseInfo.nTicksShifted) * memory->globalVars->intervalPerTick;
				//float fNextAttack = (localPlayer->nextAttack() + (memory->globalVars->intervalPerTick * TickBaseInfo.nTicksShifted));
				if ((fShiftTime < pActiveWeapon->nextPrimaryAttack())) {
					pCmd->buttons &= ~UserCmd::IN_ATTACK;
				}
			}
		}
	}
#endif

	if (TickBaseInfo.bShouldShift) {
		return;
	}

	static bool bRechargeSet{ false };
	int nTicksToShift = std::clamp(config->m_Exploits.m_TickBase.m_DoubleTap.nTicksToShift, 0, MAX_PROCESS_CMDS);
	nTicksToShift = min(nTicksToShift, TickBaseInfo.nTicksAllowedForProcessing);
	static bool bLastTickWithHeldShot{ false };
	if (true) {
		if (ShouldAttemptShift(pCmd, nTicksToShift) && (pCmd->buttons & UserCmd::IN_ATTACK)) {
			if (nTicksToShift >= config->m_Exploits.m_TickBase.m_DoubleTap.nTicksToShift) {
				TickBaseInfo.nTicksToShift = nTicksToShift;
				TickBaseInfo.bShouldShift = true;
				//Tickbase::TickBaseInfo.nSavedTickbase = localPlayer->tickBase();

#ifdef VERBOSE_DOUBLE_TAP_DEBUG
				VCON("Initiating DoubleTap at %d (TB: %d), shifting %d\n", memory->globalVars->tickCount, Tickbase::TickBaseInfo.nSavedTickbase, nTicksToShift);
#endif
			}
		}
		else if ((TickBaseInfo.nMaxUserProcessCmds > TickBaseInfo.nTicksAllowedForProcessing) && !(pCmd->buttons & UserCmd::IN_ATTACK)) {

			bool bEnemyVis{ false };
			for (EntityQuick EntQ : entitylistculled->getEnemies()) {
				if (EntQ.m_bisVisible) {
					bEnemyVis = true;
					break;
				}
			}
			if (!bEnemyVis) {
#ifdef VERBOSE_DOUBLE_TAP_DEBUG
				VCON("Initiating DT Recharge At %d, MaxUserProcessCmd %d, TicksAllowed %d\n", memory->globalVars->tickCount, TickBaseInfo.nMaxUserProcessCmds, TickBaseInfo.nTicksAllowedForProcessing);
#endif
				bRechargeSet = true;			
				bLastTickWithHeldShot = false;
			}
		}



		if (!(pCmd->buttons & UserCmd::IN_ATTACK) && bRechargeSet) {
			bRechargeSet = false;
			TickBaseInfo.bShouldRecharge = true;
		}



	}
}
#include "../SDK/OsirisSDK/NetworkChannel.h"
bool Tickbase::OnCLMove(float fAccumulatedExtraSamples, bool bFinalTick) {



	if (TickBaseInfo.bShouldRecharge) {


		if (TickBaseInfo.nTicksAllowedForProcessing < TickBaseInfo.nMaxUserProcessCmds) {
			TickBaseInfo.nTicksAllowedForProcessing++;
#ifdef VERBOSE_DOUBLE_TAP_DEBUG
			VCON("CL_Move DoubleTap Returning At %d, nTicksAllowedForProcessing %d\n", memory->globalVars->tickCount, TickBaseInfo.nTicksAllowedForProcessing);
#endif
			return true;
		}
		else {
			if (TickBaseInfo.bShouldRecharge) {
				TickBaseInfo.bShouldRecharge = false;
				TickBaseInfo.bDidRecharge = true;
				return true;
			}
			return false;
		}
	}
#ifndef CL_MOVE_DT
	return false;
#endif
	if (config->m_Exploits.m_TickBase.m_DoubleTap.bClMoveDT) {
		if (TickBaseInfo.bShouldShift) {
			TickBaseInfo.bInShift = true;
			Tickbase::TickBaseInfo.nSavedTickbase = localPlayer->tickBase();
			for (int i = 0; i < (TickBaseInfo.nTicksToShift - 1); i++) {
#ifdef VERBOSE_DOUBLE_TAP_DEBUG
				VCON("CL_Move DoubleTap Calling Original At %d, ticks already shifted %d\n", memory->globalVars->tickCount, i);
#endif
				EngineHooks::Run_Original_CL_Move(fAccumulatedExtraSamples, false);
				TickBaseInfo.nTicksAllowedForProcessing--;
				memory->globalVars->tickCount++;
				TickBaseInfo.nTicksShifted++;
			}
#ifdef VERBOSE_DOUBLE_TAP_DEBUG
			//CON("Calling Original CL_Move, End of DT Sequence\n");
#endif
			TickBaseInfo.bInShift = false;
			TickBaseInfo.bDidShift = true;
			EngineHooks::Run_Original_CL_Move(fAccumulatedExtraSamples, false);
			TickBaseInfo.nTicksShifted++;
			TickBaseInfo.nTicksAllowedForProcessing--;
			TickBaseInfo.bShouldShift = false;
			TickBaseInfo.nLastTicksShifted = TickBaseInfo.nTicksShifted;
			TickBaseInfo.nTicksToShift = 0;
			TickBaseInfo.nTicksShifted = 0;
			return true;
		}
	}
	return false;
}



typedef void(__cdecl* HRFIOrg)(float accumulated_extra_samples, bool bFinalTick);
bool Tickbase::OnHostRunFrameInput(float fAccumulatedExtraSamples, bool bFinalTick) {

#ifndef HOST_RUN_FRAME_DT
	return false;
#else
	if (config->m_Exploits.m_TickBase.m_DoubleTap.bHostRunFrameInputDT) {
		if (TickBaseInfo.bShouldShift) {
			TickBaseInfo.bInShift = true;
			Tickbase::TickBaseInfo.nSavedTickbase = localPlayer->tickBase();
			for (int i = 0; i < (TickBaseInfo.nTicksToShift - 1); i++) {
#ifdef VERBOSE_DOUBLE_TAP_DEBUG
				VCON("HostRunFrameInput DoubleTap Calling Original At %d, ticks already shifted %d\n", memory->globalVars->tickCount, i);
#endif
				((HRFIOrg)oHOSTRUNFRAMEINPUT)(fAccumulatedExtraSamples, false);
				TickBaseInfo.nTicksAllowedForProcessing--;
				memory->globalVars->tickCount++;
				TickBaseInfo.nTicksShifted++;
			}
#ifdef VERBOSE_DOUBLE_TAP_DEBUG
			CON("Calling Original HostRunFrameInput, End of DT Sequence\n");
#endif
			TickBaseInfo.bInShift = false;
			TickBaseInfo.bDidShift = true;
			((HRFIOrg)oHOSTRUNFRAMEINPUT)(fAccumulatedExtraSamples, true);
			TickBaseInfo.nTicksAllowedForProcessing--;
			TickBaseInfo.nTicksShifted++;
			TickBaseInfo.nLastTicksShifted = TickBaseInfo.nTicksShifted;
			TickBaseInfo.bShouldShift = false;
			TickBaseInfo.nTicksToShift = 0;
			TickBaseInfo.nTicksShifted = 0;
			return true;
		}
	}
	return false;
#endif






}

