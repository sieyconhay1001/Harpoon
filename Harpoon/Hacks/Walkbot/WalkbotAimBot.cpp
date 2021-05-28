#include "WalkbotAimBot.h"

#include "../Aimbot.h"
#include "../../SDK/SDKAddition/Utils/VectorMath.h"
#include "../../Memory.h"
#include "../../SDK/OsirisSDK/GlobalVars.h"
#include "../../SDK/OsirisSDK/PhysicsSurfaceProps.h"
#include "../../SDK/OsirisSDK/WeaponData.h"
#include "../../SDK/OsirisSDK/Angle.h"
#include "../../SDK/OsirisSDK/Math.h"
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

bool canScan(Entity* entity, const Vector& destination, const WeaponInfo* weaponData, int minDamage, bool allowFriendlyFire, float& damageret) noexcept
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

#include "../Other/Debug.h"



float hitChance(Entity* localPlayer, Entity* entity, Entity* weaponData, const Vector& destination, const UserCmd* cmd, const int hitChance) noexcept
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

            return (hits / hitsNeed) * 100.f;
        }

        if ((maxSeed - i + hits) < hitsNeed) {

            return (hits / hitsNeed) * 100.f;
        }
    }

    return -1.0f;
}

/* I need to calc best stop ang*/

static void Autostop(UserCmd* cmd) noexcept
{
    Vector Velocity = localPlayer->velocity();

    if (Velocity.length2D() == 0)
        return;

    static float Speed = 450.f;

    Vector Direction;
    Vector RealView;
    Math::VectorAngles(Velocity, Direction);
    interfaces->engine->getViewAngles(RealView);
    Direction.y = RealView.y - Direction.y;

    Vector Forward;
    Math::AngleVectors(Direction, &Forward);
    Vector NegativeDirection = Forward * -Speed;

    cmd->forwardmove = NegativeDirection.x;
    cmd->sidemove = NegativeDirection.y;
}


bool swapped = false;
static Entity* MainWeapon;
namespace WalkbotAimbot {


