
#include <functional>

#include "Chams.h"
#include "../Config.h"
#include "../Hooks.h"
#include "../Interfaces.h"
#include "Backtrack.h"
#include "../SDK/OsirisSDK/Entity.h"
#include "../SDK/OsirisSDK/EntityList.h"
#include "../SDK/OsirisSDK/LocalPlayer.h"
#include "../SDK/OsirisSDK/Material.h"
#include "../SDK/OsirisSDK/MaterialSystem.h"
#include "../SDK/OsirisSDK/StudioRender.h"
#include "../SDK/OsirisSDK/KeyValues.h"
#include "../SDK/OsirisSDK/RenderView.h"
#include "../Other/Animations.h"
#include "RageBot/Resolver.h"

//class Chams ChamHold::chams;
#include <string>




/*

the quick brown fox hgfhdhdhdf


*/



#include <deque>
#define NEWMAT(MAT_NAME) Materials[#MAT_NAME]

// 
// https://developer.valvesoftware.com/wiki/Glowing_Textures#.24selfillum_textures
// 
#define g_pConfig config
Chams::Chams() { // MMHHHH Paste
    {
        //selfillumfresnelminmaxexp 0 0 0 0
        const auto kv = KeyValues::fromString("VertexLitGeneric", "");     
        kv->setString("$envmaptint", "[1 1 1]");
        kv->setString("$envmapcontrast", "0");

        kv->setString("$selfillum", "1");
        kv->setString("$selfillumfresnel", "1");
        kv->setString("$selfillummaskscale", "1");
        Materials["Normal"] = interfaces->materialSystem->createMaterial("normalmcsi",kv);
    }
    NEWMAT(Aftershock) = interfaces->materialSystem->createMaterial("Aftershockmcsi", KeyValues::fromString("Aftershock", nullptr));

    NEWMAT(Flat) = interfaces->materialSystem->createMaterial("flatmcsi", KeyValues::fromString("UnlitGeneric", nullptr));
    NEWMAT(Chrome) = interfaces->materialSystem->createMaterial("chromemcsi", KeyValues::fromString("VertexLitGeneric", "$envmap env_cubemap"));
    NEWMAT(Glow) = interfaces->materialSystem->createMaterial("glowmcsi", KeyValues::fromString("VertexLitGeneric", "$additive 1 $envmap models/effects/cube_white $envmapfresnel 1 $alpha .8"));
    NEWMAT(Pearlescent) = interfaces->materialSystem->createMaterial("pearlescentmcsi", KeyValues::fromString("VertexLitGeneric", "$ambientonly 1 $phong 1 $pearlescent 3 $basemapalphaphongmask 1"));

    NEWMAT(Metallic) = interfaces->materialSystem->createMaterial("metallicmcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture white $ignorez 0 $envmap env_cubemap  $envmapcontrast 1 $nofog 1 $model 1 $nocull 0 $selfillum 1 $halfambert 1 $znearer 0 $flat 1"));
    //metallic = interfaces->materialSystem->createMaterial("custom", KeyValues::fromString("VertexLitGeneric", "$basetexture white $ignorez 0 $envmap env_cubemap $normalmapalphaenvmapmask 1 $envmapcontrast 1 $nofog 0 $model 1 $nocull 0 $selfillum 1 $halfambert 0 $znearer 0 $flat 0"));

    NEWMAT(Metalsnow) = interfaces->materialSystem->createMaterial("metalsnowmcsi", KeyValues::fromString("VertexLitGeneric", "$additive 1 $envmap models/effects/urban_puddle01a_normal"));
    NEWMAT(GlassWindow) = interfaces->materialSystem->createMaterial("glasswindowmcsi", KeyValues::fromString("LightmappedGeneric", "$basetexture glass/hr_g/glass01_inferno $normalmapalphaenvmapmask 1"));
    NEWMAT(C4Gift) = interfaces->materialSystem->createMaterial("c4_giftmcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture models/destruction_tanker/blackcable $normalmapalphaenvmapmask 1"));
    NEWMAT(UrbanPuddle) = interfaces->materialSystem->createMaterial("urban_puddlemcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture models/effects/urban_puddle01a_normal"));
    NEWMAT(CrystalCubeVertigo) = interfaces->materialSystem->createMaterial("crystal_cubemcsi", KeyValues::fromString("VertexLitGeneric", "$additive 1 $envmap models/effects/crystal_cube_vertigo_hdr $envmapfresnel 1"));
    NEWMAT(Ghost1) = interfaces->materialSystem->createMaterial("ghost1mcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture models/seagull/seagull 1 $normalmapalphaenvmapmask 1"));

    NEWMAT(Zombie) = interfaces->materialSystem->createMaterial("zombiemcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture models/player/zombie/csgo_zombie_skin 1 $normalmapalphaenvmapmask 1"));
    NEWMAT(Searchlight) = interfaces->materialSystem->createMaterial("searchlightmcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture models/props_lighting/light_shop 1 $normalmapalphaenvmapmask 1"));
    NEWMAT(Brokenglass) = interfaces->materialSystem->createMaterial("brokenglassmcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture christmas/metal_roof_snow_1 $bumpmap models/inventory_items/hydra_crystal/hydra_crystal $envmapfresnel 1  $phong 1  $basemapalphaphongmask 1 $selfillum 1 $halfambert 1 $znearer 0"));

    NEWMAT(CrystalBlue) = interfaces->materialSystem->createMaterial("crystal_bluemcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture cs_office/cs_whiteboard_04 $bumpmap models/inventory_items/hydra_crystal/hydra_crystal $envmapfresnel 1 $envmap models/effects/crystal_cube_vertigo_hdr $phong 1 $pearlescent 3 $basemapalphaphongmask 1 $selfillum 1 $halfambert 1 $znearer 0"));
    NEWMAT(Velvet) = interfaces->materialSystem->createMaterial("handrailmcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture cs_assault/assault_handrails01 $bumpmap models/inventory_items/hydra_crystal/hydra_crystal $envmapfresnel 1  $phong 1  $basemapalphaphongmask 1"));
    NEWMAT(Metalwall) = interfaces->materialSystem->createMaterial("metalwallmcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture de_vertigo/tv_news01 $bumpmap models/inventory_items/hydra_crystal/hydra_crystal $envmapfresnel 1  $phong 1  $basemapalphaphongmask 1"));

    NEWMAT(Whiteboard01) = interfaces->materialSystem->createMaterial("whiteboard01mcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture cs_office/cs_whiteboard_01 $bumpmap models/inventory_items/hydra_crystal/hydra_crystal $envmapfresnel 1  $phong 1  $basemapalphaphongmask 1"));
    NEWMAT(Whiteboard04) = interfaces->materialSystem->createMaterial("whiteboard01mcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture cs_office/cs_whiteboard_04 $bumpmap models/inventory_items/hydra_crystal/hydra_crystal $envmapfresnel 1  $phong 1  $basemapalphaphongmask 1"));
    NEWMAT(Water) = interfaces->materialSystem->createMaterial("watermcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture cs_assault/windowbrightness $bumpmap models/inventory_items/hydra_crystal/hydra_crystal $envmapfresnel 1  $phong 1  $basemapalphaphongmask 1 $selfillum 1 $halfambert 1 $znearer 0"));
    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$envmap editor/cube_vertigo $envmapcontrast 1 $basetexture dev/zone_warning proxies { texturescroll { texturescrollvar $basetexturetransform texturescrollrate 0.6 texturescrollangle 90 } }");
        kv->setString("$envmaptint", "[.7 .7 .7]");
        NEWMAT(Animated) = interfaces->materialSystem->createMaterial("animatedmcsi", kv);
    }
    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture models/player/ct_fbi/ct_fbi_glass $envmap env_cubemap");
        kv->setString("$envmaptint", "[.4 .6 .7]");
        NEWMAT(Platinum) = interfaces->materialSystem->createMaterial("platinummcsi", kv);
    }
    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture detail/dt_metal1 $additive 1 $envmap editor/cube_vertigo");
        kv->setString("$color", "[.05 .05 .05]");
        NEWMAT(Glass) = interfaces->materialSystem->createMaterial("glassmcsi", kv);
    }
    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture black $bumpmap effects/flat_normal $translucent 1 $envmap models/effects/crystal_cube_vertigo_hdr $envmapfresnel 0 $phong 1 $phongexponent 16 $phongboost 2");
        kv->setString("$phongtint", "[.2 .35 .6]");
        NEWMAT(Crystal) = interfaces->materialSystem->createMaterial("crystalmcsi", kv);
    }
    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture white $bumpmap effects/flat_normal $envmap editor/cube_vertigo $envmapfresnel .6 $phong 1 $phongboost 2 $phongexponent 8");
        kv->setString("$color2", "[.05 .05 .05]");
        kv->setString("$envmaptint", "[.2 .2 .2]");
        kv->setString("$phongfresnelranges", "[.7 .8 1]");
        kv->setString("$phongtint", "[.8 .9 1]");
        NEWMAT(Silver) = interfaces->materialSystem->createMaterial("silvermcsi", kv);
    }
    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture white $bumpmap effects/flat_normal $envmap editor/cube_vertigo $envmapfresnel .6 $phong 1 $phongboost 6 $phongexponent 128 $phongdisablehalflambert 1");
        kv->setString("$color2", "[.18 .15 .06]");
        kv->setString("$envmaptint", "[.6 .5 .2]");
        kv->setString("$phongfresnelranges", "[.7 .8 1]");
        kv->setString("$phongtint", "[.6 .5 .2]");
        NEWMAT(Gold) = interfaces->materialSystem->createMaterial("goldmcsi", kv);
    }
    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture black $bumpmap models/inventory_items/trophy_majors/matte_metal_normal $additive 1 $envmap editor/cube_vertigo $envmapfresnel 1 $normalmapalphaenvmapmask 1 $phong 1 $phongboost 20 $phongexponent 3000 $phongdisablehalflambert 1");
        kv->setString("$phongfresnelranges", "[.1 .4 1]");
        kv->setString("$phongtint", "[.8 .9 1]");
        NEWMAT(Plastic) = interfaces->materialSystem->createMaterial("plasticmcsi", kv);
    }
}

