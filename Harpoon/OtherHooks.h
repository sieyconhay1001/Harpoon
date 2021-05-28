#pragma once


#include "SDK/OsirisSDK/ModelRender.h"
#include "SDK/OsirisSDK/matrix3x4.h"
#include "SDK/OsirisSDK/Vector.h"
#include "SDK/OsirisSDK/Entity.h"

namespace PanelHooks {
	void __stdcall paintTraverse(unsigned int panel, bool forceRepaint, bool allowForce) noexcept;
	
	//paintTraverse
}

namespace SvHooks {
	bool __fastcall svCheatsGetBool(void* _this) noexcept;
}

namespace modelRenderHooks {
	void __stdcall drawModelExecute(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept;
	
}

namespace bspQueryHooks {
	struct RenderableInfo {
		Entity* renderable;
		//std::byte pad[18];
		void* alpha_property;
		int enum_count;
		int render_frame;
		unsigned short first_shadow;
		unsigned short leaf_list;
		short area;
		uint16_t flags;
		uint16_t flags2;
		Vector bloated_abs_mins;
		Vector bloated_abs_maxs;
		Vector abs_mins;
		Vector abs_maxs;
		int pad;
	};



	int __stdcall listLeavesInBox(const Vector& mins, const Vector& maxs, unsigned short* list, int listMax) noexcept;
}

namespace SurfaceHooks {
	void __stdcall setDrawColor(int r, int g, int b, int a) noexcept;
}
struct DrawModelInfo_;
struct DrawModelResults_t;
namespace studioRenderHooks{
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






#if 0
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
#endif

#if 0
	struct DrawModelResults_t
	{
		int m_ActualTriCount;
		int m_TextureMemoryBytes;
		int m_NumHardwareBones;
		int m_NumBatches;
		int m_NumMaterials;
		int m_nLODUsed;
		int m_flLODMetric;
		//CFastTimer m_RenderTime;
		//CUtlVectorFixed<IMaterial*, MAX_DRAW_MODEL_INFO_MATERIALS> m_Materials;
	};
#endif
	void __fastcall DrawModel(void* _this, void* edx, DrawModelResults_t* results, const DrawModelInfo_t* DrawModelInfo, matrix3x4* boneToWorld, float* flexWeights, float* flexDelayedWeights, const Vector& ModelOrigin, int flags) noexcept;
	void __fastcall drawModelStaticProp(void* _this, void* edx, const DrawModelInfo_t&, const matrix3x4* modelToWorld, int flags);
	void __fastcall drawModelArrayStaticProp(void* _this, void* edx, const DrawModelInfo_t& drawInfo, int nInstanceCount, const void* pInstanceData, void** pColorMeshes) noexcept;

}
class Material;

namespace materialSystemHooks {
	Material* __fastcall findMaterial(void* _this, void* edx, const char* MaterialName, const char* TextureGroupName = nullptr, bool complain = true, const char* ComplainPrefix = NULL);
	void __fastcall reloadMaterials(void* _this, void* edx, const char* pSubString = NULL) noexcept;
}

class CViewSetup;

namespace viewRenderHooks {

	struct WorldListInfo_t
	{
		int		m_ViewFogVolume;
		int		m_LeafCount;
		//LeafIndex_t* m_pLeafList;
		//LeafFogVolume_t* m_pLeafFogVolume;
	};

	class IWorldRenderList
	{
	};

	void __fastcall RenderSmokeOverlay(CViewSetup* pViewSetup, void* ecx, bool bPreViewModel);

	void __fastcall drawWorldLists(IWorldRenderList* pList, unsigned long flags, float waterZAdjust) noexcept;
	bool __fastcall DrawViewModels(void* ecx, void* edx, const CViewSetup* view, bool drawViewmodel, double a5, DWORD* a6, char a7, char a8);
	void __fastcall RenderView(void* ecx, void*, const CViewSetup& view, const CViewSetup& hudview, int nClearFlags, int whatToDraw);
	int __fastcall UnknownViewRenderFunction(void* _this, void* edx, char a2);
	int __fastcall UnknownViewRenderFunction2(void* _this, void* edx, int a2);

	void __fastcall PerformScreenOverlay(void*, void*, int x, int y, int w, int h);
	void __fastcall DisableFog(void*, void*);
	void __fastcall UpdateToneMapScalar(void*, void*);

	int __fastcall DrawUnderWaterOverlay(void* _this);
}
namespace RenderViewHooks {
	void __fastcall SceneEnd(void* _this, void* edx);
	void __fastcall DrawLights(void* _this, void* edx);
	void __fastcall Render2DEffectsPostHUD(void* _this, void*, const CViewSetup& view);
}

namespace fileSystemHooks {
	bool __fastcall allowLooseFiles(void* ecx, void* edx) noexcept;
	int __fastcall canLoadThirdPartyFiles(void* _this) noexcept;

	int __stdcall getUnveridiedFileHashes(void* _this, void* other, int MaxFiles) noexcept;
}