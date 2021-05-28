#include "OtherHooks.h"
#include "Interfaces.h"
#include "Memory.h"
#include "Hooks.h"
#include "Hooks/Antaeus/Hooks/UserMode/MinHook.h"
#include "COMPILERDEFINITIONS.h"
#include "SDK/OsirisSDK/Panel.h"
#include "SDK/OsirisSDK/ModelRender.h"
#include "SDK/OsirisSDK/matrix3x4.h"
#include "SDK/OsirisSDK/StudioRender.h"


#include "Menu.h"
#include "Config.h"
#include "Hooks.h"

#include "Hacks/Chams.h"
#include "Hacks/Grief.h"
#include "Hacks/OTHER/Debug.h"
#include <intrin.h>


#include "GameData.h"

#include <thread>
void __stdcall PanelHooks::paintTraverse(unsigned int panel, bool forceRepaint, bool allowForce) noexcept
{

    if (interfaces->panel->getName(panel) == "MatSystemTopPanel") { 
        Menu::DrawMenu();



        //GameData::updateViewMatrix();
       // Misc::drawBombTimer();
        //Misc::spectatorList();
       // Misc::watermark();
        //Misc::AnimStateMonitor();

       // Debug::run();
        //Debug::PrintLog();
        
        //Visuals::hitMarker();
    }
    hooks->panel.callOriginal<void, 41>(panel, forceRepaint, allowForce);
}




bool __fastcall SvHooks::svCheatsGetBool(void* _this) noexcept
{
    if (uintptr_t(_ReturnAddress()) == memory->cameraThink && config->visuals.thirdperson && localPlayer.get() && localPlayer->isAlive())
        return true;
    else
        return hooks->svCheats.getOriginal<bool, 13>()(_this);
}



#include "SDK/OsirisSDK/Material.h"
#include "Interfaces.h"
#include "SDK/OsirisSDK/MaterialSystem.h"
#include "SDK/OsirisSDK/KeyValues.h"


void __stdcall modelRenderHooks::drawModelExecute(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* pCustomBoneToWorld) noexcept
{
    if (config->walkbot.NoRender)
        return;

    //if (config->debug.HardMode)
    //    return;
#ifdef _DEBUG
    //Debug::QuickPrint("Entering drawModel Execute");
#endif
    if (g_pChamsRenderer->OnDrawModelExecute(ctx, state, info, pCustomBoneToWorld))
        return;


    return hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), pCustomBoneToWorld);

#if 0
    //if (Visuals::removeHands(info.model->name) || Visuals::removeSleeves(info.model->name) || Visuals::removeWeapons(info.model->name))
     //   return;

    static Chams chams;


#ifdef _DEBUG
   // Debug::QuickPrint("Calling Original drawModelExecute");
#endif
    if (!chams.render(ctx, state, info, customBoneToWorld))
        hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);


    interfaces->studioRender->forcedMaterialOverride(nullptr);

#endif
#ifdef _DEBUG
   // Debug::QuickPrint("Exiting drawModelExecute");
#endif
}


// const  ModelRenderInfo& ModelInfo


//__fastcall