typedef void(__fastcall* drawModelO)(void*, void*, DrawModelResults_t* results, const DrawModelInfo_t& DrawModelInfo, matrix3x4* boneToWorld, float* flexWeights, float* flexDelayedWeights, const Vector& ModelOrigin, int flags);

void Chams::DrawModelRenderCham(Config::Cham_s::Material Mat, void* _this, void* edx, const DrawModelInfo_t& DrawModelInfo, matrix3x4* pBoneToWorld, float* fFlexWeights, float* fFlexDelayedWeights, const Vector& vModelOrigin, int nFlags) {
    drawModelO pOriginal = reinterpret_cast<drawModelO>(g_pChamsRenderer->oDrawModel);
    DrawModelResults_t nResults;
    Material* pMaterial = GetMaterial(Mat.MaterialName);
    if (!pMaterial)
        return;
    float r, g, b;
    r = Mat.color.at(0);
    g = Mat.color.at(1);
    b = Mat.color.at(2);

    if (Mat.MaterialName == "glow" || Mat.MaterialName == "chrome" || Mat.MaterialName == "plastic" || Mat.MaterialName == "glass" || Mat.MaterialName == "crystal")
        pMaterial->findVar("$envmaptint")->setVectorValue(r, g, b);
    else
        pMaterial->colorModulate(r, g, b);


    const auto pulse = Mat.color.at(3) * (Mat.Opts.bBlinking ? std::sin(memory->globalVars->currenttime * 5) * 0.5f + 0.5f : 1.0f);

    interfaces->renderView->setBlend(pulse);

    if (Mat.MaterialName == "glow")
        pMaterial->findVar("$envmapfresnelminmaxexp")->setVecComponentValue(9.0f * (1.2f - pulse), 2);
    else
        pMaterial->alphaModulate(pulse);

    pMaterial->setMaterialVarFlag(MaterialVarFlag::IGNOREZ, Mat.Opts.bIgnorez);
    pMaterial->setMaterialVarFlag(MaterialVarFlag::WIREFRAME, Mat.Opts.bWireframe);


    interfaces->studioRender->forcedMaterialOverride(pMaterial);
    pOriginal(_this, edx, &nResults, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);
    interfaces->renderView->setBlend(1.f);
    interfaces->studioRender->forcedMaterialOverride(nullptr);

}
//int n = sizeof(BoneAccessor);
void Chams::RenderDrawModelPlayerChams(Entity* pEnt, void* _this, void* edx, const DrawModelInfo_t& DrawModelInfo, matrix3x4* pBoneToWorld, float* fFlexWeights, float* fFlexDelayedWeights, const Vector& vModelOrigin, int nFlags) {

    drawModelO pOriginal = reinterpret_cast<drawModelO>(g_pChamsRenderer->oDrawModel);
    DrawModelResults_t nResults;
    // Render Specific ESP
    Config::NEWESP::PlayerESP& PlayerSpecificESP = g_pConfig->m_ESP.m_arrPlayerESP[pEnt->index()];
    if (PlayerSpecificESP.bEnabled && PlayerSpecificESP.Cham.bEnabled) { // Player Specific
        if (PlayerSpecificESP.Cham.bCallOriginal)
            pOriginal(_this, edx, &nResults, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);

        for (Config::Cham_s::Material& Mat : PlayerSpecificESP.Cham.Materials) {
            if (Mat.bRenderInSceneEnd || !Mat.bEnabled)
                continue;

            DrawModelRenderCham(Mat, _this, edx, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);
        }
        interfaces->studioRender->forcedMaterialOverride(nullptr);
        return;
    } // Enemies
    else if (pEnt->isOtherEnemy(localPlayer.get()) && g_pConfig->m_mapChams["Enemies"].bEnabled) {
        Config::Cham_s& EnemyChamOptions = g_pConfig->m_mapChams["Enemies"];

        if (EnemyChamOptions.bCallOriginal)
            pOriginal(_this, edx, &nResults, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);

        if (false && config->debug.Animfix && Animations::data.player.at(pEnt->index()).hasBackup)
        {
            pBoneToWorld = Animations::data.player.at(pEnt->index()).matrix;
            (Vector)vModelOrigin = Animations::data.player.at(pEnt->index()).prevOrigin;
        }


        for (Config::Cham_s::Material& Mat : EnemyChamOptions.Materials) {
            if (Mat.bRenderInSceneEnd || !Mat.bEnabled)
                continue;

            DrawModelRenderCham(Mat, _this, edx, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);
            interfaces->studioRender->forcedMaterialOverride(nullptr);
        }
        if (g_pConfig->m_mapChams["Backtrack"].bEnabled) {
            Config::Cham_s& BacktrackChams = g_pConfig->m_mapChams["Backtrack"];

            for (Config::Cham_s::Material& Mat : BacktrackChams.Materials) {
                if (Mat.bRenderInSceneEnd || !Mat.bEnabled)
                    continue;

                for (Backtrack::Record& record : Backtrack::records[pEnt->index()]) {
                    if (!Backtrack::valid(record.simulationTime))
                        break;

                    DrawModelRenderCham(Mat, _this, edx, DrawModelInfo, record.matrix, fFlexWeights, fFlexDelayedWeights, record.origin, nFlags);
                }
            }
        }

        if (g_pConfig->m_mapChams["TargetedMatrix"].bEnabled && config->debug.animstatedebug.resolver.enabled) {
            Resolver::Record& record = Resolver::PlayerRecords.at(pEnt->index());
           
            if (record.ResolverMatrix)
            {
                Config::Cham_s& TargetChams = g_pConfig->m_mapChams["TargetedMatrix"];

                for (Config::Cham_s::Material& Mat : TargetChams.Materials) {
                    if (Mat.bRenderInSceneEnd || !Mat.bEnabled)
                        continue;


                    DrawModelRenderCham(Mat, _this, edx, DrawModelInfo, record.ResolverMatrix, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);
                }
            }
        }


        //if (record->ResolverMatrix) {
        //    applyChams(config->chams[L"TargetedResolverMatrix"].materials, health, record->ResolverMatrix);
        //}


        interfaces->studioRender->forcedMaterialOverride(nullptr);
        //VCON("Rendering Chams for Player %s\n", pEnt->getPlayerName(false).c_str());
        return;
    }
    else if (!localPlayer.get() && !pEnt->isOtherEnemy(localPlayer.get()) && g_pConfig->m_mapChams["Teammates"].bEnabled) {
        Config::Cham_s& TeamChamOptions = g_pConfig->m_mapChams["Teammates"];

        if (TeamChamOptions.bCallOriginal)
            pOriginal(_this, edx, &nResults, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);

        for (Config::Cham_s::Material& Mat : TeamChamOptions.Materials) {
            if (Mat.bRenderInSceneEnd || !Mat.bEnabled)
                continue;

            DrawModelRenderCham(Mat, _this, edx, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);
            interfaces->studioRender->forcedMaterialOverride(nullptr);
        }

        //VCON("Rendering Chams for Player %s\n", pEnt->getPlayerName(false).c_str());
        return;
    }
#if 1
    else if (pEnt == localPlayer.get()) {

        bool bIsScoped = pEnt->isScoped();

        if (g_pConfig->m_mapChams["LocalPlayer"].bEnabled) {
            Config::Cham_s& TeamChamOptions = g_pConfig->m_mapChams["LocalPlayer"];

            if (TeamChamOptions.bCallOriginal)
                pOriginal(_this, edx, &nResults, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);


            for (Config::Cham_s::Material Mat : TeamChamOptions.Materials) {
                if (Mat.bRenderInSceneEnd || !Mat.bEnabled)
                    continue;

                if(bIsScoped)
                    Mat.color.at(3) *= .5f;

                DrawModelRenderCham(Mat, _this, edx, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);
            }
            interfaces->studioRender->forcedMaterialOverride(nullptr);
        }


        if (g_pConfig->m_mapChams["Fake"].bEnabled && g_pConfig->antiAim.enabled) {
            Vector RenderOrigin = pEnt->getRenderOrigin();
            for (auto& i : Animations::data.fakematrix)
            {
                i[0][3] += RenderOrigin.x;
                i[1][3] += RenderOrigin.y;
                i[2][3] += RenderOrigin.z;
            }
            Config::Cham_s& TeamChamOptions = g_pConfig->m_mapChams["Fake"];

            if (TeamChamOptions.bCallOriginal)
                pOriginal(_this, edx, &nResults, DrawModelInfo, Animations::data.fakematrix, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);

            for (Config::Cham_s::Material Mat : TeamChamOptions.Materials) {
                if (Mat.bRenderInSceneEnd || !Mat.bEnabled)
                    continue;

                if (bIsScoped)
                    Mat.color.at(3) *= .5f;

                DrawModelRenderCham(Mat, _this, edx, DrawModelInfo, Animations::data.fakematrix, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);
            }
            interfaces->studioRender->forcedMaterialOverride(nullptr);

            for (auto& i : Animations::data.fakematrix)
            {
                i[0][3] -= RenderOrigin.x;
                i[1][3] -= RenderOrigin.y;
                i[2][3] -= RenderOrigin.z;
            }
        }
        return;
    }
#endif
    // We didn't render chams, lets call original
    pOriginal(_this, edx, &nResults, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);
}




