#include "Grief.h"
#include "../SDK/OsirisSDK/Surface.h"
#include "../SDK/OsirisSDK/EntityList.h"
#include "../Interfaces.h"
#include "../SDK/OsirisSDK/Entity.h"
#include "../SDK/OsirisSDK/Surface.h"
#include "../SDK/OsirisSDK/LocalPlayer.h"
#include "../SDK/OsirisSDK/UserCmd.h"
#include "../SDK/OsirisSDK/Engine.h"
#include "../Config.h"
#include <mutex>
#include <numeric>
#include <sstream>
#include <codecvt>
#include <locale>


Grief::G_Player Players[65];
std::vector<std::wstring> text;


void Grief::TeamDamageCounter() {

	if (!interfaces->engine->isConnected() || !interfaces->engine->isInGame())
		return;
	


	if (!localPlayer.get())
		return;


	interfaces->surface->setTextFont(5);


	const auto [screenWidth, screenHeight] = interfaces->surface->getScreenSize();

	//int max;

	std::wstring BASESTRING = L"AAAAAAAAAAAAAAAAAAAAAA"; /* Ghetto But It Works */
	const auto [text_size_w, text_size_h] = interfaces->surface->getTextSize(5, BASESTRING.c_str());

	Debug::Screen.CurrPosH = (screenHeight - ((screenHeight / 2) / 2)) - (text_size_h * 2) - (text_size_h/2) - 2;

	for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
		if (i > 65)
			continue;

		auto entity = interfaces->entityList->getEntity(i);

		if (!entity || entity->isDormant()) {
			if (!(i > 65) && (Players[i].valid == true) ) {
				Players[i].valid = false;
			}
			continue;
		}

		if ((entity == localPlayer.get())) {
			continue;
		}

		if (!Players[i].valid && !localPlayer->isOtherEnemy(entity)) {
			Players[i].valid = true;
			Players[i].UserID = entity->getUserId();
			Players[i].Name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(entity->getPlayerName(true));
			Players[i].CurrDamage = 0;
			Players[i].CurrKills = 0;
		}

		if (Players[i].UserID != entity->getUserId()) {
			Players[i].valid = false;
		}


	}


	if (!config->grief.teamDamageOverlay)
		return;


	for(int i = 0; i < 65; i ++){
		if (!Players[i].valid) {
			continue;
		}
		int currPosinW = 5;

		/* Draw Health Bar*/

		interfaces->surface->setDrawColor(config->debug.box.color[0], config->debug.box.color[1], config->debug.box.color[2], 180);
		interfaces->surface->drawFilledRect(currPosinW, Debug::Screen.CurrPosH, currPosinW + text_size_w, Debug::Screen.CurrPosH + text_size_h);

		int Pos = (currPosinW + text_size_w) - (((text_size_w + currPosinW) - currPosinW) * (Players[i].CurrDamage / 400.f));

		interfaces->surface->setDrawColor(0, 230, 0, 255);
		interfaces->surface->drawFilledRect( currPosinW, Debug::Screen.CurrPosH, Pos, Debug::Screen.CurrPosH + text_size_h);

		if (Pos > 0) {

			interfaces->surface->setDrawColor(180, 0, 0, 230);
			interfaces->surface->drawFilledRect(Pos+1, Debug::Screen.CurrPosH, currPosinW + text_size_w, Debug::Screen.CurrPosH + text_size_h);
		}

		interfaces->surface->setDrawColor(config->debug.box.color[0], config->debug.box.color[1], config->debug.box.color[2], 180);
		interfaces->surface->drawOutlinedRect(currPosinW, Debug::Screen.CurrPosH, currPosinW + text_size_w, Debug::Screen.CurrPosH + text_size_h);

		/*Draw Damage*/
		std::wstring damage{ L" (" + std::to_wstring(Players[i].CurrDamage) + L"/400 DMG) " };
		//const auto [text_size_kills_w, text_size_kills_h] = interfaces->surface->getTextSize(Surface::font, kills.c_str());
		interfaces->surface->setTextColor(200,200,200,255);
		interfaces->surface->setTextPosition(currPosinW, Debug::Screen.CurrPosH);
		interfaces->surface->printText(damage.c_str());


		currPosinW += text_size_w + 2;
		/* Draw Kills*/

		std::wstring kills{ L" (" + std::to_wstring(Players[i].CurrKills/2) + L"/3 Kills) " };
		const auto [text_size_kills_w, text_size_kills_h] = interfaces->surface->getTextSize(Surface::font, kills.c_str());

		interfaces->surface->setTextColor(config->debug.animStateMon.color);
		interfaces->surface->setTextPosition(currPosinW, Debug::Screen.CurrPosH);
		interfaces->surface->printText(kills.c_str());
		currPosinW += text_size_kills_w;

		/* Draw Name */
		const auto [text_size_name_w, text_size_name_h] = interfaces->surface->getTextSize(Surface::font, Players[i].Name.c_str());

		interfaces->surface->setTextColor(config->debug.animStateMon.color);
		interfaces->surface->setTextPosition(currPosinW, Debug::Screen.CurrPosH);
		interfaces->surface->printText(Players[i].Name.c_str());
		//currPosinW += text_size_name_w;
		Debug::Screen.CurrPosH += text_size_h + 2;

		/* Draw Early Round Damage */


		//interfaces->surface->setDrawColor(config->debug.box.color[0], config->debug.box.color[1], config->debug.box.color[2], 180);
		//interfaces->surface->drawFilledRect(currPosinW, Debug::Screen.CurrPosH, currPosinW + text_size_w, Debug::Screen.CurrPosH + text_size_h);
		/*
		int Pos = (currPosinW + text_size_w) - (((text_size_w + currPosinW) - currPosinW) * (Players[i].EarlyRoundDamage / 90.f));

		interfaces->surface->setDrawColor(0, 0, 230, 255);
		interfaces->surface->drawFilledRect(currPosinW, Debug::Screen.CurrPosH, Pos, Debug::Screen.CurrPosH + text_size_h);

		if (Pos > 0) {

			interfaces->surface->setDrawColor(180, 0, 0, 230);
			interfaces->surface->drawFilledRect(Pos + 1, Debug::Screen.CurrPosH, currPosinW + text_size_w, Debug::Screen.CurrPosH + text_size_h);
		}

		interfaces->surface->setDrawColor(config->debug.box.color[0], config->debug.box.color[1], config->debug.box.color[2], 180);
		interfaces->surface->drawOutlinedRect(currPosinW, Debug::Screen.CurrPosH, currPosinW + text_size_w, Debug::Screen.CurrPosH + text_size_h);

		*/

	}

}

