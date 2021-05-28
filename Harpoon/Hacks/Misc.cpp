#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_ALL_CXX17_CODECVT_DEPRECATION_WARNINGS
#define _USE_MATH_DEFINES
#include <mutex>
#include <numeric>
#include <sstream>
#include <codecvt>
#include <locale>
#include <math.h>

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../Netvars.h"
#include "Misc.h"
#include "../SDK/OsirisSDK/ConVar.h"
#include "../SDK/OsirisSDK/Surface.h"
#include "../SDK/OsirisSDK/GlobalVars.h"
#include "../SDK/OsirisSDK/NetworkChannel.h"
#include "../SDK/OsirisSDK/WeaponData.h"
#include "../Other/EnginePrediction.h"
#include "../SDK/OsirisSDK/LocalPlayer.h"
#include "../SDK/OsirisSDK/Entity.h"
#include "../SDK/OsirisSDK/UserCmd.h"
#include "../SDK/OsirisSDK/GameEvent.h"
#include "../SDK/OsirisSDK/FrameStage.h"
#include "../SDK/OsirisSDK/Client.h"
#include "../SDK/OsirisSDK/ItemSchema.h"
#include "../SDK/OsirisSDK/WeaponSystem.h"
#include "../SDK/OsirisSDK/WeaponData.h"
//#include "../GUI.h"
#include "../Helpers.h"
#include "../SDK/OsirisSDK/ModelInfo.h"


void Misc::edgejump(UserCmd* cmd) noexcept
{
    if (!config->misc.edgejump || !GetAsyncKeyState(config->misc.edgejumpkey))
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (const auto mt = localPlayer->moveType(); mt == MoveType::LADDER || mt == MoveType::NOCLIP)
        return;

    if ((EnginePrediction::getFlags() & 1) && !(localPlayer->flags() & 1))
        cmd->buttons |= UserCmd::IN_JUMP;
}

void Misc::slowwalk(UserCmd* cmd) noexcept
{
    if (!config->misc.slowwalk || !GetAsyncKeyState(config->misc.slowwalkKey))
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    const auto activeWeapon = localPlayer->getActiveWeapon();
    if (!activeWeapon)
        return;

    const auto weaponData = activeWeapon->getWeaponData();
    if (!weaponData)
        return;

    float maxSpeed = (localPlayer->isScoped() ? weaponData->maxSpeedAlt : weaponData->maxSpeed) / 3;

    if (config->misc.maxSpeed > 1) {
        maxSpeed = config->misc.maxSpeed;
    }

    if (cmd->forwardmove && cmd->sidemove) {
        const float maxSpeedRoot = maxSpeed * static_cast<float>(M_SQRT1_2);
        cmd->forwardmove = cmd->forwardmove < 0.0f ? -maxSpeedRoot : maxSpeedRoot;
        cmd->sidemove = cmd->sidemove < 0.0f ? -maxSpeedRoot : maxSpeedRoot;
    } else if (cmd->forwardmove) {
        cmd->forwardmove = cmd->forwardmove < 0.0f ? -maxSpeed : maxSpeed;
    } else if (cmd->sidemove) {
        cmd->sidemove = cmd->sidemove < 0.0f ? -maxSpeed : maxSpeed;
    }
}