#include "Hacks/Chams.h"
//std::unordered_map<std::string, bool> Models;
typedef void(__fastcall* drawModelO)(void*, void*, DrawModelResults_t* results, const DrawModelInfo_t* DrawModelInfo, matrix3x4* boneToWorld, float* flexWeights, float* flexDelayedWeights, const Vector& ModelOrigin, int flags);
void  __fastcall studioRenderHooks::DrawModel(void* _this, void* edx, DrawModelResults_t* results, const DrawModelInfo_t* DrawModelInfo, matrix3x4* boneToWorld, float* flexWeights, float* flexDelayedWeights, const Vector& ModelOrigin, int flags) noexcept {





    if (g_pChamsRenderer->OnDrawModel(_this, edx, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags)) {
        return;
    }

   
    auto original = g_pChamsRenderer->oDrawModel;
    drawModelO pNewOriginal = (drawModelO)original;
#if 0
    static Chams PropChams;

#ifdef _DEBUG
    //Debug::QuickPrint("Entered Draw Model, Calling Original");
#endif 
    if (!interfaces->engine->isInGame() || !interfaces->engine->isConnected() || !localPlayer.get()) {
        //PropChams.applyPropChams(_this, edx, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags);
        pNewOriginal(_this, edx, results, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags);
        return;
    }


    if (config->walkbot.NoRender)
        return;




        if (DrawModelInfo.m_pStudioHdr) {
            if (strstr(DrawModelInfo.m_pStudioHdr->name, "props")) {
                //Debug::QuickPrint("Chamming Props YO!");
                //Debug::QuickPrint(DrawModelInfo.m_pStudioHdr->name);
                PropChams.applyPropChams(_this, edx, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags);
                return;
            }
            else {
                if (!strstr(DrawModelInfo.m_pStudioHdr->name, "weapons") && !strstr(DrawModelInfo.m_pStudioHdr->name, "hands") && !strstr(DrawModelInfo.m_pStudioHdr->name, "sleeves")) {
                    if (config->debug.HardMode)
                        return;
                }

            }
        }
    //}
    //if(DrawModelInfo_t.m_pClientEntity)

#endif

    pNewOriginal(_this, edx, results, DrawModelInfo, boneToWorld, flexWeights, flexDelayedWeights, ModelOrigin, flags);

    
#ifdef _DEBUG
   // Debug::QuickPrint("Exiting Draw Model");
#endif 
    return;

}

/*
    struct DrawModelInfo_t
    {
        StudioHdr* m_pStudioHdr;
        void* m_pHardwareData;
        void* m_Decals;
        int				m_Skin;
        int				m_Body;
        int				m_HitboxSet;
        Entity* m_pClientEntity;
        int				m_Lod;
        void* m_pColorMeshes;
        bool			m_bStaticLighting;
        void*	m_LightingState;
    };
*/


void __fastcall studioRenderHooks::drawModelStaticProp(void* _this, void* edx, const DrawModelInfo_t& DrawModelInfo, const matrix3x4* modelToWorld, int flags) {
    if (config->walkbot.NoRender)
        return;
    //return;


    const char* formatter = {
    "Draw Static Prop Call:     \n"
    " Entity             : %i \n"
    " edx                : %i \n"
    " ClassID            : %i \n"
    };
    char buffer[2048];

    int ClassID = 0;
    if (DrawModelInfo.m_pClientEntity) {
        ClassID = (int)((Entity*)DrawModelInfo.m_pClientEntity)->getClientClass();
    }


    snprintf(buffer, 2048, formatter, edx, DrawModelInfo.m_pClientEntity, edx, ClassID);

    //Debug::QuickPrint(buffer, false);



    auto original = hooks->studioRender.getOriginal<void, 30, const DrawModelInfo_t&,const matrix3x4*, int>(DrawModelInfo, modelToWorld, flags);

    original(_this, DrawModelInfo, modelToWorld, flags);
}


//#define _MATERIAL_HOOK_DEBUG


void __fastcall  studioRenderHooks::drawModelArrayStaticProp(void* _this, void* edx, const DrawModelInfo_t& drawInfo, int nInstanceCount, const void* pInstanceData, void** pColorMeshes) noexcept {
    if (config->walkbot.NoRender)
        return;
    
    const char* formatter = {
    "Draw Model Array Static Prop Call:     \n"
            " Entity             : %i \n"
            " edx                : %i \n"
            " ClassID            : %i \n"         
            " InstanceCount      : %i \n"
    };
    char buffer[2048];

    int ClassID = 0;
    if (drawInfo.m_pClientEntity) {
        ClassID = (int)((Entity*)drawInfo.m_pClientEntity)->getClientClass();
    }


    snprintf(buffer, 2048, formatter, edx, drawInfo.m_pClientEntity, edx, ClassID, nInstanceCount);

   // Debug::QuickPrint(buffer, false);

    auto original = hooks->materialSystem.getOriginal<void, 47, const DrawModelInfo_t&, int, const void*, void**>(drawInfo, nInstanceCount, pInstanceData, pColorMeshes);
    original(_this, drawInfo, nInstanceCount, pInstanceData, pColorMeshes);

}