bool __thiscall Chams::OnDrawModel(void* _this, void* edx, const DrawModelInfo_t* pDrawModelInfo, matrix3x4* pBoneToWorld, float* fFlexWeights, float* fFlexDelayedWeights, const Vector& vModelOrigin, int nFlags) {

    //return false;
    if (m_bInSceneEndRender) {
        DrawModelInfo_t* pInfo = const_cast<DrawModelInfo_t*>(pDrawModelInfo);
        pInfo->m_bStaticLighting = true;
        return false;
    }

   if (pDrawModelInfo->m_pStudioHdr) {
       if (strstr(pDrawModelInfo->m_pStudioHdr->name, "prop")) {
           return false;
       }
   }
   DrawModelResults_t nResults;
    drawModelO pOriginal = reinterpret_cast<drawModelO>(g_pChamsRenderer->oDrawModel);
    uintptr_t pRenderableEnt = (uintptr_t)pDrawModelInfo->m_pClientEntity; // This works on Anteaus but not Harpoon???????


    if (!pRenderableEnt)
        return false;

    Entity* pEnt = reinterpret_cast<Entity*>(uintptr_t(pRenderableEnt) - 0x4);
    DrawModelInfo_t DrawModelInfo = *pDrawModelInfo;


    if (pEnt && (((int)pEnt) > 0)) { // Crash (int)pEnt > 0)  You know its ghetto, but this is Harpoon
        if (!(pEnt->index() > 64) && pEnt->isPlayer()) {
            RenderDrawModelPlayerChams(pEnt, _this, edx, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);
            return true;
        }

        //CBaseViewModel
        if (pEnt->getClientClass()->classId == (ClassId)CBaseViewModel)
              return true;
    }



    if (DrawModelInfo.m_pStudioHdr) {

        // VCON("Model Passed With pStudioHdr->name as %s\n", DrawModelInfo.m_pStudioHdr->name);

        if (strstr(DrawModelInfo.m_pStudioHdr->name, "weapons/v_") || strstr(DrawModelInfo.m_pStudioHdr->name, "weapons\\v_")) {

            if (strstr(DrawModelInfo.m_pStudioHdr->name, "sleeve")) {

                int nMatsDrawn = 0;
                if (g_pConfig->m_mapChams["Sleeves"].bEnabled) {

                    if (g_pConfig->m_mapChams["Sleeves"].bCallOriginal) {
                        nMatsDrawn++;
                        pOriginal(_this, edx, &nResults, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);
                    }


                    for (Config::Cham_s::Material& Mat : g_pConfig->m_mapChams["Sleeves"].Materials) {
                        if (Mat.bEnabled) {
                            nMatsDrawn++;
                            DrawModelRenderCham(Mat, _this, edx, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);
                        }
                    }
                }
                if (!nMatsDrawn)
                    return false;
                return true;
            } if (!strstr(DrawModelInfo.m_pStudioHdr->name, "tablet")
                && !strstr(DrawModelInfo.m_pStudioHdr->name, "parachute")
                && !strstr(DrawModelInfo.m_pStudioHdr->name, "fists")
                && !strstr(DrawModelInfo.m_pStudioHdr->name, "arms")) {

                //CON("Weapons\n");
                int nMatsDrawn = 0;
                if (g_pConfig->m_mapChams["Weapons"].bEnabled) {

                    if (g_pConfig->m_mapChams["Weapons"].bCallOriginal) {
                        nMatsDrawn++;
                        pOriginal(_this, edx, &nResults, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);
                    }
                    for (Config::Cham_s::Material& Mat : g_pConfig->m_mapChams["Weapons"].Materials) {
                        if (Mat.bEnabled) {
                            nMatsDrawn++;
                            DrawModelRenderCham(Mat, _this, edx, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);
                        }
                    }
                }
                if (!nMatsDrawn)
                    return false;
                return true;
            }
        }
        else if (strstr(DrawModelInfo.m_pStudioHdr->name, "weapon")) {
            //CON("World Model");
            int nMatsDrawn = 0;

            for (Config::Cham_s::Material& Mat : g_pConfig->m_mapChams["DroppedWeapons"].Materials) {
                if (Mat.bEnabled) {
                    nMatsDrawn++;
                    DrawModelRenderCham(Mat, _this, edx, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);
                }
            }
            if (!nMatsDrawn)
                return false;
            return true;
        }
        else if (strstr(DrawModelInfo.m_pStudioHdr->name, "chicken")) {
            int nMatsDrawn = 0;

            for (Config::Cham_s::Material& Mat : g_pConfig->m_mapChams["Chickens"].Materials) {
                if (Mat.bEnabled) {
                    nMatsDrawn++;
                    DrawModelRenderCham(Mat, _this, edx, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);
                }
            }
            if (!nMatsDrawn)
                return false;
            return true;
        }
        else if (strstr(DrawModelInfo.m_pStudioHdr->name, "player")) {
            int nMatsDrawn = 0;

            for (Config::Cham_s::Material& Mat : g_pConfig->m_mapChams["Ragdoll"].Materials) {
                if (Mat.bEnabled) {
                    nMatsDrawn++;
                    DrawModelRenderCham(Mat, _this, edx, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);
                }
            }
            if (!nMatsDrawn)
                return false;
            return true;
        }



    }
    pOriginal(_this, edx, &nResults, DrawModelInfo, pBoneToWorld, fFlexWeights, fFlexDelayedWeights, vModelOrigin, nFlags);
    return true;
}


