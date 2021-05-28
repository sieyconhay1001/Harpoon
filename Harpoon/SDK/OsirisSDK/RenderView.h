#pragma once

#include "VirtualMethod.h"

class Entity;
class Model;
class IWorldRenderList;
struct VisOverrideData_t;
struct VisibleFogVolumeInfo_t;
struct WorldListInfo_t;
struct WorldListIndicesInfo_t;
class CVolumeCuller;
class IClientRenderable;
class IBrushRenderer;
struct DrawBrushModelMode_t;
struct BrushArrayInstanceData_t;

//typedef VPlane Frustum[FRUSTUM_NUMPLANES];

enum
{
	DRAWWORLDLISTS_DRAW_STRICTLYABOVEWATER = 0x001,
	DRAWWORLDLISTS_DRAW_STRICTLYUNDERWATER = 0x002,
	DRAWWORLDLISTS_DRAW_INTERSECTSWATER = 0x004,
	DRAWWORLDLISTS_DRAW_WATERSURFACE = 0x008,
	DRAWWORLDLISTS_DRAW_SKYBOX = 0x010,
	DRAWWORLDLISTS_DRAW_CLIPSKYBOX = 0x020,
	DRAWWORLDLISTS_DRAW_SHADOWDEPTH = 0x040,
	DRAWWORLDLISTS_DRAW_REFRACTION = 0x080,
	DRAWWORLDLISTS_DRAW_REFLECTION = 0x100,
	DRAWWORLDLISTS_DRAW_WORLD_GEOMETRY = 0x200,
	DRAWWORLDLISTS_DRAW_DECALS_AND_OVERLAYS = 0x400,
	DRAWWORLDLISTS_DRAW_SIMPLE_WORLD_MODEL = 0x800,
	DRAWWORLDLISTS_DRAW_SIMPLE_WORLD_MODEL_WATER = 0x1000,
	DRAWWORLDLISTS_DRAW_SKIP_DISPLACEMENTS = 0x2000,
	DRAWWORLDLISTS_DRAW_SSAO = 0x4000,
};

enum
{
	MAT_SORT_GROUP_STRICTLY_ABOVEWATER = 0,
	MAT_SORT_GROUP_STRICTLY_UNDERWATER,
	MAT_SORT_GROUP_INTERSECTS_WATER_SURFACE,
	MAT_SORT_GROUP_WATERSURFACE,

	MAX_MAT_SORT_GROUPS
};

enum ERenderDepthMode_t
{
	DEPTH_MODE_NORMAL = 0,
	DEPTH_MODE_SHADOW = 1,
	DEPTH_MODE_SSA0 = 2,

	DEPTH_MODE_MAX
};


static const char* s_pMatSortGroupsString[] =
{
	"Sort: Abovewater",
	"Sort: Underwater",
	"Sort: IntersectsWater",
	"Sort: WaterSurface",
};
typedef int SideType;

// Used to represent sides of things like planes.
#define	SIDE_FRONT	0
#define	SIDE_BACK	1
#define	SIDE_ON		2

#define VP_EPSILON	0.01f

class VPlane
{
public:
#if 0
	VPlane();
	VPlane(const Vector& vNormal, Vector dist);

	void		Init(const Vector& vNormal, Vector dist);

	// Return the distance from the point to the plane.
	Vector		DistTo(const Vector& vVec) const;

	// Copy.
	VPlane& operator=(const VPlane& thePlane);

	// Returns SIDE_ON, SIDE_FRONT, or SIDE_BACK.
	// The epsilon for SIDE_ON can be passed in.
	SideType	GetPointSide(const Vector& vPoint, Vector sideEpsilon) const;

	// Returns SIDE_FRONT or SIDE_BACK.
	SideType	GetPointSideExact(const Vector& vPoint) const;

	// Classify the box with respect to the plane.
	// Returns SIDE_ON, SIDE_FRONT, or SIDE_BACK
	SideType	BoxOnPlaneSide(const Vector& vMin, const Vector& vMax) const;

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// Flip the plane.
	VPlane		Flip();

	// Get a point on the plane (normal*dist).
	Vector		GetPointOnPlane() const;

	// Snap the specified point to the plane (along the plane's normal).
	Vector		SnapPointToPlane(const Vector& vPoint) const;
#endif
#endif
public:
	Vector		m_Normal;
	Vector		m_Dist;

#ifdef VECTOR_NO_SLOW_OPERATIONS
private:
	// No copy constructors allowed if we're in optimal mode
	VPlane(const VPlane& vOther);
#endif
};

enum
{
	FRUSTUM_RIGHT = 0,
	FRUSTUM_LEFT = 1,
	FRUSTUM_TOP = 2,
	FRUSTUM_BOTTOM = 3,
	FRUSTUM_NEARZ = 4,
	FRUSTUM_FARZ = 5,
	FRUSTUM_NUMPLANES = 6
};