void __fastcall materialSystemHooks::reloadMaterials(void* _this, void* edx, const char* pSubString) noexcept {
    
    
    const char* formatter = {
        "reloadMaterials Call:     \n"
        "      edx             : %d\n"
        "      pSubString      : %s\n"
    };
    char buffer[2048];
    snprintf(buffer, 2048, formatter, edx, pSubString);

    Debug::QuickPrint(buffer, false);
    
    
    auto original = hooks->materialSystem.getOriginal<void, 82, const char*>(pSubString);

    if (!pSubString) {
        return original(_this, pSubString);
    }

    if (strstr(pSubString, "mcsi")) {
        Debug::QuickPrint("RELOADING CUSTOM");
    }
    
    return original(_this, pSubString);


}

Material* __fastcall materialSystemHooks::findMaterial(void* _this, void* edx, const char* MaterialName, const char* TextureGroupName, bool complain, const char* ComplainPrefix) {

    auto original = hooks->materialSystem.getOriginal<Material*, 84, const char*, const char*, bool, const char*>( MaterialName, TextureGroupName, complain, ComplainPrefix);
#if 0
    if (_ReturnAddress() == (void*)hooks->studioRender.getOriginalPtr(30)) {
        Debug::QuickPrint("findMaterial Called by drawModelStaticProp\n", false);
    }
#ifdef _MATERIAL_HOOK_DEBUG
    const char* formatter = {
        "Find Material Call:     \n"
        " edx                : %i\n"
        " Material Name      : %s\n"
        " Texture Group Name : %s \n"
        " Complain           : %i\n"
        " Complain Prefix    : %s\n"
    };
    char buffer[2048];
    snprintf(buffer, 2048, formatter, edx, MaterialName, TextureGroupName, complain, ComplainPrefix);

    Debug::QuickPrint(buffer, false);
#endif

    /*
        auto original = hooks->materialSystem.getOriginal<Material*, 84, void*, const char*, const char*, bool, const char*>(edx, MaterialName, TextureGroupName, complain, ComplainPrefix);
    Debug::QuickPrint("In findMaterial");
    */

    //Material* Mat = original(_this, edx, TextureGroupName, complain, ComplainPrefix);
    static Chams cham;
    if (strstr(MaterialName, "engine") || !TextureGroupName || !MaterialName) {
#ifdef _MATERIAL_HOOK_DEBUG
        Debug::QuickPrint("Calling Original Due to Engine/!Tex/!Mat\n", false);
#endif
        return original(_this,  MaterialName, TextureGroupName, complain, ComplainPrefix);
    }

    if (false) {
#ifdef _MATERIAL_HOOK_DEBUG
        Debug::QuickPrint("No Chams Shared\n", false);
#endif
        return original(_this, MaterialName, TextureGroupName, complain, ComplainPrefix);
    }
    
    
    if (!strcmp(TextureGroupName, "StaticProp textures") || !strcmp(TextureGroupName, "World textures") || strstr(MaterialName, "mcsi")){
#ifdef _MATERIAL_HOOK_DEBUG
        Debug::QuickPrint("Calling World Chams\n", false);
#endif
       // Debug::QuickPrint("Calling World Chams\n", false);
        if (strstr(MaterialName, "mcsi")) {
            //Debug::QuickPrint("Custom Cham Detected\n", false);
        }

        Material* mat = nullptr;
        mat = cham.worldChams(MaterialName, TextureGroupName);
        if (!mat) {
#ifdef _MATERIAL_HOOK_DEBUG
            Debug::QuickPrint("Returning due to !mat\n", false);
#endif
            return original(_this, MaterialName, TextureGroupName, complain, ComplainPrefix);
        }
        return mat;
    }
#ifdef _MATERIAL_HOOK_DEBUG
    Debug::QuickPrint("Calling Original\n", false);
#endif
#endif
    return original(_this, MaterialName, TextureGroupName, complain, ComplainPrefix);



    return nullptr;
}



