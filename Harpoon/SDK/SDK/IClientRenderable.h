class Model;
class matrix3x4;
class IClientUnknown;

#include "../OsirisSDK/Vector.h"

class IClientRenderable
{
public:
	virtual IClientUnknown* GetUnknown() = 0;
	virtual Vector const& GetRenderOrigin(void) = 0;
	virtual Vector const& GetRenderAngles(void) = 0;
	virtual bool ShouldDraw(void) = 0;
	virtual int GetRenderFlags(void) = 0;
	virtual void Unused(void) const {}
	virtual unsigned short GetShadowHandle() const = 0;
	virtual unsigned short& RenderHandle() = 0;
	virtual const Model* GetModel() const = 0;
	virtual int DrawModel(int flags, const int& instance) = 0;
	virtual int GetBody() = 0;
	virtual void GetColorModulation(float* color) = 0;
	virtual bool LODTest() = 0;
	virtual bool SetupBones(matrix3x4* matrix, int maxBones, int boneMask, float curTime) = 0;
	virtual void SetupWeights(const matrix3x4* matrix, int flexWeightCount, float* flexWeights, float* flexDelayedWeights) = 0;
	virtual void DoAnimationEvents(void) = 0;
	virtual void* GetPVSNotifyInterface() = 0;
	virtual void GetRenderBounds(Vector& mins, Vector& maxs) = 0;
	virtual void GetRenderBoundsWorldspace(Vector& mins, Vector& maxs) = 0;
	virtual void GetShadowRenderBounds(Vector& mins, Vector& maxs, int shadowType) = 0;
	virtual bool ShouldReceiveProjectedTextures(int flags) = 0;
	virtual bool GetShadowCastDistance(float* dist, int shadowType) const = 0;
	virtual bool GetShadowCastDirection(Vector* direction, int shadowType) const = 0;
	virtual bool IsShadowDirty() = 0;
	virtual void MarkShadowDirty(bool dirty) = 0;
	virtual IClientRenderable* GetShadowParent() = 0;
	virtual IClientRenderable* FirstShadowChild() = 0;
	virtual IClientRenderable* NextShadowPeer() = 0;
	virtual int ShadowCastType() = 0;
	virtual void CreateModelInstance() = 0;
	virtual unsigned short GetModelInstance() = 0;
	virtual const matrix3x4& RenderableToWorldTransform() = 0;
	virtual int LookupAttachment(const char* attachmentName) = 0;
	virtual bool GetAttachment(int number, Vector& origin, Vector& angles) = 0;
	virtual bool GetAttachment(int number, matrix3x4& matrix) = 0;
	virtual float* GetRenderClipPlane(void) = 0;
	virtual int GetSkin() = 0;
	virtual void OnThreadedDrawSetup() = 0;
	virtual bool UsesFlexDelayedWeights() = 0;
	virtual void RecordToolMessage() = 0;
	virtual bool ShouldDrawForSplitScreenUser(int slot) = 0;
	virtual unsigned char OverrideAlphaModulation(unsigned char alpha) = 0;
	virtual unsigned char OverrideShadowAlphaModulation(unsigned char alpha) = 0;
};


#define CLIENTLEAFSYSTEM_INTERFACE_VERSION	"ClientLeafSystem002"

typedef unsigned short ClientRenderHandle_t;

enum
{
	INVALID_CLIENT_RENDER_HANDLE = (ClientRenderHandle_t)0xffff,
};

enum RenderableModelType_t
{
	RENDERABLE_MODEL_UNKNOWN_TYPE = -1,
	RENDERABLE_MODEL_ENTITY = 0,
	RENDERABLE_MODEL_STUDIOMDL,
	RENDERABLE_MODEL_STATIC_PROP,
	RENDERABLE_MODEL_BRUSH,
};

enum RenderableTranslucencyType_t
{
	RENDERABLE_IS_OPAQUE = 0,
	RENDERABLE_IS_TRANSLUCENT,
	RENDERABLE_IS_TWO_PASS,	// has both translucent and opaque sub-partsa
};


