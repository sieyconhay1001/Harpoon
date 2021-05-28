#pragma once

#include "VirtualMethod.h"

class KeyValues;
class Material;
class RenderContext;
class IMatRenderContext;
class MaterialSystem {
public:
    VIRTUAL_METHOD(void, releaseResource, 43, (), (this))
    VIRTUAL_METHOD(void, reaquireResource, 44, (), (this))
    VIRTUAL_METHOD(void, uncacheAllMaterials, 78, (), (this))
    VIRTUAL_METHOD(void, reloadMaterials, 82, (const char* substring = NULL), (this, substring))
    VIRTUAL_METHOD(void, reloadTextures, 81, (),(this))
    VIRTUAL_METHOD(Material*, createMaterial, 83, (const char* materialName, KeyValues* keyValues), (this, materialName, keyValues))
    VIRTUAL_METHOD(Material*, findMaterial, 84, (const char* materialName, const char* textureGroupName = nullptr, bool complain = true, const char* complainPrefix = nullptr), (this, materialName, textureGroupName, complain, complainPrefix))
    VIRTUAL_METHOD(bool, isMaterialLoaded, 85, (const char* materialName), (this, materialName))
    VIRTUAL_METHOD(short, firstMaterial, 86, (), (this))
    VIRTUAL_METHOD(short, nextMaterial, 87, (short handle), (this, handle))
    VIRTUAL_METHOD(short, invalidMaterial, 88, (), (this))
    VIRTUAL_METHOD(Material*, getMaterial, 89, (short handle), (this, handle))
    VIRTUAL_METHOD(RenderContext*, getRenderContext, 115, (), (this))
    VIRTUAL_METHOD(IMatRenderContext*, GetRenderContext, 115, (), (this))
};
struct MaterialVideoMode_t {
    int m_Width;
    int m_Height;
    int m_Format;
    int m_RefreshRate;
};

struct MaterialSystem_Config_t {
    MaterialVideoMode_t m_VideoMode;
    float m_fMonitorGamma;
    float m_fGammaTVRangeMin;
    float m_fGammaTVRangeMax;
    float m_fGammaTVExponent;
    bool m_bGammaTVEnabled;
    bool m_bTripleBuffered;
    int m_nAASamples;
    int m_nForceAnisotropicLevel;
    int m_nSkipMipLevels;
    int m_nDxSupportLevel;
    int m_nFlags;
    bool m_bEditMode;
    char m_nProxiesTestMode;
    bool m_bCompressedTextures;
    bool m_bFilterLightmaps;
    bool m_bFilterTextures;
    bool m_bReverseDepth;
    bool m_bBufferPrimitives;
    bool m_bDrawFlat;
    bool m_bMeasureFillRate;
    bool m_bVisualizeFillRate;
    bool m_bNoTransparency;
    bool m_bSoftwareLighting;
    bool m_bAllowCheats;
    char m_nShowMipLevels;
    bool m_bShowLowResImage;
    bool m_bShowNormalMap;
    bool m_bMipMapTextures;
    char m_nFullbright;
    bool m_bFastNoBump;
    bool m_bSuppressRendering;
    bool m_bDrawGray;
    bool m_bShowSpecular;
    bool m_bShowDiffuse;
    int m_nWindowedSizeLimitWidth;
    int m_nWindowedSizeLimitHeight;
    int m_nAAQuality;
    bool m_bShadowDepthTexture;
    bool m_bMotionBlur;
    bool m_bSupportFlashlight;
    bool m_bPaintEnabled;
    char pad[0xC];
};