typedef void(__thiscall* RenderSmokeOverlayFunc)(CViewSetup*, bool);
void __fastcall viewRenderHooks::RenderSmokeOverlay(CViewSetup* pViewSetup, void* ecx, bool bPreViewModel) {
    static RenderSmokeOverlayFunc pOriginal = reinterpret_cast<RenderSmokeOverlayFunc>(hooks->viewRender.getOriginal<void,41>());

    if (g_pChamsRenderer->bRenderSmoke)
        pOriginal(pViewSetup, bPreViewModel);

    return;
}

// char __userpurge DrawViewModels@<al>(int a1@<ecx>, int a2@<ebp>, int a3@<edi>, int a4@<esi>, double a5@<st0>, _DWORD *a6, char a7, char a8)
typedef bool(__thiscall* DrawViewModelsFunc)(void*, const CViewSetup* view, bool drawViewmodel, double a5, DWORD* a6, char a7, char a8);
bool __fastcall viewRenderHooks::DrawViewModels(void* ecx, void* edx, const CViewSetup* view, bool drawViewmodel, double a5, DWORD* a6, char a7, char a8) {
    static DrawViewModelsFunc pOriginal = reinterpret_cast<DrawViewModelsFunc>(memory->oDrawViewModels);
    return pOriginal(ecx, view, drawViewmodel, a5,  a6,  a7,  a8);
}

CViewSetup* g_pView;
void* g_phudviewSetup = nullptr;
typedef void(__thiscall* RenderViewFunc)(void*, const CViewSetup& view, const CViewSetup& hudview, int nClearFlags, int whatToDraw);
void __fastcall viewRenderHooks::RenderView(void* ecx, void*, const CViewSetup& view, const CViewSetup& hudview, int nClearFlags, int whatToDraw) {
    static RenderViewFunc pOriginal = reinterpret_cast<RenderViewFunc>(memory->oRenderView);
    g_pView = const_cast<CViewSetup*>(&view);
    g_phudviewSetup = (void*)&hudview;
    pOriginal(ecx, view, hudview, nClearFlags, whatToDraw);

}

typedef int(__thiscall* DrawUnderWaterOverlayFunc)(void*);
int __fastcall viewRenderHooks::DrawUnderWaterOverlay(void* _this)
{
    static DrawUnderWaterOverlayFunc pOriginal = reinterpret_cast<DrawUnderWaterOverlayFunc>(memory->oDrawUnderWaterOverlay);
    int nRet = pOriginal(_this);

    return nRet;
}


