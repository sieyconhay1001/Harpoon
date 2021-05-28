#pragma once
#include <cstdint>
#include "../OsirisSDK/matrix3x4.h"
#include "../OsirisSDK/StudioRender.h"
#include "CMatRenderData.h"
class IMaterialProxyFactory;
class ITexture;
class IMaterialSystemHardwareConfig;
class CShadowMgr;
#define AssertMsg(VAR) /**/


#include "../OsirisSDK/Material.h"
#include "../OsirisSDK/matrix3x4.h"
#include "../OsirisSDK/MaterialSystem.h"

typedef Material IMaterial;
typedef matrix3x4 matrix3x4_t;
typedef MaterialSystem IMaterialSystem;

#include <functional>
namespace ModelRenderSystem {

	struct RenderableInstance_t
	{
		uint8_t m_nAlpha;
	};



	struct ModelRenderSystemData_t
	{
		void* m_pRenderable;
		void* m_pModelRenderable;
		RenderableInstance_t m_InstanceData;
	};

	struct TranslucentInstanceRenderData_t
	{
		StudioModelArrayInfo_t* m_pModelInfo;
		StudioArrayInstanceData_t* m_pInstanceData;
	};

	struct TranslucentTempData_t
	{
		int m_nColorMeshHandleCount;
		void* m_pColorMeshHandles;
		bool m_bReleaseRenderData;
	};

	enum ModelRenderMode_t
	{
		MODEL_RENDER_MODE_NORMAL,
		MODEL_RENDER_MODE_SHADOW_DEPTH,
		MODEL_RENDER_MODE_RTT_SHADOWS
	};

	struct ModelListNode_t
	{
		ModelRenderSystemData_t m_Entry;
		int32_t m_nInitialListIndex : 24;
		int32_t m_bBoneMerge : 1;
		int32_t m_nLOD : 7;
		void* m_pStencilState;
		ModelListNode_t* m_pNext;
	};

	struct RenderModelInfo_t : public StudioArrayInstanceData_t
	{
		ModelRenderSystemData_t m_Entry;
		void* m_hInstance;
		matrix3x4* m_pBoneToWorld;
		uint32_t m_nInitialListIndex : 24;
		int32_t m_bSetupBonesOnly : 1;
		int32_t m_bBoneMerge : 1;
	};

	struct ModelListByType_t : public StudioModelArrayInfo_t
	{
		void* m_nLightingModel;
		const void* m_pModel;
		ModelListNode_t* m_pFirstNode;
		int m_nCount;
		int m_nSetupBoneCount;
		uint32_t m_nParentDepth : 31;
		uint32_t m_bWantsStencil : 1;
		RenderModelInfo_t* m_pRenderModels;
		ModelListByType_t* m_pNextLightingModel;

		// speed up std::sort by implementing these
		//ModelListByType_t& operator=(const ModelListByType_t& rhs)
		//{
		//	memcpy(this, &rhs, sizeof(ModelListByType_t));
		//	return *this;
		//}

		//ModelListByType_t() {}

		//ModelListByType_t(const ModelListByType_t& rhs)
		//{
		//	memcpy(this, &rhs, sizeof(ModelListByType_t));
		//}
	};

	struct LightingList_t
	{
		ModelListByType_t* m_pFirstModel;
		int m_nCount;
		int m_nTotalModelCount;
	};
	/*
	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/194545-basic-props-world-model-engine-chams.html
    // Yeah I lifted it ....
	class ModelRenderSystemArray
	{
		std::function<void(ModelRenderSystemData_t*, size_t)> _drawModels;
		ModelRenderSystemData_t   _data[1000];
		size_t                  _count = 0;

	public:

		ModelRenderSystemArray(void);
		explicit ModelRenderSystemArray(const std::function<void(ModelRenderSystemData_t*, size_t)>& drawModels);

		void                    Add(const ModelRenderSystemData_t& data);

		void                    Render(void) const;
		void                    UpdateColor(const std::function<Color(Entity* entity)>& getColorForEntity);
	};





	class DrawModelsParam
	{
		ModelRenderSystemArray    _arrays[3];
		ModelRenderSystemData_t* _systemDataArray = nullptr;
		size_t                          _count = 0;

	public:

		DrawModelsParam(const std::function<void(ModelRenderSystemData_t*, size_t)>& drawModel, ModelRenderSystemData_t* systemDataArray, size_t count);

		bool        BuildArrays(void);
		void        RenderArray(int arrayType, const std::function<Color(Entity* entity)>& getColorForEntity);
	};
*/
	
	



};
enum LightType_t
{
    MATERIAL_LIGHT_DISABLE = 0,
    MATERIAL_LIGHT_POINT,
    MATERIAL_LIGHT_DIRECTIONAL,
    MATERIAL_LIGHT_SPOT,
};

enum LightType_OptimizationFlags_t
{
    LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION0 = 1,
    LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION1 = 2,
    LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION2 = 4,
};


struct LightDesc_t
{
    LightType_t		m_Type;
    Vector			m_Color;
    Vector	m_Position;
    Vector  m_Direction;
    float   m_Range;
    float   m_Falloff;
    float   m_Attenuation0;
    float   m_Attenuation1;
    float   m_Attenuation2;
    float   m_Theta;
    float   m_Phi;
    // These aren't used by DX8. . used for software lighting.
    float	m_ThetaDot;
    float	m_PhiDot;
    unsigned int	m_Flags;


    LightDesc_t() {}

private:
    // No copy constructors allowed
    LightDesc_t(const LightDesc_t& vOther);
};

enum MorphFormatFlags_t
{
    MORPH_POSITION = 0x0001,	// 3D
    MORPH_NORMAL = 0x0002,	// 3D
    MORPH_WRINKLE = 0x0004,	// 1D
    MORPH_SPEED = 0x0008,	// 1D
    MORPH_SIDE = 0x0010,	// 1D
};


//-----------------------------------------------------------------------------
// The morph format type
//-----------------------------------------------------------------------------
typedef unsigned int MorphFormat_t;


//-----------------------------------------------------------------------------
// Standard lightmaps
//-----------------------------------------------------------------------------
enum StandardLightmap_t
{
    MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE = -1,
    MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE_BUMP = -2,
    MATERIAL_SYSTEM_LIGHTMAP_PAGE_USER_DEFINED = -3
};


struct MaterialSystem_SortInfo_t
{
    IMaterial* material;
    int			lightmapPageID;
};


#define MAX_FB_TEXTURES 4

//-----------------------------------------------------------------------------
// Information about each adapter
//-----------------------------------------------------------------------------
enum
{
    MATERIAL_ADAPTER_NAME_LENGTH = 512
};

struct MaterialAdapterInfo_t
{
    char m_pDriverName[MATERIAL_ADAPTER_NAME_LENGTH];
    unsigned int m_VendorID;
    unsigned int m_DeviceID;
    unsigned int m_SubSysID;
    unsigned int m_Revision;
    int m_nDXSupportLevel;			// This is the *preferred* dx support level
    int m_nMaxDXSupportLevel;
    unsigned int m_nDriverVersionHigh;
    unsigned int m_nDriverVersionLow;
};