//-----------------------------------------------------------------------------
// The client leaf system
//-----------------------------------------------------------------------------
class IClientLeafSystemEngine
{
public:
	// Adds and removes renderables from the leaf lists
	// CreateRenderableHandle stores the handle inside pRenderable.
	virtual void CreateRenderableHandle(IClientRenderable * pRenderable, bool bRenderWithViewModels, RenderableTranslucencyType_t nType, RenderableModelType_t nModelType, uint32_t nSplitscreenEnabled = 0xFFFFFFFF) = 0; // = RENDERABLE_MODEL_UNKNOWN_TYPE ) = 0;
	virtual void RemoveRenderable(ClientRenderHandle_t handle) = 0;
	virtual void AddRenderableToLeaves(ClientRenderHandle_t renderable, int nLeafCount, unsigned short* pLeaves) = 0;
	virtual void SetTranslucencyType(ClientRenderHandle_t handle, RenderableTranslucencyType_t nType) = 0;
	virtual void RenderInFastReflections(ClientRenderHandle_t handle, bool bRenderInFastReflections) = 0;
	virtual void DisableShadowDepthRendering(ClientRenderHandle_t handle, bool bDisable) = 0;
	virtual void DisableCSMRendering(ClientRenderHandle_t handle, bool bDisable) = 0;
};


class CBaseFlex;



#define CBaseAnimatingOverlay C_BaseAnimatingOverlay


class C_BaseAnimating
{

};



class C_BaseAnimatingOverlay : public C_BaseAnimating
{
#if 0
	//DECLARE_CLASS(C_BaseAnimatingOverlay, C_BaseAnimating);
	//DECLARE_CLIENTCLASS();
	//DECLARE_PREDICTABLE();
	//DECLARE_INTERPOLATION();

	// Inherited from C_BaseAnimating
public:
	virtual C_BaseAnimatingOverlay* GetBaseAnimatingOverlay() { return this; }

	virtual void	AccumulateLayers(IBoneSetup& boneSetup, BoneVector pos[], BoneQuaternion q[], float currentTime);
	virtual void	DoAnimationEvents(CStudioHdr* pStudioHdr);
	virtual void	GetRenderBounds(Vector& theMins, Vector& theMaxs);
	virtual CStudioHdr* OnNewModel();

	virtual bool	Interpolate(float flCurrentTime);

#if defined( _PS3 )
	virtual void AccumulateLayers_AddPoseCalls(IBoneSetup_PS3& boneSetup, BoneVector pos[], BoneQuaternion q[], float currentTime);
#endif


public:
	enum
	{
		MAX_OVERLAYS = 15,
	};

	C_BaseAnimatingOverlay();
	CAnimationLayer* GetAnimOverlay(int i, bool bUseOrder = true);
	void			SetNumAnimOverlays(int num);	// This makes sure there is space for this # of layers.
	int				GetNumAnimOverlays() const;
	void			SetOverlayPrevEventCycle(int nSlot, float flValue);

	void			CheckInterpChanges(void);
	void			CheckForLayerPhysicsInvalidate(void);

	virtual bool UpdateDispatchLayer(CAnimationLayer* pLayer, CStudioHdr* pWeaponStudioHdr, int iSequence);
	void AccumulateDispatchedLayers(C_BaseAnimatingOverlay* pWeapon, CStudioHdr* pWeaponStudioHdr, IBoneSetup& boneSetup, BoneVector pos[], BoneQuaternion q[], float currentTime);
	void RegenerateDispatchedLayers(IBoneSetup& boneSetup, BoneVector pos[], BoneQuaternion q[], float currentTime);

	void AccumulateInterleavedDispatchedLayers(C_BaseAnimatingOverlay* pWeapon, IBoneSetup& boneSetup, BoneVector pos[], BoneQuaternion q[], float currentTime, bool bSetupInvisibleWeapon = false);

	virtual void	NotifyOnLayerChangeSequence(const CAnimationLayer* pLayer, const int nNewSequence) {};
	virtual void	NotifyOnLayerChangeWeight(const CAnimationLayer* pLayer, const float flNewWeight) {};
	virtual void	NotifyOnLayerChangeCycle(const CAnimationLayer* pLayer, const float flNewCycle) {};

private:
	void CheckForLayerChanges(CStudioHdr* hdr, float currentTime);

	CUtlVector < CAnimationLayer >	m_AnimOverlay;
	CUtlVector < CInterpolatedVar< CAnimationLayer > >	m_iv_AnimOverlay;

	float m_flOverlayPrevEventCycle[MAX_OVERLAYS];

	C_BaseAnimatingOverlay(const C_BaseAnimatingOverlay&); // not defined, not accessible

	friend void ResizeAnimationLayerCallback(void* pStruct, int offsetToUtlVector, int len);
#endif
};