typedef void(__thiscall* DrawModelExecuteOriginal)(void*, void*, void*, const ModelRenderInfo&, matrix3x4*);
bool Chams::OnDrawModelExecute(void* ecx, void* state, const ModelRenderInfo& info, matrix3x4* pCustomBoneToWorld) {
    if (strstr(info.model->name, "models/player/contactshadow/contactshadow_")) /* TODO: Fix */
        return true;
    return false;

    DrawModelExecuteOriginal pOriginal = reinterpret_cast<DrawModelExecuteOriginal>(hooks->modelRender.getOriginal<void, 21>());
    if (interfaces->studioRender->isForcedMaterialOverride()) {
        pOriginal(hooks->modelRender.getThis(), ecx, state, info, pCustomBoneToWorld);
        return true;
    }

    pOriginal(hooks->modelRender.getThis(), ecx, state, info, pCustomBoneToWorld);
    interfaces->studioRender->forcedMaterialOverride(nullptr);

    return true;

}

#include "../SDK/OsirisSDK/RenderView.h"
void Chams::DrawSceneEndCham(Config::Cham_s::Material Mat, Entity* pEnt) {
    DrawModelResults_t nResults;
    Material* pMaterial = GetMaterial(Mat.MaterialName);
    if (!pMaterial)
        return;

    if (Mat.bCallOriginalInSceneEnd) {
        pEnt->DrawModel(0x1, 255);
        return;
    }


    float r, g, b;
    r = Mat.color.at(0);
    g = Mat.color.at(1);
    b = Mat.color.at(2);
    const auto pulse = Mat.color.at(3) * (Mat.Opts.bBlinking ? std::sin(memory->globalVars->currenttime * 5) * 0.5f + 0.5f : 1.0f);

    pMaterial->setMaterialVarFlag(MaterialVarFlag::IGNOREZ, Mat.Opts.bIgnorez);
    pMaterial->setMaterialVarFlag(MaterialVarFlag::WIREFRAME, Mat.Opts.bWireframe);

    if (Mat.MaterialName == "glow" || Mat.MaterialName == "chrome" || Mat.MaterialName == "plastic" || Mat.MaterialName == "glass" || Mat.MaterialName == "crystal")
        pMaterial->findVar("$envmaptint")->setVectorValue(r, g, b);
    else {
        pMaterial->colorModulate(r, g, b);
    }

    if (Mat.MaterialName == "glow")
        pMaterial->findVar("$envmapfresnelminmaxexp")->setVecComponentValue(9.0f * (1.2f - pulse), 2);
    else
        pMaterial->alphaModulate(pulse);



    interfaces->renderView->setBlend(pulse);
    interfaces->renderView->setColorModulation(r, g, b);
    interfaces->studioRender->forcedMaterialOverride(pMaterial);
    pEnt->DrawModel(0x1, 255);
    interfaces->renderView->setBlend(1.f);
    interfaces->studioRender->forcedMaterialOverride(nullptr);
}



#include "../SDK/SDKADDITION/EntityListCull.hpp"


void __forceinline Chams::DrawPlayerSceneEndCham(Entity* pEnt, Config::Cham_s::Material Materials[10])
{
    for (int i = 0; i < 10; i++) {

        Config::Cham_s::Material Mat = Materials[i];

        if (!Mat.bRenderInSceneEnd || !Mat.bEnabled || !Mat.Opts.bIgnorez)
            continue;

        DrawSceneEndCham(Mat, pEnt);
    }
    Config::Cham_s::Material NMat;
    NMat.bCallOriginalInSceneEnd = true;
    DrawSceneEndCham(NMat, pEnt);

    for (int i = 0; i < 10; i++) {

        Config::Cham_s::Material Mat = Materials[i];

        if (!Mat.bRenderInSceneEnd || !Mat.bEnabled || Mat.Opts.bIgnorez)
            continue;

        DrawSceneEndCham(Mat, pEnt);
    }


}

bool Chams::OnSceneEnd() {
    m_bInSceneEndRender = true;

#if 0
    for (int i = 0; i < interfaces->entityList->getHighestEntityIndex(); i++) {
        Entity* pEnt = interfaces->entityList->getEntity(i);
        if (!pEnt || pEnt->isDormant() || pEnt->isPlayer())
            continue;

        if ((pEnt->getClientClass()->classId == (ClassId)CBaseWeaponWorldModel) || (pEnt->getClientClass()->classId == (ClassId)CBaseCombatWeapon) || (pEnt->getClientClass()->classId == (ClassId)CBaseViewModel)) {
            Debug::QuickPrint("ViewModel!!!");
            if (g_pConfig->m_mapChams["Weapons"].bEnabled) {
                Config::Cham_s& Weapons = g_pConfig->m_mapChams["Weapons"];
                for (Config::Cham_s::Material& Mat : Weapons.Materials) {
                    if (!Mat.bRenderInSceneEnd || !Mat.bEnabled)
                        continue;
                    DrawSceneEndCham(Mat, pEnt);
                }
                interfaces->studioRender->forcedMaterialOverride(nullptr);
            }
            else {
                pEnt->DrawModel(0x1, 255);
            }
        }
    }
#endif

    for (EntityQuick& EntQ : entitylistculled->getEntities()) {
        if (EntQ.m_bisDormant || !EntQ.m_bisAlive || EntQ.m_bisLocalPlayer)
            continue;

        Entity* pEnt = EntQ.entity;
        Config::NEWESP::PlayerESP& SpecificPlayerESP = g_pConfig->m_ESP.m_arrPlayerESP[EntQ.index];
        if (SpecificPlayerESP.bEnabled && SpecificPlayerESP.Cham.bEnabled) {
                DrawPlayerSceneEndCham(EntQ.entity, SpecificPlayerESP.Cham.Materials);
        }
        else if (pEnt->isOtherEnemy(localPlayer.get()))
        {
            Config::Cham_s& EnemyChamOptions = g_pConfig->m_mapChams["Enemies"];
            if (EnemyChamOptions.bEnabled) {
                DrawPlayerSceneEndCham(EntQ.entity, EnemyChamOptions.Materials);
                interfaces->studioRender->forcedMaterialOverride(nullptr);
            }
            continue;
        }
        else if (!pEnt->isOtherEnemy(localPlayer.get()) && g_pConfig->m_mapChams["Teammates"].bEnabled) {
            Config::Cham_s& TeamChamOptions = g_pConfig->m_mapChams["Teammates"];
            if (TeamChamOptions.bEnabled)
            {
                DrawPlayerSceneEndCham(EntQ.entity, TeamChamOptions.Materials);
                interfaces->studioRender->forcedMaterialOverride(nullptr);
                continue;
            }
        }
    }


    m_bInSceneEndRender = false;
    interfaces->studioRender->forcedMaterialOverride(nullptr);
    return true;
}

















#if 0
bool iden::HasBeenCalled = false;

