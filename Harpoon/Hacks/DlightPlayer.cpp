#include "../SDK/SDK/IEffects.h"
#include "../SDK/SDKAddition/EntityListCull.hpp"

#include "../SDK/OsirisSDK/Entity.h"
#include "../Memory.h"
#include "../Interfaces.h"
#include "../SDK/OsirisSDK/GlobalVars.h"
#include "DlightPlayer.h"

std::vector<Player_Dlight::PDlight> Player_Dlight::playerLights(65);

void Player_Dlight::SetUpPlayerLight(int i) {

    if (!config->visuals.dlight.enabled && !config->visuals.lpdlight.enabled)
        return;

    Entity* entity = interfaces->entityList->getEntity(i);
    if (!entity || entity->isDormant() || !entity->isAlive()
        || (!entity->isOtherEnemy(localPlayer.get()) && (!((entity == localPlayer.get()) && config->visuals.lpdlight.enabled))))
        return;

    dlight_t* dLight = interfaces->iveffects->CL_AllocDlight(entity->index());
    dlight_t* eLight = interfaces->iveffects->CL_AllocElight(entity->index());

    dLight->die = memory->globalVars->currenttime + 9999999.f;
    dLight->radius = dLight->color.exponent = config->visuals.dlightRadius;
    if (true) {
        dLight->color.r = (std::byte)(config->visuals.dlight.color[0] *255);
        dLight->color.g = (std::byte)(config->visuals.dlight.color[1] * 255);
        dLight->color.b = (std::byte)(config->visuals.dlight.color[2] * 255);
    }
    else {
        dLight->color.r = (std::byte)2;
        dLight->color.g = (std::byte)48;
        dLight->color.b = (std::byte)22;
    }
    dLight->color.exponent = config->visuals.dlightExponent;
    dLight->flags = 0;
    dLight->key = entity->index();
    dLight->decay = 20.0f;
    dLight->m_Direction = entity->getBonePosition(0);
    dLight->origin = entity->getBonePosition(0);
    *eLight = *dLight;
    playerLights.at(entity->index()).hasBeenCreated = true;
    playerLights.at(entity->index()).dlight = dLight;
    playerLights.at(entity->index()).elight = eLight;
    playerLights.at(entity->index()).dlight_settings = *dLight;
}


void Player_Dlight::SetupLights() {
    for (EntityQuick EntQuick : entitylistculled->getEntities()) {

        if (EntQuick.m_bisLocalPlayer)
        {
            if (!config->visuals.lpdlight.enabled)
                return;
        }
        else if (!config->visuals.dlight.enabled)
        {
            return;
        }

        int i = EntQuick.index;
        if (EntQuick.m_bisDormant || !EntQuick.m_bisAlive || (!EntQuick.m_bisEnemy && !EntQuick.m_bisLocalPlayer)) {
            if ((i >= 0) && (i < 65)) {
                if (playerLights.at(i).hasBeenCreated) {
                    playerLights.at(i).dlight->die = memory->globalVars->currenttime;
                    playerLights.at(i).elight->die = memory->globalVars->currenttime;
                }
                playerLights.at(i).hasBeenCreated = false;
            }
            continue;
        }

        Config::ColorToggle dlight = config->visuals.dlight;
        int dlightRadius = config->visuals.dlightRadius;
        int dlightExponent = config->visuals.dlightExponent;
        if (EntQuick.m_bisLocalPlayer)
        {
            if (!config->visuals.lpdlight.enabled)
                return;

            dlight = config->visuals.lpdlight;
            dlightRadius = config->visuals.lpdlightRadius;
            dlightExponent = config->visuals.lpdlightExponent;
        }


        Entity* entity = EntQuick.entity;
        if (!playerLights.at(i).hasBeenCreated){
            SetUpPlayerLight(i);
        }

        {
            playerLights.at(i).dlight_settings.origin = entity->getBonePosition(0);
            playerLights.at(i).dlight_settings.die = memory->globalVars->currenttime + 9999999.f;
            playerLights.at(i).dlight_settings.m_Direction = entity->getBonePosition(0);
            playerLights.at(i).dlight_settings.color.r = (std::byte)(dlight.color[0] * 255);
            playerLights.at(i).dlight_settings.color.g = (std::byte)(dlight.color[1] * 255);
            playerLights.at(i).dlight_settings.color.b = (std::byte)(dlight.color[2] * 255);
            playerLights.at(i).dlight_settings.radius = dlightRadius;
            playerLights.at(i).dlight_settings.color.exponent = dlightExponent;
            *(playerLights.at(i).dlight) = playerLights.at(i).dlight_settings;
            *(playerLights.at(i).elight) = playerLights.at(i).dlight_settings;

        }


    }
}
