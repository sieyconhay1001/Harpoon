
#include "Walkbot.h"
#include "WalkbotSoundDispatchSystem.h"

#include "../../SDK/SDKAddition/SoundSystem.h"

#include <array>
static void player_hurt(GameEvent* event) {
	Entity* attacker = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("attacker")));

	if (!attacker || (attacker != localPlayer.get()))
		return;


	if (event->getInt("health") > 0)
		return;


	if (strstr("glock", event->getString("weapon"))) {
		SoundSystem::soundPlayer->Play("Glawk");
		return;
	}

	std::array<std::string, 3> sounds = { "PS1", "20Seconds", "Penal" };
	int randnum = (std::rand() % sounds.size());
	SoundSystem::soundPlayer->Play(sounds.at(randnum));






}



void WalkbotSound::WalkbotSound(GameEvent* event) {

#ifndef WALKBOT_BUILD
	//if (!config->walkbot.enabled)
	//	return;
#endif

#ifndef TROLL_BUILD
	if (!config->walkbot.talk)
		return;
#endif

	switch (fnv::hashRuntime(event->getName())) {
	case fnv::hash("player_hurt"):
		player_hurt(event);
		break;
	}









}