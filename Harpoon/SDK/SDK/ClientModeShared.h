#pragma once
#include "../OsirisSDK/Client.h"
#include "CHudChat.h"


// Message mode types
enum
{
    MM_NONE = 0,
    MM_SAY,
    MM_SAY_TEAM,
};

enum CursorCode
{
    dc_user,
    dc_none,
    dc_arrow,
    dc_ibeam,
    dc_hourglass,
    dc_waitarrow,
    dc_crosshair,
    dc_up,
    dc_sizenwse,
    dc_sizenesw,
    dc_sizewe,
    dc_sizens,
    dc_sizeall,
    dc_no,
    dc_hand,
    dc_blank, // don't show any custom vgui cursor, just let windows do it stuff (for HTML widget)
    dc_last,
};

typedef unsigned long HCursor;

class Panel;
class CViewSetup;
class Entity;
class UserCmd;

#include "../OsirisSDK/GameEvent.h"
class IClientMode
{
    // Misc.
    public:
        virtual			~IClientMode() {}

        // Called before the HUD is initialized.
        virtual void	InitViewport() = 0;

        // One time init when .dll is first loaded.
        virtual void	Init() = 0;

        // Called when vgui is shutting down.
        virtual void	VGui_Shutdown() = 0;

        // One time call when dll is shutting down
        virtual void	Shutdown() = 0;

        // Called when switching from one IClientMode to another.
        // This can re-layout the view and such.
        // Note that Enable and Disable are called when the DLL initializes and shuts down.
        virtual void	Enable() = 0;
        virtual void	EnableWithRootPanel(Panel* pRoot) = 0;

        // Called when it's about to go into another client mode.
        virtual void	Disable() = 0;

        // Called when initializing or when the view changes.
        // This should move the viewport into the correct position.
        virtual void	Layout(bool bForce = false) = 0;

        // Gets at the viewport, if there is one...
        virtual Panel* GetViewport() = 0;

        // Gets a panel hierarchically below the viewport by name like so "ASWHudInventoryMode/SuitAbilityPanel/ItemPanel1"...
        virtual Panel* GetPanelFromViewport(const char* pchNamePath) = 0;

        // Gets at the viewports vgui panel animation controller, if there is one...
        //virtual AnimationController* GetViewportAnimationController() = 0;
        virtual void* GetViewportAnimationController() = 0;

        // called every time shared client dll/engine data gets changed,
        // and gives the cdll a chance to modify the data.
        virtual void	ProcessInput(bool bActive) = 0;

        // The mode can choose to draw/not draw entities.
        virtual bool	ShouldDrawDetailObjects() = 0;
        virtual bool	ShouldDrawEntity(Entity* pEnt) = 0;
        virtual bool	ShouldDrawLocalPlayer(Entity* pPlayer) = 0;
        virtual bool	ShouldDrawParticles() = 0;

        // The mode can choose to not draw fog
        virtual bool	ShouldDrawFog(void) = 0;

        virtual void	OverrideView(CViewSetup* pSetup) = 0;
        virtual void	OverrideAudioState(void* pAudioState) = 0;
        virtual int		KeyInput(int down, int keynum, const char* pszCurrentBinding) = 0;
        virtual void	StartMessageMode(int iMessageModeType) = 0;
        virtual Panel* GetMessagePanel() = 0;
        virtual void	OverrideMouseInput(float* x, float* y) = 0;
        virtual bool	CreateMove(float flInputSampleTime, UserCmd* cmd) = 0;

        virtual void	LevelInit(const char* newmap) = 0;
        virtual void	LevelShutdown(void) = 0;

        // Certain modes hide the view model
        virtual bool	ShouldDrawViewModel(void) = 0;
        virtual bool	ShouldDrawCrosshair(void) = 0;

        // Let mode override viewport for engine
        virtual void	AdjustEngineViewport(int& x, int& y, int& width, int& height) = 0;

        // Called before rendering a view.
        virtual void	PreRender(CViewSetup* pSetup) = 0;

        // Called after everything is rendered.
        virtual void	PostRender(void) = 0;

        virtual void	PostRenderVGui() = 0;

        virtual void	ActivateInGameVGuiContext(Panel* pPanel) = 0;
        virtual void	DeactivateInGameVGuiContext() = 0;
        virtual float	GetViewModelFOV(void) = 0;

        virtual bool	CanRecordDemo(char* errorMsg, int length) const = 0;

        virtual const char* GetServerName(void) = 0;
        virtual void		SetServerName(wchar_t* name) = 0;
        virtual const char* GetMapName(void) = 0;
        virtual void		SetMapName(wchar_t* name) = 0;

        virtual void	OnColorCorrectionWeightsReset(void) = 0;
        virtual float	GetColorCorrectionScale(void) const = 0;

        virtual int		HudElementKeyInput(int down, int keynum, const char* pszCurrentBinding) = 0;