//-----------------------------------------------------------------------------
// Video mode info..
//-----------------------------------------------------------------------------
#if 0
struct MaterialVideoMode_t
{
    int m_Width;			// if width and height are 0 and you select 
    int m_Height;			// windowed mode, it'll use the window size
    ImageFormat m_Format;	// use ImageFormats (ignored for windowed mode)
    int m_RefreshRate;		// 0 == default (ignored for windowed mode)
};
#endif
enum ShaderParamType_t
{
	SHADER_PARAM_TYPE_TEXTURE,
	SHADER_PARAM_TYPE_INTEGER,
	SHADER_PARAM_TYPE_COLOR,
	SHADER_PARAM_TYPE_VEC2,
	SHADER_PARAM_TYPE_VEC3,
	SHADER_PARAM_TYPE_VEC4,
	SHADER_PARAM_TYPE_ENVMAP,	// obsolete
	SHADER_PARAM_TYPE_FLOAT,
	SHADER_PARAM_TYPE_BOOL,
	SHADER_PARAM_TYPE_FOURCC,
	SHADER_PARAM_TYPE_MATRIX,
	SHADER_PARAM_TYPE_MATERIAL,
	SHADER_PARAM_TYPE_STRING,
	SHADER_PARAM_TYPE_MATRIX4X2
};

enum MaterialMatrixMode_t
{
	MATERIAL_VIEW = 0,
	MATERIAL_PROJECTION,

	// Texture matrices
	MATERIAL_TEXTURE0,
	MATERIAL_TEXTURE1,
	MATERIAL_TEXTURE2,
	MATERIAL_TEXTURE3,
	MATERIAL_TEXTURE4,
	MATERIAL_TEXTURE5,
	MATERIAL_TEXTURE6,
	MATERIAL_TEXTURE7,

	MATERIAL_MODEL,

	// Total number of matrices
	NUM_MATRIX_MODES = MATERIAL_MODEL + 1,

	// Number of texture transforms
	NUM_TEXTURE_TRANSFORMS = MATERIAL_TEXTURE7 - MATERIAL_TEXTURE0 + 1
};

// FIXME: How do I specify the actual number of matrix modes?
const int NUM_MODEL_TRANSFORMS = 53;
const int MATERIAL_MODEL_MAX = MATERIAL_MODEL + NUM_MODEL_TRANSFORMS;

enum MaterialPrimitiveType_t
{
	MATERIAL_POINTS = 0x0,
	MATERIAL_LINES,
	MATERIAL_TRIANGLES,
	MATERIAL_TRIANGLE_STRIP,
	MATERIAL_LINE_STRIP,
	MATERIAL_LINE_LOOP,	// a single line loop
	MATERIAL_POLYGON,	// this is a *single* polygon
	MATERIAL_QUADS,
	MATERIAL_INSTANCED_QUADS, // (X360) like MATERIAL_QUADS, but uses vertex instancing

	// This is used for static meshes that contain multiple types of
	// primitive types.	When calling draw, you'll need to specify
	// a primitive type.
	MATERIAL_HETEROGENOUS
};

enum MaterialPropertyTypes_t
{
	MATERIAL_PROPERTY_NEEDS_LIGHTMAP = 0,					// bool
	MATERIAL_PROPERTY_OPACITY,								// int (enum MaterialPropertyOpacityTypes_t)
	MATERIAL_PROPERTY_REFLECTIVITY,							// vec3_t
	MATERIAL_PROPERTY_NEEDS_BUMPED_LIGHTMAPS				// bool
};

// acceptable property values for MATERIAL_PROPERTY_OPACITY
enum MaterialPropertyOpacityTypes_t
{
	MATERIAL_ALPHATEST = 0,
	MATERIAL_OPAQUE,
	MATERIAL_TRANSLUCENT
};

enum MaterialBufferTypes_t
{
	MATERIAL_FRONT = 0,
	MATERIAL_BACK
};

enum MaterialCullMode_t
{
	MATERIAL_CULLMODE_CCW,	// this culls polygons with counterclockwise winding
	MATERIAL_CULLMODE_CW	// this culls polygons with clockwise winding
};

enum MaterialIndexFormat_t
{
	MATERIAL_INDEX_FORMAT_UNKNOWN = -1,
	MATERIAL_INDEX_FORMAT_16BIT = 0,
	MATERIAL_INDEX_FORMAT_32BIT,
};

enum MaterialFogMode_t
{
	MATERIAL_FOG_NONE,
	MATERIAL_FOG_LINEAR,
	MATERIAL_FOG_LINEAR_BELOW_FOG_Z,
};

enum MaterialHeightClipMode_t
{
	MATERIAL_HEIGHTCLIPMODE_DISABLE,
	MATERIAL_HEIGHTCLIPMODE_RENDER_ABOVE_HEIGHT,
	MATERIAL_HEIGHTCLIPMODE_RENDER_BELOW_HEIGHT
};

enum MaterialNonInteractiveMode_t
{
	MATERIAL_NON_INTERACTIVE_MODE_NONE = -1,
	MATERIAL_NON_INTERACTIVE_MODE_STARTUP = 0,
	MATERIAL_NON_INTERACTIVE_MODE_LEVEL_LOAD,

	MATERIAL_NON_INTERACTIVE_MODE_COUNT,
};


#if 1


enum ImageFormat
{
	IMAGE_FORMAT_UNKNOWN = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F,			// Single-channel 32-bit floating point
	IMAGE_FORMAT_RGB323232F,
	IMAGE_FORMAT_RGBA32323232F,

	// Depth-stencil texture formats for shadow depth mapping
	IMAGE_FORMAT_NV_DST16,		// 
	IMAGE_FORMAT_NV_DST24,		//
	IMAGE_FORMAT_NV_INTZ,		// Vendor-specific depth-stencil texture
	IMAGE_FORMAT_NV_RAWZ,		// formats for shadow depth mapping 
	IMAGE_FORMAT_ATI_DST16,		// 
	IMAGE_FORMAT_ATI_DST24,		//
	IMAGE_FORMAT_NV_NULL,		// Dummy format which takes no video memory

	// Compressed normal map formats
	IMAGE_FORMAT_ATI2N,			// One-surface ATI2N / DXN format
	IMAGE_FORMAT_ATI1N,			// Two-surface ATI1N format

#if defined( _X360 )
	// Depth-stencil texture formats
	IMAGE_FORMAT_X360_DST16,
	IMAGE_FORMAT_X360_DST24,
	IMAGE_FORMAT_X360_DST24F,
	// supporting these specific formats as non-tiled for procedural cpu access
	IMAGE_FORMAT_LINEAR_BGRX8888,
	IMAGE_FORMAT_LINEAR_RGBA8888,
	IMAGE_FORMAT_LINEAR_ABGR8888,
	IMAGE_FORMAT_LINEAR_ARGB8888,
	IMAGE_FORMAT_LINEAR_BGRA8888,
	IMAGE_FORMAT_LINEAR_RGB888,
	IMAGE_FORMAT_LINEAR_BGR888,
	IMAGE_FORMAT_LINEAR_BGRX5551,
	IMAGE_FORMAT_LINEAR_I8,
	IMAGE_FORMAT_LINEAR_RGBA16161616,

	IMAGE_FORMAT_LE_BGRX8888,
	IMAGE_FORMAT_LE_BGRA8888,
#endif

	IMAGE_FORMAT_DXT1_RUNTIME,
	IMAGE_FORMAT_DXT5_RUNTIME,

	NUM_IMAGE_FORMATS
};