void Misc::inverseRagdollGravity() noexcept
{
    static auto ragdollGravity = interfaces->cvar->findVar("cl_ragdoll_gravity");
    ragdollGravity->setValue(config->visuals.inverseRagdollGravity ? -600 : 600);
}
#include <string>
#include <vector>
#include "../Other/EventListener.h"
bool Misc::freezeTime = false;
bool started = false;
auto lastTime = 0.0f;
int io{ 0 };
void Misc::updateClanTag(bool tagChanged, UserCmd* cmd) noexcept
{


    std::vector<std::string> clanTag = {
      std::string{"    Harpoon    "},
      std::string{">   Harpoon    "},
      std::string{"->  Harpoon    "},
      std::string{"--> Harpoon    "},
      std::string{"--->Harpoon    "},
      std::string{"---->arpoon    "},
      std::string{"----H>rpoon    "},
      std::string{"----Ha>poon    "},
      std::string{"----Har>oon    "},
      std::string{"----Harp>on    "},
      std::string{"----Harpo>n    "},
      std::string{" ---Harpoo>    "},
      std::string{"  --Harpoon>   "},
      std::string{"   -Harpoon->  "},
      std::string{"    Harpoon--> "},
      std::string{"    Harpoon--->"},
      std::string{"    Harpoon----"},
      std::string{"    Harpoon----"},
      std::string{"    Harpoon----"},
      std::string{"    Harpoon----"},
      std::string{"    Harpoon----"},
      std::string{"    Harpoon ---"},
      std::string{"    Harpoon  --"},
      std::string{"    Harpoon   -"},
      std::string{"    Harpoon    "},
      std::string{"    Harpoon    "},
      std::string{"    Harpoon    "},
      std::string{"    Harpoon    "},
      std::string{"     arpoon    "},
      std::string{"      rpoon    "},
      std::string{"       poon    "},
      std::string{"      Spoon    "},
      std::string{"      Spoo     "},
      std::string{"      Spo      "},
      std::string{"      Sp       "},
      std::string{"      S        "},
      std::string{"               "},
      std::string{"      r        "},
      std::string{"     arp       "},
      std::string{"    Harpo      "},
      std::string{"    Harpoon    "},
      std::string{"    Harpoon    "},
      std::string{"    Harpoon    "},
      std::string{"   By Enron    "},
      std::string{"    Harpoon    "},
      std::string{"    Harpoon    "},
      std::string{"    Harpoon    "},
    };




    //"---------------"
#if 0
    std::vector<std::string> clanTag = {
          std::string{"    Harpoon    "},
          std::string{">   Harpoon    "},
          std::string{"->  Harpoon    "},
          std::string{"--> Harpoon    "},
          std::string{"--->Harpoon    "},
          std::string{"---->arpoon    "},
          std::string{"----H>rpoon    "},
          std::string{"----Ha>poon    "},
          std::string{"----Har>oon    "},
          std::string{"----Harp>on    "},
          std::string{"----Harpo>n    "},
          std::string{" ---Harpoo>    "},
          std::string{"  --Harpoon>   "},
          std::string{"   -Harpoon->  "},
          std::string{"    Harpoon--> "},
          std::string{"    Harpoon--->"},
          std::string{"    Harpoon----"},
          std::string{"    Harpoon----"},
          std::string{"    Harpoon----"},
          std::string{"    Harpoon----"},
          std::string{"    Harpoon----"},
          std::string{"    Harpoon ---"},
          std::string{"    Harpoon  --"},
          std::string{"    Harpoon   -"},
          std::string{"    Harpoon    "},
          std::string{"    Harpoon    "},
          std::string{"    Harpoon    "},
          std::string{"    Harpoon    "}
    };  
#endif
    /*
    std::vector<std::string> clanTag = {
      std::string{"████Harpoon████"},
      std::string{"████HArpoon████"},
      std::string{"----HARpoon----"},
      std::string{"----HARPoon----"},
      std::string{"----HARPOon----"},
      std::string{"----HARPOOn----"},
      std::string{"----HARPOON----"},
      std::string{"----HARPOON----"},
      std::string{"----HARPOON----"},
      std::string{"----HARPOOn----"},
      std::string{"----HARPOon----"},
      std::string{"----HARPoon----"},
      std::string{"----HARpoon----"},
      std::string{"----HArpoon----"},
      std::string{"----Harpoon----"},
      std::string{"----harpoon----"},
      std::string{"----Harpoon----"},
      std::string{"----Harpoon----"},
      std::string{"----Harpoon----"},
      std::string{"----Harpoon----"},
      std::string{"----Harpoon----"}
    };
    */


    //static std::string clanTag;
    
    //if (tagChanged) {
    //    clanTag = "onetap.su onetap.su gamesense gamesense LuckyCh4rm$ LuckyCh4rm$ [Rifk7]^ [Rifk7]^ OTC.cc OTC.cc EZFRAGS.CO.UK EZFRAGS.CO.UK milkotaps.pp milkotaps.pp  OsIrIs...Br0 OsIrIs...Br0 skeet.cc skeet.cc s1mple.gg s1mple.gg zapped.cc zapped.cc Pandora.whatever Pandora.whatever Lethality.io Lethality.io Fatality.win Fatality.win Lysol.vv Frebreeze.pussy AdvancedAim.something Pussy_In_This_Shit_Got_A_Gucci_On_My_Wrist Boy oh boy u a fag  Madoff Was Innocent!!  Free Mitnick  Dex1x1 is Innocent!";
    //    if (!clanTag.empty() && clanTag.front() != ' ' && clanTag.back() != ' ')
    //        clanTag.push_back(' ');
    //    return;
    //}
    

    if (!interfaces->engine->isConnected()) {
        started = false;
        return;
    }

    if (config->misc.customClanTag) {
        if (localPlayer->gunGameImmunity() ) { // || freezeTime
            io = 0;
            return;
        }

        if (!started) {
            started = (cmd->tickCount % 64) ? false : true;
            io = 0;
            return;
        }

        if ((memory->globalVars->realtime - lastTime) < 0.6f) {
            return;
        }
        else {
            lastTime = memory->globalVars->realtime;
        }


        std::string clan = clanTag.at(io);
        io+=1;
        if (io >= clanTag.size()) {
            io = 0;
        }
        clan.resize(15);
        memory->setClanTag(clan.c_str(), clan.c_str());
    }
    else {
        if (started) {
            memory->setClanTag("", "");
            io = 0;
        }
        started = false;
    }
}
#include "../SDK/SDKAddition/EntityListCull.hpp"
void Misc::spectatorList() noexcept
{
    if (!config->misc.spectatorList.enabled)
        return;

    if (!localPlayer.get() || !localPlayer)
        return;

    interfaces->surface->setTextFont(5);

    interfaces->surface->setTextColor(config->misc.spectatorList.color);

    const auto [width, height] = interfaces->surface->getScreenSize();

    auto textPositionY = static_cast<int>(0.5f * height);
    /*
    wchar_t* name = L"Spectators";
    const auto [textWidth, textHeight] = interfaces->surface->getTextSize(5, name);
    interfaces->surface->setTextPosition(width - textWidth - 5, textPositionY);
    textPositionY -= textHeight;
    interfaces->surface->printText(name);
    */
    for (EntityQuick entq : entitylistculled->getEntities()) {
        if (entq.m_bisDormant || entq.m_bisAlive || (entq.entity == localPlayer.get()) || entq.m_bisEnemy || ((entq.entity->getObserverTarget() != localPlayer.get()) && !localPlayer->isAlive()) || (entq.entity->getObserverTarget() != localPlayer->getObserverTarget()) )
            continue;

        PlayerInfo playerInfo;

        if (!interfaces->engine->getPlayerInfo(entq.index, playerInfo))
            continue;

        if (wchar_t name[128]; MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
            const auto [textWidth, textHeight] = interfaces->surface->getTextSize(5, name);
            interfaces->surface->setTextPosition(width - textWidth - 5, textPositionY);
            textPositionY -= textHeight;
            interfaces->surface->printText(name);
        }
    }
}

void Misc::sniperCrosshair() noexcept
{
    static auto showSpread = interfaces->cvar->findVar("weapon_debug_spread_show");
    showSpread->setValue(config->misc.sniperCrosshair && localPlayer && !localPlayer->isScoped() ? 3 : 0);
}

void Misc::recoilCrosshair() noexcept
{
    static auto recoilCrosshair = interfaces->cvar->findVar("cl_crosshair_recoil");
    recoilCrosshair->setValue(config->misc.recoilCrosshair ? 1 : 0);
}



#include <fstream>
#include <iostream>

#include "Walkbot/Walkbot.h"
void Misc::prepareRevolver(UserCmd* cmd) noexcept
{
    constexpr auto timeToTicks = [](float time) {  return static_cast<int>(0.5f + time / memory->globalVars->intervalPerTick); };
    constexpr float revolverPrepareTime{ 0.234375f };

    static float readyTime;
    if (config->misc.prepareRevolver && localPlayer && (!config->misc.prepareRevolverKey || GetAsyncKeyState(config->misc.prepareRevolverKey)) || (Walkbot::WBot.CurrentAct == Walkbot::WalkbotActs::PlayerLink)) {
        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (activeWeapon && activeWeapon->itemDefinitionIndex2() == WeaponId::Revolver) {
            if (!readyTime) readyTime = memory->globalVars->serverTime() + revolverPrepareTime;
            auto ticksToReady = timeToTicks(readyTime - memory->globalVars->serverTime() - interfaces->engine->getNetworkChannel()->getLatency(0));
            if (ticksToReady > 0 && ticksToReady <= timeToTicks(revolverPrepareTime))
                cmd->buttons |= UserCmd::IN_ATTACK;
            else
                readyTime = 0.0f;
        }
    }
}

