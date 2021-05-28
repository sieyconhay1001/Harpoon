#include <cassert>

#include "EventListener.h"
#include "../fnv.h"
#include "../Hacks/Misc.h"
#include "../Hacks/Visuals.h"
#include "../Interfaces.h"
#include "Memory.h"
//#include "Walkbot.h"
#include "../Hacks/OTHER/Debug.h"
#include "../Hacks/Grief.h"
#include "../Hacks/Walkbot/WalkbotSoundDispatchSystem.h"
#include "../SDK/OsirisSDK/Client.h"
#include "../SDK/SDK/ClientModeShared.h"
#include "../Memory.h"



//#include "../SDK/SDKAddition/SoundSystem.h"

EventListener::EventListener() noexcept
{
    assert(interfaces);

    interfaces->gameEventManager->addListener(this, "item_purchase");
    interfaces->gameEventManager->addListener(this, "round_start");
    interfaces->gameEventManager->addListener(this, "round_prestart");
    interfaces->gameEventManager->addListener(this, "round_freeze_end");
    interfaces->gameEventManager->addListener(this, "player_hurt");
    interfaces->gameEventManager->addListener(this, "start_vote");
    interfaces->gameEventManager->addListener(this, "vote_cast");

    interfaces->gameEventManager->addListener(this, "player_death");
    interfaces->gameEventManager->addListener(this, "bullet_impact");
    
    interfaces->gameEventManager->addListener(this, "hegrenade_detonate");
    interfaces->gameEventManager->addListener(this, "flashbang_detonate");

    interfaces->gameEventManager->addListener(this, "grenade_bounce");
    interfaces->gameEventManager->addListener(this, "molotov_detonate");
    if (const auto desc = memory->getEventDescriptor(interfaces->gameEventManager, "player_death", nullptr))
        std::swap(desc->listeners[0], desc->listeners[desc->listeners.size - 1]);
    else
        assert(false);
}

void EventListener::remove() noexcept
{
    assert(interfaces);

    interfaces->gameEventManager->removeListener(this);
}
    




#include "../Hacks/RageBot/Resolver.h"

#include "../SDK/OsirisSDK/Entity.h"
void VOTE_REVEAL(GameEvent* event) {

    if ((fnv::hashRuntime(event->getName())) == fnv::hash("start_vote")) {
        short uId = event->getInt("userid");
        int8_t type = event->getInt("type");
        short vote_parameter = event->getInt("type");
        char buffer[4096];
        snprintf(buffer, 4096, "UserID: %d Voted %d of vote tpye %d", uId, vote_parameter, type);
        Debug::QuickPrint(buffer);
    }
    else {
        int uId = event->getInt("entityid");
        short type = event->getInt("vote_option");
        char buffer[4096];
        

        Entity* voter = interfaces->entityList->getEntity(uId);

        if (!voter)
            return;

        const char* color = GreenChat;
        if (voter->isOtherEnemy(localPlayer.get()))
            color = RedChat;


        snprintf(buffer, 4096, "[%sVoteReveal%s] %s%s%s Voted %s%s", color, WhiteChat, PurpleChat, voter->getPlayerName(true).c_str(), WhiteChat, type ? RedChat : GreenChat, type ? "No" : "Yes");





        ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, buffer);

        Debug::LogItem log;

        std::string str{ buffer };
        std::wstring wstr( str.begin(),str.end() );

        log.text.push_back(wstr);
        log.PrintToScreen = true;
        log.PrintToConsole = true;
       
        Debug::LOG_OUT.push_back(log);

        Debug::QuickPrint(buffer);
    }
}

void EventListener::fireGameEvent(GameEvent* event)
{




    Resolver::GetHits(event);

    switch (fnv::hashRuntime(event->getName())) {
    case fnv::hash("round_start"):
        //Misc::freezeTime = false;
        WalkbotSound::WalkbotSound(event);
        break;
    case fnv::hash("round_prestart"):
        Misc::freezeTime = true;
        break;
    case fnv::hash("item_purchase"):
        break;
    case fnv::hash("round_freeze_end"):
        Misc::freezeTime = false;
        Grief::last_round_start = memory->globalVars->currenttime;
        WalkbotSound::WalkbotSound(event);
        break;
    case fnv::hash("player_death"):
        Grief::CalculateTeamDamage(event);
        //SkinChanger::updateStatTrak(*event);
        //SkinChanger::overrideHudIcon(*event);
        //Misc::killMessage(*event);
        WalkbotSound::WalkbotSound(event);
        Misc::killSound(*event);
        break;
    case fnv::hash("bullet_impact"):
        Visuals::bulletBeams(event);
        //StreamProofESP::bulletTracer(event);
        break;
    case fnv::hash("player_hurt"):
    {
       // if (config->misc.boganCunt) {
       //     SoundSystem::soundPlayer->Play("BOGAN_CUNT", true);
       // }
    }
        Grief::CalculateTeamDamage(event);
        WalkbotSound::WalkbotSound(event);
        //StreamProofESP::bulletTracer(event); <---- Make Happen only on damage delt? As indicator?
        Misc::AttackIndicator(event);
        Misc::playHitSound(*event);
        Visuals::hitEffect(event);
        Visuals::hitMarker(event);
        Debug::Logger(event);
        break;
    case fnv::hash("hegrenade_detonate"):
    case fnv::hash("flashbang_detonate"):
        Visuals::grenadeBeams(event);
        //break;//lol fall through
    case fnv::hash("grenade_bounce"):
    case fnv::hash("molotov_detonate"):
        Visuals::BounceRing(event);
        break;
    case fnv::hash("start_vote"):
    case fnv::hash("vote_cast"):
        VOTE_REVEAL(event);
        break;
    case fnv::hash("hltv_chat"):
        ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "(HGOTV Chat) %s", event->getString("text"));
        break;
    }
}