Chams::Chams() noexcept
{
    normal = interfaces->materialSystem->createMaterial("normalmcsi", KeyValues::fromString("VertexLitGeneric", nullptr));
    flat = interfaces->materialSystem->createMaterial("flatmcsi", KeyValues::fromString("UnlitGeneric", nullptr));
    chrome = interfaces->materialSystem->createMaterial("chromemcsi", KeyValues::fromString("VertexLitGeneric", "$envmap env_cubemap"));
    glow = interfaces->materialSystem->createMaterial("glowmcsi", KeyValues::fromString("VertexLitGeneric", "$additive 1 $envmap models/effects/cube_white $envmapfresnel 1 $alpha .8"));
    pearlescent = interfaces->materialSystem->createMaterial("pearlescentmcsi", KeyValues::fromString("VertexLitGeneric", "$ambientonly 1 $phong 1 $pearlescent 3 $basemapalphaphongmask 1"));

    metallic = interfaces->materialSystem->createMaterial("metallicmcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture white $ignorez 0 $envmap env_cubemap  $envmapcontrast 1 $nofog 1 $model 1 $nocull 0 $selfillum 1 $halfambert 1 $znearer 0 $flat 1"));
    //metallic = interfaces->materialSystem->createMaterial("custom", KeyValues::fromString("VertexLitGeneric", "$basetexture white $ignorez 0 $envmap env_cubemap $normalmapalphaenvmapmask 1 $envmapcontrast 1 $nofog 0 $model 1 $nocull 0 $selfillum 1 $halfambert 0 $znearer 0 $flat 0"));

    metalsnow = interfaces->materialSystem->createMaterial("metalsnowmcsi", KeyValues::fromString("VertexLitGeneric", "$additive 1 $envmap models/effects/urban_puddle01a_normal"));
    glasswindow = interfaces->materialSystem->createMaterial("glasswindowmcsi", KeyValues::fromString("LightmappedGeneric", "$basetexture glass/hr_g/glass01_inferno $normalmapalphaenvmapmask 1"));
    c4_gift = interfaces->materialSystem->createMaterial("c4_giftmcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture models/destruction_tanker/blackcable $normalmapalphaenvmapmask 1"));
    urban_puddle = interfaces->materialSystem->createMaterial("urban_puddlemcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture models/effects/urban_puddle01a_normal"));
    crystal_cube = interfaces->materialSystem->createMaterial("crystal_cubemcsi", KeyValues::fromString("VertexLitGeneric", "$additive 1 $envmap models/effects/crystal_cube_vertigo_hdr $envmapfresnel 1"));
    ghost1 = interfaces->materialSystem->createMaterial("ghost1mcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture models/seagull/seagull 1 $normalmapalphaenvmapmask 1"));

    zombie = interfaces->materialSystem->createMaterial("zombiemcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture models/player/zombie/csgo_zombie_skin 1 $normalmapalphaenvmapmask 1"));
    searchlight = interfaces->materialSystem->createMaterial("searchlightmcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture models/props_lighting/light_shop 1 $normalmapalphaenvmapmask 1"));
    brokenglass = interfaces->materialSystem->createMaterial("brokenglassmcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture christmas/metal_roof_snow_1 $bumpmap models/inventory_items/hydra_crystal/hydra_crystal $envmapfresnel 1  $phong 1  $basemapalphaphongmask 1 $selfillum 1 $halfambert 1 $znearer 0"));

    crystal_blue = interfaces->materialSystem->createMaterial("crystal_bluemcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture cs_office/cs_whiteboard_04 $bumpmap models/inventory_items/hydra_crystal/hydra_crystal $envmapfresnel 1 $envmap models/effects/crystal_cube_vertigo_hdr $phong 1 $pearlescent 3 $basemapalphaphongmask 1 $selfillum 1 $halfambert 1 $znearer 0"));
    velvet = interfaces->materialSystem->createMaterial("handrailmcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture cs_assault/assault_handrails01 $bumpmap models/inventory_items/hydra_crystal/hydra_crystal $envmapfresnel 1  $phong 1  $basemapalphaphongmask 1"));
    metalwall = interfaces->materialSystem->createMaterial("metalwallmcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture de_vertigo/tv_news01 $bumpmap models/inventory_items/hydra_crystal/hydra_crystal $envmapfresnel 1  $phong 1  $basemapalphaphongmask 1"));

    whiteboard01 = interfaces->materialSystem->createMaterial("whiteboard01mcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture cs_office/cs_whiteboard_01 $bumpmap models/inventory_items/hydra_crystal/hydra_crystal $envmapfresnel 1  $phong 1  $basemapalphaphongmask 1"));
    whiteboard04 = interfaces->materialSystem->createMaterial("whiteboard01mcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture cs_office/cs_whiteboard_04 $bumpmap models/inventory_items/hydra_crystal/hydra_crystal $envmapfresnel 1  $phong 1  $basemapalphaphongmask 1"));
    water = interfaces->materialSystem->createMaterial("watermcsi", KeyValues::fromString("VertexLitGeneric", "$basetexture cs_assault/windowbrightness $bumpmap models/inventory_items/hydra_crystal/hydra_crystal $envmapfresnel 1  $phong 1  $basemapalphaphongmask 1 $selfillum 1 $halfambert 1 $znearer 0"));

    /*
    $baseTexture black $bumpmap models/inventory_items/hydra_bronze/hydra_bronze $additive 1 $envmap editor/cube_vertigo $envmapfresnel 1 $normalmapalphaenvmapmask 1
    \

    models/inventory_items/hydra_crystal/hydra_crystal

    odels/inventory_items/hydra_bronze/hydra_bronze

    models/inventory_items/trophy_majors/crystal_blue
     models/inventory_items/trophy_majors/velvet

    */

    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$envmap editor/cube_vertigo $envmapcontrast 1 $basetexture dev/zone_warning proxies { texturescroll { texturescrollvar $basetexturetransform texturescrollrate 0.6 texturescrollangle 90 } }");
        kv->setString("$envmaptint", "[.7 .7 .7]");
        animated = interfaces->materialSystem->createMaterial("animatedmcsi", kv);
    }

    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture models/player/ct_fbi/ct_fbi_glass $envmap env_cubemap");
        kv->setString("$envmaptint", "[.4 .6 .7]");
        platinum = interfaces->materialSystem->createMaterial("platinummcsi", kv);
    }

    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture detail/dt_metal1 $additive 1 $envmap editor/cube_vertigo");
        kv->setString("$color", "[.05 .05 .05]");
        glass = interfaces->materialSystem->createMaterial("glassmcsi", kv);
    }

    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture black $bumpmap effects/flat_normal $translucent 1 $envmap models/effects/crystal_cube_vertigo_hdr $envmapfresnel 0 $phong 1 $phongexponent 16 $phongboost 2");
        kv->setString("$phongtint", "[.2 .35 .6]");
        crystal = interfaces->materialSystem->createMaterial("crystalmcsi", kv);
    }

    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture white $bumpmap effects/flat_normal $envmap editor/cube_vertigo $envmapfresnel .6 $phong 1 $phongboost 2 $phongexponent 8");
        kv->setString("$color2", "[.05 .05 .05]");
        kv->setString("$envmaptint", "[.2 .2 .2]");
        kv->setString("$phongfresnelranges", "[.7 .8 1]");
        kv->setString("$phongtint", "[.8 .9 1]");
        silver = interfaces->materialSystem->createMaterial("silvermcsi", kv);
    }

    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture white $bumpmap effects/flat_normal $envmap editor/cube_vertigo $envmapfresnel .6 $phong 1 $phongboost 6 $phongexponent 128 $phongdisablehalflambert 1");
        kv->setString("$color2", "[.18 .15 .06]");
        kv->setString("$envmaptint", "[.6 .5 .2]");
        kv->setString("$phongfresnelranges", "[.7 .8 1]");
        kv->setString("$phongtint", "[.6 .5 .2]");
        gold = interfaces->materialSystem->createMaterial("goldmcsi", kv);
    }

    {
        const auto kv = KeyValues::fromString("VertexLitGeneric", "$baseTexture black $bumpmap models/inventory_items/trophy_majors/matte_metal_normal $additive 1 $envmap editor/cube_vertigo $envmapfresnel 1 $normalmapalphaenvmapmask 1 $phong 1 $phongboost 20 $phongexponent 3000 $phongdisablehalflambert 1");
        kv->setString("$phongfresnelranges", "[.1 .4 1]");
        kv->setString("$phongtint", "[.8 .9 1]");
        plastic = interfaces->materialSystem->createMaterial("plasticmcsi", kv);
    }
}

bool Chams::render(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    appliedChams = false;
    this->ctx = ctx;
    this->state = state;
    this->info = &info;
    this->customBoneToWorld = customBoneToWorld;


    auto ModelName = info.model->name; 

    if (strstr(ModelName, "w_c4/c4")) {
        const auto entity = interfaces->entityList->getEntity(info.entityIndex);
        if (entity && !entity->isDormant() && !entity->isPlayer() && entity->isBomb())
            entity->body() = 2;
    }
    if (std::string_view{ info.model->name }.starts_with("models/weapons/v_")) {
        // info.model->name + 17 -> small optimization, skip "models/weapons/v_"
        if (std::strstr(info.model->name + 17, "sleeve"))
            renderSleeves();
        else if (std::strstr(info.model->name + 17, "arms"))
            renderHands();
        else if (!std::strstr(info.model->name + 17, "tablet")
            && !std::strstr(info.model->name + 17, "parachute")
            && !std::strstr(info.model->name + 17, "fists"))
            renderWeapons();
    }
    else if (strstr(info.model->name, "weapons")) {
        applyChams(config->chams[L"DroppedWeapons"].materials, localPlayer->health());
    }
    else if (strstr(ModelName, "chicken")) {
        applyChams(config->chams[L"Chickens"].materials, localPlayer->health());
    }
    else {
        const auto entity = interfaces->entityList->getEntity(info.entityIndex);
        if (entity && !entity->isDormant() && entity->isPlayer())
            renderPlayer(entity);
        if (entity && !entity->isDormant()) {
            if ((int)entity->getClientClass()->classId == (int)CFish) {

                renderFish(entity);
            }
        }


    }

    return appliedChams;
}

