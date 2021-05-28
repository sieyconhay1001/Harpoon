#include "../fnv.h"
#include "Visuals.h"

#include "../SDK/OsirisSDK/ConVar.h"
#include "../SDK/OsirisSDK/Entity.h"
#include "../SDK/OsirisSDK/FrameStage.h"
#include "../SDK/OsirisSDK/GameEvent.h"
#include "../SDK/OsirisSDK/GlobalVars.h"
#include "../SDK/OsirisSDK/Input.h"
#include "../SDK/OsirisSDK/Material.h"
#include "../SDK/OsirisSDK/MaterialSystem.h"
#include "../SDK/OsirisSDK/NetworkStringTable.h"
#include "../SDK/OsirisSDK/RenderContext.h"
#include "../SDK/OsirisSDK/Surface.h"
#include "../SDK/OsirisSDK/ModelInfo.h"
#include "../SDK/OsirisSDK/Beams.h"


#include <array>
#include <xstring>

void Visuals::playerModel(FrameStage stage) noexcept
{
    if (stage != FrameStage::RENDER_START && stage != FrameStage::RENDER_END)
        return;

    static int originalIdx = 0;

    if (!localPlayer) {
        originalIdx = 0;
        return;
    }

    constexpr auto getModel = [](int team) constexpr noexcept -> const char* {
        constexpr std::array models{
        "models/player/custom_player/legacy/ctm_fbi_variantb.mdl",
        "models/player/custom_player/legacy/ctm_fbi_variantf.mdl",
        "models/player/custom_player/legacy/ctm_fbi_variantg.mdl",
        "models/player/custom_player/legacy/ctm_fbi_varianth.mdl",
        "models/player/custom_player/legacy/ctm_sas_variantf.mdl",
        "models/player/custom_player/legacy/ctm_st6_variante.mdl",
        "models/player/custom_player/legacy/ctm_st6_variantg.mdl",
        "models/player/custom_player/legacy/ctm_st6_varianti.mdl",
        "models/player/custom_player/legacy/ctm_st6_variantk.mdl",
        "models/player/custom_player/legacy/ctm_st6_variantm.mdl",
        "models/player/custom_player/legacy/tm_balkan_variantf.mdl",
        "models/player/custom_player/legacy/tm_balkan_variantg.mdl",
        "models/player/custom_player/legacy/tm_balkan_varianth.mdl",
        "models/player/custom_player/legacy/tm_balkan_varianti.mdl",
        "models/player/custom_player/legacy/tm_balkan_variantj.mdl",
        "models/player/custom_player/legacy/tm_leet_variantf.mdl",
        "models/player/custom_player/legacy/tm_leet_variantg.mdl",
        "models/player/custom_player/legacy/tm_leet_varianth.mdl",
        "models/player/custom_player/legacy/tm_leet_varianti.mdl",
        "models/player/custom_player/legacy/tm_phoenix_variantf.mdl",
        "models/player/custom_player/legacy/tm_phoenix_variantg.mdl",
        "models/player/custom_player/legacy/tm_phoenix_varianth.mdl",
        
        "models/player/custom_player/legacy/tm_pirate.mdl",
        "models/player/custom_player/legacy/tm_pirate_varianta.mdl",
        "models/player/custom_player/legacy/tm_pirate_variantb.mdl",
        "models/player/custom_player/legacy/tm_pirate_variantc.mdl",
        "models/player/custom_player/legacy/tm_pirate_variantd.mdl",
        "models/player/custom_player/legacy/tm_anarchist.mdl",
        "models/player/custom_player/legacy/tm_anarchist_varianta.mdl",
        "models/player/custom_player/legacy/tm_anarchist_variantb.mdl",
        "models/player/custom_player/legacy/tm_anarchist_variantc.mdl",
        "models/player/custom_player/legacy/tm_anarchist_variantd.mdl",
        "models/player/custom_player/legacy/tm_balkan_varianta.mdl",
        "models/player/custom_player/legacy/tm_balkan_variantb.mdl",
        "models/player/custom_player/legacy/tm_balkan_variantc.mdl",
        "models/player/custom_player/legacy/tm_balkan_variantd.mdl",
        "models/player/custom_player/legacy/tm_balkan_variante.mdl",
        "models/player/custom_player/legacy/tm_jumpsuit_varianta.mdl",
        "models/player/custom_player/legacy/tm_jumpsuit_variantb.mdl",
        "models/player/custom_player/legacy/tm_jumpsuit_variantc.mdl"
        };

        switch (team) {
        case 2: return static_cast<std::size_t>(config->visuals.playerModelT - 1) < models.size() ? models[config->visuals.playerModelT - 1] : nullptr;
        case 3: return static_cast<std::size_t>(config->visuals.playerModelCT - 1) < models.size() ? models[config->visuals.playerModelCT - 1] : nullptr;
        default: return nullptr;
        }
    };

    if (const auto model = getModel(localPlayer->team())) {
        if (stage == FrameStage::RENDER_START) {
            originalIdx = localPlayer->modelIndex();
            if (const auto modelprecache = interfaces->networkStringTableContainer->findTable("modelprecache")) {
                modelprecache->addString(false, model);
                const auto viewmodelArmConfig = memory->getPlayerViewmodelArmConfigForPlayerModel(model);
                modelprecache->addString(false, viewmodelArmConfig[2]);
                modelprecache->addString(false, viewmodelArmConfig[3]);
            }
        }

        const auto idx = stage == FrameStage::RENDER_END && originalIdx ? originalIdx : interfaces->modelInfo->getModelIndex(model);

        localPlayer->setModelIndex(idx);

        if (const auto ragdoll = interfaces->entityList->getEntityFromHandle(localPlayer->ragdoll()))
            ragdoll->setModelIndex(idx);
    }
}