#if defined( POSIX  ) || defined( DX_TO_GL_ABSTRACTION )
typedef enum _D3DFORMAT
{
	D3DFMT_INDEX16,
	D3DFMT_D16,
	D3DFMT_D24S8,
	D3DFMT_A8R8G8B8,
	D3DFMT_A4R4G4B4,
	D3DFMT_X8R8G8B8,
	D3DFMT_R5G6R5,
	D3DFMT_X1R5G5B5,
	D3DFMT_A1R5G5B5,
	D3DFMT_L8,
	D3DFMT_A8L8,
	D3DFMT_A,
	D3DFMT_DXT1,
	D3DFMT_DXT3,
	D3DFMT_DXT5,
	D3DFMT_V8U8,
	D3DFMT_Q8W8V8U8,
	D3DFMT_X8L8V8U8,
	D3DFMT_A16B16G16R16F,
	D3DFMT_A16B16G16R16,
	D3DFMT_R32F,
	D3DFMT_A32B32G32R32F,
	D3DFMT_R8G8B8,
	D3DFMT_D24X4S4,
	D3DFMT_A8,
	D3DFMT_R5G6B5,
	D3DFMT_D15S1,
	D3DFMT_D24X8,
	D3DFMT_VERTEXDATA,
	D3DFMT_INDEX32,

	// adding fake D3D format names for the vendor specific ones (eases debugging/logging)

	// NV shadow depth tex
	D3DFMT_NV_INTZ = 0x5a544e49,	// MAKEFOURCC('I','N','T','Z')
	D3DFMT_NV_RAWZ = 0x5a574152,	// MAKEFOURCC('R','A','W','Z')

	// NV null tex
	D3DFMT_NV_NULL = 0x4c4c554e,	// MAKEFOURCC('N','U','L','L')

	// ATI shadow depth tex
	D3DFMT_ATI_D16 = 0x36314644,	// MAKEFOURCC('D','F','1','6')
	D3DFMT_ATI_D24S8 = 0x34324644,	// MAKEFOURCC('D','F','2','4')

	// ATI 1N and 2N compressed tex
	D3DFMT_ATI_2N = 0x32495441,	// MAKEFOURCC('A', 'T', 'I', '2')
	D3DFMT_ATI_1N = 0x31495441,	// MAKEFOURCC('A', 'T', 'I', '1')

	D3DFMT_UNKNOWN
} D3DFORMAT;
#endif

//-----------------------------------------------------------------------------
// Color structures
//-----------------------------------------------------------------------------

struct BGRA8888_t
{
	unsigned char b;		// change the order of names to change the 
	unsigned char g;		//  order of the output ARGB or BGRA, etc...
	unsigned char r;		//  Last one is MSB, 1st is LSB.
	unsigned char a;
	inline BGRA8888_t& operator=(const BGRA8888_t& in)
	{
		*(unsigned int*)this = *(unsigned int*)&in;
		return *this;
	}
};

struct RGBA8888_t
{
	unsigned char r;		// change the order of names to change the 
	unsigned char g;		//  order of the output ARGB or BGRA, etc...
	unsigned char b;		//  Last one is MSB, 1st is LSB.
	unsigned char a;
	inline RGBA8888_t& operator=(const BGRA8888_t& in)
	{
		r = in.r;
		g = in.g;
		b = in.b;
		a = in.a;
		return *this;
	}
};

struct RGB888_t
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	inline RGB888_t& operator=(const BGRA8888_t& in)
	{
		r = in.r;
		g = in.g;
		b = in.b;
		return *this;
	}
	inline bool operator==(const RGB888_t& in) const
	{
		return (r == in.r) && (g == in.g) && (b == in.b);
	}
	inline bool operator!=(const RGB888_t& in) const
	{
		return (r != in.r) || (g != in.g) || (b != in.b);
	}
};

struct BGR888_t
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	inline BGR888_t& operator=(const BGRA8888_t& in)
	{
		r = in.r;
		g = in.g;
		b = in.b;
		return *this;
	}
};

// 360 uses this structure for x86 dxt decoding
#if defined( _X360 )
#pragma bitfield_order( push, lsb_to_msb )
#endif
struct BGR565_t
{
	unsigned short b : 5;		// order of names changes
	unsigned short g : 6;		//  byte order of output to 32 bit
	unsigned short r : 5;
	inline BGR565_t& operator=(const BGRA8888_t& in)
	{
		r = in.r >> 3;
		g = in.g >> 2;
		b = in.b >> 3;
		return *this;
	}
	inline BGR565_t& Set(int red, int green, int blue)
	{
		r = red >> 3;
		g = green >> 2;
		b = blue >> 3;
		return *this;
	}
};
#if defined( _X360 )
#pragma bitfield_order( pop )
#endif

struct BGRA5551_t
{
	unsigned short b : 5;		// order of names changes
	unsigned short g : 5;		//  byte order of output to 32 bit
	unsigned short r : 5;
	unsigned short a : 1;
	inline BGRA5551_t& operator=(const BGRA8888_t& in)
	{
		r = in.r >> 3;
		g = in.g >> 3;
		b = in.b >> 3;
		a = in.a >> 7;
		return *this;
	}
};

struct BGRA4444_t
{
	unsigned short b : 4;		// order of names changes
	unsigned short g : 4;		//  byte order of output to 32 bit
	unsigned short r : 4;
	unsigned short a : 4;
	inline BGRA4444_t& operator=(const BGRA8888_t& in)
	{
		r = in.r >> 4;
		g = in.g >> 4;
		b = in.b >> 4;
		a = in.a >> 4;
		return *this;
	}
};

struct RGBX5551_t
{
	unsigned short r : 5;
	unsigned short g : 5;
	unsigned short b : 5;
	unsigned short x : 1;
	inline RGBX5551_t& operator=(const BGRA8888_t& in)
	{
		r = in.r >> 3;
		g = in.g >> 3;
		b = in.b >> 3;
		return *this;
	}
};

//-----------------------------------------------------------------------------
// some important constants
//-----------------------------------------------------------------------------
#define ARTWORK_GAMMA ( 2.2f )
#define IMAGE_MAX_DIM ( 2048 )


//-----------------------------------------------------------------------------
// information about each image format
//-----------------------------------------------------------------------------
struct ImageFormatInfo_t
{
	const char* m_pName;
	int m_NumBytes;
	int m_NumRedBits;
	int m_NumGreeBits;
	int m_NumBlueBits;
	int m_NumAlphaBits;
	bool m_IsCompressed;
};


//-----------------------------------------------------------------------------
// Various methods related to pixelmaps and color formats
//-----------------------------------------------------------------------------
namespace ImageLoader
{

	bool GetInfo(const char* fileName, int* width, int* height, enum ImageFormat* imageFormat, float* sourceGamma);
	int  GetMemRequired(int width, int height, int depth, ImageFormat imageFormat, bool mipmap);
	int  GetMipMapLevelByteOffset(int width, int height, enum ImageFormat imageFormat, int skipMipLevels);
	void GetMipMapLevelDimensions(int* width, int* height, int skipMipLevels);
	int  GetNumMipMapLevels(int width, int height, int depth = 1);
	bool Load(unsigned char* imageData, const char* fileName, int width, int height, enum ImageFormat imageFormat, float targetGamma, bool mipmap);
	bool Load(unsigned char* imageData, FILE* fp, int width, int height,
		enum ImageFormat imageFormat, float targetGamma, bool mipmap);

	// convert from any image format to any other image format.
	// return false if the conversion cannot be performed.
	// Strides denote the number of bytes per each line, 
	// by default assumes width * # of bytes per pixel
	bool ConvertImageFormat(const unsigned char* src, enum ImageFormat srcImageFormat,
		unsigned char* dst, enum ImageFormat dstImageFormat,
		int width, int height, int srcStride = 0, int dstStride = 0);

	// must be used in conjunction with ConvertImageFormat() to pre-swap and post-swap
	void PreConvertSwapImageData(unsigned char* pImageData, int nImageSize, ImageFormat imageFormat, int width = 0, int stride = 0);
	void PostConvertSwapImageData(unsigned char* pImageData, int nImageSize, ImageFormat imageFormat, int width = 0, int stride = 0);
	void ByteSwapImageData(unsigned char* pImageData, int nImageSize, ImageFormat imageFormat, int width = 0, int stride = 0);
	bool IsFormatValidForConversion(ImageFormat fmt);