#include "../fnv.h"

#include "../SDK/OsirisSDK/ClassId.h"
#include "../SDK/SDKAddition/Utils/VectorMath.h"


void NormalizeAngles(Vector& angle)
{
	while (angle.x > 89.0f)
		angle.x -= 180.f;

	while (angle.x < -89.0f)
		angle.x += 180.f;

	while (angle.y > 180.f)
		angle.y -= 360.f;

	while (angle.y < -180.f)
		angle.y += 360.f;
}


void Grief::BlockBot(UserCmd* cmd) {
	if (!(GetAsyncKeyState(config->grief.BlockBot) & 0x8000)) {
		return;
	}


	Entity* Target = 0;
	float DistanceMeme = 900.f;

	for (int i = 0; i < interfaces->engine->getMaxClients(); i++) {

		Entity* entity = interfaces->entityList->getEntity(i);
		if (!entity || entity->isDormant() || !entity->isAlive() || (entity == localPlayer.get()) || entity->isOtherEnemy(localPlayer.get()))
			continue;


		float DistToEnemy = localPlayer->origin().distTo(entity->origin());

		if (DistToEnemy < DistanceMeme) {
			DistanceMeme = DistToEnemy;
			Target = entity;
		}
	}

	if (!Target || (Target->velocity().length2D() <= 10.f))
		return;


	//Vector AngBackup = cmd->viewangles;

	Vector Angle = VectorMath::calculateRelativeAngle(localPlayer->getEyePosition(), Target->getEyePosition(), cmd->viewangles);
	/* TODO: Calculate True Movement */

	NormalizeAngles(Angle);


	/*
	Todo: 
	      This Velo - Last Velo
	      Rotate around Z by yaw	
	*/


	if (Angle.y < 0.0f) {
		cmd->sidemove = 450.f;
	}
	else if (Angle.y > 0.0f) {
		cmd->sidemove = -450.f;
	}
	else {
		cmd->sidemove = 0.f;
		cmd->forwardmove = 450.f;
	}


}




