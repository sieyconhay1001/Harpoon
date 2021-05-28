#pragma once

#include <vector>

#include "../Config.h"

class Entity;
struct ModelRenderInfo;
class matrix3x4;
class Material;
#include "../OtherHooks.h"
#include "../SDK/SDK/ModelRenderSystem.h"

#include <map>
#include <unordered_map>
#include "../Hacks/OTHER/Debug.h"

class Chams {
public:
    Chams();
    bool __thiscall OnDrawModel(void* _this, void* edx, const DrawModelInfo_t* DrawModelInfo, matrix3x4* boneToWorld, float* flexWeights, float* flexDelayedWeights, const Vector& ModelOrigin, int flags);
    bool OnDrawModelExecute(void*, void*, const ModelRenderInfo&, matrix3x4*);
    void RenderDrawModelPlayerChams(Entity* pEnt, void* _this, void* edx, const DrawModelInfo_t& DrawModelInfo, matrix3x4* pBoneToWorld, float* fFlexWeights, float* fFlexDelayedWeights, const Vector& vModelOrigin, int nFlags);
    void DrawModelRenderCham(Config::Cham_s::Material Mat, void* _this, void* edx, const DrawModelInfo_t& DrawModelInfo, matrix3x4* pBoneToWorld, float* fFlexWeights, float* fFlexDelayedWeights, const Vector& vModelOrigin, int nFlags);
    void DrawSceneEndCham(Config::Cham_s::Material Mat, Entity* pEnt);
    void __forceinline DrawPlayerSceneEndCham(Entity* pEnt, Config::Cham_s::Material Materials[10]);
    bool OnSceneEnd();
    bool OnRenderModels();
    bool OnDrawModels();

    inline std::vector<std::string> GetMaterialList() {
        if (Materials.size() == nLastGrabSize)
            return MaterialList;
        MaterialList.clear();
        for (std::map<std::string, Material*>::iterator it = Materials.begin(); it != Materials.end(); ++it) {
            MaterialList.push_back(it->first);
        }
        return MaterialList;
    }
    bool bRenderSmoke{ false };
    bool bRenderViewModel{ false };
    void* oDrawModel{ nullptr }; 
private:
    int nLastGrabSize = 0; // Don't Bother With going through everything if it aint updated
    std::map<std::string, Material*> Materials;
    std::vector<std::string> MaterialList;
    inline Material* GetMaterial(std::string MatName) {
       // if (Materials.find(MatName) == Materials.end())
       //    VCON("Material %s Not Found!\n", MatName.c_str());


        return Materials[MatName];
    }
    bool m_bInSceneEndRender{ false };
};
inline Chams* g_pChamsRenderer;
















#if 0
class Chams {
public:
    Chams() noexcept;
    bool render(void*, void*, const ModelRenderInfo&, matrix3x4*) noexcept;
    bool PropChams();
    Material* worldChams(const char* Material, const char* TextureGroup);

    void SetMat(Material* mat) noexcept;

    void applyPropChams(void* _this, void* edx, const studioRenderHooks::DrawModelInfo_t& DrawModelInfo, matrix3x4* boneToWorld, float* flexWeights, float* flexDelayedWeights, const Vector& ModelOrigin, int flags);
    void applyDynamicPropChams(void* _this, void* edx, StudioModelArrayInfo2_t* pInfo, int nModelTypeCount, ModelRenderSystem::ModelListByType_t* pModelList, int nTotalModelCount, ModelRenderSystem::ModelRenderMode_t renderMode, int nFlags, const std::array<Config::Material, 10>& chams) noexcept;
    void RenderModelsChams(void* _this, void* edx, StudioModelArrayInfo2_t* pInfo, int nModelTypeCount, ModelRenderSystem::ModelListByType_t* pModelList, int nTotalModelCount, ModelRenderSystem::ModelRenderMode_t renderMode, int nFlags) noexcept;


    void UpdateChams();
private:
    void renderPlayer(Entity* player, bool dead = false) noexcept;
    void renderWeapons() noexcept;
    void renderHands() noexcept;
    void renderSleeves() noexcept;
    void renderSmoke() noexcept;
    void renderFish(Entity*) noexcept;
    void renderChicken(Entity*) noexcept;
    Material* getMat(Config::SingleCham cham) noexcept;
    enum ChamsId {
        ALLIES = 0,
        ENEMIES,
        PLANTING,
        DEFUSING,
        LOCALPLAYER,
        WEAPONS,
        HANDS,
        BACKTRACK,
        SLEEVES
    };

    Material* normal;
    Material* flat;
    Material* animated;
    Material* platinum;
    Material* glass;
    Material* crystal;
    Material* chrome;
    Material* silver;
    Material* gold;
    Material* plastic;
    Material* glow;
    Material* pearlescent;
    Material* metallic;

    Material* metalsnow;
    Material* glasswindow;
    Material* c4_gift;
    Material* urban_puddle;
    Material* crystal_cube;
    Material* ghost1;

    Material* zombie;
    Material* searchlight;
    Material* brokenglass;

    Material* crystal_blue;
    Material* velvet;
    Material* water;
    Material* metalwall;

    Material* whiteboard01;
    Material* whiteboard04;