#include <sstream>
#include <codecvt>
#include <locale>
#include <cstddef>
#include <string_view>

//#include "Chams.h"
#include "../SDK/OsirisSDK/Material.h"
#include "../SDK/OsirisSDK/MaterialSystem.h"
#include "../SDK/OsirisSDK/StudioRender.h"
#include "../SDK/OsirisSDK/KeyValues.h"



/*
// #STR: "fog_color", "-1 -1 -1"
int Fog_Color__()
{
  SetConvar("fog_color", "-1 -1 -1", 0x4000, 0, 0, 0, 0, 0, 0); // or something to that affect
  return atexit(sub_A92AB0);
}
*/


/*
        struct FogController {
            struct FogSettings {
                Config::ColorToggle Color;
                int iStartDistance{ -1 };
                int iEndDistance{ -1 };
                float flHdrColorScale{ 1.f };
            } Fog, Sky;
        } FogControl;
*/

// "As of July 2020 none of the standard library implementations provide std::format" But why?
inline void colorToStr(std::array<float,3> color, char* buff) {
    sprintf(buff, "%d %d %d",
        (static_cast<int>(color[0] * 255.f)),
        (static_cast<int>(color[1] * 255.f)),
        (static_cast<int>(color[2] * 255.f))
    );
}

static struct FogOptions {
    ConVar* enable;
    ConVar* color;
    ConVar* maxDensity;
    ConVar* hdr;
    ConVar* start;
    ConVar* end;
};

static void SetFogOptions(Config::Visuals::FogController::FogSettings FogSettings, FogOptions& Fog) {
    if (!FogSettings.Color.enabled)
        return;

    /* Probably Should Run this only once, but who cares*/
    Fog.enable->onChangeCallbacks.size = 0; 
    Fog.color->onChangeCallbacks.size = 0;
    Fog.maxDensity->onChangeCallbacks.size = 0;
    Fog.hdr->onChangeCallbacks.size = 0;
    Fog.start->onChangeCallbacks.size = 0;
    Fog.end->onChangeCallbacks.size = 0;

    char buff[13];

    Fog.enable->setValue(1);
    colorToStr(FogSettings.Color.color, buff);
    Fog.color->setValue(buff);
    Fog.maxDensity->setValue(FogSettings.Color.color[3]);
    Fog.hdr->setValue(FogSettings.flHdrColorScale);
    Fog.start->setValue(FogSettings.iStartDistance);
    Fog.end->setValue(FogSettings.iEndDistance);

}

void Visuals::FogControl() noexcept {

    if (!config->visuals.FogControl.Fog.Color.enabled && !config->visuals.FogControl.Sky.Color.enabled)
        return;

    /* Our ConVars */


    static FogOptions Fog, Sky;
    static bool init = false;

    if (!interfaces->engine->isInGame() || !localPlayer.get() || !localPlayer->isAlive())
    {
        init = false;
        return;
    }

    if (!init) { /* Init ConVars, but only once*/
        Fog.enable = interfaces->cvar->findVar("fog_enable");
        Fog.color = interfaces->cvar->findVar("fog_color");
        Fog.maxDensity = interfaces->cvar->findVar("fog_maxdensity");
        Fog.hdr = interfaces->cvar->findVar("fog_hdrcolorscale");
        Fog.start = interfaces->cvar->findVar("fog_start");
        Fog.end = interfaces->cvar->findVar("fog_end");

        Sky.enable = interfaces->cvar->findVar("fog_enableskybox");
        Sky.color = interfaces->cvar->findVar("fog_colorskybox");
        Sky.maxDensity = interfaces->cvar->findVar("fog_maxdensityskybox");
        Sky.hdr = interfaces->cvar->findVar("fog_hdrcolorscaleskybox");
        Sky.start = interfaces->cvar->findVar("fog_startskybox");
        Sky.end = interfaces->cvar->findVar("fog_endskybox");
        init = true;
        ConVar* fog_override{ interfaces->cvar->findVar("fog_override") };
        fog_override->onChangeCallbacks.size = 0;
        fog_override->setValue(1);
    }

    SetFogOptions(config->visuals.FogControl.Fog, Fog);
    SetFogOptions(config->visuals.FogControl.Sky, Sky);
}

