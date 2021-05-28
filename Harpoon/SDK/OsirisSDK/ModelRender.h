#pragma once

#include "Utils.h"
#include "Vector.h"

class matrix3x4;

struct Model {
    void* handle;
    char name[260];
    int	loadFlags;
    int	serverCount;
    int	type;
    int	flags;
    Vector mins, maxs;
};

struct ModelRenderInfo {
    Vector origin;
    Vector angles;
    char pad[4];
    void* renderable;
    const Model* model;
    const matrix3x4* modelToWorld;
    const matrix3x4* lightingOffset;
    const Vector* lightingOrigin;
    int flags;
    int entityIndex;
    int32_t m_skin;
    int32_t m_body;
    int32_t m_hit_box_set;
    int32_t m_model_instance;
};


/*
struct DrawModelInfo_t
{
    StudioHdr* m_pStudioHdr;
    uintptr_t* m_pHardwareData;
    StudioDecalHandle_t m_Decals;
    int                m_Skin;
    int                m_Body;
    int                m_HitboxSet;
    void* m_pClientEntity;
    int                m_Lod;
    ColorMeshInfo_t* m_pColorMeshes;
    bool            m_bStaticLighting;
    MaterialLightingState_t    m_LightingState;

    IMPLEMENT_OPERATOR_EQUAL(DrawModelInfo_t);
};
*/