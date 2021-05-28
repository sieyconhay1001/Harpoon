#pragma once

#include "../GameData.h"

#include "AimAssist.h"
#include "OTHER/Debug.h"


#include "../SDK/OsirisSDK/UserCmd.h"
#include "../SDK/OsirisSDK/Vector.h"
#include "../SDK/OsirisSDK/Surface.h"
#include "../SDK/OsirisSDK/ConVar.h"
#include "../SDK/OsirisSDK/LocalPlayer.h"
#include "../SDK/OsirisSDK/Engine.h"
#include "../SDK/OsirisSDK/EngineTrace.h"
#include "../SDK/OsirisSDK/Entity.h"

#include "../SDK/SDKAddition/Utils/ScreenMath.h"
#include "../SDK/SDKAddition/EntityListCull.hpp"



class MouseMonitor {
public:
    void UpdateChange(float dist, Entity* entity) {
        if (entity != TargetedEnt) {
            Set = false;
            HasHitEnt = false;
            TargetedEnt = entity;
        } 

        if (Set){
            ChangeFromTarget = PreviousDistance - dist;
            Set = true;
        }
        PreviousDistance = dist;
        if (!Set) {
            Set = true;
        }
    }
    void HasExcededFOV() {
        Set = false;
        HasHitEnt = false;
        TargetedEnt = nullptr;
    }


    void UpdateHit(bool hit) {
        HasHitEnt = hit;
    }
    
    bool HasHit() {
        return HasHitEnt; // Currently Broke?
    }

    bool IsAwayFromTarget() {
        if (ChangeFromTarget > 0)
            return false;
        return true;

    }

    bool IsTowardsTarget() {
        if (ChangeFromTarget > 0)
            return true;
        return false;
    }

private:
    float PreviousDistance = 99999.f;
    float ChangeFromTarget;
    bool Set = false;
    bool HasHitEnt = false;
    Entity* TargetedEnt = nullptr;

} MouseMon;



namespace AimAssist {

    void Run(UserCmd* cmd) {

        if (!config->aimassist_single.enabled)
            return;

        if (localPlayer->nextAttack() > memory->globalVars->serverTime())
            return;

        static bool set = false;
        static float Sensitivity = 0.0f;
        static ConVar* sensitityVar{ interfaces->cvar->findVar("sensitivity") };

        if (!sensitityVar)
            return;

        if (Sensitivity > 0.f) { // Damn RIP To those negative Sens Users
            sensitityVar->setValue(Sensitivity);
        }

        Entity* entity = entitylistculled->getEntNearestToCrosshair();

        if (!entity)
            return;

        if (!entity->isVisible()) {
            MouseMon.HasExcededFOV();
                return;
        }

        float dist = ScreenMath::worldToScreenDistanceFromCenter(entity->getEyePosition());

        if (dist > config->aimassist_single.FOV) {
            MouseMon.HasExcededFOV();
            return;
        } else if (dist < 0) { // OffScreen
            return;
        }

        
        MouseMon.UpdateChange(dist, entity);

        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (!activeWeapon || !activeWeapon->clip() || activeWeapon->nextPrimaryAttack() > memory->globalVars->serverTime())
            return;

        auto weaponIndex = getWeaponIndex(activeWeapon->itemDefinitionIndex2());
        if (!weaponIndex)
            return;

        const auto weaponData = activeWeapon->getWeaponData();
        if (!weaponData)
            return;

        const auto aimPunch = localPlayer->getAimPunch();

        const Vector viewAngles{ std::cos(degreesToRadians(cmd->viewangles.x)) * std::cos(degreesToRadians(cmd->viewangles.y)) * weaponData->range,
                                 std::cos(degreesToRadians(cmd->viewangles.x)) * std::sin(degreesToRadians(cmd->viewangles.y)) * weaponData->range,
                                -std::sin(degreesToRadians(cmd->viewangles.x)) * weaponData->range };

        Trace trace;
        interfaces->engineTrace->clipRayToEntity({ localPlayer->getEyePosition(), localPlayer->getEyePosition() + viewAngles }, 0x4600400B, entity, trace);


        if (!set) {
            Sensitivity = sensitityVar->getFloat();
            set = true;
        }

        sensitityVar->onChangeCallbacks.size = 0;
        if (!trace.entity || !trace.entity->isPlayer() || (trace.entity != entity)) {

            if (MouseMon.HasHit() || !config->aimassist_single.onlyAfterHit) {
                if (MouseMon.IsAwayFromTarget()) {          
                    sensitityVar->setValue(Sensitivity - ((1.f - (dist / config->aimassist_single.FOV)) * (Sensitivity * (config->aimassist_single.AwayPenalty / 100.f)))); // 
                }
                else if (MouseMon.IsTowardsTarget()){
                    sensitityVar->setValue(Sensitivity + ((dist / config->aimassist_single.FOV) * (Sensitivity * (config->aimassist_single.maxSpeed / 100.f)))); //  
                }
            }
        }
        else {
            MouseMon.UpdateHit(true);
            sensitityVar->setValue(Sensitivity - (Sensitivity * (config->aimassist_single.Penalty/100.f)));
        }

    }
}