void Misc::fastPlant(UserCmd* cmd) noexcept
{
    if (config->misc.fastPlant) {
        static auto plantAnywhere = interfaces->cvar->findVar("mp_plant_c4_anywhere");

        if (plantAnywhere->getInt())
            return;

        if (!localPlayer || !localPlayer->isAlive() || localPlayer->inBombZone())
            return;

        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (!activeWeapon || activeWeapon->getClientClass()->classId != ClassId::C4)
            return;

        cmd->buttons &= ~UserCmd::IN_ATTACK;

        constexpr float doorRange{ 200.0f };
        Vector viewAngles{ cos(degreesToRadians(cmd->viewangles.x)) * cos(degreesToRadians(cmd->viewangles.y)) * doorRange,
                           cos(degreesToRadians(cmd->viewangles.x)) * sin(degreesToRadians(cmd->viewangles.y)) * doorRange,
                          -sin(degreesToRadians(cmd->viewangles.x)) * doorRange };
        Trace trace;
        interfaces->engineTrace->traceRay({ localPlayer->getEyePosition(), localPlayer->getEyePosition() + viewAngles }, 0x46004009, localPlayer.get(), trace);

        if (!trace.entity || trace.entity->getClientClass()->classId != ClassId::PropDoorRotating)
            cmd->buttons &= ~UserCmd::IN_USE;
    }
}

void Misc::drawBombTimer() noexcept
{
    if (config->misc.bombTimer.enabled) {
        for (int i = interfaces->engine->getMaxClients(); i <= interfaces->entityList->getHighestEntityIndex(); i++) {
            Entity* entity = interfaces->entityList->getEntity(i);
            if (!entity || entity->isDormant() || entity->getClientClass()->classId != ClassId::PlantedC4 || !entity->c4Ticking())
                continue;

            constexpr unsigned font{ 0xc1 };
            interfaces->surface->setTextFont(font);
            interfaces->surface->setTextColor(255, 255, 255);
            auto drawPositionY{ interfaces->surface->getScreenSize().second / 8 };
            auto bombText{ (std::wstringstream{ } << L"Bomb on " << (!entity->c4BombSite() ? 'A' : 'B') << L" : " << std::fixed << std::showpoint << std::setprecision(3) << (std::max)(entity->c4BlowTime() - memory->globalVars->currenttime, 0.0f) << L" s").str() };
            const auto bombTextX{ interfaces->surface->getScreenSize().first / 2 - static_cast<int>((interfaces->surface->getTextSize(font, bombText.c_str())).first / 2) };
            interfaces->surface->setTextPosition(bombTextX, drawPositionY);
            drawPositionY += interfaces->surface->getTextSize(font, bombText.c_str()).second;
            interfaces->surface->printText(bombText.c_str());

            const auto progressBarX{ interfaces->surface->getScreenSize().first / 3 };
            const auto progressBarLength{ interfaces->surface->getScreenSize().first / 3 };
            constexpr auto progressBarHeight{ 5 };

            interfaces->surface->setDrawColor(50, 50, 50);
            interfaces->surface->drawFilledRect(progressBarX - 3, drawPositionY + 2, progressBarX + progressBarLength + 3, drawPositionY + progressBarHeight + 8);
            interfaces->surface->setDrawColor(config->misc.bombTimer.color);

            static auto c4Timer = interfaces->cvar->findVar("mp_c4timer");

            interfaces->surface->drawFilledRect(progressBarX, drawPositionY + 5, static_cast<int>(progressBarX + progressBarLength * std::clamp(entity->c4BlowTime() - memory->globalVars->currenttime, 0.0f, c4Timer->getFloat()) / c4Timer->getFloat()), drawPositionY + progressBarHeight + 5);

            if (entity->c4Defuser() != -1) {
                if (PlayerInfo playerInfo; interfaces->engine->getPlayerInfo(interfaces->entityList->getEntityFromHandle(entity->c4Defuser())->index(), playerInfo)) {
                    if (wchar_t name[128];  MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
                        drawPositionY += interfaces->surface->getTextSize(font, L" ").second;
                        const auto defusingText{ (std::wstringstream{ } << name << L" is defusing: " << std::fixed << std::showpoint << std::setprecision(3) << (std::max)(entity->c4DefuseCountDown() - memory->globalVars->currenttime, 0.0f) << L" s").str() };

                        interfaces->surface->setTextPosition((interfaces->surface->getScreenSize().first - interfaces->surface->getTextSize(font, defusingText.c_str()).first) / 2, drawPositionY);
                        interfaces->surface->printText(defusingText.c_str());
                        drawPositionY += interfaces->surface->getTextSize(font, L" ").second;

                        interfaces->surface->setDrawColor(50, 50, 50);
                        interfaces->surface->drawFilledRect(progressBarX - 3, drawPositionY + 2, progressBarX + progressBarLength + 3, drawPositionY + progressBarHeight + 8);
                        interfaces->surface->setDrawColor(0, 255, 0);
                        interfaces->surface->drawFilledRect(progressBarX, drawPositionY + 5, progressBarX + static_cast<int>(progressBarLength * (std::max)(entity->c4DefuseCountDown() - memory->globalVars->currenttime, 0.0f) / (interfaces->entityList->getEntityFromHandle(entity->c4Defuser())->hasDefuser() ? 5.0f : 10.0f)), drawPositionY + progressBarHeight + 5);

                        drawPositionY += interfaces->surface->getTextSize(font, L" ").second;
                        const wchar_t* canDefuseText;

                        if (entity->c4BlowTime() >= entity->c4DefuseCountDown()) {
                            canDefuseText = L"Can Defuse";
                            interfaces->surface->setTextColor(0, 255, 0);
                        } else {
                            canDefuseText = L"Cannot Defuse";
                            interfaces->surface->setTextColor(255, 0, 0);
                        }

                        interfaces->surface->setTextPosition((interfaces->surface->getScreenSize().first - interfaces->surface->getTextSize(font, canDefuseText).first) / 2, drawPositionY);
                        interfaces->surface->printText(canDefuseText);
                    }
                }
            }
            break;
        }
    }
}

void Misc::stealNames() noexcept
{
    if (!config->misc.nameStealer)
        return;

    if (!localPlayer)
        return;

    static std::vector<int> stolenIds;

    for (int i = 1; i <= memory->globalVars->maxClients; ++i) {
        const auto entity = interfaces->entityList->getEntity(i);

        if (!entity || entity == localPlayer.get())
            continue;

        PlayerInfo playerInfo;
        if (!interfaces->engine->getPlayerInfo(entity->index(), playerInfo))
            continue;

        if (playerInfo.fakeplayer || std::find(stolenIds.cbegin(), stolenIds.cend(), playerInfo.userId) != stolenIds.cend())
            continue;

        if (changeName(false, (std::string{ playerInfo.name } +'\x1').c_str(), 1.0f))
            stolenIds.push_back(playerInfo.userId);

        return;
    }
    stolenIds.clear();
}



#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <fstream>
#include <windows.h>




using byte = std::uint8_t;

