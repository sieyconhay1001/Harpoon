#pragma once
#include "AutoQueuer.h"

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


void AutoQueuer::Run() {
	if (config->debug.AutoQueue) {

		if (!memory->MatchSingleton || *(*(uint8_t**)memory->MatchSingleton + 0xC)) {
			// Match Already Started (OR INVALID SINGLETON, IN WHICH CASE YOU BUGGED YOUR CODE OR OUTDATED SIG), Talk to Walkbot Server About That....
		}
		else {
			IMatchSession* session = interfaces->matchFramework->GetMatchSession();
			if (session) {
				Debug::QuickPrint("{AutoQueue} Starting Match Session");
				KeyValues* SessionSettings = session->GetSessionSettings();
				SessionSettings->setString("game/type", "classic");
				SessionSettings->setString("game/mode", "competitive");
				SessionSettings->setString("game/map", "de_dust2");
				session->UpdateSessionSettings(SessionSettings);
				static char newchar{ 0 };
				typedef void* (__cdecl* CallFn)();
				memory->HandleMatchStart(((CallFn)memory->GetSingleton)(), "", &newchar, &newchar, &newchar);
			}
			else {
				Debug::QuickPrint("{AutoQueue} Creating Match Session");
				memory->CreateMatchSession();
			}
		}

	}
}
// https://developer.valvesoftware.com/wiki/CSGO_Game_Mode_Commands