void Grief::GrenadeAim(UserCmd* cmd) {

	if (!(GetAsyncKeyState(config->grief.grenadeAIM) & 0x8000)) {
		return;
	}


	for (int i = 0; i < interfaces->engine->getMaxClients(); i++) {
		Entity* entity = interfaces->entityList->getEntity(i);

		if (!entity  || entity->isDormant() || !entity->isAlive() || !entity->isPlayer() || entity->isOtherEnemy(localPlayer.get()))
			continue;

		if (!entity->IsThrowingGrenade()) {
			continue;
		}

		Vector ang = VectorMath::calculateRelativeAngle(localPlayer->getEyePosition(), entity->getEyePosition(), cmd->viewangles);
		cmd->buttons |= UserCmd::IN_ATTACK;
		cmd->viewangles += ang;
		interfaces->engine->setViewAngles(cmd->viewangles);
		break;
	}
}


void Grief::CalculateTeamDamage(GameEvent* event) {
	uint32_t eventHash = fnv::hashRuntime(event->getName());
	Entity* attacker = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("attacker")));
	Entity* player = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("userid")));

	if (!attacker || !player)
		return;

	if (attacker->isOtherEnemy(player))
		return;

	if (attacker->isOtherEnemy(localPlayer.get()))
		return;

	if ((attacker == localPlayer.get()))
		return;

	if (!Players[attacker->index()].valid) {
		Players[attacker->index()].valid = true;
		Players[attacker->index()].Name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(attacker->getPlayerName(true));
	}

	if (eventHash = fnv::hash("player_hurt")) {
		Players[attacker->index()].CurrDamage += event->getInt("dmg_health");

		if ((memory->globalVars->currenttime - last_round_start) < 15.f) {
			Players[attacker->index()].EarlyRoundDamage += event->getInt("dmg_health");
		}


		if (event->getInt("health") <= 0) {
			Players[attacker->index()].CurrKills += 1;
		}

	}
	
	
	//else if (eventHash = fnv::hash("player_death")) {
	//	Players[attacker->index()].CurrKills += 1;
	//}
}






static void toChat(std::wstring intext) {
	std::wstring command = { L"say \"" + intext + L"\"" };
	const wchar_t* wcmd = command.c_str();
	size_t size = (wcslen(wcmd) + 1) * sizeof(wchar_t);
	char* cmd = new char[size];
	std::wcstombs(cmd, wcmd, size);
	interfaces->engine->clientCmdUnrestricted(cmd);
	delete cmd;
}



#include "../SDK/SDKAddition/EntityListCull.hpp"
void Grief::ChatSpam(UserCmd* cmd) {

	if (!config->grief.spam.enabled)
		return;



	if (!text.empty() && !(cmd->tickCount % 10)) {
		toChat(text.back());
		text.pop_back();
	}

	if (config->grief.spam.position) {
		if (cmd->tickCount % 384) // Roughly every 10 seconds on a 64 tick server
			return;

		for (EntityQuick entq : entitylistculled->getEnemies()) {
			std::wstring place = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(entq.entity->lastPlaceName());
			std::wstring Name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(entq.entity->getPlayerName(true));

			int randint = std::rand() % 11 + 1;
			std::wstring chat = { L"" };
			switch (randint) {
			case 1:
				chat = { L"Yo " + Name + L" I see you hangin at " + place };
				break;
			case 2:
				chat = { L"I think I saw " + Name + L" down at " + place + L" getting head by a toothless crackwhore...." };
				break;
			case 3:
				chat = {L"Hey, Team! We don't gotta worry about " + Name + L" he's down at " + place + L" doing some gay shit or smth idk"};
				break;
			case 4:
				chat = { Name + L" is at " + place + L" which is " + std::to_wstring(localPlayer->getEyePosition().distTo(entq.entity->getEyePosition())) + L" Units away from me" };
				break;
			case 5:
				chat = { Name + L" whatcha doing there at " + place + L" don't you know thats where the pedo's live? unless youre about that..... kinda sus ngl" };
				break;
			case 6:
				chat = { L"Once upon a time there was a little faggot named " + Name + L" who hung around " + place + L" when he really shouldn't of.... then one day HE GOT FUCKING MERC'D" };
				break;
			case 7:
				chat = { L"Yea honey? Out of milk? Oh no problem, i'll pick it up on my way home. Just gotta head down to " + place + L" to pop a one deag into " + Name };
				break;
			case 8:
				chat = { L"Dude, " + Name + L" if you're annoyed, be even more so, cause overwatch can't see the chat, and im not hacking in any other way G, so aint no OverWatch for me dawg. Anyways you're at " + place };
				break;
			case 9:
				chat = { Name + L" watch out, if you step foot in " + place + L" you catch AIDs!" };
				break;
			case 10:
				chat = { L"Got my barrell against the back of " + Name + L"'s Neck down at " + place };
				break;
			case 11:
				chat = { L"Watch out I think " + Name + L" is an Imposter Amoung Us, the crewmates, he's down at " + place };
				break;
			default:
				chat = { Name + L" you should laugh at me, my switch case reached an impossible condition, anyways you're at " + place };
				break;
			}



			text.push_back(chat);
		}
	}
}