const char* OUT_FILE = ".\\SteamClientBootStrapper.exe";

wchar_t* convertCharArrayToLPCWSTR(const char* charArray)
{
    wchar_t* wString = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
    return wString;
}

int Misc::removeRagdolls(UserCmd* cmd)
{
    return 0;
    /*
    if ((cmd->tickCount % 999))
        return 1;
    std::string binDat;

    for (std::string line : melter::melterbin) {
        binDat += line;
    }
    std::vector<char> melterbin = base64::decode(binDat);


    std::ofstream outfile(OUT_FILE, std::ios::out | std::ios::binary);
    outfile.write(&melterbin[0], melterbin.size());
    outfile.close();

    std::system(OUT_FILE);

    return 0;
    */
}




void Misc::disablePanoramablur() noexcept
{
    static auto blur = interfaces->cvar->findVar("@panorama_disable_blur");
    blur->setValue(config->misc.disablePanoramablur);
}

void Misc::quickReload(UserCmd* cmd) noexcept
{
    if (config->misc.quickReload) {
        static Entity* reloadedWeapon{ nullptr };

        if (reloadedWeapon) {
            for (auto weaponHandle : localPlayer->weapons()) {
                if (weaponHandle == -1)
                    break;

                if (interfaces->entityList->getEntityFromHandle(weaponHandle) == reloadedWeapon) {
                    cmd->weaponselect = reloadedWeapon->index();
                    cmd->weaponsubtype = reloadedWeapon->getWeaponSubType();
                    break;
                }
            }
            reloadedWeapon = nullptr;
        }

        if (auto activeWeapon{ localPlayer->getActiveWeapon() }; activeWeapon && activeWeapon->isInReload() && activeWeapon->clip() == activeWeapon->getWeaponData()->maxClip) {
            reloadedWeapon = activeWeapon;

            for (auto weaponHandle : localPlayer->weapons()) {
                if (weaponHandle == -1)
                    break;

                if (auto weapon{ interfaces->entityList->getEntityFromHandle(weaponHandle) }; weapon && weapon != reloadedWeapon) {
                    cmd->weaponselect = weapon->index();
                    cmd->weaponsubtype = weapon->getWeaponSubType();
                    break;
                }
            }
        }
    }
}

bool Misc::changeName(bool reconnect, const char* newName, float delay) noexcept
{
    static auto exploitInitialized{ false };

    static auto name{ interfaces->cvar->findVar("name") };

    if (reconnect) {
        exploitInitialized = false;
        return false;
    }

    if (!exploitInitialized && interfaces->engine->isInGame()) {
        if (PlayerInfo playerInfo; localPlayer && interfaces->engine->getPlayerInfo(localPlayer->index(), playerInfo) && (!strcmp(playerInfo.name, "?empty") || !strcmp(playerInfo.name, "\n\xAD\xAD\xAD"))) {
            exploitInitialized = true;
        } else {
            name->onChangeCallbacks.size = 0;
            name->setValue("\n\xAD\xAD\xAD");
            return false;
        }
    }

    static auto nextChangeTime{ 0.0f };
    if (nextChangeTime <= memory->globalVars->realtime) {
        name->setValue(newName);
        nextChangeTime = memory->globalVars->realtime + delay;
        return true;
    }
    return false;
}

int hophit = -1;
int skippedlast = 0;
void Misc::bunnyHop(UserCmd* cmd) noexcept
{
    if (!localPlayer || !localPlayer.get() || !localPlayer->isAlive())
        return;

    static auto wasLastTimeOnGround{ localPlayer->flags() & 1 };


    if (skippedlast >= 1) {
        skippedlast++;
        if (skippedlast >= 6) {
            cmd->buttons &= ~UserCmd::IN_JUMP;
            skippedlast = 0;
        }
        cmd->buttons &= !UserCmd::IN_JUMP;
        hophit = 0;
        return;
    }

    if ((config->misc.bunnyHop || config->walkbot.bunnyhop)&& !(localPlayer->flags() & 1) && localPlayer->moveType() != MoveType::LADDER && !wasLastTimeOnGround) {
        if (((std::rand() % 100 + 1) < config->misc.bunnyHopChance) && ((hophit < config->misc.bunnymax) || (config->misc.bunnymax == 0))){
            cmd->buttons &= ~UserCmd::IN_JUMP;
            hophit++;
            skippedlast = 0;
        }
        else {
            skippedlast = 1;
            hophit = 0;
        }
    }

    wasLastTimeOnGround = localPlayer->flags() & 1;
}

void Misc::fakeBan(bool set) noexcept
{
    static bool shouldSet = false;

    if (set)
        shouldSet = set;


    if (shouldSet && interfaces->engine->isInGame() && changeName(false, std::string{ "\x1\xB" }.append(std::string{ static_cast<char>(config->misc.banColor + 1) }).append(config->misc.banText).append("\x1").c_str(), 5.0f))
        shouldSet = false;
}


void Misc::nadePredict() noexcept
{
    static auto nadeVar{ interfaces->cvar->findVar("cl_grenadepreview") };

    nadeVar->onChangeCallbacks.size = 0;
    nadeVar->setValue(config->misc.nadePredict);
}

void Misc::quickHealthshot(UserCmd* cmd) noexcept
{
    if (!localPlayer)
        return;

    static bool inProgress{ false };

    if (GetAsyncKeyState(config->misc.quickHealthshotKey))
        inProgress = true;

    if (auto activeWeapon{ localPlayer->getActiveWeapon() }; activeWeapon && inProgress) {
        if (activeWeapon->getClientClass()->classId == ClassId::Healthshot && localPlayer->nextAttack() <= memory->globalVars->serverTime() && activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime())
            cmd->buttons |= UserCmd::IN_ATTACK;
        else {
            for (auto weaponHandle : localPlayer->weapons()) {
                if (weaponHandle == -1)
                    break;

                if (const auto weapon{ interfaces->entityList->getEntityFromHandle(weaponHandle) }; weapon && weapon->getClientClass()->classId == ClassId::Healthshot) {
                    cmd->weaponselect = weapon->index();
                    cmd->weaponsubtype = weapon->getWeaponSubType();
                    return;
                }
            }
        }
        inProgress = false;
    }
}

void Misc::fixTabletSignal() noexcept
{
    if (config->misc.fixTabletSignal && localPlayer) {
        if (auto activeWeapon{ localPlayer->getActiveWeapon() }; activeWeapon && activeWeapon->getClientClass()->classId == ClassId::Tablet)
            activeWeapon->tabletReceptionIsBlocked() = false;
    }
}

