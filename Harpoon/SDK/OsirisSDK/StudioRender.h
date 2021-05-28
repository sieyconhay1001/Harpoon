#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

#include "Material.h"
#include "VirtualMethod.h"
#include "matrix3x4.h"

class StudioHdr;
struct ShaderStencilState_t {
	bool m_bEnable;
	int m_FailOp;
	int m_ZFailOp;
	int m_PassOp;
	int m_CompareFunc;
	int m_nReferenceValue;
	uint32_t m_nTestMask;
	uint32_t m_nWriteMask;
};


struct StudioRenderConfig_t
{
	float fEyeShiftX;	// eye X position
	float fEyeShiftY;	// eye Y position
	float fEyeShiftZ;	// eye Z position
	float fEyeSize;		// adjustment to iris textures
	float fEyeGlintPixelWidthLODThreshold;

	int maxDecalsPerModel;
	int drawEntities;
	int skin;
	int fullbright;

	bool bEyeMove : 1;		// look around
	bool bSoftwareSkin : 1;
	bool bNoHardware : 1;
	bool bNoSoftware : 1;
	bool bTeeth : 1;
	bool bEyes : 1;
	bool bFlex : 1;
	bool bWireframe : 1;
	bool bDrawNormals : 1;
	bool bDrawTangentFrame : 1;
	bool bDrawZBufferedWireframe : 1;
	bool bSoftwareLighting : 1;
	bool bShowEnvCubemapOnly : 1;
	bool bWireframeDecals : 1;

	// Reserved for future use
	int m_nReserved[4];
};

struct DrawModelResults_t
{
	int m_ActualTriCount;
	int m_TextureMemoryBytes;
	int m_NumHardwareBones;
	int m_NumBatches;
	int m_NumMaterials;
	int m_nLODUsed;
	int m_flLODMetric;
};


struct ColorMeshInfo_t
{
	// A given color mesh can own a unique Mesh, or it can use a shared Mesh
	// (in which case it uses a sub-range defined by m_nVertOffset and m_nNumVerts)
	void * m_pMesh;
	void* m_pPooledVBAllocator;
	int						m_nVertOffsetInBytes;
	int						m_nNumVerts;
};

//#pragma pack(push, 0)
struct DrawModelInfo_t
{
	StudioHdr* m_pStudioHdr;
	void* m_pHardwareData;
	void* m_Decals;
	int				m_Skin;
	int				m_Body;
	int				m_HitboxSet;
	void* m_pClientEntity;
	int				m_Lod;
	ColorMeshInfo_t* m_pColorMeshes;
	bool			m_bStaticLighting;
	uint32_t	m_LightingState;
};
//#pragma pack(pop)

enum
{
	// This is because we store which flashlights are on which model 
	// in a 32-bit field (see ModelArrayInstanceData_t::m_nFlashlightUsage)
	MAX_FLASHLIGHTS_PER_INSTANCE_DRAW_CALL = 32
};



struct StudioModelArrayInfo2_t
{
	int						m_nFlashlightCount;
	void* m_pFlashlights;	// NOTE: Can have at most MAX_FLASHLIGHTS_PER_INSTANCE_DRAW_CALL of these
};

struct StudioModelArrayInfo_t : public StudioModelArrayInfo2_t
{
	StudioHdr* m_pStudioHdr;
	void* m_pHardwareData;
};

struct StudioArrayData_t
{
	StudioHdr* m_pStudioHdr;
	void* m_pHardwareData;
	void* m_pInstanceData; // See StudioShadowArrayInstanceData_t or StudioArrayInstanceData_t
	int						m_nCount;
};

struct StudioShadowArrayInstanceData_t
{
	int m_nLOD;
	int m_nBody;
	int m_nSkin;
	matrix3x4* m_pPoseToWorld;
	float* m_pFlexWeights;
	float* m_pDelayedFlexWeights;
};

struct StudioArrayInstanceData_t : public StudioShadowArrayInstanceData_t
{
	void* m_pLightingState;
	void* m_pDecalLightingState;
	void* m_pEnvCubemapTexture;
	void* m_Decals;
	uint32_t m_nFlashlightUsage;	// Mask indicating which flashlights to use.
	ShaderStencilState_t* m_pStencilState;
	ColorMeshInfo_t* m_pColorMeshInfo;
	//Vector4D m_DiffuseModulation;
};