typedef int(__thiscall* UnknownViewRenderFunctionFunc)(void*, char a2);
int __fastcall viewRenderHooks::UnknownViewRenderFunction(void* _this, void* edx, char a2) {
    static UnknownViewRenderFunctionFunc pOriginal = reinterpret_cast<UnknownViewRenderFunctionFunc>(memory->oUnknownViewRenderFunc);
    int nRet = pOriginal(_this, a2);
    return nRet;
}
#include "SDK/OsirisSDK/RenderView.h"
#include "SDK/SDK/CViewSetup.h"
#include "SDK/SDK/ViewRender.h"
typedef void(__thiscall* PerformScreenOverlayFunc_t)(void*, int, int, int, int);
bool g_bShouldDrawLights = false;
bool g_bShouldDrawScreenOverlay = false;
bool g_ShouldStopFog = false;
bool g_bUpdateToneMapScalar = false;
typedef int(__thiscall* UnknownViewRenderFunctionFunc2)(void*, int a2);
int __fastcall viewRenderHooks::UnknownViewRenderFunction2(void* _this, void* edx, int a2) {

    
    static PerformScreenOverlayFunc_t PerfScreenOverlay = (PerformScreenOverlayFunc_t)memory->oPerformScreenOverlay;
    if (PerfScreenOverlay)
    {
        g_bShouldDrawScreenOverlay = true;
        //PerfScreenOverlay(_this, g_pView->x, g_pView->y, g_pView->width, g_pView->height);
        //memory->viewRender->Render2DEffectsPostHUD((const CViewSetup*)g_phudviewSetup);
        //g_bShouldDrawScreenOverlay = false;
    }



    static ConVar* fullbright = interfaces->cvar->findVar("mat_fullbright");
    fullbright->setValue(1);
    UpdateToneMapScalar(_this, 0);


    if(config->debug.bRenderChamsOverViewModel)
        g_pChamsRenderer->OnSceneEnd();

    g_bShouldDrawLights = true;
    interfaces->renderView->DrawLights();
    g_bShouldDrawLights = false;

    g_ShouldStopFog = true;
    memory->viewRender->DisableFog();
    g_ShouldStopFog = false;

    static UnknownViewRenderFunctionFunc2 pOriginal = reinterpret_cast<UnknownViewRenderFunctionFunc2>(memory->oUnknownViewRenderFunc2);
    int nRet = pOriginal(_this, a2);

    fullbright->setValue(0);
    UpdateToneMapScalar(_this, 0);

    return nRet;
}


void __fastcall viewRenderHooks::PerformScreenOverlay(void* _this, void*, int x, int y, int w, int h)
{
    if (!g_bShouldDrawScreenOverlay)
        return;

    static PerformScreenOverlayFunc_t pOriginal{ (PerformScreenOverlayFunc_t)memory->oPerformScreenOverlay };

    pOriginal(_this, x, y, w, h);
}

typedef void(__thiscall* DisableFogFunc_t)(void*);
void __fastcall viewRenderHooks::DisableFog(void* _this, void*)
{

    static DisableFogFunc_t pOriginal = reinterpret_cast<DisableFogFunc_t>(hooks->viewRender.getOriginal<void, 37>());

    if (g_ShouldStopFog)
        pOriginal(_this);

}


void __fastcall viewRenderHooks::UpdateToneMapScalar(void* _this, void*)
{
    ((DisableFogFunc_t)memory->oUpdateToneMapScalar)(_this);
}



int __stdcall bspQueryHooks::listLeavesInBox(const Vector& mins, const Vector& maxs, unsigned short* list, int listMax) noexcept
{
    if (std::uintptr_t(_ReturnAddress()) == memory->listLeaves) {
        if (const auto info = *reinterpret_cast<RenderableInfo**>(std::uintptr_t(_AddressOfReturnAddress()) + 0x14); info && info->renderable) {
            if (const auto ent = VirtualMethod::call<Entity*, 7>(info->renderable - 4); ent && ent->isPlayer()) {
                if (true) {//config->misc.disableModelOcclusion
                    // FIXME: sometimes players are rendered above smoke, maybe sort render list?
                    info->flags &= ~0x100;
                    info->flags2 |= 0x40;
                    constexpr float maxCoord = 16384.0f;
                    constexpr float minCoord = -maxCoord;
                    constexpr Vector min{ minCoord, minCoord, minCoord };
                    constexpr Vector max{ maxCoord, maxCoord, maxCoord };
                    return hooks->bspQuery.callOriginal<int, 6>(std::cref(min), std::cref(max), list, listMax);
                }
            }
        }
    }
    return hooks->bspQuery.callOriginal<int, 6>(std::cref(mins), std::cref(maxs), list, listMax);
}



namespace SurfaceHooks {
    void __stdcall SurfaceHooks::setDrawColor(int r, int g, int b, int a) noexcept {
        if (config->visuals.noScopeOverlay && (*static_cast<std::uint32_t*>(_ReturnAddress()) == 0x20244C8B || *reinterpret_cast<std::uint32_t*>(std::uintptr_t(_ReturnAddress()) + 6) == 0x01ACB7FF))
            a = 0;
        hooks->surface.callOriginal<void, 15>(r, g, b, a);
    }
}