#include "Chams.h"
#include "../SDK/OsirisSDK/NetworkChannel.h"
#include "../Memory.h"
void Visuals::colorWorld() noexcept
{

    if (memory->clientState->signonState < SIGNONSTATE_FULL)
        return;


    if (!config->visuals.world.enabled && !config->visuals.sky.enabled && !config->visuals.props.enabled && !config->visuals.smokecolor.enabled && !config->visuals.smoketrans
        && !config->visuals.PrecacheWorld.enabled && !config->visuals.PrecacheProps.enabled && !config->visuals.NonCached.enabled && !config->visuals.NonCachedWorld.enabled && !config->visuals.all.enabled) {
        return;
    }

    if (!localPlayer || localPlayer->isDormant())
        return;

    if (!interfaces->engine->isInGame())
        return;

    for (short h = interfaces->materialSystem->firstMaterial(); h != interfaces->materialSystem->invalidMaterial(); h = interfaces->materialSystem->nextMaterial(h)) {
        const auto mat = interfaces->materialSystem->getMaterial(h);

        if (!mat)
            continue;


        const std::string_view textureGroup = mat->getTextureGroupName();



        if ((config->visuals.PrecacheWorld.enabled && (textureGroup.starts_with("World") && mat->isPrecached()))) {
                mat->colorModulate(config->visuals.PrecacheWorld.color);
                mat->alphaModulate(config->visuals.PrecacheWorld.color[3]);
        } else if ((config->visuals.NonCachedWorld.enabled && (textureGroup.starts_with("World") && !mat->isPrecached()))) {
                mat->colorModulate(config->visuals.NonCachedWorld.color);
                mat->alphaModulate(config->visuals.NonCachedWorld.color[3]);
        }
        else if (config->visuals.world.enabled && (textureGroup.starts_with("World") && (!textureGroup.starts_with("StaticProp")))) {
                mat->colorModulate(config->visuals.world.color);
                mat->alphaModulate(config->visuals.world.color[3]);
                mat->setMaterialVarFlag((MaterialVarFlag)MATERIAL_VAR_FLAT, 1);

        }
        else if (config->visuals.sky.enabled && textureGroup.starts_with("SkyBox")) {
                mat->colorModulate(config->visuals.sky.color);
        }
        else if (config->visuals.PrecacheProps.enabled && textureGroup.starts_with("StaticProp") && mat->isPrecached()) {
            ConVar* static_Prop = interfaces->cvar->findVar("r_DrawSpecificStaticProp");
            static_Prop->setValue(0);
                mat->colorModulate(config->visuals.PrecacheProps.color);
                mat->alphaModulate(config->visuals.PrecacheProps.color[3]);
        }
        else if (config->visuals.NonCached.enabled && textureGroup.starts_with("StaticProp") && !mat->isPrecached()) {
            ConVar* static_Prop = interfaces->cvar->findVar("r_DrawSpecificStaticProp");
            static_Prop->setValue(0);
                mat->colorModulate(config->visuals.NonCached.color);
                mat->alphaModulate(config->visuals.NonCached.color[3]);
        }
        else if (config->visuals.props.enabled && textureGroup.starts_with("StaticProp")) {
            ConVar* static_Prop = interfaces->cvar->findVar("r_DrawSpecificStaticProp");
            static_Prop->setValue(0);
                mat->colorModulate(config->visuals.props.color);
                mat->alphaModulate(config->visuals.props.color[3]);
        }
        else if (config->visuals.all.enabled) {
            mat->colorModulate(config->visuals.all.color);
            mat->alphaModulate(config->visuals.all.color[3]);
        }

    }
    if (config->visuals.smokecolor.enabled || config->visuals.smoketrans) {
        constexpr std::array smokeMaterials{
            "particle/vistasmokev1/vistasmokev1_emods",
            "particle/vistasmokev1/vistasmokev1_emods_impactdust",
            "particle/vistasmokev1/vistasmokev1_fire",
            "particle/vistasmokev1/vistasmokev1_smokegrenade",
            "particle/vistasmokev1/vistasmokev1",
            "particle/smokegrenade",
            "particle/smokegrenade1",
            "particle/smokegrenade2",
            "particle/smokegrenade3",
            "particle/smokegrenade_2",
            "particle/smokegrenade_sc",
            "particle/smokegrenade1",
            "particle/vistasmokev1_add_nearcull",
            "particle/smokesprites001",
            "effects/smoke/smoke_cloud",
            "effects/smoke/smoke_cloud_detail"
        };

        for (const auto mat : smokeMaterials) {
            const auto material = interfaces->materialSystem->findMaterial(mat);
            if (config->visuals.smokecolor.enabled) {
                material->colorModulate(config->visuals.smokecolor.color);
            }
            if (config->visuals.smoketrans) {
                material->alphaModulate((config->visuals.smokealpha / 1000.0f));
            }
        }
    }

}


void Visuals::MoltovColor() noexcept {
    if (!localPlayer || localPlayer->isDormant() || !config->visuals.moltovcolor.enabled)
        return;

    if (!interfaces->engine->isInGame())
        return;

    for (short h = interfaces->materialSystem->firstMaterial(); h != interfaces->materialSystem->invalidMaterial(); h = interfaces->materialSystem->nextMaterial(h)) {
        const auto mat = interfaces->materialSystem->getMaterial(h);
        if (!mat)
            continue;


        const std::string_view textureGroup = mat->getTextureGroupName();

        if (strstr(mat->getName(),"fire") || strstr(mat->getName(), "Fire") || (textureGroup.find("Fire") != std::string_view::npos) || (textureGroup.find("fire") != std::string_view::npos)) {
            mat->colorModulate(config->visuals.moltovcolor.color);

        }
    }
}
/*
void Visuals::transparentWorld() noexcept
{
    if (!config->visuals.worldtrans && !config->visuals.proptrans && !config->visuals.preworldtrans && !config->visuals.preproptrans)
       return;

    if (!interfaces->engine->isInGame())
        return;

    if (!localPlayer || localPlayer->isDormant() || !localPlayer->isAlive())
        return;

    for (short h = interfaces->materialSystem->firstMaterial(); h != interfaces->materialSystem->invalidMaterial(); h = interfaces->materialSystem->nextMaterial(h)) {
        const auto mat = interfaces->materialSystem->getMaterial(h);

        if (!mat)
           continue;


        const std::string_view textureGroup = mat->getTextureGroupName();

        if ((textureGroup.starts_with("World") && config->visuals.preworldtrans)) {
                mat->alphaModulate((config->visuals.alphaval / 100.0f));
        }

        if ((textureGroup.starts_with("World") && config->visuals.worldtrans)) {
            mat->alphaModulate((config->visuals.prealphaval / 100.0f));
        }
        if ((textureGroup.starts_with("StaticProp") && config->visuals.proptrans)) {
            
            ConVar* static_Prop = interfaces->cvar->findVar("r_DrawSpecificStaticProp");
            static_Prop->setValue(0);    

            mat->alphaModulate((config->visuals.propalphaval / 100.0f));
        }

        if ((textureGroup.starts_with("StaticProp") && config->visuals.preproptrans)) {

            ConVar* static_Prop = interfaces->cvar->findVar("r_DrawSpecificStaticProp");
            static_Prop->setValue(0);

            mat->alphaModulate((config->visuals.prepropalphaval / 100.f));
        }
    }
}
*/
void Visuals::NightModeExtended() noexcept {
    if (!config->visuals.NightMode.enabled)
        return;
    if (!interfaces->engine->isInGame())
        return;

    if (config->visuals.matgrey) {
        ConVar* mat_grey = interfaces->cvar->findVar("mat_drawgray");
        mat_grey->setValue(1);
    }
    if (!localPlayer || localPlayer->isDormant() || !localPlayer->isAlive())
        return;
    ConVar* ambilight_r = interfaces->cvar->findVar("mat_ambient_light_r");
    ConVar* ambilight_g = interfaces->cvar->findVar("mat_ambient_light_g");
    ConVar* ambilight_b = interfaces->cvar->findVar("mat_ambient_light_b");

    ambilight_r->setValue(config->visuals.NightMode.color[0]);
    ambilight_g->setValue(config->visuals.NightMode.color[1]);
    ambilight_b->setValue(config->visuals.NightMode.color[2]);
    
}