	//-----------------------------------------------------------------------------
	// convert back and forth from D3D format to ImageFormat, regardless of
	// whether it's supported or not
	//-----------------------------------------------------------------------------
	//ImageFormat D3DFormatToImageFormat(D3DFORMAT format);
	//D3DFORMAT ImageFormatToD3DFormat(ImageFormat format);
	void Sub();
	void Sub2();
	// Flags for ResampleRGBA8888
	enum
	{
		RESAMPLE_NORMALMAP = 0x1,
		RESAMPLE_ALPHATEST = 0x2,
		RESAMPLE_NICE_FILTER = 0x4,
		RESAMPLE_CLAMPS = 0x8,
		RESAMPLE_CLAMPT = 0x10,
		RESAMPLE_CLAMPU = 0x20,
	};

	struct ResampleInfo_t
	{

		ResampleInfo_t() : m_nFlags(0), m_flAlphaThreshhold(0.4f), m_flAlphaHiFreqThreshhold(0.4f), m_nSrcDepth(1), m_nDestDepth(1)
		{
			m_flColorScale[0] = 1.0f, m_flColorScale[1] = 1.0f, m_flColorScale[2] = 1.0f, m_flColorScale[3] = 1.0f;
			m_flColorGoal[0] = 0.0f, m_flColorGoal[1] = 0.0f, m_flColorGoal[2] = 0.0f, m_flColorGoal[3] = 0.0f;
		}

		unsigned char* m_pSrc;
		unsigned char* m_pDest;

		int m_nSrcWidth;
		int m_nSrcHeight;
		int m_nSrcDepth;

		int m_nDestWidth;
		int m_nDestHeight;
		int m_nDestDepth;

		float m_flSrcGamma;
		float m_flDestGamma;

		float m_flColorScale[4];	// Color scale factors RGBA
		float m_flColorGoal[4];		// Color goal values RGBA		DestColor = ColorGoal + scale * (SrcColor - ColorGoal)

		float m_flAlphaThreshhold;
		float m_flAlphaHiFreqThreshhold;

		int m_nFlags;
	};

	bool ResampleRGBA8888(const ResampleInfo_t& info);
	bool ResampleRGBA16161616(const ResampleInfo_t& info);
	bool ResampleRGB323232F(const ResampleInfo_t& info);

	void ConvertNormalMapRGBA8888ToDUDVMapUVLX8888(const unsigned char* src, int width, int height,
		unsigned char* dst_);
	void ConvertNormalMapRGBA8888ToDUDVMapUVWQ8888(const unsigned char* src, int width, int height,
		unsigned char* dst_);
	void ConvertNormalMapRGBA8888ToDUDVMapUV88(const unsigned char* src, int width, int height,
		unsigned char* dst_);

	void ConvertIA88ImageToNormalMapRGBA8888(const unsigned char* src, int width,
		int height, unsigned char* dst,
		float bumpScale);

	void NormalizeNormalMapRGBA8888(unsigned char* src, int numTexels);


	//-----------------------------------------------------------------------------
	// Gamma correction
	//-----------------------------------------------------------------------------
	void GammaCorrectRGBA8888(unsigned char* src, unsigned char* dst,
		int width, int height, int depth, float srcGamma, float dstGamma);


	//-----------------------------------------------------------------------------
	// Makes a gamma table
	//-----------------------------------------------------------------------------
	void ConstructGammaTable(unsigned char* pTable, float srcGamma, float dstGamma);


	//-----------------------------------------------------------------------------
	// Gamma corrects using a previously constructed gamma table
	//-----------------------------------------------------------------------------
	void GammaCorrectRGBA8888(unsigned char* pSrc, unsigned char* pDst,
		int width, int height, int depth, unsigned char* pGammaTable);


	//-----------------------------------------------------------------------------
	// Generates a number of mipmap levels
	//-----------------------------------------------------------------------------
	void GenerateMipmapLevels(unsigned char* pSrc, unsigned char* pDst, int width,
		int height, int depth, ImageFormat imageFormat, float srcGamma, float dstGamma,
		int numLevels = 0);

	// Low quality mipmap generation, but way faster. 
	void GenerateMipmapLevelsLQ(unsigned char* pSrc, unsigned char* pDst, int width, int height,
		ImageFormat imageFormat, int numLevels);

	//-----------------------------------------------------------------------------
	// operations on square images (src and dst can be the same)
	//-----------------------------------------------------------------------------
	bool RotateImageLeft(const unsigned char* src, unsigned char* dst,
		int widthHeight, ImageFormat imageFormat);
	bool RotateImage180(const unsigned char* src, unsigned char* dst,
		int widthHeight, ImageFormat imageFormat);
	bool FlipImageVertically(void* pSrc, void* pDst, int nWidth, int nHeight, ImageFormat imageFormat, int nDstStride = 0);
	bool FlipImageHorizontally(void* pSrc, void* pDst, int nWidth, int nHeight, ImageFormat imageFormat, int nDstStride = 0);
	bool SwapAxes(unsigned char* src,
		int widthHeight, ImageFormat imageFormat);


	//-----------------------------------------------------------------------------
	// Returns info about each image format
	//-----------------------------------------------------------------------------
	ImageFormatInfo_t const& ImageFormatInfo(ImageFormat fmt);


	//-----------------------------------------------------------------------------
	// Gets the name of the image format
	//-----------------------------------------------------------------------------
	inline char const* GetName(ImageFormat fmt)
	{
		return ImageFormatInfo(fmt).m_pName;
	}


	//-----------------------------------------------------------------------------
	// Gets the size of the image format in bytes
	//-----------------------------------------------------------------------------
	inline int SizeInBytes(ImageFormat fmt)
	{
		return ImageFormatInfo(fmt).m_NumBytes;
	}

	//-----------------------------------------------------------------------------
	// Does the image format support transparency?
	//-----------------------------------------------------------------------------
	inline bool IsTransparent(ImageFormat fmt)
	{
		return ImageFormatInfo(fmt).m_NumAlphaBits > 0;
	}


	//-----------------------------------------------------------------------------
	// Is the image format compressed?
	//-----------------------------------------------------------------------------
	inline bool IsCompressed(ImageFormat fmt)
	{
		return ImageFormatInfo(fmt).m_IsCompressed;
	}

	//-----------------------------------------------------------------------------
	// Is any channel > 8 bits?
	//-----------------------------------------------------------------------------
	inline bool HasChannelLargerThan8Bits(ImageFormat fmt)
	{
		ImageFormatInfo_t info = ImageFormatInfo(fmt);
		return (info.m_NumRedBits > 8 || info.m_NumGreeBits > 8 || info.m_NumBlueBits > 8 || info.m_NumAlphaBits > 8);
	}

	inline bool IsRuntimeCompressed(ImageFormat fmt)
	{
		return (fmt == IMAGE_FORMAT_DXT1_RUNTIME) || (fmt == IMAGE_FORMAT_DXT5_RUNTIME);
	}

} // end namespace ImageLoader
#endif

class RadianEuler;
class Quaternion				// same data-layout as engine's vec4_t,
{								//		which is a Vector[4]
public:
	inline Quaternion(void) {

		// Initialize to NAN to catch errors
#ifdef _DEBUG
#ifdef VECTOR_PARANOIA
		x = y = z = w = Vector_NAN;
#endif
#endif
	}
	inline Quaternion(Vector ix, Vector iy, Vector iz, Vector iw) : x(ix), y(iy), z(iz), w(iw) { }
	inline Quaternion(RadianEuler const& angle);	// evil auto type promotion!!!