#include "SDK/SDK/ViewRender.h"
#include "SDK/SDK/IEffects.h"

struct dworldlight_t
{
    Vector origin; //0x0000 
    Vector intensity; //0x000C 
    Vector normal; //0x0018 
    int cluster; //0x0024 
    int type; //0x0028 
    int style; //0x002C 
    float stopdot; //0x0030 
    float stopdot2; //0x0034 
    float exponent; //0x0038 
    float radius; //0x003C 
    float constant_attn; //0x0040 
    float linear_attn; //0x0044 
    float quadratic_attn; //0x0048 
    int flags; //0x004C 
    int texinfo; //0x0050 
    int owner; //0x0054 
    char pad_0x0058[0xC]; //0x0058
};

class CHostState
{
public:
    char pad_0x0000[0xA8]; //0x0000
    int numworldlights;
    dworldlight_t* worldlights;

}; //Size=0x00B0


namespace RenderViewHooks {


    typedef void(__fastcall* SceneEndFunc)(void*, void*);
    void __fastcall SceneEnd(void* _this, void* edx) {
        //CON("SCENE END");
        SceneEndFunc pOriginal{ reinterpret_cast<SceneEndFunc>(hooks->renderView.getOriginal<void,9>()) };
        pOriginal(_this, edx);
        
        g_pChamsRenderer->bRenderSmoke = true;
        memory->viewRender->RenderSmokeOverlay(true);
        g_pChamsRenderer->bRenderSmoke = false;
        if(!config->debug.bRenderChamsOverViewModel)
            g_pChamsRenderer->OnSceneEnd();
        
        return;
    }

    typedef void(__thiscall* Render2DEffectsPostHUDFunc_t)(void*, const CViewSetup& view);
    void __fastcall Render2DEffectsPostHUD(void* _this, void*, const CViewSetup& view) {
        //if (!g_bShouldDrawScreenOverlay)
        //    return;
        // 55 8B EC 83 E4 F8 A1 ? ? ? ? 83 EC 14 F3 0F 10 05 ? ? ? ?
        Render2DEffectsPostHUDFunc_t pOriginal{ reinterpret_cast<Render2DEffectsPostHUDFunc_t>(hooks->renderView.getOriginal<void,39>()) };
        pOriginal(_this, view);
        return;
    }

    typedef void(__thiscall* DrawLightsFunc_t)(void*);
    void __fastcall DrawLights(void* _this, void* edx) {

        if (!g_bShouldDrawLights)
            return;

        DrawLightsFunc_t pOriginal = reinterpret_cast<DrawLightsFunc_t>(hooks->renderView.getOriginal<void, 21>()) ;
        static CHostState* pHostState{ **(CHostState * * *)(Memory::findPattern_ex(L"engine", "\xA1????\x83\xC3\x64") + 1) };
        if(config->visuals.DrawLights.enabled)
        { 
            for (int i = 0; i < pHostState->numworldlights; i++)
            {
                dworldlight_t* pLight = &pHostState->worldlights[i];
                if (!pLight)
                    continue;

                // pLight->exponent = config->visuals.DrawLights.color.at(3);
                pLight->intensity = Vector{ 500000.f * config->visuals.DrawLights.color.at(0), 500000.f * config->visuals.DrawLights.color.at(1), 500000.f * config->visuals.DrawLights.color.at(2) };
            }
        }


        pOriginal(_this);
        return;
    }


}







namespace fileSystemHooks {
    bool __fastcall allowLooseFiles(void* ecx, void* edx) noexcept {
        return true;
    }

    int __fastcall canLoadThirdPartyFiles(void* _this) noexcept {
        return 1;
    }

    int __stdcall getUnveridiedFileHashes(void* _this, void* other, int MaxFiles) noexcept {
        return 0;

        //hooks->fileSystem.callOriginal<>(maxFiles)


    }


}