void Visuals::modifySmoke(FrameStage stage) noexcept
{
    if ((stage != FrameStage::RENDER_START) && (stage != FrameStage::RENDER_END))
        return;

    if (!interfaces->engine->isInGame())
        return;

    if (!localPlayer || localPlayer->isDormant() || !localPlayer->isAlive())
        return;


    //if(!memory->globalVars-)

    constexpr std::array smokeMaterials{
            "particle/vistasmokev1/vistasmokev1_emods",
            "particle/vistasmokev1/vistasmokev1_emods_impactdust",
            "particle/vistasmokev1/vistasmokev1_fire",
            "particle/vistasmokev1/vistasmokev1_smokegrenade",
            "particle/vistasmokev1/vistasmokev1",
            "particle/particle_smokegrenade",
            "particle/particle_smokegrenade1",
            "particle/particle_smokegrenade2",
            "particle/particle_smokegrenade3",
            "particle/particle_smokegrenade_2",
            "particle/particle_smokegrenade_sc",
            "particle/particle_smokegrenade1",
            "particle/vistasmokev1_add_nearcull",
            "particle/smokesprites001",
            "effects/smoke/smoke_cloud",
            "effects/smoke/smoke_cloud_detail"
        };
    

    //constexpr std::array smokeMaterials{
    //    "particle/vistasmokev1/vistasmokev1_emods",
    //    "particle/vistasmokev1/vistasmokev1_emods_impactdust",
    //    "particle/vistasmokev1/vistasmokev1_fire",
    //    "particle/vistasmokev1/vistasmokev1_smokegrenade"
    //};

    for (const auto mat : smokeMaterials) {

        ConVar* static_Prop = interfaces->cvar->findVar("r_DrawSpecificStaticProp");
        static_Prop->setValue(0);

        const auto material = interfaces->materialSystem->findMaterial(mat);
        if (config->visuals.smokecolor.enabled) {
            material->setMaterialVarFlag(MaterialVarFlag::MATERIAL_VAR_ADDITIVE, true);
            material->colorModulate(config->visuals.smokecolor.color);
        }
        if (config->visuals.smoketrans) {
            material->setMaterialVarFlag(MaterialVarFlag::MATERIAL_VAR_ADDITIVE, true);
            material->alphaModulate((config->visuals.smokealpha / 1000.0f));
        }
        material->setMaterialVarFlag(MaterialVarFlag::NO_DRAW, stage == FrameStage::RENDER_START && config->visuals.noSmoke);
        material->setMaterialVarFlag(MaterialVarFlag::WIREFRAME, stage == FrameStage::RENDER_START && config->visuals.wireframeSmoke);
        char flag  = 1 << config->visuals.customflag;
        material->setMaterialVarFlag((MaterialVarFlag)(flag), stage == FrameStage::RENDER_START && config->visuals.smokeflagmanual);
    }
}



#include "../SDK/OsirisSDK/Engine.h"
#include "../SDK/OsirisSDK/EngineTrace.h"
#include "../SDK/SDK/CViewSetup.h"


void Visuals::thirdperson(CViewSetup* setup, bool inOverride) noexcept
{

    static bool isInThirdperson{ true };
    static float lastTime{ 0.0f };

    if (GetAsyncKeyState(config->visuals.thirdpersonKey) && memory->globalVars->realtime - lastTime > 0.5f) {
        isInThirdperson = !isInThirdperson;
        lastTime = memory->globalVars->realtime;
    }	
    
    static bool once = false;
    static bool once2 = false;

    if (config->visuals.thirdperson) {
        if (memory->input->isCameraInThirdPerson = ((!config->visuals.thirdpersonKey || isInThirdperson) && localPlayer)) {
            if (localPlayer->isAlive() && !inOverride) {
                once2 = false;
                memory->input->cameraOffset.z = static_cast<float>(config->visuals.thirdpersonDistance);
                localPlayer->m_iObserverMode() = localPlayer->isAlive() ? OBS_MODE_NONE : OBS_MODE_IN_EYE;
                once = false;
            }
            else if(inOverride && !localPlayer->isAlive()){

                static bool once = false;

                if (localPlayer->m_iObserverMode() != OBS_MODE_CHASE) {
                    localPlayer->m_iObserverMode() = OBS_MODE_CHASE;
                }


                Entity* spec = localPlayer->getObserverTarget();
                if (!spec)
                    return;


                if (once2)
                    localPlayer->m_iObserverMode() = 5;

                if (localPlayer->m_iObserverMode() == 4)
                    once2 = true;

                static Vector angles;
                interfaces->engine->getViewAngles(angles);
                setup->angles = angles;

                Trace tr;
                

                Vector forward, right, up;
                Vector cam_offset = spec->getEyePosition() + angles;

                Ray ray(spec->getEyePosition(), cam_offset);
                TraceFilter trace_filter(spec);
                interfaces->engineTrace->traceRay(ray, MASK_SHOT, trace_filter, tr);
                setup->origin = tr.endpos;

            }
        }
    }
}