	//inline void Init(Vector ix = 0.0f, Vector iy = 0.0f, Vector iz = 0.0f, Vector iw = 0.0f) { x = ix; y = iy; z = iz; w = iw; }

	bool IsValid() const;
	void Invalidate();

	bool operator==(const Quaternion& src) const;
	bool operator!=(const Quaternion& src) const;

	Vector* Base() { return (Vector*)this; }
	const Vector* Base() const { return (Vector*)this; }

	// array access...
	Vector operator[](int i) const;
	Vector& operator[](int i);

	Vector x, y, z, w;
};

#if 1
// fixme: should move this into something else.
struct FlashlightState_t
{
    FlashlightState_t()
    {
        m_bEnableShadows = false;						// Provide reasonable defaults for shadow depth mapping parameters
        m_bDrawShadowFrustum = false;
        m_flShadowMapResolution = 1024.0f;
        m_flShadowFilterSize = 3.0f;
        m_flShadowSlopeScaleDepthBias = 16.0f;
        m_flShadowDepthBias = 0.0005f;
        m_flShadowJitterSeed = 0.0f;
        m_flShadowAtten = 0.0f;
        m_bScissor = false;
        m_nLeft = -1;
        m_nTop = -1;
        m_nRight = -1;
        m_nBottom = -1;
        m_nShadowQuality = 0;
    }

    Vector m_vecLightOrigin;
    Quaternion m_quatOrientation;
    float m_NearZ;
    float m_FarZ;
    float m_fHorizontalFOVDegrees;
    float m_fVerticalFOVDegrees;
    float m_fQuadraticAtten;
    float m_fLinearAtten;
    float m_fConstantAtten;
    float m_Color[4];
    ITexture* m_pSpotlightTexture;
    int m_nSpotlightTextureFrame;

    // Shadow depth mapping parameters
    bool  m_bEnableShadows;
    bool  m_bDrawShadowFrustum;
    float m_flShadowMapResolution;
    float m_flShadowFilterSize;
    float m_flShadowSlopeScaleDepthBias;
    float m_flShadowDepthBias;
    float m_flShadowJitterSeed;
    float m_flShadowAtten;
    int   m_nShadowQuality;

    // Getters for scissor members
    bool DoScissor() { return m_bScissor; }
    int GetLeft() { return m_nLeft; }
    int GetTop() { return m_nTop; }
    int GetRight() { return m_nRight; }
    int GetBottom() { return m_nBottom; }

private:

    friend class CShadowMgr;

    bool m_bScissor;
    int m_nLeft;
    int m_nTop;
    int m_nRight;
    int m_nBottom;
};
#endif
//-----------------------------------------------------------------------------
// Flags to be used with the Init call
//-----------------------------------------------------------------------------
enum MaterialInitFlags_t
{
    MATERIAL_INIT_ALLOCATE_FULLSCREEN_TEXTURE = 0x2,
    MATERIAL_INIT_REFERENCE_RASTERIZER = 0x4,
};

//-----------------------------------------------------------------------------
// Flags to specify type of depth buffer used with RT
//-----------------------------------------------------------------------------

// GR - this is to add RT with no depth buffer bound

enum MaterialRenderTargetDepth_t
{
    MATERIAL_RT_DEPTH_SHARED = 0x0,
    MATERIAL_RT_DEPTH_SEPARATE = 0x1,
    MATERIAL_RT_DEPTH_NONE = 0x2,
    MATERIAL_RT_DEPTH_ONLY = 0x3,
};

//-----------------------------------------------------------------------------
// A function to be called when we need to release all vertex buffers
// NOTE: The restore function will tell the caller if all the vertex formats
// changed so that it can flush caches, etc. if it needs to (for dxlevel support)
//-----------------------------------------------------------------------------
enum RestoreChangeFlags_t
{
    MATERIAL_RESTORE_VERTEX_FORMAT_CHANGED = 0x1,
};


// NOTE: All size modes will force the render target to be smaller than or equal to
// the size of the framebuffer.
enum RenderTargetSizeMode_t
{
    RT_SIZE_NO_CHANGE = 0,			// Only allowed for render targets that don't want a depth buffer
    // (because if they have a depth buffer, the render target must be less than or equal to the size of the framebuffer).
    RT_SIZE_DEFAULT = 1,				// Don't play with the specified width and height other than making sure it fits in the framebuffer.
    RT_SIZE_PICMIP = 2,				// Apply picmip to the render target's width and height.
    RT_SIZE_HDR = 3,					// frame_buffer_width / 4
    RT_SIZE_FULL_FRAME_BUFFER = 4,	// Same size as frame buffer, or next lower power of 2 if we can't do that.
    RT_SIZE_OFFSCREEN = 5,			// Target of specified size, don't mess with dimensions
    RT_SIZE_FULL_FRAME_BUFFER_ROUNDED_UP = 6, // Same size as the frame buffer, rounded up if necessary for systems that can't do non-power of two textures.
    RT_SIZE_REPLAY_SCREENSHOT = 7,	// Rounded down to power of 2, essentially...
    RT_SIZE_LITERAL = 8				// Use the size passed in. Don't clamp it to the frame buffer size. Really.
};

typedef void (*MaterialBufferReleaseFunc_t)();
typedef void (*MaterialBufferRestoreFunc_t)(int nChangeFlags);	// see RestoreChangeFlags_t
typedef void (*ModeChangeCallbackFunc_t)(void);

typedef int VertexBufferHandle_t;
typedef unsigned short MaterialHandle_t;


#define INVALID_OCCLUSION_QUERY_OBJECT_HANDLE ( (OcclusionQueryObjectHandle_t)0 )



class IRefCounted {
private:
    volatile long refCount;

public:
    virtual int AddRef() = 0;
    virtual int Release() = 0;
};


struct Vector4D {
	float x, y, z, w;
};

struct Vector2D {
	float x, y;
};


class VMatrix {
	Vector	m[4][4];
};

class IMesh;
typedef uint64_t VertexFormat_t;
class IIndexBuffer;
class IVertexBuffer;
class ICallQueue;
class IMorph;
class MorphWeight_t;

enum StencilOperation_t
{
#if !defined( _X360 )
	STENCILOPERATION_KEEP = 1,
	STENCILOPERATION_ZERO = 2,
	STENCILOPERATION_REPLACE = 3,
	STENCILOPERATION_INCRSAT = 4,
	STENCILOPERATION_DECRSAT = 5,
	STENCILOPERATION_INVERT = 6,
	STENCILOPERATION_INCR = 7,
	STENCILOPERATION_DECR = 8,
#else
	STENCILOPERATION_KEEP = D3DSTENCILOP_KEEP,
	STENCILOPERATION_ZERO = D3DSTENCILOP_ZERO,
	STENCILOPERATION_REPLACE = D3DSTENCILOP_REPLACE,
	STENCILOPERATION_INCRSAT = D3DSTENCILOP_INCRSAT,
	STENCILOPERATION_DECRSAT = D3DSTENCILOP_DECRSAT,
	STENCILOPERATION_INVERT = D3DSTENCILOP_INVERT,
	STENCILOPERATION_INCR = D3DSTENCILOP_INCR,
	STENCILOPERATION_DECR = D3DSTENCILOP_DECR,
#endif
	STENCILOPERATION_FORCE_DWORD = 0x7fffffff
};