struct GetTriangles_Vertex_t
{
	Vector m_Position;
	Vector m_Normal;
	/*
	Vector4D m_TangentS;
	Vector2D m_TexCoord;
	Vector4D m_BoneWeight;
	int m_BoneIndex[4];
	int m_NumBones;

	IMPLEMENT_OPERATOR_EQUAL(GetTriangles_Vertex_t);
	*/
};

struct GetTriangles_MaterialBatch_t
{
	Material* m_pMaterial;
	//CUtlVector<GetTriangles_Vertex_t> m_Verts;
	//CUtlVector<int> m_TriListIndices;
};

struct GetTriangles_Output_t
{
	//CUtlVector<GetTriangles_MaterialBatch_t> m_MaterialBatches;
	matrix3x4 m_PoseToWorld[256];

	//DISALLOW_OPERATOR_EQUAL(GetTriangles_Output_t);
};

enum OverrideType_t
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
};

enum
{
	ADDDECAL_TO_ALL_LODS = -1
};


//-----------------------------------------------------------------------------
// DrawModel flags
//-----------------------------------------------------------------------------
enum
{
	STUDIORENDER_DRAW_ENTIRE_MODEL = 0,
	STUDIORENDER_DRAW_OPAQUE_ONLY = 0x01,
	STUDIORENDER_DRAW_TRANSLUCENT_ONLY = 0x02,
	STUDIORENDER_DRAW_GROUP_MASK = 0x03,

	STUDIORENDER_DRAW_NO_FLEXES = 0x04,
	STUDIORENDER_DRAW_STATIC_LIGHTING = 0x08,

	STUDIORENDER_DRAW_ACCURATETIME = 0x10,		// Use accurate timing when drawing the model.
	STUDIORENDER_DRAW_NO_SHADOWS = 0x20,
	STUDIORENDER_DRAW_GET_PERF_STATS = 0x40,

	STUDIORENDER_DRAW_WIREFRAME = 0x80,

	STUDIORENDER_DRAW_ITEM_BLINK = 0x100,

	STUDIORENDER_SHADOWDEPTHTEXTURE = 0x200,

	STUDIORENDER_NO_SKIN = 0x400,

	STUDIORENDER_SKIP_DECALS = 0x800,
};


enum class OverrideType {
    Normal = 0,
    BuildShadows,
    DepthWrite,
    CustomMaterial, // weapon skins
    SsaoDepthWrite
};

class StudioRender {
    std::byte pad_0[0x250];
    Material* materialOverride;
    std::byte pad_1[0xC];
    OverrideType overrideType;
public:
    VIRTUAL_METHOD(void, forcedMaterialOverride, 33, (Material* material, OverrideType type = OverrideType::Normal, int index = -1), (this, material, type, index))
	VIRTUAL_METHOD(void, drawModelShadowArray, 48, (const StudioModelArrayInfo_t& drawInfo, int nCount, StudioArrayInstanceData_t* pInstanceData, int nInstanceStride, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL), (this, drawInfo, nCount, pInstanceData, nInstanceStride, flags))
    VIRTUAL_METHOD(void, drawModelArray, 49, (StudioModelArrayInfo2_t& info, int nCount, StudioArrayData_t* pArrayData,int nInstanceStride, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL),(this,info, nCount, pArrayData, nInstanceStride, flags))
		//VIRTUAL_METHOD(void, DrawModel, 29, (void* edx, void* results, void* modelinf, matrix3x4* bone_to_world, float* flex_weights, float* flex_delayed_weights, const Vector& model_origin, int flags), (this, edx, results, modelinf, bone_to_world ))
    bool isForcedMaterialOverride() noexcept
    {
        if (!materialOverride)
            return overrideType == OverrideType::DepthWrite || overrideType == OverrideType::SsaoDepthWrite; // see CStudioRenderContext::IsForcedMaterialOverride
        return std::string_view{ materialOverride->getName() }.starts_with("dev/glow");
    }
};