typedef VPlane Frustum[FRUSTUM_NUMPLANES];

//-----------------------------------------------------------------------------
// Leaf index
//-----------------------------------------------------------------------------
typedef unsigned short LeafIndex_t;
enum
{
	INVALID_LEAF_INDEX = (LeafIndex_t)~0
};


//-----------------------------------------------------------------------------
// Describes the leaves to be rendered this view, set by BuildWorldLists

//-----------------------------------------------------------------------------
// NOTE: This is slightly slower on x360 but saves memory
#if 1
struct WorldListLeafData_t
{
	LeafIndex_t	leafIndex;	// 16 bits
	int16	waterData;
	uint16 	firstTranslucentSurface;	// engine-internal list index
	uint16	translucentSurfaceCount;	// count of translucent surfaces+disps
};
#else
struct WorldListLeafData_t
{
	uint32	leafIndex;
	int32	waterData;
	uint32	firstTranslucentSurface;	// engine-internal list index
	uint32	translucentSurfaceCount;	// count of translucent surfaces+disps
};
#endif
struct WorldListInfo_t
{
	int		m_ViewFogVolume;
	int		m_LeafCount;
	bool	m_bHasWater;
	WorldListLeafData_t* m_pLeafDataList;
};

class IWorldRenderList : public IRefCounted
{
};

//-----------------------------------------------------------------------------
// Describes the fog volume for a particular point
//-----------------------------------------------------------------------------
struct VisibleFogVolumeInfo_t
{
	int		m_nVisibleFogVolume;
	int		m_nVisibleFogVolumeLeaf;
	bool	m_bEyeInFogVolume;
	float	m_flDistanceToWater;
	float	m_flWaterHeight;
	IMaterial* m_pFogVolumeMaterial;
};


//-----------------------------------------------------------------------------
// Vertex format for brush models
//-----------------------------------------------------------------------------
struct BrushVertex_t
{
	Vector		m_Pos;
	Vector		m_Normal;
	Vector		m_TangentS;
	Vector		m_TangentT;
	Vector2D	m_TexCoord;
	Vector2D	m_LightmapCoord;

private:
	BrushVertex_t(const BrushVertex_t& src);
};

//-----------------------------------------------------------------------------
// Visibility data for area portal culling
//-----------------------------------------------------------------------------
struct VisOverrideData_t
{
	Vector		m_vecVisOrigin;					// The point to to use as the viewpoint for area portal backface cull checks.
	float		m_fDistToAreaPortalTolerance;	// The distance from an area portal before using the full screen as the viewable portion.

	// PORTAL2-specific
	Vector		m_vPortalCorners[4];			// When rendering a portal view, these are the 4 corners of the portal you are looking through (used to shrink the frustum)
	bool		m_bTrimFrustumToPortalCorners;

	Vector		m_vPortalOrigin;
	Vector		m_vPortalForward;
	float		m_flPortalRadius;

};