enum StencilComparisonFunction_t
{
#if !defined( _X360 )
	STENCILCOMPARISONFUNCTION_NEVER = 1,
	STENCILCOMPARISONFUNCTION_LESS = 2,
	STENCILCOMPARISONFUNCTION_EQUAL = 3,
	STENCILCOMPARISONFUNCTION_LESSEQUAL = 4,
	STENCILCOMPARISONFUNCTION_GREATER = 5,
	STENCILCOMPARISONFUNCTION_NOTEQUAL = 6,
	STENCILCOMPARISONFUNCTION_GREATEREQUAL = 7,
	STENCILCOMPARISONFUNCTION_ALWAYS = 8,
#else
	STENCILCOMPARISONFUNCTION_NEVER = D3DCMP_NEVER,
	STENCILCOMPARISONFUNCTION_LESS = D3DCMP_LESS,
	STENCILCOMPARISONFUNCTION_EQUAL = D3DCMP_EQUAL,
	STENCILCOMPARISONFUNCTION_LESSEQUAL = D3DCMP_LESSEQUAL,
	STENCILCOMPARISONFUNCTION_GREATER = D3DCMP_GREATER,
	STENCILCOMPARISONFUNCTION_NOTEQUAL = D3DCMP_NOTEQUAL,
	STENCILCOMPARISONFUNCTION_GREATEREQUAL = D3DCMP_GREATEREQUAL,
	STENCILCOMPARISONFUNCTION_ALWAYS = D3DCMP_ALWAYS,
#endif

	STENCILCOMPARISONFUNCTION_FORCE_DWORD = 0x7fffffff
};