    void Run(Walkbot::WalkbotData& Bot, UserCmd* cmd) {


        if (!localPlayer || !localPlayer.get() || localPlayer->isDormant() || !localPlayer->isAlive()) {
            swapped = false;
            return;
        }

        Bot.ShouldReturn = false;

        if (!Bot.TargetedEntity || (Bot.TargetedEntity == nullptr) || (Bot.TargetedEntity == localPlayer.get())) {
            Walkbot::WBot.TargetedEntity = localPlayer.get();
            return;
        }

        Entity* entity = Bot.TargetedEntity;

        if (!entity || (entity == localPlayer.get()) || entity->isDormant() || !entity->isAlive() ||  !entity->isOtherEnemy(localPlayer.get())) {
            Bot.TargetedEntity = localPlayer.get();
            return;
        }

        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (!activeWeapon)
            return;

        auto weaponIndex = getWeaponIndex(activeWeapon->itemDefinitionIndex2());
        if (!weaponIndex)
            return;



        if (activeWeapon->isKnife()) {
                for (auto i = 0; i < 64; ++i)
                {
                    auto Weapon = interfaces->entityList->getEntityFromHandle(localPlayer->weapons()[i]);
                    if (Weapon && (Weapon->isPrimary() || Weapon->isPistol()) && Weapon->clip() && !(Weapon->itemDefinitionIndex2() == WeaponId::Revolver))
                    {
                        cmd->weaponselect = Weapon->index();
                        cmd->weaponsubtype = Weapon->getWeaponSubType();
                        swapped = true;
                        Bot.Status = Walkbot::WalkbotStatus::InEngagement;
                        Bot.ShouldReturn = true;
                        return;
                    }
                }
        }


        if (entity->isVisible()) {
            if (!activeWeapon->isPistol() && !activeWeapon->clip()) {
                for (auto i = 0; i < 64; ++i)
                {
                    auto Weapon = interfaces->entityList->getEntityFromHandle(localPlayer->weapons()[i]);
                    if (Weapon && Weapon->isPistol() && Weapon->clip() && !(Weapon->itemDefinitionIndex2() == WeaponId::Revolver))
                    {
                        cmd->weaponselect = Weapon->index();
                        cmd->weaponsubtype = Weapon->getWeaponSubType();
                        swapped = true;
                        return;
                    }
                }             
            }
        }

        if (!activeWeapon->clip())
            return;

        auto bestFov = Bot.Settings.AimbotParams.FOV;
        Vector bestTarget{ };
        auto localPlayerEyePosition = localPlayer->getEyePosition();

        const auto aimPunch = activeWeapon->requiresRecoilControl() ? localPlayer->getAimPunch() : Vector{ };





        if (entity->gunGameImmunity())
            return;

        auto boneList = Bot.Settings.AimbotParams.ignoreHead ? std::initializer_list{ 4, 3, 7, 6, 5 } : std::initializer_list{ 8, 7, 6, 5, 4, 3 };
            float damageret = 0;
            float maxDamage = 0;
            for (auto bone : boneList) {
                auto bonePosition = entity->getBonePosition(bone);
                if (!entity->isVisible(bonePosition))//&& (config->aimbot[weaponIndex].visibleOnly || !canScan(entity, bonePosition, activeWeapon->getWeaponData(), config->aimbot[weaponIndex].killshot ? entity->health() : config->aimbot[weaponIndex].minDamage, config->aimbot[weaponIndex].friendlyFire, damageret)))
                    continue;
                auto angle = VectorMath::calculateRelativeAngle(localPlayerEyePosition, bonePosition, cmd->viewangles + aimPunch);
                auto fov = std::hypotf(angle.x, angle.y);
                if (fov < bestFov) {
                    bestFov = fov;
                    bestTarget = bonePosition;

                }
            }
        



        if (bestTarget.notNull() && !memory->lineGoesThroughSmoke(localPlayer->getEyePosition(), bestTarget, 1)){
            Autostop(cmd);
            Bot.ShouldReturn = true;
            auto angle = VectorMath::calculateRelativeAngle(localPlayer->getEyePosition(), bestTarget, cmd->viewangles + aimPunch);
            bool clamped{ false };
            if (fabs(angle.x) > 28.f || fabs(angle.y) > 28.f) {
                angle.x = std::clamp(angle.x, -28.f, 28.f);
                angle.y = std::clamp(angle.y, -28.f, 28.f);
                clamped = true;
            }


            if ((fabs(angle.x) > 2.f) || (fabs(angle.y) > 2.f)) {
                if (Bot.Settings.AimbotParams.VariableSmoothing) {
                    float SmoothFrac = 1.f - ((((fabs(angle.x) + .5f) / 28.f) + ((fabs(angle.y) + .5f) / 28.f)) * .5f);
                    //SmoothFrac = 1.f - log((((fabs(angle.x) + .5f) / 28.f) + ((fabs(angle.y) + .5f) / 28.f)) * .5f)+1;
                    angle /= (Bot.Settings.AimbotParams.AimSmoothing * SmoothFrac);
                }
                else {
                    angle /= Bot.Settings.AimbotParams.AimSmoothing;
                }
            }



            cmd->viewangles += angle;
            interfaces->engine->setViewAngles(cmd->viewangles);

            if ((activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime()) && (activeWeapon->isSniperRifle() && !localPlayer->isScoped())) {
                cmd->buttons |= UserCmd::IN_ATTACK2;
            }

            if (Bot.Settings.AimbotParams.hitchance > hitChance(localPlayer.get(), entity, activeWeapon, angle, cmd, Bot.Settings.AimbotParams.hitchance)) {
                return;
            }

            if ((activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime()) && !clamped) {
                cmd->buttons |= UserCmd::IN_ATTACK;
            }

            return;
        }
        else {
            if (activeWeapon->nextPrimaryAttack() < memory->globalVars->serverTime() && activeWeapon->isSniperRifle() && localPlayer->isScoped())
                cmd->buttons |= UserCmd::IN_ATTACK2;
        }




    }

}