        virtual void	DoPostScreenSpaceEffects(const CViewSetup* pSetup) = 0;

        virtual void	UpdateCameraManUIState(int iType, int nOptionalParam, uint64_t xuid) = 0;
        virtual void	ScoreboardOff(void) = 0;
        virtual void	GraphPageChanged(void) = 0;

        // Updates.
        public:

            // Called every frame.
            virtual void	Update() = 0;

            virtual void	SetBlurFade(float scale) = 0;
            virtual float	GetBlurFade(void) = 0;
};

class ClientModeShared : public IClientMode, public GameEventListener
    {
#if 1
    // IClientMode overrides.
    public:   
        ClientModeShared() {};
    	virtual				~ClientModeShared() = 0;
    	virtual void		Init() = 0;
    	virtual void		InitViewport() = 0;
    	virtual void		VGui_Shutdown() = 0;
    	virtual void		Shutdown() = 0;
    	virtual void		Enable() = 0;
    	virtual void		EnableWithRootPanel(Panel* pRoot) = 0;
    	virtual void		Disable() = 0;
    	virtual void		Layout(bool bForce = false) = 0;
    	virtual Panel *GetViewport() = 0;
    	virtual Panel *GetPanelFromViewport(const char *pchNamePath) = 0;
    	//virtual AnimationController *GetViewportAnimationController() { return m_pViewport->GetAnimationController(); };
        virtual void* GetViewportAnimationController() = 0;
    	virtual void		ProcessInput(bool bActive) = 0;
    	virtual bool		ShouldDrawDetailObjects() = 0;
    	virtual bool		ShouldDrawEntity(Entity *pEnt) = 0;
    	virtual bool		ShouldDrawLocalPlayer(Entity *pPlayer) = 0;
    	virtual bool		ShouldDrawParticles() = 0;
    	virtual bool		ShouldDrawFog(void) = 0;
    	virtual void		OverrideView(CViewSetup *pSetup) = 0;
    	virtual void		OverrideAudioState(void *pAudioState) { return; }
    	virtual int			KeyInput(int down, int keynum, const char *pszCurrentBinding) = 0;
    	virtual void		StartMessageMode(int iMessageModeType) = 0;
    	virtual Panel *GetMessagePanel() = 0;
    	virtual void		OverrideMouseInput(float *x, float *y) = 0;
    	virtual bool		CreateMove(float flInputSampleTime, UserCmd *cmd) = 0;
    	virtual void		LevelInit(const char *newmap) = 0;
    	virtual void		LevelShutdown(void) = 0;
    	virtual bool		ShouldDrawViewModel(void) = 0;
    	virtual bool		ShouldDrawCrosshair(void) = 0;
    	virtual void		AdjustEngineViewport(int& x, int& y, int& width, int& height) =0; //29
    	// Called before rendering a view.
    	virtual void		PreRender(CViewSetup *pSetup) { return; }; // 30 just returns
    	// Called after everything is rendered.
    	virtual void		PostRender(void) = 0;
    	virtual void		PostRenderVGui(void) = 0;
    	virtual void		ActivateInGameVGuiContext(Panel *pPanel) = 0;
    	virtual void		DeactivateInGameVGuiContext(void) = 0;
    	virtual float		GetViewModelFOV(void) = 0;
    	virtual bool		CanRecordDemo(char *errorMsg, int length) const { return true; }
    	virtual const char*	GetServerName(void) = 0;
    	virtual void		SetServerName(char* name) = 0;
    	virtual const char*	GetMapName(void) = 0;
    	virtual void		SetMapName(char* name) = 0; // 40
    	virtual void		OnColorCorrectionWeightsReset(void) = 0;
    	virtual float		GetColorCorrectionScale(void) const = 0;
    	virtual int			HudElementKeyInput(int down, int nKeynum, const char *pszCurrentBinding) = 0;
    	virtual void		DoPostScreenSpaceEffects(const CViewSetup *pSetup) = 0; // 44
    public:
    	// Called every frame.
    	virtual void	Update() = 0;
    	virtual void	SetBlurFade(float scale) {};
    	virtual float	GetBlurFade(void) { return 0.0f; };
    	virtual void	ReloadScheme(void) = 0;
    	virtual void	ReloadSchemeWithRoot(unsigned int root) = 0;
    	virtual void	OverrideRenderBounds(int& a1, int& a2, int& a3, int& a4, int& a5, int& a6) = 0; // 50
    	virtual void	FireGameEvent(GameEvent* gameEvent) = 0; // 51
     
    	virtual int			GetSplitScreenPlayerSlot() const = 0;
#endif
    public:
    	void* ptr1;
    	void* ptr2;
    	void* m_pViewport;
        //char pad[15];
    	// Message mode handling
    	// All modes share a common chat interface
    	CHudChat			*m_pChatElement; /* ClientMode + 28 */
    	HCursor			m_CursorNone;
        void *m_pWeaponSelection;
    	int						m_nRootSize[2];
    };