class IMatRenderContext : public IRefCounted
{
public:
    virtual void				BeginRender() = 0;
    virtual void				EndRender() = 0;
    virtual void				Flush(bool flushHardware = false) = 0;
    virtual void				BindLocalCubemap(ITexture* pTexture) = 0;
    virtual void				SetRenderTarget(ITexture* pTexture) = 0;
    virtual ITexture* GetRenderTarget(void) = 0;
    virtual void				GetRenderTargetDimensions(int& width, int& height) const = 0;
    virtual void				Bind(IMaterial* material, void* proxyData = 0) = 0;
    virtual void				BindLightmapPage(int lightmapPageID) = 0;
    virtual void				DepthRange(float zNear, float zFar) = 0;
    virtual void				ClearBuffers(int bClearColor, int bClearDepth, int bClearStencil = 0) = 0;
    virtual void				ReadPixels(int x, int y, int width, int height, unsigned char* data, ImageFormat dstFormat) = 0;
    virtual void unk124() = 0;
    virtual void unk125() = 0;
    virtual void				SetAmbientLight(float r, float g, float b) = 0;
    virtual void				SetLight(int lightNum, const LightDesc_t& desc) = 0;
    virtual void				SetAmbientLightCube(Vector4D cube[6]) = 0;
    virtual void				CopyRenderTargetToTexture(ITexture* pTexture) = 0;
    virtual void				SetFrameBufferCopyTexture(ITexture* pTexture, int textureIndex = 0) = 0;
    virtual ITexture* GetFrameBufferCopyTexture(int textureIndex) = 0;
    virtual void				MatrixMode(MaterialMatrixMode_t matrixMode) = 0;
    virtual void				PushMatrix(void) = 0;
    virtual void				PopMatrix(void) = 0;
    virtual void				LoadMatrix(VMatrix const& matrix) = 0;
    virtual void				LoadMatrix(matrix3x4_t const& matrix) = 0;
    virtual void				MultMatrix(VMatrix const& matrix) = 0;
    virtual void				MultMatrix(matrix3x4_t const& matrix) = 0;
    virtual void				MultMatrixLocal(VMatrix const& matrix) = 0;
    virtual void				MultMatrixLocal(matrix3x4_t const& matrix) = 0;
    virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, VMatrix* matrix) = 0;
    virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, matrix3x4_t* matrix) = 0;
    virtual void				LoadIdentity(void) = 0;
    virtual void				Ortho(double left, double top, double right, double bottom, double zNear, double zFar) = 0;
    virtual void				PerspectiveX(double fovx, double aspect, double zNear, double zFar) = 0;
    virtual void				PickMatrix(int x, int y, int width, int height) = 0;
    virtual void				Rotate(float angle, float x, float y, float z) = 0;
    virtual void				Translate(float x, float y, float z) = 0;
    virtual void				Scale(float x, float y, float z) = 0;
    virtual void				Viewport(int x, int y, int width, int height) = 0;
    virtual void				GetViewport(int& x, int& y, int& width, int& height) const = 0;
    virtual void				CullMode(MaterialCullMode_t cullMode) = 0;
    virtual void				SetHeightClipMode(MaterialHeightClipMode_t nHeightClipMode) = 0;
    virtual void				SetHeightClipZ(float z) = 0;
    virtual void				FogMode(MaterialFogMode_t fogMode) = 0;
    virtual void				FogStart(float fStart) = 0;
    virtual void				FogEnd(float fEnd) = 0;
    virtual void				SetFogZ(float fogZ) = 0;
    virtual MaterialFogMode_t	GetFogMode(void) = 0;
    virtual void				FogColor3f(float r, float g, float b) = 0;
    virtual void				FogColor3fv(float const* rgb) = 0;
    virtual void				FogColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
    virtual void				FogColor3ubv(unsigned char const* rgb) = 0;
    virtual void				GetFogColor(unsigned char* rgb) = 0;
    virtual void				SetNumBoneWeights(int numBones) = 0;
    virtual IMesh* CreateStaticMesh(VertexFormat_t fmt, const char* pTextureBudgetGroup, IMaterial* pMaterial = NULL) = 0;
    virtual void DestroyStaticMesh(IMesh* mesh) = 0;
    virtual IMesh* GetDynamicMesh(bool buffered = true, IMesh* pVertexOverride = 0, IMesh* pIndexOverride = 0, IMaterial* pAutoBind = 0) = 0;
    virtual IVertexBuffer* CreateStaticVertexBuffer(VertexFormat_t fmt, int nVertexCount, const char* pTextureBudgetGroup) = 0;
    virtual IIndexBuffer* CreateStaticIndexBuffer(MaterialIndexFormat_t fmt, int nIndexCount, const char* pTextureBudgetGroup) = 0;
    virtual void DestroyVertexBuffer(IVertexBuffer*) = 0;
    virtual void DestroyIndexBuffer(IIndexBuffer*) = 0;
    virtual IVertexBuffer* GetDynamicVertexBuffer(int streamID, VertexFormat_t vertexFormat, bool bBuffered = true) = 0;
    virtual IIndexBuffer* GetDynamicIndexBuffer(MaterialIndexFormat_t fmt, bool bBuffered = true) = 0;
    virtual void BindVertexBuffer(int streamID, IVertexBuffer* pVertexBuffer, int nOffsetInBytes, int nFirstVertex, int nVertexCount, VertexFormat_t fmt, int nRepetitions = 1) = 0;
    virtual void BindIndexBuffer(IIndexBuffer* pIndexBuffer, int nOffsetInBytes) = 0;
    virtual void Draw(MaterialPrimitiveType_t primitiveType, int firstIndex, int numIndices) = 0;
    virtual int  SelectionMode(bool selectionMode) = 0;
    virtual void SelectionBuffer(unsigned int* pBuffer, int size) = 0;
    virtual void ClearSelectionNames() = 0;
    virtual void LoadSelectionName(int name) = 0;
    virtual void PushSelectionName(int name) = 0;
    virtual void PopSelectionName() = 0;
    /*virtual void		ClearColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
    virtual void		ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) = 0;*/
    virtual void	OverrideDepthEnable(bool bEnable, bool bDepthEnable) = 0;
    virtual void	DrawScreenSpaceQuad(IMaterial* pMaterial) = 0;
    virtual void	SyncToken(const char* pToken) = 0;
    virtual float	ComputePixelWidthOfSphere(const Vector& origin, float flRadius) = 0;
    virtual void		ClearColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
    virtual void		ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) = 0;
    virtual int CreateOcclusionQueryObject(void) = 0;
    virtual void DestroyOcclusionQueryObject(int) = 0;
    virtual void BeginOcclusionQueryDrawing(int) = 0;
    virtual void EndOcclusionQueryDrawing(int) = 0;
    virtual int OcclusionQuery_GetNumPixelsRendered(int) = 0;
    virtual void SetFlashlightMode(bool bEnable) = 0;
    virtual void SetFlashlightState(const FlashlightState_t& state, const VMatrix& worldToTexture) = 0;
    virtual MaterialHeightClipMode_t GetHeightClipMode() = 0;
    virtual float	ComputePixelDiameterOfSphere(const Vector& vecAbsOrigin, float flRadius) = 0;
    virtual void	EnableUserClipTransformOverride(bool bEnable) = 0;
    virtual void	UserClipTransform(const VMatrix& worldToView) = 0;
    virtual bool GetFlashlightMode() const = 0;
    virtual void ResetOcclusionQueryObject(int) = 0;
    virtual void Unused3() {}
    virtual IMorph* CreateMorph(unsigned int format, const char* pDebugName) = 0;
    virtual void DestroyMorph(IMorph* pMorph) = 0;
    virtual void BindMorph(IMorph* pMorph) = 0;
    virtual void SetFlexWeights(int nFirstWeight, int nCount, const MorphWeight_t* pWeights) = 0;
    virtual void Unused4() {};
    virtual void Unused5() {};
    virtual void Unused6() {};
    virtual void Unused7() {};
    virtual void ReadPixelsAndStretch(void* pSrcRect, void* pDstRect, unsigned char* pBuffer, ImageFormat dstFormat, int nDstStride) = 0;
    virtual void GetWindowSize(int& width, int& height) const = 0;
    virtual void DrawScreenSpaceRectangle(IMaterial* pMaterial, int destx, int desty, int width, int height, float src_texture_x0, float src_texture_y0, float src_texture_x1, float src_texture_y1, int src_texture_width, int src_texture_height, void* pClientRenderable = NULL, int nXDice = 1, int nYDice = 1) = 0;
    virtual void LoadBoneMatrix(int boneIndex, const matrix3x4_t& matrix) = 0;
    virtual void PushRenderTargetAndViewport() = 0;
    virtual void PushRenderTargetAndViewport(ITexture* pTexture) = 0;
    virtual void PushRenderTargetAndViewport(ITexture* pTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;
    virtual void PushRenderTargetAndViewport(ITexture* pTexture, ITexture* pDepthTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;
    virtual void PopRenderTargetAndViewport(void) = 0;
    virtual void BindLightmapTexture(ITexture* pLightmapTexture) = 0;
    virtual void CopyRenderTargetToTextureEx(ITexture* pTexture, int nRenderTargetID, void* pSrcRect, void* pDstRect = NULL) = 0;
    virtual void CopyTextureToRenderTargetEx(int nRenderTargetID, ITexture* pTexture, void* pSrcRect, void* pDstRect = NULL) = 0;
    virtual void PerspectiveOffCenterX(double fovx, double aspect, double zNear, double zFar, double bottom, double top, double left, double right) = 0;
    virtual void SetFloatRenderingParameter(int parm_number, float value) = 0;
    virtual void SetIntRenderingParameter(int parm_number, int value) = 0;
    virtual void SetVectorRenderingParameter(int parm_number, Vector const& value) = 0;
    virtual void SetStencilEnable(bool onoff) = 0;
    virtual void SetStencilFailOperation(StencilOperation_t op) = 0;
    virtual void SetStencilZFailOperation(StencilOperation_t op) = 0;
    virtual void SetStencilPassOperation(StencilOperation_t op) = 0;
    virtual void SetStencilCompareFunction(int cmpfn) = 0;
    virtual void SetStencilReferenceValue(int ref) = 0;
    virtual void SetStencilTestMask(unsigned int msk) = 0;
    virtual void SetStencilWriteMask(unsigned int msk) = 0;
    virtual void ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax, int value) = 0;
    virtual void SetRenderTargetEx(int nRenderTargetID, ITexture* pTexture) = 0;
    virtual void PushCustomClipPlane(const float* pPlane) = 0;
    virtual void PopCustomClipPlane(void) = 0;
    virtual void GetMaxToRender(IMesh* pMesh, bool bMaxUntilFlush, int* pMaxVerts, int* pMaxIndices) = 0;
    virtual int GetMaxVerticesToRender(IMaterial* pMaterial) = 0;
    virtual int GetMaxIndicesToRender() = 0;
    virtual void DisableAllLocalLights() = 0;
    virtual int CompareMaterialCombos(IMaterial* pMaterial1, IMaterial* pMaterial2, int lightMapID1, int lightMapID2) = 0;
    virtual IMesh* GetFlexMesh() = 0;
    virtual void SetFlashlightStateEx(const FlashlightState_t& state, const VMatrix& worldToTexture, ITexture* pFlashlightDepthTexture) = 0;
    virtual ITexture* GetLocalCubemap() = 0;
    virtual void ClearBuffersObeyStencil(bool bClearColor, bool bClearDepth) = 0;
    virtual bool EnableClipping(bool bEnable) = 0;
    virtual void GetFogDistances(float* fStart, float* fEnd, float* fFogZ) = 0;
    virtual void BeginPIXEvent(unsigned long color, const char* szName) = 0;
    virtual void EndPIXEvent() = 0;
    virtual void SetPIXMarker(unsigned long color, const char* szName) = 0;
    virtual void BeginBatch(IMesh* pIndices) = 0;
    virtual void BindBatch(IMesh* pVertices, IMaterial* pAutoBind = NULL) = 0;
    virtual void DrawBatch(int firstIndex, int numIndices) = 0;
    virtual void EndBatch() = 0;
    virtual ICallQueue* GetCallQueue() = 0;
    virtual void GetWorldSpaceCameraPosition(Vector* pCameraPos) = 0;
    virtual void GetWorldSpaceCameraVectors(Vector* pVecForward, Vector* pVecRight, Vector* pVecUp) = 0;
    virtual void				ResetToneMappingScale(float monoscale) = 0; 			// set scale to monoscale instantly with no chasing
    virtual void				SetGoalToneMappingScale(float monoscale) = 0; 			// set scale to monoscale instantly with no chasing
    virtual void				TurnOnToneMapping() = 0;
    virtual void				SetToneMappingScaleLinear(const Vector& scale) = 0;
    virtual Vector				GetToneMappingScaleLinear(void) = 0;
    virtual void				SetShadowDepthBiasFactors(float fSlopeScaleDepthBias, float fDepthBias) = 0;
    virtual void				PerformFullScreenStencilOperation(void) = 0;
    virtual void				SetLightingOrigin(/*Vector vLightingOrigin*/float x, float y, float z) = 0;																		//158
    virtual void				SetScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor) = 0;
    virtual void				BeginMorphAccumulation() = 0;
    virtual void				EndMorphAccumulation() = 0;
    virtual void				AccumulateMorph(IMorph* pMorph, int nMorphCount, const MorphWeight_t* pWeights) = 0;
    virtual void				PushDeformation(int const* Deformation) = 0;
    virtual void				PopDeformation() = 0;
    virtual int					GetNumActiveDeformations() const = 0;
    virtual bool				GetMorphAccumulatorTexCoord(Vector2D* pTexCoord, IMorph* pMorph, int nVertex) = 0;
    virtual IMesh* GetDynamicMeshEx(VertexFormat_t vertexFormat, bool bBuffered = true, IMesh* pVertexOverride = 0, IMesh* pIndexOverride = 0, IMaterial* pAutoBind = 0) = 0;
    virtual void				FogMaxDensity(float flMaxDensity) = 0;
    virtual IMaterial* GetCurrentMaterial() = 0;
    virtual int  GetCurrentNumBones() const = 0;
    virtual void* GetCurrentProxy() = 0;
    virtual void EnableColorCorrection(bool bEnable) = 0;
    virtual int AddLookup(const char* pName) = 0;
    virtual bool RemoveLookup(int handle) = 0;
    virtual void LockLookup(int handle) = 0;
    virtual void LoadLookup(int handle, const char* pLookupName) = 0;
    virtual void UnlockLookup(int handle) = 0;
    virtual void SetLookupWeight(int handle, float flWeight) = 0;
    virtual void ResetLookupWeights() = 0;
    virtual void SetResetable(int handle, bool bResetable) = 0;
    virtual void SetFullScreenDepthTextureValidityFlag(bool bIsValid) = 0;
    virtual void SetNonInteractivePacifierTexture(ITexture* pTexture, float flNormalizedX, float flNormalizedY, float flNormalizedSize) = 0;
    virtual void SetNonInteractiveTempFullscreenBuffer(ITexture* pTexture, MaterialNonInteractiveMode_t mode) = 0;
    virtual void EnableNonInteractiveMode(MaterialNonInteractiveMode_t mode) = 0;
    virtual void RefreshFrontBufferNonInteractive() = 0;
    virtual void* LockRenderData(int nSizeInBytes) = 0;
    virtual void			UnlockRenderData(void* pData) = 0;
    virtual void			AddRefRenderData() = 0;
    virtual void			ReleaseRenderData() = 0;
    virtual bool			IsRenderData(const void* pData) const = 0;
    virtual void			PrintfVA(char* fmt, va_list vargs) = 0;
    virtual void			Printf(const char* fmt, ...) = 0;
    virtual float			Knob(char* knobname, float* setvalue = NULL) = 0;
    virtual void OverrideAlphaWriteEnable(bool bEnable, bool bAlphaWriteEnable) = 0;
    virtual void OverrideColorWriteEnable(bool bOverrideEnable, bool bColorWriteEnable) = 0;
    virtual void ClearBuffersObeyStencilEx(bool bClearColor, bool bClearAlpha, bool bClearDepth) = 0;
};