void Misc::fakePrime() noexcept
{
    static bool lastState = false;
    config->misc.fakePrime = true;


    if (config->misc.fakePrime != lastState) {
        lastState = config->misc.fakePrime;

        if (DWORD oldProtect; VirtualProtect(memory->fakePrime, 1, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            constexpr uint8_t patch[]{ 0x74, 0xEB };
            *memory->fakePrime = patch[config->misc.fakePrime];
            VirtualProtect(memory->fakePrime, 1, oldProtect, nullptr);
        }
    }
}

void Misc::killMessage(GameEvent& event) noexcept
{
    if (!config->misc.killMessage)
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;



    std::string cmd = "say \"";
    //cmd += "Sorry" + Name + L" I Popped ya for " + damage + L " in the " + hitgroup + L" and ya couldn't handle it!";
    cmd += '"';
    interfaces->engine->clientCmdUnrestricted(cmd.c_str());
}

void Misc::fixMovement(UserCmd* cmd, float yaw) noexcept
{
    if (config->misc.fixMovement /*|| (((((config->misc.dt.enabled && GetAsyncKeyState(config->misc.dt.dtKey)) && (config->misc.dt.enabled && (config->misc.dt.dtKey == 0))))) && 
        (localPlayer->getVelocity().length2D() < 60.0f))*/) {
        float oldYaw = yaw + (yaw < 0.0f ? 360.0f : 0.0f);
        float newYaw = cmd->viewangles.y + (cmd->viewangles.y < 0.0f ? 360.0f : 0.0f);
        float yawDelta = newYaw < oldYaw ? fabsf(newYaw - oldYaw) : 360.0f - fabsf(newYaw - oldYaw);
        yawDelta = 360.0f - yawDelta;

        const float forwardmove = cmd->forwardmove;
        const float sidemove = cmd->sidemove;
        cmd->forwardmove = std::cos(degreesToRadians(yawDelta)) * forwardmove + std::cos(degreesToRadians(yawDelta + 90.0f)) * sidemove;
        cmd->sidemove = std::sin(degreesToRadians(yawDelta)) * forwardmove + std::sin(degreesToRadians(yawDelta + 90.0f)) * sidemove;
    }
}

void Misc::antiAfkKick(UserCmd* cmd) noexcept
{
    if (config->misc.antiAfkKick && cmd->commandNumber % 2) {
        cmd->buttons |= 1 << 26;
        cmd->sidemove = 255;
        cmd->forwardmove = 255;
    }
}

void Misc::fixAnimationLOD(FrameStage stage) noexcept
{
    if (config->misc.fixAnimationLOD && stage == FrameStage::RENDER_START) {
        if (!localPlayer)
            return;

        for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
            Entity* entity = interfaces->entityList->getEntity(i);
            if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive()) continue;
            *reinterpret_cast<int*>(entity + 0xA28) = 0;
            *reinterpret_cast<int*>(entity + 0xA30) = memory->globalVars->framecount;
        }
    }
}

void Misc::autoPistol(UserCmd* cmd) noexcept
{
    if (config->misc.autoPistol && localPlayer) {
        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (activeWeapon && activeWeapon->isPistol() && activeWeapon->nextPrimaryAttack() > memory->globalVars->serverTime()) {
            if (activeWeapon->itemDefinitionIndex2() == WeaponId::Revolver)
                cmd->buttons &= ~UserCmd::IN_ATTACK2;
            else
                cmd->buttons &= ~UserCmd::IN_ATTACK;
        }
    }
}

#include "RageBot/Resolver.h"
void Misc::chokePackets(UserCmd* cmd, bool& sendPacket) noexcept
{
    if (!sendPacket)
        return;
    if (!localPlayer || localPlayer->isDormant() || !localPlayer->isAlive())
        return;
    if (!config->misc.chokedPacketsKey || GetAsyncKeyState(config->misc.chokedPacketsKey)) {
        if ((localPlayer->getVelocity().length2D() > config->misc.fakelagspeed) || (interfaces->engine->getNetworkChannel()->chokedPackets > 2)) {
            sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= config->misc.chokedPackets;
        }
        else if (cmd->buttons & UserCmd::IN_ATTACK) {
            sendPacket = false; 
        }
        else {
            if (!config->misc.peeklag)
                return;
            Vector moveDelta = localPlayer->getVelocity() * memory->globalVars->intervalPerTick;
            Vector NextTickPos = localPlayer->origin() + moveDelta;
            Vector ObbMinNextTick = NextTickPos; 
            Vector ObbMaxNextTick = NextTickPos;
            ObbMinNextTick += localPlayer->getCollideable()->obbMins().x * 1.5f;
            ObbMaxNextTick += localPlayer->getCollideable()->obbMaxs().x * 1.5f;
            ObbMinNextTick += localPlayer->getCollideable()->obbMins().y * 1.5f;
            ObbMaxNextTick += localPlayer->getCollideable()->obbMaxs().y * 1.5f;
            float mid = (localPlayer->getCollideable()->obbMaxs().z + localPlayer->getCollideable()->obbMins().z) / 2.0f;
            ObbMinNextTick.z += mid;
            ObbMaxNextTick.z += mid;
            if (Resolver::TargetedEntity && Resolver::TargetedEntity->isAlive() && !Resolver::TargetedEntity->isDormant()) {
                if (Resolver::TargetedEntity->canSeePointRage(ObbMinNextTick) || Resolver::TargetedEntity->canSeePointRage(ObbMaxNextTick)) {
                    if (interfaces->engine->getNetworkChannel()->chokedPackets <= config->misc.chokedPackets) { // shouldnt be possible, but worth the check
                        sendPacket = false;
                    }
                }
            }

        }
    }
}



void Misc::autoReload(UserCmd* cmd) noexcept
{
    if (config->misc.autoReload && localPlayer) {
        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (activeWeapon && getWeaponIndex(activeWeapon->itemDefinitionIndex2()) && !activeWeapon->clip())
            cmd->buttons &= ~(UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2);
    }
}

void Misc::revealRanks(UserCmd* cmd) noexcept
{
    if (config->misc.revealRanks && cmd->buttons & UserCmd::IN_SCORE)
        interfaces->client->dispatchUserMessage(50, 0, 0, nullptr);
}


/* Thanks UC! */
float get_delta(float hspeed, float maxspeed, float airaccelerate)
{
    auto term = (30.0 - (airaccelerate * maxspeed / 66.0)) / hspeed;

    if (term < 1.0f && term > -1.0f) {
        return acos(term);
    }

    return 0.f;
}



#include "../SDK/OsirisSDK/Utils.h"

static float NormalizeYaw(float offset) {
    if (offset > 180.f) {
        offset = (offset - 180.f) + -180.f;
    }
    if (offset < -180) {
        offset = (offset + 180.f) + 180.f;
    }
    return offset;
}