    constexpr auto dispatchMaterial(int id) const noexcept
    {
        switch (id) {
        default:
        case 0: return normal;
        case 1: return flat;
        case 2: return animated;
        case 3: return platinum;
        case 4: return glass;
        case 5: return chrome;
        case 6: return crystal;
        case 7: return silver;
        case 8: return gold;
        case 9: return plastic;
        case 10: return glow;
        case 11: return pearlescent;
        case 12: return metallic;
        case 13: return metalsnow;
        case 14: return glasswindow;
        case 15: return c4_gift;
        case 16: return urban_puddle;
        case 17: return crystal_cube;
        case 18: return ghost1;
        case 19: return zombie;
        case 20: return searchlight;
        case 21: return brokenglass;
        case 22: return crystal_blue;
        case 23: return velvet;
        case 24: return water;
        case 25: return metalwall;
        case 26: return whiteboard01;
        case 27: return whiteboard04;
        }
    }

    bool appliedChams;
    void* ctx;
    void* state;
    const ModelRenderInfo* info;
    matrix3x4* customBoneToWorld;

    void applyChams(const std::array<Config::Material, 10>& chams, int health = 0, matrix3x4* customMatrix = nullptr) noexcept;
};


namespace iden{
    extern bool HasBeenCalled;
}

//namespace ChamHold {
//    extern class Chams chams;
//}









/*
#pragma once

#include "../Config.h"

struct ModelRenderInfo;
class matrix3x4;
class Material;

class Chams {
public:
    Chams() noexcept;
    bool render(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;
private:
    bool renderPlayers(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;
    void renderWeapons(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;
    void renderHands(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;
    void renderSleeves(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;
    /*
    enum ChamsId {
        ALLIES_ALL = 0,
        ALLIES_VISIBLE,
        ALLIES_OCCLUDED,

        ENEMIES_ALL,
        ENEMIES_VISIBLE,
        ENEMIES_OCCLUDED,

        PLANTING_ALL,
        PLANTING_VISIBLE,
        PLANTING_OCCLUDED,

        DEFUSING_ALL,
        DEFUSING_VISIBLE,
        DEFUSING_OCCLUDED,

        LOCALPLAYER,
        WEAPONS,
        HANDS,
        BACKTRACK,
        SLEEVES,
        DESYNC
    };
    *//*
    enum ChamsId {
        ALLIES = 0,
        ENEMIES,
        PLANTING,
        DEFUSING,
        LOCALPLAYER,
        WEAPONS,
        HANDS,
        BACKTRACK,
        SLEEVES,
        DESYNC
    };
    Material* normal;
    Material* flat;
    Material* animated;
    Material* platinum;
    Material* glass;
    Material* crystal;
    Material* chrome;
    Material* silver;
    Material* gold;
    Material* plastic;
    Material* glow;
    Material* pearlescent;
    Material* metallic;

    constexpr auto dispatchMaterial(int id) const noexcept
    {
        switch (id) {
        default:
        case 0: return normal;
        case 1: return flat;
        case 2: return animated;
        case 3: return platinum;
        case 4: return glass;
        case 5: return chrome;
        case 6: return crystal;
        case 7: return silver;
        case 8: return gold;
        case 9: return plastic;
        case 10: return glow;
        case 11: return pearlescent;
        case 12: return metallic;
        }
    }

    void applyChams(const Config::Chams::Material& chams, bool ignorez, int health = 0) const noexcept;
};

*/
/*

#pragma once

#include <vector>

#include "../Config.h"

class Entity;
struct ModelRenderInfo;
class matrix3x4;
class Material;

class Chams {
public:
    Chams() noexcept;
    bool render(void*, void*, const ModelRenderInfo&, matrix3x4*) noexcept;
private:
    void renderPlayer(Entity* player) noexcept;
    void renderWeapons() noexcept;
    void renderHands() noexcept;
    void renderSleeves() noexcept;

    enum ChamsId {
        ALLIES = 0,
        ENEMIES,
        PLANTING,
        DEFUSING,
        LOCALPLAYER,
        WEAPONS,
        HANDS,
        BACKTRACK,
        SLEEVES,
        DESYNC
    };

    Material* normal;
    Material* flat;
    Material* animated;
    Material* platinum;
    Material* glass;
    Material* crystal;
    Material* chrome;
    Material* silver;
    Material* gold;
    Material* plastic;
    Material* glow;
    Material* pearlescent;
    Material* metallic;

    constexpr auto dispatchMaterial(int id) const noexcept
    {
        switch (id) {
        default:
        case 0: return normal;
        case 1: return flat;
        case 2: return animated;
        case 3: return platinum;
        case 4: return glass;
        case 5: return chrome;
        case 6: return crystal;
        case 7: return silver;
        case 8: return gold;
        case 9: return plastic;
        case 10: return glow;
        case 11: return pearlescent;
        case 12: return metallic;
        }
    }

    bool appliedChams;
    void* ctx;
    void* state;
    const ModelRenderInfo* info;
    matrix3x4* customBoneToWorld;

    void applyChams(const std::vector<Config::Chams::Material>& chams, int health = 0, matrix3x4* customMatrix = nullptr) noexcept;
};
*/
#endif