void Visuals::removeVisualRecoil(FrameStage stage) noexcept
{
    if (!localPlayer || !localPlayer->isAlive())
        return;

    static Vector aimPunch;
    static Vector viewPunch;

    if (stage == FrameStage::RENDER_START) {
        aimPunch = localPlayer->aimPunchAngle();
        viewPunch = localPlayer->viewPunchAngle();

        if (config->visuals.noAimPunch && !config->misc.recoilCrosshair)
            localPlayer->aimPunchAngle() = Vector{ };

        if (config->visuals.noViewPunch)
            localPlayer->viewPunchAngle() = Vector{ };

    } else if (stage == FrameStage::RENDER_END) {
        localPlayer->aimPunchAngle() = aimPunch;
        localPlayer->viewPunchAngle() = viewPunch;
    }
}

void Visuals::removeBlur(FrameStage stage) noexcept
{
    if (stage != FrameStage::RENDER_START && stage != FrameStage::RENDER_END)
        return;

    static auto blur = interfaces->materialSystem->findMaterial("dev/scope_bluroverlay");
    blur->setMaterialVarFlag(MaterialVarFlag::NO_DRAW, stage == FrameStage::RENDER_START && config->visuals.noBlur);
}

void Visuals::updateBrightness() noexcept
{
    static auto brightness = interfaces->cvar->findVar("mat_force_tonemap_scale");
    brightness->setValue(config->visuals.brightness);
}

void Visuals::removeGrass(FrameStage stage) noexcept
{
    if (stage != FrameStage::RENDER_START && stage != FrameStage::RENDER_END)
        return;

    constexpr auto getGrassMaterialName = []() noexcept -> const char* {
        switch (fnv::hashRuntime(interfaces->engine->getLevelName())) {
        case fnv::hash("dz_blacksite"): return "detail/detailsprites_survival";
        case fnv::hash("dz_sirocco"): return "detail/dust_massive_detail_sprites";
        case fnv::hash("dz_junglety"): return "detail/tropical_grass";
        default: return nullptr;
        }
    };

    if (const auto grassMaterialName = getGrassMaterialName())
        interfaces->materialSystem->findMaterial(grassMaterialName)->setMaterialVarFlag(MaterialVarFlag::NO_DRAW, stage == FrameStage::RENDER_START && config->visuals.noGrass);
}



void Visuals::remove3dSky() noexcept
{
    static bool lastVal;
    static auto sky = interfaces->cvar->findVar("r_3dsky");

    bool shouldSet = (!config->visuals.no3dSky || (config->visuals.skybox > 1));

    if (lastVal != shouldSet) {
        shouldSet = lastVal;
        sky->setValue(shouldSet);
    }

   
}

void Visuals::removeShadows() noexcept
{
    static auto shadows = interfaces->cvar->findVar("cl_csm_enabled");
    shadows->setValue(!config->visuals.noShadows);
}


void Visuals::NoScopeInZoom(FrameStage stage) noexcept {
    //config->visuals.thirdpersonDistance;
    //config->visuals.fov
    /*
    if (stage == FrameStage::RENDER_START) {
        if (localPlayer->isScoped()) {
            localPlayer->fov() = config->visuals.fov;
        }
    }
    */

}

void Visuals::applyZoom(FrameStage stage) noexcept
{
    if (config->visuals.zoom && localPlayer) {
        if (stage == FrameStage::RENDER_START && (localPlayer->fov() == 90 || localPlayer->fovStart() == 90)) {
            static bool scoped{ false };

            if (GetAsyncKeyState(config->visuals.zoomKey) & 1)
                scoped = !scoped;

            if (scoped) {
                localPlayer->fov() = 40;
                localPlayer->fovStart() = 40;
            }
        }
    }
}

#define DRAW_SCREEN_EFFECT(material) \
{ \
    const auto drawFunction = memory->drawScreenEffectMaterial; \
    int w, h; \
    interfaces->surface->getScreenSize(w, h); \
    __asm { \
        __asm push h \
        __asm push w \
        __asm push 0 \
        __asm xor edx, edx \
        __asm mov ecx, material \
        __asm call drawFunction \
        __asm add esp, 12 \
    } \
}

void Visuals::applyScreenEffects() noexcept
{
    if (!config->visuals.screenEffect)
        return;

    const auto material = interfaces->materialSystem->findMaterial([] {
        constexpr std::array effects{
            "effects/dronecam",
            "effects/underwater_overlay",
            "effects/healthboost",
            "effects/dangerzone_screen"
        };

        if (config->visuals.screenEffect <= 2 || static_cast<std::size_t>(config->visuals.screenEffect - 2) >= effects.size())
            return effects[0];
        return effects[config->visuals.screenEffect - 2];
    }());

    if (config->visuals.screenEffect == 1)
        material->findVar("$c0_x")->setValue(0.0f);
    else if (config->visuals.screenEffect == 2)
        material->findVar("$c0_x")->setValue(0.1f);
    else if (config->visuals.screenEffect >= 4)
        material->findVar("$c0_x")->setValue(1.0f);

    DRAW_SCREEN_EFFECT(material)
}

