#include "AutoWinner.h"



#include "../COMPILERDEFINITIONS.h"
#include "../Timing.h"
#include "../Memory.h"
#include "../Interfaces.h"
#include "../Hacks/OTHER/Debug.h"
#include "../SDK/OsirisSDK/NetworkChannel.h"
#include "../SDK/OsirisSDK/LocalPlayer.h"
#include "../SDK/OsirisSDK/Entity.h"
#include "../Hacks/TickbaseManager.h"
#include "../Other/AutoQueuer.h"

#include "../SDK/SDK/IMatchFramework.h"
#include "../SDK/OsirisSDK/KeyValues.h"
#include "../Hacks/OTHER/Debug.h"


/* The Most Boring Way To Reach Global.... */

void AutoWinnerPrint(const char* szStr) {
	char str[4096];
	snprintf(str, 4096, "{AutoWinner} %s\n", szStr);
	memory->conColorMsg({ 0,250,250,255 }, str);
}


void AutoWinner::Run() {

	config->misc.antiAfkKick = false;
	if (!config->autowin.enabled)
		return;


	if (!interfaces->engine->isInGame() || !interfaces->engine->isConnected()) {
		config->misc.autoAccept = true;
		config->debug.AutoQueue = true;
	}

	if (!localPlayer.get()) {
		return;
	}

	if (!memory->gameRules) {
		Debug::QuickWarning("{AutoWinner} NO GAMERULESPROXY PTR FOUND!");
		return;
	}

	Entity* GameRulesProxy = *memory->gameRules;
	if (!GameRulesProxy) {
		AutoWinnerPrint("NO GAMERULES PROXY FOUND!");
		return;
	}

	static int lastValue{ 0 };
	int roundWon = GameRulesProxy->totalRoundsPlayed();
	if ((roundWon != lastValue)){
		lastValue = roundWon;
		AutoWinnerPrint(("Rounds Won = " + std::to_string(roundWon)).c_str());
		AutoWinnerPrint(("Warmup  = " + std::to_string(GameRulesProxy->WarmupPeriod())).c_str());
		AutoWinnerPrint(("HasMatchStarted  = " + std::to_string(GameRulesProxy->HasMatchStarted())).c_str());
	}

	static bool shouldEnabled{ true };
	static float lastActivateTime{ 0.f };
	static int frozenServerTime{ 0 };
	static int PlayerCount = 0;
	static float onTime{ 11.f };
	static float deadTime{ 0.f };
	static float lastDeadTime{ 0.f };
	static bool InFreeze{ false };
	static bool wasOn{ false };
	PlayerCount = 0;
	for (int i = 0; (shouldEnabled) && (i < interfaces->entityList->getHighestEntityIndex()); i++) {
		Entity* ent = interfaces->entityList->getEntity(i);

		if (!ent)
			continue;

		/*
		
		Dont activate on bots, localplayer, spectators, or the other team (if they're dead)
		
		
		*/
		if ((ent != localPlayer.get()) && ent->isPlayer() && !ent->isBot() && (ent->team() != 1) && ((!ent->isOtherEnemy(localPlayer.get())) || ent->isAlive() || (ent->lifeState() == LIFE_ALIVE)))  {
			PlayerCount++;
		}

	}

	if ((PlayerCount == 0) && localPlayer->isAlive() && !localPlayer->isDormant()) {
		if (!config->walkbot.enabled) {
			AutoWinnerPrint("Starting Walkbot To Complete Objectives");
			config->walkbot.enabled = true;
			config->walkbot.lookSmoothing = 95.f;
		}
	}
	else {
		config->walkbot.enabled = false;
	}
	if (InFreeze) {
		config->walkbot.enabled = false;
	}


	if (((memory->globalVars->realtime - lastActivateTime) > onTime) && config->mmcrasher.enabled) {
		shouldEnabled = true;
		config->mmcrasher.enabled = false;
		frozenServerTime = Timing::TimingStats.m_fLastNetTickUpdateAlwaysRecord + 8;
		PlayerCount = 0;
		deadTime = memory->globalVars->realtime;
		wasOn = true;
		return;
	}

	if ((frozenServerTime < Timing::TimingStats.m_fLastNetTickUpdateAlwaysRecord) && wasOn) {
		if ((memory->globalVars->realtime - deadTime) > 32.f) {
			AutoWinnerPrint("Turning Down OnTime");
			onTime -= .5f;
		}
		else if ((memory->globalVars->realtime - deadTime) < 32.f) {
			AutoWinnerPrint("Turning Up OnTime");
			onTime += .5f;
		}
		InFreeze = false;
		wasOn = false;
	}





	if ((GameRulesProxy->HasMatchStarted()) && (PlayerCount > 0) && shouldEnabled && (frozenServerTime < Timing::TimingStats.m_fLastNetTickUpdateAlwaysRecord)) {
		config->walkbot.enabled = false;
		InFreeze = true;
		shouldEnabled = false;
		lastActivateTime = memory->globalVars->realtime;
		config->mmcrasher.enabled = true;
		AutoWinnerPrint("Lagger Enabled");
		AutoWinnerPrint(("PlayerCount = " + std::to_string(PlayerCount)).c_str());
		PlayerCount = 0;
	}


	if (localPlayer.get() && localPlayer->isAlive()) {
		if (!config->walkbot.enabled) {
			config->misc.antiAfkKick = true;
		}
	}
	else {
		config->misc.antiAfkKick = false;
	}


}