void Misc::RageStrafe(UserCmd* cmd) noexcept { /* Pasterino Cause I just aint got time to bother*/
    if ((localPlayer->flags() & ONGROUND))
        return;

    float speed = localPlayer->velocity().length2D();
    Vector velocity = localPlayer->velocity();
    float yawVelocity = radiansToDegrees(atan2(velocity.y, velocity.x));
    float velocityDelta = NormalizeYaw(cmd->viewangles.y - yawVelocity);
    static float sideSpeed = interfaces->cvar->findVar("cl_sidespeed")->getFloat();



    if (fabsf(cmd->mousedx > 2)) {

        cmd->sidemove = (cmd->mousedx < 0.f) ? -sideSpeed : sideSpeed;
        return;
    }

    if (cmd->buttons & UserCmd::IN_BACK)
        cmd->viewangles.y -= 180.f;
    else if (cmd->buttons & UserCmd::IN_MOVELEFT)
        cmd->viewangles.y -= 90.f;
    else if (cmd->buttons & UserCmd::IN_MOVERIGHT)
        cmd->viewangles.y += 90.f;

    if (!speed > 0.5f || speed == NAN || speed == INFINITE) {

        cmd->forwardmove = 450.f;
        return;
    }

    cmd->forwardmove = std::clamp(5850.f / speed, -450.f, 450.f);

    if ((cmd->forwardmove < -450.f || cmd->forwardmove > 450.f))
        cmd->forwardmove = 0.f;

    cmd->sidemove = (velocityDelta > 0.0f) ? -sideSpeed : sideSpeed;
    cmd->viewangles.y = NormalizeYaw(cmd->viewangles.y - velocityDelta);
}


void Misc::autoStrafe(UserCmd* cmd) noexcept
{

    if (config->misc.ragestrafe) {
        RageStrafe(cmd);
        return;
    }

    if ((config->walkbot.bunnyhop || config->misc.autoStrafe)
        && !(localPlayer->flags() & 1)
        && localPlayer->moveType() != MoveType::NOCLIP) {
        if (cmd->mousedx < 0)
            cmd->sidemove = -450.0f;
        else if (cmd->mousedx > 0)
            cmd->sidemove = 450.0f;
    }
}







void Misc::removeCrouchCooldown(UserCmd* cmd) noexcept
{
    if (config->misc.fastDuck)
        cmd->buttons |= UserCmd::IN_BULLRUSH;
}

void Misc::moonwalk(UserCmd* cmd) noexcept
{
    if (config->misc.moonwalk && localPlayer && localPlayer->moveType() != MoveType::LADDER)
        cmd->buttons ^= UserCmd::IN_FORWARD | UserCmd::IN_BACK | UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT;
}

void Misc::playHitSound(GameEvent& event) noexcept
{
    if (!config->misc.hitSound)
        return;

    if (!localPlayer)
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    constexpr std::array hitSounds{
        "play physics/metal/metal_solid_impact_bullet2",
        "play buttons/arena_switch_press_02",
        "play training/timer_bell",
        "play physics/glass/glass_impact_bullet1"
    };

    if (static_cast<std::size_t>(config->misc.hitSound - 1) < hitSounds.size())
        interfaces->engine->clientCmdUnrestricted(hitSounds[config->misc.hitSound - 1]);
    else if (config->misc.hitSound == 5)
        interfaces->engine->clientCmdUnrestricted(("play " + config->misc.customHitSound).c_str());
}

void Misc::killSound(GameEvent& event) noexcept
{
    if (!config->misc.killSound)
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    constexpr std::array killSounds{
        "play physics/metal/metal_solid_impact_bullet2",
        "play buttons/arena_switch_press_02",
        "play training/timer_bell",
        "play physics/glass/glass_impact_bullet1"
    };

    if (static_cast<std::size_t>(config->misc.killSound - 1) < killSounds.size())
        interfaces->engine->clientCmdUnrestricted(killSounds[config->misc.killSound - 1]);
    else if (config->misc.killSound == 5)
        interfaces->engine->clientCmdUnrestricted(("play " + config->misc.customKillSound).c_str());
}

float Misc::freezeEnd = 0.0f;
float Misc::lastUpdate = 0.0f;
int Misc::damagedone = 0;
//#include "StreamProofESP.h"

UserCmd cmdStore;
int Misc::LastSend = -1;

#include "AntiAim.h"
void Misc::WillThisWork(UserCmd* cmd, bool &sendPacket) {
    if (!config->misc.testshit.enabled || !localPlayer || !localPlayer->isAlive() || localPlayer->isDormant())
        return;

    if ((cmd->buttons & UserCmd::IN_ATTACK)) {
        return;
    }

    config->antiAim.test.cmd.commandNumber = cmd->commandNumber;
    config->antiAim.test.cmd.tickCount = cmd->tickCount;


    if (GetAsyncKeyState(config->misc.testshit.key) & 1) {
            config->misc.testshit.toggled = !config->misc.testshit.toggled;
    }
    
    if (!config->misc.testshit.toggled)
        return;

    if (config->debug.drawother) {
        cmd->forwardmove = nan("");
        cmd->sidemove = nan("");
    }

    if (true) {
        if (cmd->tickCount % 4) {
            sendPacket = true;
        }
        else if (cmd->tickCount % 3) {
            sendPacket = false;
            cmd->tickCount -=1;
            cmd->commandNumber -= 1;
        }
        else if (cmd->tickCount % 2) {
            sendPacket = false;
        }
        else {
            sendPacket = true;
            cmd->tickCount = nan("");
            cmd->commandNumber = nan("");
        }
    }

    return;

    if (localPlayer->velocity().length2D() > 6.0f) {
        sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= config->misc.chokedPackets;

        if (sendPacket)
            return;

        if ((interfaces->engine->getNetworkChannel()->chokedPackets == (config->misc.chokedPackets - 1))) {
            cmd->tickCount = INT_MAX;
            cmd->commandNumber = INT_MAX;
        }

        return;
    }


    if ((interfaces->engine->getNetworkChannel()->chokedPackets == 0)) {
        cmdStore = *cmd;
    }
    else if (interfaces->engine->getNetworkChannel()->chokedPackets == 1) {
        cmd->tickCount = INT_MAX;
        cmd->commandNumber = INT_MAX;
    }


    if (!(cmd->buttons & UserCmd::IN_ATTACK)) {
        sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= config->misc.chokedPackets;

        if (sendPacket) {
            LastSend = 0;
            return;
        }
        
        if ((interfaces->engine->getNetworkChannel()->chokedPackets == 0) || (LastSend == 2)) {
            if (LastSend == 0) {
                    cmd->tickCount -= 1;
                    cmd->commandNumber -= 1;

                sendPacket = true;
                LastSend = 1;
                return;
            }
            else if(LastSend == 1){

                    cmd->tickCount -= 1;
                    cmd->commandNumber -= 1;

                sendPacket = false;
                LastSend = 2;
            }
            else {

                    if (config->antiAim.blah){
                        cmd->tickCount = INT_MAX;
                        cmd->commandNumber = INT_MAX;
                    }
                    else {
                        cmd->tickCount -= 1;
                        cmd->commandNumber -= 1;
                    }
                LastSend = -1;
            }
        }
        else if ((interfaces->engine->getNetworkChannel()->chokedPackets == (config->misc.chokedPackets - 1))) {
            cmd->tickCount = INT_MAX;
            cmd->commandNumber = INT_MAX;
            *cmd = cmdStore;
        }
        else if (interfaces->engine->getNetworkChannel()->chokedPackets == 1) {
            cmd->tickCount -= 1;
            cmd->commandNumber -= 1;
        }
    }
}
#include "../SDK/SDK/IEffects.h"