void Visuals::hitEffect(GameEvent* event) noexcept
{


    if (config->visuals.hitEffect && localPlayer) {
        static float lastHitTime = 0.0f;

        if (event && interfaces->engine->getPlayerForUserID(event->getInt("attacker")) == localPlayer->index()) {
            lastHitTime = memory->globalVars->realtime;
            return;
        }

        if (lastHitTime + config->visuals.hitEffectTime >= memory->globalVars->realtime) {
            constexpr auto getEffectMaterial = [] {
                static constexpr const char* effects[]{
                "effects/dronecam",
                "effects/underwater_overlay",
                "effects/healthboost",
                "effects/dangerzone_screen"
                };

                if (config->visuals.hitEffect <= 2)
                    return effects[0];
                return effects[config->visuals.hitEffect - 2];
            };

           
            auto material = interfaces->materialSystem->findMaterial(getEffectMaterial());
            if (config->visuals.hitEffect == 1)
                material->findVar("$c0_x")->setValue(0.0f);
            else if (config->visuals.hitEffect == 2)
                material->findVar("$c0_x")->setValue(0.1f);
            else if (config->visuals.hitEffect >= 4)
                material->findVar("$c0_x")->setValue(1.0f);

            DRAW_SCREEN_EFFECT(material)
        }
    }
}

void Visuals::hitMarker(GameEvent* event) noexcept
{
    if (config->visuals.hitMarker == 0 || !localPlayer)
        return;

    static float lastHitTime = 0.0f;

    if (event && interfaces->engine->getPlayerForUserID(event->getInt("attacker")) == localPlayer->index()) {
        lastHitTime = memory->globalVars->realtime;
        return;
    }

    if (lastHitTime + config->visuals.hitMarkerTime < memory->globalVars->realtime)
        return;

    switch (config->visuals.hitMarker) {
    case 1:
        const auto [width, height] = interfaces->surface->getScreenSize();

        const auto width_mid = width / 2;
        const auto height_mid = height / 2;

        interfaces->surface->setDrawColor(255, 255, 255, 255);
        interfaces->surface->drawLine(width_mid + 10, height_mid + 10, width_mid + 4, height_mid + 4);
        interfaces->surface->drawLine(width_mid - 10, height_mid + 10, width_mid - 4, height_mid + 4);
        interfaces->surface->drawLine(width_mid + 10, height_mid - 10, width_mid + 4, height_mid - 4);
        interfaces->surface->drawLine(width_mid - 10, height_mid - 10, width_mid - 4, height_mid - 4);
        break;
    }
}

void Visuals::disablePostProcessing(FrameStage stage) noexcept
{
    if (stage != FrameStage::RENDER_START && stage != FrameStage::RENDER_END)
        return;

    *memory->disablePostProcessing = stage == FrameStage::RENDER_START && config->visuals.disablePostProcessing;
}

void Visuals::reduceFlashEffect() noexcept
{
    if (localPlayer)
        localPlayer->flashMaxAlpha() = 255.0f - config->visuals.flashReduction * 2.55f;
}

bool Visuals::removeHands(const char* modelName) noexcept
{
    return config->visuals.noHands && std::strstr(modelName, "arms") && !std::strstr(modelName, "sleeve");
}

bool Visuals::removeSleeves(const char* modelName) noexcept
{
    return config->visuals.noSleeves && std::strstr(modelName, "sleeve");
}


bool Visuals::removeWeapons(const char* modelName) noexcept
{

    return config->visuals.noWeapons && std::strstr(modelName, "models/weapons/v_")
        && !std::strstr(modelName, "arms") && !std::strstr(modelName, "tablet")
        && !std::strstr(modelName, "parachute") && !std::strstr(modelName, "fists");
}

void Visuals::skybox(FrameStage stage) noexcept
{
    if (stage != FrameStage::RENDER_START && stage != FrameStage::RENDER_END)
        return;

    constexpr std::array skyboxes{ "cs_baggage_skybox_", "cs_tibet", "embassy", "italy", "jungle", "nukeblank", "office", "sky_cs15_daylight01_hdr", "sky_cs15_daylight02_hdr", "sky_cs15_daylight03_hdr", "sky_cs15_daylight04_hdr", "sky_csgo_cloudy01", "sky_csgo_night_flat", "sky_csgo_night02", "sky_day02_05_hdr", "sky_day02_05", "sky_dust", "sky_l4d_rural02_ldr", "sky_venice", "vertigo_hdr", "vertigo", "vertigoblue_hdr", "vietnam", "sky_lunacy", "sky_borealis01", "cliff", "blue", "city1", "neb1","xen8", "sky2mh_"};

    if (stage == FrameStage::RENDER_START && static_cast<std::size_t>(config->visuals.skybox - 1) < skyboxes.size()) {
        memory->loadSky(skyboxes[config->visuals.skybox - 1]);
    } else {
        static const auto sv_skyname = interfaces->cvar->findVar("sv_skyname");
        memory->loadSky(sv_skyname->string);
    }
}

