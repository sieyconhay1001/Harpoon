
#include "Aimbot.h"


#include "../Config.h"
#include "../SDK/OsirisSDK/ConVar.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/OsirisSDK/Entity.h"
#include "../SDK/OsirisSDK/UserCmd.h"
#include "../SDK/OsirisSDK/Vector.h"
#include "../SDK/OsirisSDK/WeaponId.h"
#include "../SDK/OsirisSDK/GlobalVars.h"
#include "../SDK/OsirisSDK/PhysicsSurfaceProps.h"
#include "../SDK/OsirisSDK/WeaponData.h"
#include "../SDK/OsirisSDK/ModelInfo.h"
#include "../SDK/OsirisSDK/matrix3x4.h"
#include "../SDK/OsirisSDK/Math.h"
#include "Backtrack.h"
#include "../SDK/OsirisSDK/Angle.h"
//DEBUG
#include <fstream>
#include <iostream>
#include <cstddef>
#include <thread>
#include <future>

#include <math.h>

static float handleBulletPenetration(SurfaceData* enterSurfaceData, const Trace& enterTrace, const Vector& direction, Vector& result, float penetration, float damage) noexcept
{
    Vector end;
    Trace exitTrace;
    __asm {
        mov ecx, end
        mov edx, enterTrace
    }
    if (!memory->traceToExit(enterTrace.endpos.x, enterTrace.endpos.y, enterTrace.endpos.z, direction.x, direction.y, direction.z, exitTrace))
        return -1.0f;

    SurfaceData* exitSurfaceData = interfaces->physicsSurfaceProps->getSurfaceData(exitTrace.surface.surfaceProps);

    float damageModifier = 0.16f;
    float penetrationModifier = (enterSurfaceData->penetrationmodifier + exitSurfaceData->penetrationmodifier) / 2.0f;

    if (enterSurfaceData->material == 71 || enterSurfaceData->material == 89) {
        damageModifier = 0.05f;
        penetrationModifier = 3.0f;
    }
    else if (enterTrace.contents >> 3 & 1 || enterTrace.surface.flags >> 7 & 1) {
        penetrationModifier = 1.0f;
    }

    if (enterSurfaceData->material == exitSurfaceData->material) {
        if (exitSurfaceData->material == 85 || exitSurfaceData->material == 87)
            penetrationModifier = 3.0f;
        else if (exitSurfaceData->material == 76)
            penetrationModifier = 2.0f;
    }

    damage -= 11.25f / penetration / penetrationModifier + damage * damageModifier + (exitTrace.endpos - enterTrace.endpos).squareLength() / 24.0f / penetrationModifier;

    result = exitTrace.endpos;
    return damage;
}


bool Aimbot::canScan(Entity* entity, const Vector& destination, const WeaponInfo* weaponData, int minDamage, bool allowFriendlyFire, float& damageret) noexcept
{
    if (!localPlayer)
        return false;

    float damage{ static_cast<float>(weaponData->damage) };

    Vector start{ localPlayer->getEyePosition() };
    Vector direction{ destination - start };
    direction /= direction.length();

    int hitsLeft = 4;

    while (damage >= 1.0f && hitsLeft) {
        Trace trace;
        interfaces->engineTrace->traceRay({ start, destination }, 0x4600400B, localPlayer.get(), trace);

        if (!allowFriendlyFire && trace.entity && trace.entity->isPlayer() && !localPlayer->isOtherEnemy(trace.entity))
            return false;

        if (trace.fraction == 1.0f)
            break;

        if (trace.entity == entity && trace.hitgroup > HitGroup::Generic && trace.hitgroup <= HitGroup::RightLeg) {
            damage = HitGroup::getDamageMultiplier(trace.hitgroup) * damage * powf(weaponData->rangeModifier, trace.fraction * weaponData->range / 500.0f);

            if (float armorRatio{ weaponData->armorRatio / 2.0f }; HitGroup::isArmored(trace.hitgroup, trace.entity->hasHelmet()))
                damage -= (trace.entity->armor() < damage * armorRatio / 2.0f ? trace.entity->armor() * 4.0f : damage) * (1.0f - armorRatio);

            damageret = damage;
            return (damage >= minDamage) || (damage >= entity->health());


        }
        const auto surfaceData = interfaces->physicsSurfaceProps->getSurfaceData(trace.surface.surfaceProps);

        if (0.1f > surfaceData->penetrationmodifier)
            break;

        damage = handleBulletPenetration(surfaceData, trace, direction, start, weaponData->penetration, damage);

        damageret = damage;

        hitsLeft--;
    }
    return false;
}