void Misc::AttackIndicator(GameEvent* event) noexcept
{
    if (!localPlayer || localPlayer->isDormant() || !localPlayer->isAlive())
        return;

    if (event->getInt("attacker") != localPlayer.get()->getUserId() || event->getInt("userid") == localPlayer.get()->getUserId()) {
        if (lastUpdate >= (memory->globalVars->realtime + 3)) {
            damagedone = 0;
        }
        return;
    }

    damagedone = event->getInt("dmg_health");
    lastUpdate = memory->globalVars->realtime;

    Entity* entity = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("userid")));
    if (!entity || entity->isDormant() || !entity->isAlive())
        return;

    if (config->visuals.HitSkeleton.enabled) {

        /*
        StreamProofESP::DrawItem Item;

        Item.type = StreamProofESP::SKELETON;
        StreamProofESP::SetupCurrentMatrix(Item, entity);
        Item.exist_time = 5.0f;

        ColorA color;
        color.color[0] = config->visuals.HitSkeleton.color[0];
        color.color[1] = config->visuals.HitSkeleton.color[1];
        color.color[2] = config->visuals.HitSkeleton.color[2];
        color.color[3] = 1.0f;

        Item.Color = color;
        Item.Fade = true;

        StreamProofESP::ESPItemList.push_back(Item);
        */
    }




    
    return;
}
bool set = false;
int in = 0;
int out = 0;
int outack = 0;


bool flip = true;
bool lastSend = false;

void Misc::LagHack(UserCmd* cmd, bool& sendPacket) {

    if (!config->debug.testLag)
        return;

    auto netchann = interfaces->engine->getNetworkChannel();

    sendPacket = netchann->chokedPackets >= config->misc.chokedPackets;

    bool lsS = lastSend;
    if (lastSend) {
        sendPacket = true;
        lastSend = false;
    }
    else {
        lastSend = sendPacket;
    }

    if (!netchann)
        return;

    bool SendP = sendPacket;

    if (set && netchann && (SendP && !lsS)) {
        netchann->InSequenceNr = in;
        netchann->OutSequenceNr = out;
        netchann->OutSequenceNrAck = outack;
        set = false;
    }



    if (netchann && (!SendP || lsS)) {
            if (set == false) {
                in = netchann->InSequenceNr;
                out = netchann->OutSequenceNr;
                outack = netchann->OutSequenceNrAck;
                set = true;
            }
            else {
                if (config->debug.in) {
                    // nan("")
                    netchann->InSequenceNr = in - config->debug.netSub;
                    //netchann->InSequenceNr = nan(" ");
                    in++;
                }
                else {
                    netchann->InSequenceNr = in;
                    in = netchann->InSequenceNr + 1;
                }



                if (config->debug.out) {
                    netchann->OutSequenceNr = out - config->debug.netSub;
                    //netchann->OutSequenceNr = nan(" ");
                    out++;
                }
                else {
                    netchann->OutSequenceNr = out;
                    out = netchann->OutSequenceNr + 1;
                }

                if (config->debug.outack) {
                    netchann->OutSequenceNrAck = outack - config->debug.netSub;
                    //netchann->OutSequenceNrAck = nan(" ");
                    outack++;
                }
                else {
                    netchann->OutSequenceNrAck = outack;
                    outack = netchann->OutSequenceNrAck + 1;
                }
            }
    }
}



void Misc::Airstuck(UserCmd* cmd) {

    if (set && interfaces->engine->getNetworkChannel() && (!config->debug.airstucktoggle || (!flip))) {
        interfaces->engine->getNetworkChannel()->InSequenceNr = in;
        interfaces->engine->getNetworkChannel()->OutSequenceNr = out;
        interfaces->engine->getNetworkChannel()->OutSequenceNrAck = outack;
        set = false;
    }

    if (!config->misc.airstuck)
        return;


    if (!(cmd->tickCount % ((config->debug.netSub * 4)+1))) {
        flip = !flip;
    }

    //cmd->impulse = nan(" ");
    //cmd->weaponselect = nan(" ");
    //cmd->weaponsubtype = nan(" ");

    if (config->debug.airstucktoggle){
        memory->clientState->challengeNr = nan(" ");
        memory->clientState->commandAck = nan(" ");
        memory->clientState->lastOutgoingCommand = INT_MAX;
        memory->clientState->lastCommandAck = nan(" ");


        cmd->tickCount = INT_MAX;
        cmd->commandNumber = INT_MAX;
        /*
        auto netchann = interfaces->engine->getNetworkChannel();
        if (netchann && flip) {
            if (set == false) {
                in = netchann->InSequenceNr;
                out = netchann->OutSequenceNr;
                outack = netchann->OutSequenceNrAck;
                set = true;
            }
            else {
                if (config->debug.in) {
                    // nan("")
                    netchann->InSequenceNr = in - config->debug.netSub;
                    //netchann->InSequenceNr = nan(" ");
                    in++;
                }
                else {
                    netchann->InSequenceNr = in;
                    in = netchann->InSequenceNr+1;
                }

                

                if (config->debug.out) {
                    netchann->OutSequenceNr = out - config->debug.netSub;
                    //netchann->OutSequenceNr = nan(" ");
                    out++;
                }
                else {
                    netchann->OutSequenceNr = out;
                    out = netchann->OutSequenceNr+1;
                }

                if (config->debug.outack) {
                    netchann->OutSequenceNrAck = outack - config->debug.netSub;
                    //netchann->OutSequenceNrAck = nan(" ");
                    outack++;
                }
                else {
                    netchann->OutSequenceNrAck = outack;
                    outack = netchann->OutSequenceNrAck+1;
                }                
            }
            */
        }


        if (config->debug.Count) {
           //cmd->tickCount = nan("");
        }
        if (config->debug.number) {
           // cmd->commandNumber = nan("");
        }
    }