void Visuals::bulletBeams(GameEvent* event) noexcept
{
    if (!config->visuals.bulletTracers.enabled || !interfaces->engine->isInGame() || !interfaces->engine->isConnected() || memory->renderBeams == nullptr)
        return;

    const auto player = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("userid")));

    if (!player || !localPlayer)
        return;

    if(((player == localPlayer.get()) && !config->visuals.bulletTracers.enabled))
        return;

    if (localPlayer->isOtherEnemy(player) && !config->visuals.bulletTracersEnemy.enabled)
        return;



    Vector position;
    position.x = event->getFloat("x");
    position.y = event->getFloat("y");
    position.z = event->getFloat("z");

    BeamInfo_t beam_info;
    beam_info.m_nType = TE_BEAMPOINTS;
    //beam_info.m_pszModelName = "sprites/physbeam.vmt";
    //beam_info.m_nModelIndex = -1;
    beam_info.m_pszModelName = "sprites/purplelaser1.vmt";
    beam_info.m_nModelIndex = interfaces->modelInfo->getModelIndex("sprites/purplelaser1.vmt");
    beam_info.m_flHaloScale = 0.f;
    beam_info.m_flLife = player->isOtherEnemy(localPlayer.get()) ? .8f : 1.5f;
    beam_info.m_flWidth = 1.f;
    beam_info.m_flEndWidth = 1.f;
    beam_info.m_flFadeLength = 0.1f;
    beam_info.m_flAmplitude = 2.f;
    beam_info.m_flBrightness = 255.f;
    beam_info.m_flSpeed = 0.2f;
    beam_info.m_nStartFrame = 0;
    beam_info.m_flFrameRate = 0.f;
    beam_info.m_flRed = player->isOtherEnemy(localPlayer.get()) ? config->visuals.bulletTracersEnemy.color[0] * 255 : config->visuals.bulletTracers.color[0] * 255;
    beam_info.m_flGreen = player->isOtherEnemy(localPlayer.get()) ? config->visuals.bulletTracersEnemy.color[1] * 255 : config->visuals.bulletTracers.color[1] * 255;
    beam_info.m_flBlue = player->isOtherEnemy(localPlayer.get()) ? config->visuals.bulletTracersEnemy.color[2] * 255 : config->visuals.bulletTracers.color[2] * 255;
    beam_info.m_nSegments = config->visuals.beamseg;


    beam_info.m_bRenderable = true;
    beam_info.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;

    // create beam backwards because it looks nicer.
    beam_info.m_vecStart = position;
    beam_info.m_vecEnd = player->getEyePosition();

    auto beam = memory->renderBeams->CreateBeamPoints(beam_info);
    if (beam)
        memory->renderBeams->DrawBeam(beam);


    if (player->isOtherEnemy(localPlayer.get())) {
        beam_info.m_pszModelName = "sprites/physbeam.vmt";
        beam_info.m_nModelIndex = -1;
        beam_info.m_flLife = .7f;
        auto beam = memory->renderBeams->CreateBeamPoints(beam_info);
        if (beam)
            memory->renderBeams->DrawBeam(beam);
    }

}

//#include "StreamProofESP.h"
#include "../ConfigStructs.h"

void Visuals::grenadeBeams(GameEvent* event) noexcept
{
    // !config->visuals.grenadeBeams.enabled ||
    if (!interfaces->engine->isInGame() || !interfaces->engine->isConnected() || memory->renderBeams == nullptr)
        return;

    const auto player = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("userid")));
    
    
    //std::vector<Vector> player_points;

    if (!player || !localPlayer || (player != localPlayer.get() && !player->isOtherEnemy(localPlayer.get())))
        return;

    Vector position;
    position.x = event->getFloat("x");
    position.y = event->getFloat("y");
    position.z = event->getFloat("z");


    

   

    for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
        auto entity = interfaces->entityList->getEntity(i);
        if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive() || !entity->isOtherEnemy(localPlayer.get())) {
            continue;
        }
        /*
        StreamProofESP::DrawItem Item;

        Item.type = StreamProofESP::LINE;
        Item.Fade = true;
        Item.StartPos = position;
        Item.BoundType = 1;
        Item.boundstartEntity = entity;
        Item.exist_time = 10.0f;
        Item.thickness = 2.0f;
        ColorA color;
        color.color[0] = config->visuals.grenadeBeams.color[0];
        color.color[1] = config->visuals.grenadeBeams.color[1];
        color.color[2] = config->visuals.grenadeBeams.color[2];
        color.color[3] = 1.0f;


        if (entity->isFlashed()) {
            color.color[0] = 1.0f;
            color.color[1] = 1.0f;
            color.color[2] = 1.0f;
        }
        else if (entity->health() < 20) {
            color.color[0] = 1.0f;
            color.color[1] = 0;
            color.color[2] = 0;
        }

        Item.Color = color;



        StreamProofESP::ESPItemList.push_back(Item);
        */
    }
    
    BeamInfo_t beam_info;
    beam_info.m_nType = TE_BEAMPOINTS;
    beam_info.m_pszModelName = "sprites/glow01.vmt";
    beam_info.m_nModelIndex = -1;
    beam_info.m_flHaloScale = 1.f;
    beam_info.m_flLife = 10.f;
    beam_info.m_flWidth = 1.f;
    beam_info.m_flEndWidth = 1.f;
    beam_info.m_flFadeLength = 0.1f;
    beam_info.m_flAmplitude = 2.f;
    beam_info.m_flBrightness = 255.f;
    beam_info.m_flSpeed = 0.1f;
    beam_info.m_nStartFrame = 0;
    beam_info.m_flFrameRate = 0.f;
    beam_info.m_flRed = config->visuals.grenadeBeams.color[0] * 255;
    beam_info.m_flGreen = config->visuals.grenadeBeams.color[1] * 255;
    beam_info.m_flBlue = config->visuals.grenadeBeams.color[2] * 255;
    beam_info.m_nSegments = 35;
    beam_info.m_bRenderable = true;
    beam_info.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;
    beam_info.m_nType = TE_BEAMPOINTS;
    // create beam backwards because it looks nicer.

    for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
        auto entity = interfaces->entityList->getEntity(i);
        if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive() || !entity->isOtherEnemy(localPlayer.get())) {
            continue;
        }

        beam_info.m_vecStart = position;
        beam_info.m_vecEnd = entity->getEyePosition();

        if (entity->isFlashed()) {
            beam_info.m_flRed =  255;
            beam_info.m_flGreen = 255;
            beam_info.m_flBlue = 255;
        }
        else if (entity->health() < 20) {
            beam_info.m_flRed = 255;
            beam_info.m_flGreen = 0;
            beam_info.m_flBlue = 0;
        }

        auto beam = memory->renderBeams->CreateBeamPoints(beam_info);


        if (beam)
            memory->renderBeams->DrawBeam(beam);
        
    }
    
}