void Chams::renderPlayer(Entity* player, bool dead) noexcept
{
    if (!localPlayer)
        return;

    const auto health = player->health();

    if (const auto activeWeapon = player->getActiveWeapon(); activeWeapon && activeWeapon->getClientClass()->classId == ClassId::C4 && activeWeapon->c4StartedArming() && std::any_of(config->chams[L"Planting"].materials.cbegin(), config->chams[L"Planting"].materials.cend(), [](const Config::Material& mat) { return mat.enabled; })) {
        if (dead == false) {
            applyChams(config->chams[L"Planting"].materials, health);
        }
    }
    else if (player->isDefusing() && std::any_of(config->chams[L"Defusing"].materials.cbegin(), config->chams[L"Defusing"].materials.cend(), [](const Config::Material& mat) { return mat.enabled; })) {
        if (dead == false) {
            applyChams(config->chams[L"Defusing"].materials, health);
        }
    }
    else if (player == localPlayer.get()) {
        
        if(Animations::lpPlayerUpdate.size() > 0)
            applyChams(config->chams[L"LP Latency"].materials, health, Animations::lpPlayerUpdate.back().matrix);
        if(Animations::lpPlayerUpdateExtended.size() > 0)
            applyChams(config->chams[L"LP Latency"].materials, health, Animations::lpPlayerUpdateExtended.back().matrix);

        if (config->antiAim.enabled) {
            for (auto& i : Animations::data.fakematrix)
            {
                i[0][3] += info->origin.x;
                i[1][3] += info->origin.y;
                i[2][3] += info->origin.z;
            }

            auto mat = config->chams[L"Desync"].materials;
            if (localPlayer->isScoped()) {
                for (int b = 0; b < mat.size(); b++) {
                    auto m_material = &(mat.at(b));
                    m_material->color[3] = 0.4;
                }
            }
            applyChams(mat, health, Animations::data.fakematrix);

            for (auto& i : Animations::data.fakematrix)
            {
                i[0][3] -= info->origin.x;
                i[1][3] -= info->origin.y;
                i[2][3] -= info->origin.z;
            }

            auto lmat = config->chams[L"Local player"].materials;
            if (localPlayer->isScoped()) {
                for (int b = 0; b < lmat.size(); b++) {
                    auto m_material = &(lmat.at(b));
                    m_material->color[3] = 0.4;
                }
            }

            applyChams(lmat, health);
        }
        else {
            if (!localPlayer->isScoped()) {
                applyChams(config->chams[L"Local player"].materials, health);
            }
            else {
                auto lmat = config->chams[L"Local player"].materials;
                if (localPlayer->isScoped()) {
                    for (int b = 0; b < lmat.size(); b++) {
                        auto m_material = &(lmat.at(b));
                        m_material->color[3] = 0.4;
                    }
                }
            }


        }







    }
    else if (localPlayer->isOtherEnemy(player)) {




        if (!player->isDormant() || dead == true) {



            if (!config->debug.animstatedebug.resolver.enabled) {
                if (dead == false) {
                    applyChams(config->chams[L"Enemies"].materials, health);
                }

                if (config->debug.Animfix) {
                    applyChams(config->chams[L"Original Matrix"].materials, health, Animations::data.player[player->index()].originalMatrix);
                    applyChams(config->chams[L"Animations Matrix"].materials, health, Animations::data.player[player->index()].matrix);
                }




            }


                if (config->debug.Animfix) {
                    applyChams(config->chams[L"Original Matrix"].materials, health, Animations::data.player[player->index()].originalMatrix);
                    applyChams(config->chams[L"Animations Matrix"].materials, health, Animations::data.player[player->index()].matrix);
                }

                auto record = &Resolver::PlayerRecords[player->index()];
                if ((!record || (record->invalid == true) || (!record->once ) || !record->Original)) { // && !record->noDesync
                    if (dead == false) {
                        applyChams(config->chams[L"Enemies"].materials, health);
                    }
                }
                else {
                    applyChams(config->chams[L"Enemies"].materials, health, record->Original);
                }

                if (record->wasTargeted) {
                    auto mat = config->chams[L"Targeted"].materials;
                        if (dead == false) {
                            applyChams(mat, health);
                        }
                    interfaces->studioRender->forcedMaterialOverride(nullptr);
                    if (!record->shots.empty() && config->debug.showshots) {
                        auto newmat = config->chams[L"ShotAt"].materials;
                        for (auto shot : record->shots) {

                            for (int b = 0; b < newmat.size(); b++) {
                                auto m_material = &(newmat.at(b));
                                m_material->color[3] = m_material->color[3] - (m_material->color[3] * ((memory->globalVars->currenttime - shot.simtime) / 4.f));
                            }
                            //mat = newmat; /* Only Works if I do this? Why? I wish I knew. Debug this.*/
                            applyChams(newmat, health, shot.matrix);
                        }
                    }
                    interfaces->studioRender->forcedMaterialOverride(nullptr);
                }
                interfaces->studioRender->forcedMaterialOverride(nullptr);
                for (Resolver::matrixWrapper matrix : record->ResolverMatrices) {
                    if (!matrix.targeted) {
                        //applyChams(config->chams[L"TargetedResolverMatrix"].materials, health, matrix.Matrix);
                        applyChams(config->chams[L"ResolverMatrices"].materials, health, matrix.Matrix);
                    }
                }
                if (record->ResolverMatrix) {
                    applyChams(config->chams[L"TargetedResolverMatrix"].materials, health, record->ResolverMatrix);
                }



            }







        
    
        if (config->backtrack.enabled) {
            auto record = &Backtrack::records[player->index()];
            if (record && record->size()) { // && Backtrack::valid(record->front().simulationTime)
                if (!player->isDormant()) {
                    if (!appliedChams)
                        hooks->modelRender.callOriginal<void, 21>(ctx, state, info, customBoneToWorld);
                    if (config->backtrack.backtrackAll) {
                        for (int i = 0; i < record->size(); i += config->backtrack.step) {
                            auto matrix = record->at(i).matrix;
                            if (!record->at(i).btTargeted) {

                                bool UsingTB = false;
                                if (!Backtrack::valid(record->at(i).simulationTime, &UsingTB))
                                    continue;

                                if (UsingTB) {
                                    applyChams(config->chams[L"Tickbase BT"].materials, health, matrix);
                                }
                                else if (config->backtrack.backtrackx88) {
                                    auto mat = config->chams[L"Backtrack"].materials;
                                    for (int b = 0; b < mat.size(); b++) {
                                        auto m_material = &(mat.at(b));
                                        if (!m_material->enabled)
                                            continue;
                                        m_material->color[0] = (m_material->color[0] + ((config->backtrack.x88.color[0] - m_material->color[0]) * ((float)i / (float)record->size())));
                                        m_material->color[1] = (m_material->color[1] + ((config->backtrack.x88.color[1] - m_material->color[1]) * ((float)i / (float)record->size())));
                                        m_material->color[2] = (m_material->color[2] + ((config->backtrack.x88.color[2] - m_material->color[2]) * ((float)i / (float)record->size())));
                                        m_material->color[3] = (m_material->color[3] + ((config->backtrack.x88.color[3] - m_material->color[3]) * ((float)i / (float)record->size())));
                                    }
                                    applyChams(mat, health, matrix);
                                }
                                else {
                                    applyChams(config->chams[L"Backtrack"].materials, health, matrix);
                                }
                            }
                            else {
                                auto mat = config->chams[L"Backtrack"].materials;
                                for (int b = 0; b < mat.size(); b++) {
                                    auto m_material = &(mat.at(b));
                                    m_material->color = { .2, 1, .2 };
                                    m_material->color[3] = 1.0;

                                }
                                record->at(i).btTargeted = false;
                                applyChams(mat, health, matrix);
                            }

                        }
                    }
                    else {
                        applyChams(config->chams[L"Backtrack"].materials, health, record->back().matrix);
                    }




                }

                if (config->backtrack.extendedrecords) {
                    if (!appliedChams)
                        hooks->modelRender.callOriginal<void, 21>(ctx, state, info, customBoneToWorld);
                    auto ext_record = &Backtrack::extended_records[player->index()];
                    if (ext_record && !(ext_record->empty()) && (ext_record->size() >= 1)) {
                        auto mat = config->chams[L"BreadCrumbs"].materials;
                        for (int i = 0; i < ext_record->size(); i++) {
                            auto erecord = &(ext_record->at(i));
                            auto matrix = erecord->matrix;
                            erecord->alpha -= (.020f * (config->backtrack.breadexisttime / 100)); //(.001 - .015)
                            if (erecord->alpha < 0) {
                                erecord->invalid = true;
                                continue;
                            }
                            for (int b = 0; b < mat.size(); b++) {
                                auto m_material = &(mat.at(b));
                                m_material->color[3] = erecord->alpha;
                            }
                            applyChams(mat, health, matrix);
                        }
                    }
                }

                interfaces->studioRender->forcedMaterialOverride(nullptr);

            }
        }



    }
    else {
        if (dead == false) {
            applyChams(config->chams[L"Allies"].materials, health);
        }
    }
}