class RenderView {
public:
	virtual void			DrawBrushModel(Entity* baseentity,Model* model,const Vector& origin,const Vector& angles,bool bUnused) = 0;
	virtual void			DrawIdentityBrushModel(IWorldRenderList* pList, Model* model) = 0;
	virtual void			TouchLight(struct dlight_t* light) = 0;
	virtual void			Draw3DDebugOverlays(void) = 0;
	virtual void			SetBlend(float blend) = 0;
	virtual float			GetBlend(void) = 0;
	virtual void			SetColorModulation(float const* blend) = 0;
	virtual void			GetColorModulation(float* blend) = 0;
	virtual void			SceneBegin(void) = 0;
	virtual void			SceneEnd(void) = 0;
	virtual void			GetVisibleFogVolume(const Vector& eyePoint, const VisOverrideData_t* pVisOverrideData, VisibleFogVolumeInfo_t* pInfo) = 0;
	virtual	IWorldRenderList* CreateWorldList() = 0;
	virtual void			BuildWorldLists_Epilogue(IWorldRenderList* pList, WorldListInfo_t* pInfo, bool bShadowDepth) = 0;
	virtual void			BuildWorldLists(IWorldRenderList* pList, WorldListInfo_t* pInfo, int iForceFViewLeaf, const VisOverrideData_t* pVisData = NULL, bool bShadowDepth = false, float* pReflectionWaterHeight = NULL) = 0;
	virtual void			DrawWorldLists(IMatRenderContext* pRenderContext, IWorldRenderList* pList, unsigned long flags, float waterZAdjust) = 0;
	virtual void			GetWorldListIndicesInfo(WorldListIndicesInfo_t* pIndicesInfoOut, IWorldRenderList* pList, unsigned long nFlags) = 0;
	// Optimization for top view
	virtual void			DrawTopView(bool enable) = 0;
	virtual void			TopViewNoBackfaceCulling(bool bDisable) = 0;
	virtual void			TopViewNoVisCheck(bool bDisable) = 0;
	virtual void			TopViewBounds(Vector2D const& mins, Vector2D const& maxs) = 0;
	virtual void			SetTopViewVolumeCuller(const CVolumeCuller* pVolumeCuller) = 0;
	virtual void			DrawLights(void) = 0;
	virtual void			DrawMaskEntities(void) = 0;
	virtual void			DrawTranslucentSurfaces(IMatRenderContext* pRenderContext, IWorldRenderList* pList, int* pSortList, int sortCount, unsigned long flags) = 0;
	virtual void			DrawLineFile(void) = 0;
	virtual void			DrawLightmaps(IWorldRenderList* pList, int pageId) = 0;
	virtual void			ViewSetupVis(bool novis, int numorigins, const Vector origin[]) = 0;
	virtual bool			AreAnyLeavesVisible(int* leafList, int nLeaves) = 0;
	virtual	void			VguiPaint(void) = 0;
	virtual void			ViewDrawFade(byte* color, IMaterial* pMaterial, bool mapFullTextureToScreen = true) = 0;
	virtual void			OLD_SetProjectionMatrix(float fov, float zNear, float zFar) = 0;
	virtual Vector		    GetLightAtPoint(Vector& pos) = 0;
	virtual int				GetViewEntity(void) = 0;
	virtual bool			IsViewEntity(int entindex) = 0;
	virtual float			GetFieldOfView(void) = 0;
	virtual unsigned char** GetAreaBits(void) = 0;
	virtual void			SetFogVolumeState(int nVisibleFogVolume, bool bUseHeightFog) = 0;
	virtual void			InstallBrushSurfaceRenderer(IBrushRenderer* pBrushRenderer) = 0;
	virtual void			DrawBrushModelShadow(IClientRenderable* pRenderable) = 0;
	virtual void Render2DEffectsPostHUD(void* view) = 0;
	//virtual	bool			LeafContainsTranslucentSurfaces(IWorldRenderList* pList, int sortIndex, unsigned long flags) = 0;
	virtual bool			DoesBoxIntersectWaterVolume(const Vector& mins, const Vector& maxs, int leafWaterDataID) = 0;
	virtual void			SetAreaState(unsigned char* chAreaBits,unsigned char* chAreaPortalBits) = 0;
	virtual void			VGui_Paint(int mode) = 0;
	virtual void			Push3DView(/*IMatRenderContext* pRenderContext, const CViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes*/) = 0;
	virtual void			Push2DView(/*IMatRenderContext* pRenderContext, const CViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes*/) = 0;
	virtual void			PopView(/*IMatRenderContext* pRenderContext, Frustum frustumPlanes*/) = 0;
	virtual void			SetMainView(const Vector& vecOrigin, const Vector& angles) = 0;
	virtual void			ViewSetupVisEx(bool novis, int numorigins, const Vector origin[], unsigned int& returnFlags) = 0;
	virtual void			OverrideViewFrustum(Frustum custom) = 0;
	virtual void			DrawBrushModelShadowDepth(Entity* baseentity, Model* model, const Vector& origin, const Vector& angles, ERenderDepthMode_t DepthMode) = 0;
	virtual void			UpdateBrushModelLightmap(Model* model, IClientRenderable* pRenderable) = 0;
	virtual void			BeginUpdateLightmaps(void) = 0;
	virtual void			EndUpdateLightmaps() = 0;
	virtual void			OLD_SetOffCenterProjectionMatrix(float fov, float zNear, float zFar, float flAspectRatio, float flBottom, float flTop, float flLeft, float flRight) = 0;
	virtual void			OLD_SetProjectionMatrixOrtho(float left, float top, float right, float bottom, float zNear, float zFar) = 0;
	virtual void			Push3DView(IMatRenderContext* pRenderContext, const CViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes, ITexture* pDepthTexture) = 0;
	virtual void			GetMatricesForView(const CViewSetup& view, VMatrix* pWorldToView, VMatrix* pViewToProjection, VMatrix* pWorldToProjection, VMatrix* pWorldToPixels) = 0;
	virtual void			DrawBrushModelEx(Entity* baseentity, Model* model, const Vector& origin, const Vector& angles, DrawBrushModelMode_t mode) = 0;
	virtual bool			DoesBrushModelNeedPowerOf2Framebuffer(const Model* model) = 0;
	virtual void			DrawBrushModelArray(IMatRenderContext* pContext, int nCount, const BrushArrayInstanceData_t* pInstanceData, int nModelTypeFlags) = 0;

    VIRTUAL_METHOD(void, setBlend, 4, (float alpha), (this, alpha))
    VIRTUAL_METHOD(void, setColorModulation, 6, (const float* colors), (this, colors))

    constexpr void setColorModulation(float r, float g, float b) noexcept
    {
        float color[3]{ r, g, b };
        setColorModulation(color);
    }
};