static void setRandomSeed(int seed) noexcept
{
    using randomSeedFn = void(*)(int);
    static auto randomSeed{ reinterpret_cast<randomSeedFn>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomSeed")) };
    randomSeed(seed);
}

static float getRandom(float min, float max) noexcept
{
    using randomFloatFn = float(*)(float, float);
    static auto randomFloat{ reinterpret_cast<randomFloatFn>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat")) };
    return randomFloat(min, max);
}

#include "Other/Debug.h"



float Aimbot::hitChance(Entity* localPlayer, Entity* entity, Entity* weaponData, const Vector& destination, const UserCmd* cmd, const int hitChance) noexcept
{

    if (!hitChance)
        return 100.0f;

    Debug::LogItem item;
    item.PrintToConsole = true;

    int maxSeed = 256;

    const Angle angles(destination + cmd->viewangles);

    int hits = 0;
    const int hitsNeed = static_cast<int>(static_cast<float>(maxSeed) * (static_cast<float>(hitChance) / 100.f));

    const auto weapSpread = weaponData->getSpread();
    const auto weapInaccuracy = weaponData->getInaccuracy();
    const auto localEyePosition = localPlayer->getEyePosition();
    const auto range = weaponData->getWeaponData()->range;

    for (int i = 0; i < maxSeed; ++i)
    {
        setRandomSeed(i + 1);
        float inaccuracy = getRandom(0.f, 1.f);
        float spread = getRandom(0.f, 1.f);
        const float spreadX = getRandom(0.f, 2.f * static_cast<float>(M_PI));
        const float spreadY = getRandom(0.f, 2.f * static_cast<float>(M_PI));

        const auto weaponIndex = weaponData->itemDefinitionIndex2();
        const auto recoilIndex = weaponData->recoilIndex();
        if (weaponIndex == WeaponId::Revolver)
        {
            if (cmd->buttons & UserCmd::IN_ATTACK2)
            {
                inaccuracy = 1.f - inaccuracy * inaccuracy;
                spread = 1.f - spread * spread;
            }
        }
        else if (weaponIndex == WeaponId::Negev && recoilIndex < 3.f)
        {
            for (int i = 3; i > recoilIndex; --i)
            {
                inaccuracy *= inaccuracy;
                spread *= spread;
            }

            inaccuracy = 1.f - inaccuracy;
            spread = 1.f - spread;
        }

        inaccuracy *= weapInaccuracy;
        spread *= weapSpread;

        Vector spreadView{ (cosf(spreadX) * inaccuracy) + (cosf(spreadY) * spread),
                           (sinf(spreadX) * inaccuracy) + (sinf(spreadY) * spread) };
        Vector direction{ (angles.forward + (angles.right * spreadView.x) + (angles.up * spreadView.y)) * range };

        static Trace trace;
        interfaces->engineTrace->clipRayToEntity({ localEyePosition, localEyePosition + direction }, 0x4600400B, entity, trace);
        if (trace.entity == entity)
            ++hits;



        if (hits >= hitsNeed) {
            if (config->debug.aimbotcoutdebug) {
                item.PrintToScreen = false;
                item.text.push_back(L"Hitchance Completed With " + std::to_wstring((hits / hitsNeed) * 100.f) + L" Likelihood of hitting");
                Debug::LOG_OUT.push_front(item);
            }
            return (hits / hitsNeed) * 100.f;
        }

        if ((maxSeed - i + hits) < hitsNeed) {
            if (config->debug.aimbotcoutdebug) {
                item.PrintToScreen = false;
                item.text.push_back(L"Hitchance Completed With " + std::to_wstring((hits / hitsNeed) * 100.f) + L" Likelihood of hitting");
                Debug::LOG_OUT.push_front(item);
            }
            return -1.0f;
        }
    }
    if (config->debug.aimbotcoutdebug) {
        item.PrintToScreen = false;
        item.text.push_back(L"Hitchance Completed With " + std::to_wstring((hits / hitsNeed) * 100.f) + L" Likelihood of hitting");
        Debug::LOG_OUT.push_front(item);
    }
    return -1.0f;
}


#include "../SDK/SDKAddition/Utils/VectorMath.h"
#include "../SDK/SDKAddition/EntityListCull.hpp"
void Aimbot::Run(UserCmd* cmd, bool& sendPacket)
{
    //if (!localPlayer || localPlayer->nextAttack() > memory->globalVars->serverTime()) {
    //    return;
    //}

    const auto activeWeapon = localPlayer->getActiveWeapon();
    if (!activeWeapon || !activeWeapon->clip())
        return;

    auto weaponIndex = getWeaponIndex(activeWeapon->itemDefinitionIndex2());
    if (!weaponIndex)
        return;

    auto weaponClass = getWeaponClass(activeWeapon->itemDefinitionIndex2());
    if (!config->aimbot[weaponIndex].enabled && !GetAsyncKeyState(config->aimbot[weaponIndex].key))
        weaponIndex = weaponClass;

    if (!config->aimbot[weaponIndex].enabled && !GetAsyncKeyState(config->aimbot[weaponIndex].key))
        weaponIndex = 0;

    if (!config->aimbot[weaponIndex].betweenShots && activeWeapon->nextPrimaryAttack() > memory->globalVars->serverTime())
        return;

    if (!config->aimbot[weaponIndex].betweenShots && activeWeapon->nextPrimaryAttack() > memory->globalVars->serverTime())
        return;

    if (!config->aimbot[weaponIndex].ignoreFlash && localPlayer->isFlashed())
        return;


    if (config->aimbot[weaponIndex].enabled && (cmd->buttons & UserCmd::IN_ATTACK || config->aimbot[weaponIndex].autoShot || config->aimbot[weaponIndex].aimlock)) {

        if (config->aimbot[weaponIndex].scopedOnly && activeWeapon->isSniperRifle() && !localPlayer->isScoped())
            return;

        auto bestFov = config->aimbot[weaponIndex].fov;
        Vector bestTarget{ };
        auto localPlayerEyePosition = localPlayer->getEyePosition();

        const auto aimPunch = activeWeapon->requiresRecoilControl() ? localPlayer->getAimPunch() : Vector{ };


        Entity* ent_save = localPlayer.get();
        for (EntityQuick EntQuick : entitylistculled->getEnemies()) {
            Entity* entity = EntQuick.entity;

            if (entity->gunGameImmunity())
                continue;

            auto boneList = config->aimbot[weaponIndex].bone == 1 ? std::initializer_list{ 8, 4, 3, 7, 6, 5 } : std::initializer_list{ 8, 7, 6, 5, 4, 3 };

            if (!config->aimbot[weaponIndex].multipointenabled) {

                float damageret = 0;
                float maxDamage = 0;
                for (auto bone : boneList) {
                    auto bonePosition = entity->getBonePosition(config->aimbot[weaponIndex].bone > 1 ? 10 - config->aimbot[weaponIndex].bone : bone);




                    //  && (config->aimbot[weaponIndex].visibleOnly
                    if (((!entity->isVisible(bonePosition) && !config->aimbot[weaponIndex].autowall)) || !canScan(entity, bonePosition, activeWeapon->getWeaponData(), config->aimbot[weaponIndex].killshot ? entity->health() : config->aimbot[weaponIndex].minDamage, config->aimbot[weaponIndex].friendlyFire, damageret))
                        continue;

                    auto angle = VectorMath::calculateRelativeAngle(localPlayerEyePosition, bonePosition, cmd->viewangles + aimPunch);



                    auto fov = std::hypotf(angle.x, angle.y);
                    if (fov < bestFov) {
                        bestFov = fov;
                        bestTarget = bonePosition;

                    }
                    if (config->aimbot[weaponIndex].bone) {
                        ent_save = entity;
                        break;
                    }
                }
            }



            if (bestTarget.notNull() && (config->aimbot[weaponIndex].ignoreSmoke
                || !memory->lineGoesThroughSmoke(localPlayer->getEyePosition(), bestTarget, 1))) {



                static Vector lastAngles{ cmd->viewangles };
                static int lastCommand{ };

                if (lastCommand == cmd->commandNumber - 1 && lastAngles.notNull() && config->aimbot[weaponIndex].silent)
                    cmd->viewangles = lastAngles;

                auto angle = VectorMath::calculateRelativeAngle(localPlayer->getEyePosition(), bestTarget, cmd->viewangles + aimPunch);
                bool clamped{ false };

                if (fabs(angle.x) > config->misc.maxAngleDelta || fabs(angle.y) > config->misc.maxAngleDelta) {
                    angle.x = std::clamp(angle.x, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);
                    angle.y = std::clamp(angle.y, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);
                    clamped = true;
                }


                if (config->aimbot[weaponIndex].hitChance > hitChance(localPlayer.get(), ent_save, activeWeapon, angle, cmd, config->aimbot[weaponIndex].hitChance)) {
                    if (config->aimbot[weaponIndex].shootonhconly) {
                        cmd->buttons &= ~UserCmd::IN_ATTACK;
                    }


                    return;
                }

                angle /= config->aimbot[weaponIndex].smooth; // If its a bot, we FUCK EM UP!
                cmd->viewangles += angle;
                if (!config->aimbot[weaponIndex].silent)
                    interfaces->engine->setViewAngles(cmd->viewangles);

                if (activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime() && activeWeapon->isSniperRifle() && !localPlayer->isScoped())
                    cmd->buttons |= UserCmd::IN_ATTACK2;


                if (activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime() && !clamped && activeWeapon->getInaccuracy() <= config->aimbot[weaponIndex].maxShotInaccuracy) {
                    cmd->buttons |= UserCmd::IN_ATTACK;
                }
                if (clamped)
                    cmd->buttons &= ~UserCmd::IN_ATTACK;

                if (clamped || config->aimbot[weaponIndex].smooth > 1.0f) lastAngles = cmd->viewangles;
                else lastAngles = Vector{ };

                lastCommand = cmd->commandNumber;
            }
        }
    }
}