int Visuals::lastSize = 0;



void Visuals::BounceRing(GameEvent* event) noexcept
{
    if (!config->visuals.grenadeBounce.enabled || !interfaces->engine->isInGame() || !interfaces->engine->isConnected() || memory->renderBeams == nullptr)
        return;

    //const auto player = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("userid")));
    //const auto nade = interfaces->entityList->getEntity(event->getInt("entityid"));

    //std::vector<Vector> player_points;


    if (!localPlayer)
        return;

    Vector position;
    position.x = event->getFloat("x");
    position.y = event->getFloat("y");
    position.z = event->getFloat("z");

    BeamInfo_t beam_info;
    beam_info.m_nType = TE_BEAMRINGPOINT;
    beam_info.m_pszModelName = "sprites/purplelaser1.vmt";
    beam_info.m_nModelIndex = interfaces->modelInfo->getModelIndex("sprites/purplelaser1.vmt");
    beam_info.m_pszHaloName = "sprites/purplelaser1.vmt";
    beam_info.m_nHaloIndex = interfaces->modelInfo->getModelIndex("sprites/purplelaser1.vmt");
    beam_info.m_flHaloScale = 5;
    beam_info.m_flLife = 3.f;
    beam_info.m_flWidth = 15;
    beam_info.m_flFadeLength = 1.0f;
    beam_info.m_flAmplitude = 0.f;
    beam_info.m_flBrightness = 255;
    beam_info.m_flSpeed = 0;
    beam_info.m_nStartFrame = 0;
    beam_info.m_flFrameRate = 1;
    beam_info.m_nSegments = 1;
    beam_info.m_bRenderable = true;
    beam_info.m_nFlags = 0;
    beam_info.m_vecCenter = position;
    beam_info.m_flStartRadius = 1;
    beam_info.m_flEndRadius = 300;
    beam_info.m_flRed = config->visuals.grenadeBounce.color[0] * 255;
    beam_info.m_flGreen = config->visuals.grenadeBounce.color[1] * 255;
    beam_info.m_flBlue = config->visuals.grenadeBounce.color[2] * 255;

    // create beam backwards because it looks nicer.

    // create beam backwards because it looks nicer.
    beam_info.m_vecStart = position;

    auto beam = memory->renderBeams->CreateBeamRingPoint(beam_info);
    if (beam)
        memory->renderBeams->DrawBeam(beam);
}
/*
#include "../Walkbot.h"
void Visuals::WalkbotRing() noexcept
{
    if (!interfaces->engine->isInGame() || !interfaces->engine->isConnected() || memory->renderBeams == nullptr)
        return;

    //const auto player = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("userid")));
    //const auto nade = interfaces->entityList->getEntity(event->getInt("entityid"));

    //std::vector<Vector> player_points;

    if (!localPlayer || Walkbot::curr_path.empty())
        return;

    if (Walkbot::curr_path.size() == lastSize)
        return;

    lastSize = Walkbot::curr_path.size();

    //if (true)
    //    return;


    Vector position;
    position.x = Walkbot::curr_path.front().toVector().x;
    position.y = Walkbot::curr_path.front().toVector().y;
    position.z = Walkbot::curr_path.front().toVector().z;

    BeamInfo_t beam_info;
    beam_info.m_nType = TE_BEAMRINGPOINT;
    beam_info.m_pszModelName = "sprites/purplelaser1.vmt";
    beam_info.m_nModelIndex = interfaces->modelInfo->getModelIndex("sprites/purplelaser1.vmt");
    beam_info.m_pszHaloName = "sprites/purplelaser1.vmt";
    beam_info.m_nHaloIndex = interfaces->modelInfo->getModelIndex("sprites/purplelaser1.vmt");
    beam_info.m_flHaloScale = 5;
    beam_info.m_flLife = 3.f;
    beam_info.m_flWidth = 15;
    beam_info.m_flFadeLength = 1.0f;
    beam_info.m_flAmplitude = 0.f;
    beam_info.m_flBrightness = 255;
    beam_info.m_flSpeed = 0;
    beam_info.m_nStartFrame = 0;
    beam_info.m_flFrameRate = 1;
    beam_info.m_nSegments = 1;
    beam_info.m_bRenderable = true;
    beam_info.m_nFlags = 0;
    beam_info.m_vecCenter = position;
    beam_info.m_flStartRadius = 1;
    beam_info.m_flEndRadius = 300;
    beam_info.m_flRed = config->visuals.grenadeBounce.color[0] * 255;
    beam_info.m_flGreen = config->visuals.grenadeBounce.color[1] * 255;
    beam_info.m_flBlue = config->visuals.grenadeBounce.color[2] * 255;

    // create beam backwards because it looks nicer.

    // create beam backwards because it looks nicer.
    beam_info.m_vecStart = position;

    auto beam = memory->renderBeams->CreateBeamRingPoint(beam_info);
    if (beam)
        memory->renderBeams->DrawBeam(beam);
}
*/
/*
void Visuals::AimbotFov() noexcept {

}
*/
