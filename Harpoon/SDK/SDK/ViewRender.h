#pragma once
#include "../OsirisSDK/VirtualMethod.h"
class CViewSetup;
class Entity;
class Material;
class ViewRender { /* TODO: Find DrawViewModels and populate subs*/
public:

 public:
	virtual void                Init(void) = 0;
	virtual void                LevelInit(void) = 0;
	virtual void                LevelShutdown(void) = 0;
	virtual void                Shutdown(void) = 0;
	virtual void                OnRenderStart() = 0;
	virtual	void                Render(void* rect) = 0;
	virtual void                RenderView(const CViewSetup& view, const CViewSetup& hudview, int nClearFlags, int whatToDraw) = 0;
	virtual int                 GetDrawFlags() = 0;
	virtual void                StartPitchDrift(void) = 0;
	virtual void                StopPitchDrift(void) = 0;
	virtual void*				GetFrustum() = 0;
	virtual bool                ShouldDrawBrushModels(void) = 0;
	virtual const CViewSetup*   GetPlayerViewSetup(void) const = 0;
	virtual const CViewSetup*   GetViewSetup(void) const = 0;
	virtual void                DisableVis(void) = 0;
	virtual int                 BuildWorldListsNumber() const = 0;
	virtual void                SetCheapWaterStartDistance(float flCheapWaterStartDistance) = 0;
	virtual void                SetCheapWaterEndDistance(float flCheapWaterEndDistance) = 0;
	virtual void                GetWaterLODParams(float& flCheapWaterStartDistance, float& flCheapWaterEndDistance) = 0;
	virtual void                DriftPitch(void) = 0;
	virtual void                SetScreenOverlayMaterial(Material* pMaterial) = 0;
	virtual Material*			GetScreenOverlayMaterial() = 0;
	virtual void                WriteSaveGameScreenshot(const char* pFilename) = 0;
	virtual void                WriteSaveGameScreenshotOfSize(const char* pFilename, int width, int height) = 0;
	virtual void                QueueOverlayRenderView(const CViewSetup& view, int nClearFlags, int whatToDraw) = 0;
	virtual float               GetZNear() = 0;
	virtual float               GetZFar() = 0;
	virtual void                GetScreenFadeDistances(float* min, float* max) = 0;
	virtual Entity*				GetCurrentlyDrawingEntity() = 0;
	virtual void                SetCurrentlyDrawingEntity(Entity* pEnt) = 0; // 29
	virtual void				sub_30() = 0;
	virtual void				sub_31() = 0;
	virtual void				sub_32() = 0;
	virtual void				sub_33() = 0;
	virtual void				sub_34() = 0;
	virtual void				sub_35() = 0;
	virtual void				sub_36() = 0;
	virtual void				DisableFog() = 0;
	virtual void				Render2DEffectsPreHUD(const CViewSetup* view) = 0;
	virtual void				Render2DEffectsPostHUD(const CViewSetup* view) = 0;
	virtual void				RenderPlayerSprites(void) = 0;
	virtual void				RenderSmokeOverlay(bool bPreViewModel = true) = 0;
	virtual void				PerformFlashbangEffect(const CViewSetup& view) = 0;
	virtual void				PerformNightVisionEffect(const CViewSetup& view) = 0;
	//VIRTUAL_METHOD(void, DrawSmokeOverlay, 41, (bool bPreViewModel), (this, bPreViewModel))
	//VIRTUAL_METHOD(void, PerformFlashBangEffect, 42, (const CViewSetup* view), (this, view))
	//VIRTUAL_METHOD(void, LevelInit, 2, (void),(this))
};


#define COMBINE1(X,Y) X##Y  // helper macro
#define COMBINE(X,Y) COMBINE1(X,Y)

#define SUBFUNC() virtual void COMBINE(sub_,__LINE__ )() = 0;
#define FUNC_PLACEHOLDER(VAR) void COMBINE(Func, __LINE__)();



class CViewRender {
public:
	virtual void                Init(void) = 0;
	virtual void                LevelInit(void) = 0;
	virtual void                LevelShutdown(void) = 0;
	virtual void                Shutdown(void) = 0;
	virtual void                OnRenderStart() = 0;
	virtual	void                Render(void* rect) = 0;
	virtual void				RenderView(const CViewSetup& view, const CViewSetup& hudViewSetup, int nClearFlags, int whatToDraw) = 0;
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
	SUBFUNC();
};