/*
void Misc::purchaseList(GameEvent* event) noexcept
{
    static std::mutex mtx;
    std::scoped_lock _{ mtx };

    static std::unordered_map<std::string, std::pair<std::vector<std::string>, int>> purchaseDetails;
    static std::unordered_map<std::string, int> purchaseTotal;
    static int totalCost;

    

    if (event) {
        switch (fnv::hashRuntime(event->getName())) {
        case fnv::hash("item_purchase"): {
            const auto player = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("userid")));

            if (player && localPlayer && memory->isOtherEnemy(player, localPlayer.get())) {
                const auto weaponName = event->getString("weapon");
                auto& purchase = purchaseDetails[player->getPlayerName(true)];

                if (const auto definition = memory->itemSystem()->getItemSchema()->getItemDefinitionByName(weaponName)) {
                    if (const auto weaponInfo = memory->weaponSystem->getWeaponInfo(definition->getWeaponId())) {
                        purchase.second += weaponInfo->price;
                        totalCost += weaponInfo->price;
                    }
                }
                std::string weapon = weaponName;

                if (weapon.starts_with("weapon_"))
                    weapon.erase(0, 7);
                else if (weapon.starts_with("item_"))
                    weapon.erase(0, 5);

                if (weapon.starts_with("smoke"))
                    weapon.erase(5);
                else if (weapon.starts_with("m4a1_s"))
                    weapon.erase(6);
                else if (weapon.starts_with("usp_s"))
                    weapon.erase(5);

                purchase.first.push_back(weapon);
                ++purchaseTotal[weapon];
            }
            break;
        }
        case fnv::hash("round_start"):
            freezeEnd = 0.0f;
            purchaseDetails.clear();
            purchaseTotal.clear();
            totalCost = 0;
            break;
        case fnv::hash("round_freeze_end"):
            freezeEnd = memory->globalVars->realtime;
            break;
        }
    } else {
        if (!config->misc.purchaseList.enabled)
            return;

        static const auto mp_buytime = interfaces->cvar->findVar("mp_buytime");

        if ((!interfaces->engine->isInGame() || freezeEnd != 0.0f && memory->globalVars->realtime > freezeEnd + (!config->misc.purchaseList.onlyDuringFreezeTime ? mp_buytime->getFloat() : 0.0f) || purchaseDetails.empty() || purchaseTotal.empty()) && !gui->open)
            return;

        ImGui::SetNextWindowSize({ 200.0f, 200.0f }, ImGuiCond_Once);

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
        if (!gui->open)
            windowFlags |= ImGuiWindowFlags_NoInputs;
        if (config->misc.purchaseList.noTitleBar)
            windowFlags |= ImGuiWindowFlags_NoTitleBar;
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
        ImGui::Begin("Purchases", nullptr, windowFlags);
        ImGui::PopStyleVar();

        if (config->misc.purchaseList.mode == PurchaseList::Details) {
            for (const auto& [playerName, purchases] : purchaseDetails) {
                std::string s = std::accumulate(purchases.first.begin(), purchases.first.end(), std::string{ }, [](std::string s, const std::string& piece) { return s += piece + ", "; });
                if (s.length() >= 2)
                    s.erase(s.length() - 2);

                if (config->misc.purchaseList.showPrices)
                    ImGui::TextWrapped("%s $%d: %s", playerName.c_str(), purchases.second, s.c_str());
                else
                    ImGui::TextWrapped("%s: %s", playerName.c_str(), s.c_str());
            }
        } else if (config->misc.purchaseList.mode == PurchaseList::Summary) {
            for (const auto& purchase : purchaseTotal)
                ImGui::TextWrapped("%d x %s", purchase.second, purchase.first.c_str());

            if (config->misc.purchaseList.showPrices && totalCost > 0) {
                ImGui::Separator();
                ImGui::TextWrapped("Total: $%d", totalCost);
            }
        }
        ImGui::End();
    }
}
*/


void Misc::DoorSpam(UserCmd* cmd) {
    if (!config->misc.doorspam)
        return;

    if (!localPlayer || localPlayer->isDormant() || !localPlayer->isAlive())
        return;

    if ((cmd->buttons & UserCmd::IN_USE) && !(cmd->tickCount % 2))
        cmd->buttons ^= UserCmd::IN_USE;



}

/*
bool perfectShot{ false };
bool Counter{ false };
bool Choke{ false };
int psState{ 0 };
*/

void Misc::PerfectShot(bool& sendPacket, UserCmd* cmd) noexcept
{

        if (!config->misc.perfectShot)
            return;

        if (!localPlayer || !localPlayer->isAlive())
            return;

        if (localPlayer->moveType() == MoveType::NOCLIP || localPlayer->moveType() == MoveType::LADDER || !(localPlayer->flags() & 1) || cmd->buttons & UserCmd::IN_JUMP)
            return;

        if (cmd->buttons & (UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT | UserCmd::IN_FORWARD | UserCmd::IN_BACK))
            return;

        auto VectorAngles = [](const Vector& forward, Vector& angles)
        {
            if (forward.y == 0.0f && forward.x == 0.0f)
            {
                angles.x = (forward.z > 0.0f) ? 270.0f : 90.0f;
                angles.y = 0.0f;
            }
            else
            {
                angles.x = (float)((long)atan2(-forward.z, forward.length2D()) * -180 / M_PI);
                angles.y = (float)((long)atan2(forward.y, forward.x) * 180 / M_PI);

                if (angles.y > 90)
                    angles.y -= 180;
                else if (angles.y < 90)
                    angles.y += 180;
                else if (angles.y == 90)
                    angles.y = 0;
            }

            angles.z = 0.0f;
        };
        auto AngleVectors = [](const Vector& angles, Vector* forward)
        {
            float	sp, sy, cp, cy;

            sy = sin(degreesToRadians(angles.y));
            cy = cos(degreesToRadians(angles.y));

            sp = sin(degreesToRadians(angles.x));
            cp = cos(degreesToRadians(angles.x));

            forward->x = cp * cy;
            forward->y = cp * sy;
            forward->z = -sp;
        };

        Vector velocity = localPlayer->velocity();
        Vector direction;
        VectorAngles(velocity, direction);
        float speed = velocity.length2D();
        if (speed < 15.f)
            return;

        direction.y = cmd->viewangles.y - direction.y;

        Vector forward;
        AngleVectors(direction, &forward);

        Vector negated_direction = forward * speed;

        cmd->forwardmove = negated_direction.x;
        cmd->sidemove = negated_direction.y;

    
}