template <class T>
class CBaseAutoPtr
{
public:
    CBaseAutoPtr() : m_pObject(0) {}
    CBaseAutoPtr(T* pFrom) : m_pObject(pFrom) {}

    operator const void* () const { return m_pObject; }
    operator void* () { return m_pObject; }

    operator const T* () const { return m_pObject; }
    operator const T* () { return m_pObject; }
    operator T* () { return m_pObject; }

    int			operator=(int i) { AssertMsg(i == 0, "Only NULL allowed on integer assign"); m_pObject = 0; return 0; }
    T* operator=(T* p) { m_pObject = p; return p; }

    bool        operator !() const { return (!m_pObject); }
    bool        operator!=(int i) const { AssertMsg(i == 0, "Only NULL allowed on integer compare"); return (m_pObject != NULL); }
    bool		operator==(const void* p) const { return (m_pObject == p); }
    bool		operator!=(const void* p) const { return (m_pObject != p); }
    bool		operator==(T* p) const { return operator==((void*)p); }
    bool		operator!=(T* p) const { return operator!=((void*)p); }
    bool		operator==(const CBaseAutoPtr<T>& p) const { return operator==((const void*)p); }
    bool		operator!=(const CBaseAutoPtr<T>& p) const { return operator!=((const void*)p); }

    T* operator->() { return m_pObject; }
    T& operator *() { return *m_pObject; }
    T** operator &() { return &m_pObject; }

    const T* operator->() const { return m_pObject; }
    const T& operator *() const { return *m_pObject; }
    T* const* operator &() const { return &m_pObject; }

protected:
    CBaseAutoPtr(const CBaseAutoPtr<T>& from) : m_pObject(from.m_pObject) {}
    void operator=(const CBaseAutoPtr<T>& from) { m_pObject = from.m_pObject; }

    T* m_pObject;
};

//---------------------------------------------------------

template <class T>
class CRefPtr : public CBaseAutoPtr<T>
{
    typedef CBaseAutoPtr<T> BaseClass;
public:
    CRefPtr() {}
    CRefPtr(T* pInit) : BaseClass(pInit) {}
    CRefPtr(const CRefPtr<T>& from) : BaseClass(from) {}
    ~CRefPtr() { if (BaseClass::m_pObject) BaseClass::m_pObject->Release(); }

    void operator=(const CRefPtr<T>& from) { BaseClass::operator=(from); }

    int operator=(int i) { return BaseClass::operator=(i); }
    T* operator=(T* p) { return BaseClass::operator=(p); }

    operator bool() const { return !BaseClass::operator!(); }
    operator bool() { return !BaseClass::operator!(); }

    void SafeRelease() { if (BaseClass::m_pObject) BaseClass::m_pObject->Release(); BaseClass::m_pObject = 0; }
    void AssignAddRef(T* pFrom) { SafeRelease(); if (pFrom) pFrom->AddRef(); BaseClass::m_pObject = pFrom; }
    void AddRefAssignTo(T*& pTo) { ::SafeRelease(pTo); if (BaseClass::m_pObject) BaseClass::m_pObject->AddRef(); pTo = BaseClass::m_pObject; }
};


class CMatRenderContextPtr : public CRefPtr<IMatRenderContext>
{
    typedef CRefPtr<IMatRenderContext> BaseClass;
public:
    CMatRenderContextPtr() {}
    CMatRenderContextPtr(IMatRenderContext* pInit) : BaseClass(pInit) { if (BaseClass::m_pObject) BaseClass::m_pObject->BeginRender(); }
    CMatRenderContextPtr(IMaterialSystem* pFrom) : BaseClass(pFrom->GetRenderContext()) { if (BaseClass::m_pObject) BaseClass::m_pObject->BeginRender(); }
    ~CMatRenderContextPtr() { if (BaseClass::m_pObject) BaseClass::m_pObject->EndRender(); }

    IMatRenderContext* operator=(IMatRenderContext* p) { if (p) p->BeginRender(); return BaseClass::operator=(p); }

    void SafeRelease() { if (BaseClass::m_pObject) BaseClass::m_pObject->EndRender(); BaseClass::SafeRelease(); }
    void AssignAddRef(IMatRenderContext* pFrom) { if (BaseClass::m_pObject) BaseClass::m_pObject->EndRender(); BaseClass::AssignAddRef(pFrom); BaseClass::m_pObject->BeginRender(); }

    void GetFrom(IMaterialSystem* pFrom) { AssignAddRef(pFrom->GetRenderContext()); }


private:
    CMatRenderContextPtr(const CMatRenderContextPtr& from);
    void operator=(const CMatRenderContextPtr& from);

};

class Texture_t {
public:
    char pad[0xC];
    void* texture_ptr;
};

class ITexture {
public:
    char pad[0x50];
    Texture_t** m_pTextureHandles;
};


#include "../OsirisSDK/ConVar.h"
#include "../../Interfaces.h"
#include "../../ClientHooks.h"

//class ClientHooks::CModelRenderSystem {
//public:
//	void  _fastcall RenderModels(void* _this, void* edx, StudioModelArrayInfo2_t* pInfo, int nModelTypeCount, ModelListByType_t* pModelList, int nTotalModelCount, ModelRenderMode_t renderMode, int nFlags);
//};








