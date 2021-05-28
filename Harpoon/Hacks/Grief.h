#pragma once

#include "Other/Debug.h"
#include "../SDK/OsirisSDK/GameEvent.h"


class UserCmd;



namespace Grief {
	void TeamDamageCounter();
	void CalculateTeamDamage(GameEvent* event);
	void GrenadeAim(UserCmd* cmd);
	void BlockBot(UserCmd* cmd);
	void ChatSpam(UserCmd* cmd);
	void TriggerBotNameSteal(UserCmd* cmd);

	struct G_Player {
		bool valid = false;
		int CurrDamage = 0;
		int EarlyRoundDamage = 0;
		int CurrKills = 0;
		int UserID = 0;
		std::wstring Name = L"";
	};


	struct G_Message {
		std::wstring text;
	};


	inline float last_round_start = 0.f;

	//extern G_Player Players[65];


}