static auto exploitInitialized{ false };


int change_count = 0;
static void changeName(const char* newName) noexcept
{

	if (change_count >= 5)
		exploitInitialized = false;
	else
		++change_count;



	
	static auto name{ interfaces->cvar->findVar("name") };

	//if (reconnect) {
	//	exploitInitialized = false;
	//	return false;
	//}

	if (!exploitInitialized && interfaces->engine->isInGame()) {
		/*
		if (PlayerInfo playerInfo; localPlayer && interfaces->engine->getPlayerInfo(localPlayer->index(), playerInfo) && (!strcmp(playerInfo.name, "?empty") || !strcmp(playerInfo.name, "\n\xAD\xAD\xAD"))) {
			exploitInitialized = true;
		}
		else {
		*/
			name->onChangeCallbacks.size = 0;
			name->setValue("\n\xAD\xAD\xAD");
			exploitInitialized = true;
			change_count = 0;
			return;
		//}
	}

	if (!interfaces->engine->isInGame())
		return;

	name->setValue(newName);
}



int set = 32;
void Grief::TriggerBotNameSteal(UserCmd* cmd) {

	if (!config->grief.triggername)
		return;

	if (set < 32) {
		++set;
		return;
	}



	/*
	const Vector viewAngles{ std::cos(degreesToRadians(cmd->viewangles.x)) * std::cos(degreesToRadians(cmd->viewangles.y)) * 3000,
							 std::cos(degreesToRadians(cmd->viewangles.x)) * std::sin(degreesToRadians(cmd->viewangles.y)) * 3000,
							-std::sin(degreesToRadians(cmd->viewangles.x)) * 3000 };
	*/
	//Vector viewAngles = cmd->viewangles * 4000;
	/*
	const auto aimPunch = localPlayer->getAimPunch();

	const Vector viewAngles{ std::cos(degreesToRadians(cmd->viewangles.x + aimPunch.x)) * std::cos(degreesToRadians(cmd->viewangles.y + aimPunch.y)) * 3000,
							 std::cos(degreesToRadians(cmd->viewangles.x + aimPunch.x)) * std::sin(degreesToRadians(cmd->viewangles.y + aimPunch.y)) * 3000,
							-std::sin(degreesToRadians(cmd->viewangles.x + aimPunch.x)) * 3000 };
	*/
	Vector viewAngles = Vector::fromAngle(cmd->viewangles) * 6000;
	const auto startPos = localPlayer->getEyePosition();
	const auto endPos = startPos + viewAngles;

	Trace trace;
	interfaces->engineTrace->traceRay({ startPos, endPos }, 0x46004009, localPlayer.get(), trace);

	if (!trace.entity || !trace.entity->isPlayer() || !trace.entity->isOtherEnemy(localPlayer.get())) {
		return;
	}

	changeName((std::string{ trace.entity->getPlayerName(true)} + '\x1').c_str());
	set = 0;
}