void Chams::renderWeapons() noexcept
{
    applyChams(config->chams[L"Weapons"].materials, 100);
}


void Chams::renderFish(Entity* fish) noexcept
{
    applyChams(config->chams[L"Fish"].materials, fish->health());
}

void Chams::renderChicken(Entity* fish) noexcept
{
    applyChams(config->chams[L"Chicken"].materials, fish->health());
}


void Chams::renderHands() noexcept
{
    if (!localPlayer || !localPlayer->isAlive())
        return;

    applyChams(config->chams[L"Hands"].materials, localPlayer->health());
}

void Chams::renderSmoke() noexcept {
    if (!localPlayer || !localPlayer->isAlive())
        return;

    applyChams(config->chams[L"Hands"].materials, localPlayer->health());
}


void Chams::renderSleeves() noexcept
{
    if (!localPlayer || !localPlayer->isAlive())
        return;

    applyChams(config->chams[L"Sleeves"].materials, localPlayer->health());
}



Material* Chams::getMat(Config::SingleCham cham) noexcept {
    if (!cham.enabled)
        return 0;
    Material* material = dispatchMaterial(cham.material);

    if (!material)
        return 0;

    float r, g, b;
    r = cham.color[0];
    g = cham.color[1];
    b = cham.color[2];
    if (material == glow || material == chrome || material == plastic || material == glass || material == crystal)
        material->findVar("$envmaptint")->setVectorValue(r, g, b);
    else
        material->colorModulate(r, g, b);

    const auto pulse = cham.color[3] * (cham.blinking ? std::sin(memory->globalVars->currenttime * 5) * 0.5f + 0.5f : 1.0f);

    if (material == glow)
        material->findVar("$envmapfresnelminmaxexp")->setVecComponentValue(9.0f * (1.2f - pulse), 2);
    else
        material->alphaModulate(pulse);

    material->setMaterialVarFlag(MaterialVarFlag::IGNOREZ, cham.ignorez);
    material->setMaterialVarFlag(MaterialVarFlag::WIREFRAME, cham.wireframe);

    return material;

}

void Chams::SetMat(Material* mat) noexcept {
    *mat = *normal;
}


static void create() {

}

bool s = false;
bool w = false;



Material* pworldMat;
Material * pstaticMat;

void Chams::UpdateChams() {
    if (!s || !w)
        return;

    *pworldMat = *(getMat(config->World));
    *pstaticMat = *(getMat(config->StaticProps));
}

Material* Chams::worldChams(const char* Material, const char* TextureGroup) {
    iden::HasBeenCalled = true;
    if (config->StaticProps.enabled && !strcmp(TextureGroup, "StaticProp textures")) {
        return getMat(config->StaticProps);
    }
    else if (config->World.enabled && !strcmp(TextureGroup, "World textures")) {
        return getMat(config->World);
    }
    return 0;
}

#define DUMB_MENU_SHIT
typedef void(__fastcall* drawModelO)(void*, void*, studioRenderHooks::DrawModelResults_t* results, const studioRenderHooks::DrawModelInfo_t& DrawModelInfo, matrix3x4* boneToWorld, float* flexWeights, float* flexDelayedWeights, const Vector& ModelOrigin, int flags);
void Chams::applyPropChams(void * _this, void * edx, const studioRenderHooks::DrawModelInfo_t& DrawModelInfo, matrix3x4* boneToWorld, float* flexWeights, float* flexDelayedWeights, const Vector& ModelOrigin, int flags) {
    auto original = hooks->studioRender.getOriginal<void, 29, void*, const studioRenderHooks::DrawModelInfo_t&, matrix3x4*, float*, float*, const Vector&, int>(edx, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags);
    //original(_this, edx, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags);
    drawModelO pNewOriginal = (drawModelO)original;
    studioRenderHooks::DrawModelResults_t results;
    if (!interfaces->engine->isInGame() || !interfaces->engine->isConnected() || !localPlayer.get()) {
        //const auto material = normal;

#ifdef DUMB_MENU_SHIT
        if (DrawModelInfo.m_pStudioHdr && DrawModelInfo.m_pStudioHdr->name) {
            Debug::QuickPrint(DrawModelInfo.m_pStudioHdr->name);
            if (strstr(DrawModelInfo.m_pStudioHdr->name, "player")) {
                const auto material = glasswindow;
                const auto material2 = zombie;

                interfaces->studioRender->forcedMaterialOverride(material);
                pNewOriginal(_this, edx, &results, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags);
                interfaces->studioRender->forcedMaterialOverride(zombie);
                pNewOriginal(_this, edx, &results, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags);
                interfaces->studioRender->forcedMaterialOverride(nullptr);
                return;
            }
            else if (strstr(DrawModelInfo.m_pStudioHdr->name, "weapon") && false) {           
                studioRenderHooks::DrawModelResults_t results;
                drawModelO pNewOriginal = (drawModelO)original;

                const auto material = glass;
                if (!iden::HasBeenCalled) {
                    float r, g, b;
                    r = 0;
                    g = 0;
                    b = 0;

                    if (material == glow || material == chrome || material == plastic || material == glass || material == crystal)
                        material->findVar("$envmaptint")->setVectorValue(r, g, b);
                    else
                        material->colorModulate(r, g, b);


                    if (material == glow)
                        material->findVar("$envmapfresnelminmaxexp")->setVecComponentValue(9.0f * (1.2f - 0), 2);
                }
                interfaces->studioRender->forcedMaterialOverride(material);
                pNewOriginal(_this, edx, &results, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags);
                return;
            }
        }
#endif
        pNewOriginal(_this, edx, &results, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags);
        return;
    }
    else {
        pNewOriginal(_this, edx, &results, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags);
        return;
    }


    if (interfaces->studioRender->isForcedMaterialOverride()) {
        pNewOriginal(_this, edx, &results, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags);
        return;
    }
    
    bool set = false;
    for (const auto& cham : config->chams[L"Props"].materials) {
        if (!cham.enabled)
            continue;

        const auto material = dispatchMaterial(cham.material);
        if (!material)
            continue;

        if (!iden::HasBeenCalled) {
            float r, g, b;
            r = cham.color[0];
            g = cham.color[1];
            b = cham.color[2];

            if (material == glow || material == chrome || material == plastic || material == glass || material == crystal)
                material->findVar("$envmaptint")->setVectorValue(r, g, b);
            else
                material->colorModulate(r, g, b);

            const auto pulse = cham.color[3] * (cham.blinking ? std::sin(memory->globalVars->currenttime * 5) * 0.5f + 0.5f : 1.0f);

            if (material == glow)
                material->findVar("$envmapfresnelminmaxexp")->setVecComponentValue(9.0f * (1.2f - pulse), 2);
            else
                material->alphaModulate(pulse);

            material->setMaterialVarFlag(MaterialVarFlag::IGNOREZ, cham.ignorez);
            material->setMaterialVarFlag(MaterialVarFlag::WIREFRAME, cham.wireframe);
        }

        if (!cham.cover) {
            pNewOriginal(_this, edx, &results, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags);
            interfaces->studioRender->forcedMaterialOverride(nullptr);
        }

        interfaces->studioRender->forcedMaterialOverride(material);



        pNewOriginal(_this, edx, &results, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags);

    }

    if (!set) {
        pNewOriginal(_this, edx, &results, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags);
        return;
    }

    iden::HasBeenCalled = true;
    //original(_this, edx, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags);
}

//void __fastcall RenderModels(void* _this, void* edx, StudioModelArrayInfo2_t* pInfo, int nModelTypeCount, ModelRenderSystem::ModelListByType_t* pModelList, int nTotalModelCount, ModelRenderSystem::ModelRenderMode_t renderMode, int nFlags) noexcept;
//((RenderModelsFunc)oRenderModels)(_this, pInfo, nModelTypeCount, pModelList, nTotalModelCount, renderMode, nFlags);






#include "../Memory.h"

#include "../SDK/SDK/ModelRenderSystem.h"
typedef void(__thiscall* RenderModelsFunc)(void*, StudioModelArrayInfo2_t*, int, ModelRenderSystem::ModelListByType_t*, int, ModelRenderSystem::ModelRenderMode_t, int);
void Chams::applyDynamicPropChams(void* _this, void* edx, StudioModelArrayInfo2_t* pInfo, int nModelTypeCount, ModelRenderSystem::ModelListByType_t* pModelList, int nTotalModelCount, ModelRenderSystem::ModelRenderMode_t renderMode, int nFlags, const std::array<Config::Material, 10>& chams) noexcept {
    
    
    bool set = false;
    for (const auto& cham : chams) {
        if (!cham.enabled)
            continue;

        const auto material = dispatchMaterial(cham.material);
        if (!material)
            continue;


            float r, g, b;
            r = cham.color[0];
            g = cham.color[1];
            b = cham.color[2];

            if (material == glow || material == chrome || material == plastic || material == glass || material == crystal)
                material->findVar("$envmaptint")->setVectorValue(r, g, b);
            else
                material->colorModulate(r, g, b);

            const auto pulse = cham.color[3] * (cham.blinking ? std::sin(memory->globalVars->currenttime * 5) * 0.5f + 0.5f : 1.0f);

            if (material == glow)
                material->findVar("$envmapfresnelminmaxexp")->setVecComponentValue(9.0f * (1.2f - pulse), 2);
            else
                material->alphaModulate(pulse);

            material->setMaterialVarFlag(MaterialVarFlag::IGNOREZ, cham.ignorez);
            material->setMaterialVarFlag(MaterialVarFlag::WIREFRAME, cham.wireframe);
        

        if (!cham.cover) {
            ((RenderModelsFunc)oRenderModels)(_this, pInfo, nModelTypeCount, pModelList, nTotalModelCount, renderMode, nFlags);
            interfaces->studioRender->forcedMaterialOverride(nullptr);
        }

        interfaces->studioRender->forcedMaterialOverride(material);

        set = true;
        ((RenderModelsFunc)oRenderModels)(_this, pInfo, nModelTypeCount, pModelList, nTotalModelCount, renderMode, nFlags);
        

    }

    if (!set) {
        ((RenderModelsFunc)oRenderModels)(_this, pInfo, nModelTypeCount, pModelList, nTotalModelCount, renderMode, nFlags);
        return;
    }


    //original(_this, edx, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags);
}


#include "OTHER/Debug.h"

void Chams::RenderModelsChams(void* _this, void* edx, StudioModelArrayInfo2_t* pInfo, int nModelTypeCount, ModelRenderSystem::ModelListByType_t* pModelList, int nTotalModelCount, ModelRenderSystem::ModelRenderMode_t renderMode, int nFlags) noexcept {

    for (int i = 0; i < nModelTypeCount; ++i)
    {
        ModelRenderSystem::ModelListByType_t& list = pModelList[i];

        if (list.m_pStudioHdr) {
            const char* ModelName = list.m_pStudioHdr->name;
            //Debug::QuickPrint(ModelName);
            if (strstr(ModelName, "dropped") || strstr(ModelName, "weapons")) {
                applyDynamicPropChams(_this, edx, pInfo, nModelTypeCount, pModelList, nTotalModelCount, renderMode, nFlags, config->chams[L"Weapons"].materials);
                return;
            }
            else if (strstr(ModelName, "chicken")) {
                applyDynamicPropChams(_this, edx, pInfo, nModelTypeCount, pModelList, nTotalModelCount, renderMode, nFlags, config->chams[L"Chickens"].materials);
                return;
            }
            else if (strstr(ModelName, "player")) {
                applyDynamicPropChams(_this, edx, pInfo, nModelTypeCount, pModelList, nTotalModelCount, renderMode, nFlags, config->chams[L"Ragdolls"].materials);
                return;
            }
            else {
                applyDynamicPropChams(_this, edx, pInfo, nModelTypeCount, pModelList, nTotalModelCount, renderMode, nFlags, config->chams[L"Dynamic Props"].materials);
                return;
            }
        }
        else {
            ((RenderModelsFunc)oRenderModels)(_this, pInfo, nModelTypeCount, pModelList, nTotalModelCount, renderMode, nFlags);
            return;
        }
    }

    ((RenderModelsFunc)oRenderModels)(_this, pInfo, nModelTypeCount, pModelList, nTotalModelCount, renderMode, nFlags);


}



#include "../SDK/OsirisSDK/RenderView.h"
void Chams::applyChams(const std::array<Config::Material, 10>& chams, int health, matrix3x4* customMatrix) noexcept
{
    for (const auto& cham : chams) {

        
        if (!cham.enabled || !cham.ignorez)
            continue;

        const auto material = dispatchMaterial(cham.material);
        if (!material)
            continue;

        float r, g, b;
        if (cham.healthBased && health) {
            r = 1.0f - health / 100.0f;
            g = health / 100.0f;
            b = 0.0f;
        }
        else {
            r = cham.color[0];
            g = cham.color[1];
            b = cham.color[2];
        }

        if (material == glow || material == chrome || material == plastic || material == glass || material == crystal)
            material->findVar("$envmaptint")->setVectorValue(r, g, b);
        else
            material->colorModulate(r, g, b);

        const auto pulse = cham.color[3] * (cham.blinking ? std::sin(memory->globalVars->currenttime * 5) * 0.5f + 0.5f : 1.0f);


        interfaces->renderView->setBlend(cham.color[3]);

        if (material == glow)
            material->findVar("$envmapfresnelminmaxexp")->setVecComponentValue(9.0f * (1.2f - pulse), 2);
        else
            material->alphaModulate(pulse);

        material->setMaterialVarFlag(MaterialVarFlag::IGNOREZ, true);
        material->setMaterialVarFlag(MaterialVarFlag::WIREFRAME, cham.wireframe);
        interfaces->studioRender->forcedMaterialOverride(material);
        hooks->modelRender.callOriginal<void, 21>(ctx, state, info, customMatrix ? customMatrix : customBoneToWorld);
        interfaces->studioRender->forcedMaterialOverride(nullptr);
        appliedChams = true;
    }

#if 1
    for (const auto& cham : chams) {
        if (!cham.enabled || cham.ignorez)
            continue;

        const auto material = dispatchMaterial(cham.material);
        if (!material)
            continue;

        float r, g, b;
        if (cham.healthBased && health) {
            r = 1.0f - health / 100.0f;
            g = health / 100.0f;
            b = 0.0f;
        }
        else {
            r = cham.color[0];
            g = cham.color[1];
            b = cham.color[2];
        }

        if (material == glow || material == chrome || material == plastic || material == glass || material == crystal)
            material->findVar("$envmaptint")->setVectorValue(r, g, b);
        else
            material->colorModulate(r, g, b);

        const auto pulse = cham.color[3] * (cham.blinking ? std::sin(memory->globalVars->currenttime * 5) * 0.5f + 0.5f : 1.0f);
        interfaces->renderView->setBlend(cham.color[3]);

        if (material == glow)
            material->findVar("$envmapfresnelminmaxexp")->setVecComponentValue(9.0f * (1.2f - pulse), 2);
        else
            material->alphaModulate(pulse);

        if (cham.cover && !appliedChams)
            hooks->modelRender.callOriginal<void, 21>(ctx, state, info, customMatrix ? customMatrix : customBoneToWorld);

        material->setMaterialVarFlag(MaterialVarFlag::IGNOREZ, false);
        material->setMaterialVarFlag(MaterialVarFlag::WIREFRAME, cham.wireframe);
        interfaces->studioRender->forcedMaterialOverride(material);
        hooks->modelRender.callOriginal<void, 21>(ctx, state, info, customMatrix ? customMatrix : customBoneToWorld);
        appliedChams = true;
    }


    interfaces->renderView->setBlend(1.f);

#endif
}


/*
if (config->antiAim.enabled && Animations::data.gotMatrix) {
    for (auto& i : Animations::data.fakematrix)
    {
        i[0][3] += info.origin.x;
        i[1][3] += info.origin.y;
        i[2][3] += info.origin.z;
    }
    if (appliedChams)
        hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);

    applyChams(config->chams[DESYNC].materials, entity->health(), Animations::data.fakematrix);
    hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), Animations::data.fakematrix);
    interfaces->studioRender->forcedMaterialOverride(nullptr);
    appliedChams = true;
    for (auto& i : Animations::data.fakematrix)
    {
        i[0][3] -= info.origin.x;
        i[1][3] -= info.origin.y;
        i[2][3] -= info.origin.z;
    }
}
*/
#endif