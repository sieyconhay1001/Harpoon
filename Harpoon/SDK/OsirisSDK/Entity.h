#pragma once

#include "AnimState.h"
#include "ClientClass.h"
#include "Cvar.h"
#include "Engine.h"
#include "EngineTrace.h"
#include "EntityList.h"
#include "LocalPlayer.h"
#include "matrix3x4.h"
#include "ModelRender.h"
#include "Utils.h"
#include "VarMapping.h"
#include "Vector.h"
//#include <vector>
#include "VirtualMethod.h"
#include "WeaponData.h"
#include "WeaponId.h"
#include "StudioRender.h"
#include "AnimState.h"

#include "../../Config.h"
#include "../../Interfaces.h"
#include "../../Memory.h"
#include "../../Netvars.h"
#include "../../MemAlloc.h"


#include <functional>

// m_lifeState values
#define	LIFE_ALIVE				0 // alive
#define	LIFE_DYING				1 // playing death animation or still falling off of a ledge waiting to hit ground
#define	LIFE_DEAD				2 // dead. lying still.
#define LIFE_RESPAWNABLE		3
#define LIFE_DISCARDBODY		4

struct AnimState;

struct AnimationLayer
{
public:
    bool clientblend;
    float blendin;
    StudioHdr* studioHdr;
    int dispatchedsrc;
    int dispatcheddst;
    unsigned int order;
    unsigned int sequence;
    float prevcycle;
    float weight;
    float weightdeltarate;
    float playbackRate;
    float cycle;
    void* owner;
    int invalidatephysicsbits;
};

enum class MoveType {
    MOVETYPE_NONE = 0,	// never moves
    MOVETYPE_ISOMETRIC,			// For players -- in TF2 commander view, etc.
    MOVETYPE_WALK,				// Player only - moving on the ground
    MOVETYPE_STEP,				// gravity, special edge handling -- monsters use this
    MOVETYPE_FLY,				// No gravity, but still collides with stuff
    MOVETYPE_FLYGRAVITY,		// flies through the air + is affected by gravity
    MOVETYPE_VPHYSICS,			// uses VPHYSICS for simulation
    MOVETYPE_PUSH,				// no clip to world, push and crush
    NOCLIP = 8,
    LADDER = 9,
    MOVETYPE_NOCLIP = 8,
    MOVETYPE_LADDER = 9,
    MOVETYPE_OBSERVER,			// Observer movement, depends on player's observer mode
    MOVETYPE_CUSTOM,			// Allows the entity to describe its own physics

    // should always be defined as the last item in the list
    MOVETYPE_LAST = MOVETYPE_CUSTOM,

    MOVETYPE_MAX_BITS = 4
};

// edict->movecollide values
enum MoveCollide_t
{
    MOVECOLLIDE_DEFAULT = 0,

    // These ones only work for MOVETYPE_FLY + MOVETYPE_FLYGRAVITY
    MOVECOLLIDE_FLY_BOUNCE,	// bounces, reflects, based on elasticity of surface and object - applies friction (adjust velocity)
    MOVECOLLIDE_FLY_CUSTOM,	// Touch() will modify the velocity however it likes
    MOVECOLLIDE_FLY_SLIDE,  // slides along surfaces (no bounce) - applies friciton (adjusts velocity)

    MOVECOLLIDE_COUNT,		// Number of different movecollides

    // When adding new movecollide types, make sure this is correct
    MOVECOLLIDE_MAX_BITS = 3
};

// edict->solid values
// NOTE: Some movetypes will cause collisions independent of SOLID_NOT/SOLID_TRIGGER when the entity moves
// SOLID only effects OTHER entities colliding with this one when they move - UGH!

// Solid type basically describes how the bounding volume of the object is represented
// NOTE: SOLID_BBOX MUST BE 2, and SOLID_VPHYSICS MUST BE 6
// NOTE: These numerical values are used in the FGD by the prop code (see prop_dynamic)
enum SolidType_t
{
    SOLID_NONE = 0,	// no solid model
    SOLID_BSP = 1,	// a BSP tree
    SOLID_BBOX = 2,	// an AABB
    SOLID_OBB = 3,	// an OBB (not implemented yet)
    SOLID_OBB_YAW = 4,	// an OBB, constrained so that it can only yaw
    SOLID_CUSTOM = 5,	// Always call into the entity for tests
    SOLID_VPHYSICS = 6,	// solid vphysics object, get vcollide from the model and collide with that
    SOLID_LAST,
};

enum SolidFlags_t
{
    FSOLID_CUSTOMRAYTEST = 0x0001,	// Ignore solid type + always call into the entity for ray tests
    FSOLID_CUSTOMBOXTEST = 0x0002,	// Ignore solid type + always call into the entity for swept box tests
    FSOLID_NOT_SOLID = 0x0004,	// Are we currently not solid?
    FSOLID_TRIGGER = 0x0008,	// This is something may be collideable but fires touch functions
                                            // even when it's not collideable (when the FSOLID_NOT_SOLID flag is set)
                                            FSOLID_NOT_STANDABLE = 0x0010,	// You can't stand on this
                                            FSOLID_VOLUME_CONTENTS = 0x0020,	// Contains volumetric contents (like water)
                                            FSOLID_FORCE_WORLD_ALIGNED = 0x0040,	// Forces the collision rep to be world-aligned even if it's SOLID_BSP or SOLID_VPHYSICS
                                            FSOLID_USE_TRIGGER_BOUNDS = 0x0080,	// Uses a special trigger bounds separate from the normal OBB
                                            FSOLID_ROOT_PARENT_ALIGNED = 0x0100,	// Collisions are defined in root parent's local coordinate space
                                            FSOLID_TRIGGER_TOUCH_DEBRIS = 0x0200,	// This trigger will touch debris objects
                                            FSOLID_TRIGGER_TOUCH_PLAYER = 0x0400,	// This trigger will touch only players
                                            FSOLID_NOT_MOVEABLE = 0x0800,	// Assume this object will not move

                                            FSOLID_MAX_BITS = 12
};

enum Collision_Group_t
{
    COLLISION_GROUP_NONE = 0,
    COLLISION_GROUP_DEBRIS,			// Collides with nothing but world and static stuff
    COLLISION_GROUP_DEBRIS_TRIGGER, // Same as debris, but hits triggers
    COLLISION_GROUP_INTERACTIVE_DEBRIS,	// Collides with everything except other interactive debris or debris
    COLLISION_GROUP_INTERACTIVE,	// Collides with everything except interactive debris or debris
    COLLISION_GROUP_PLAYER,
    COLLISION_GROUP_BREAKABLE_GLASS,
    COLLISION_GROUP_VEHICLE,
    COLLISION_GROUP_PLAYER_MOVEMENT,  // For HL2, same as Collision_Group_Player, for
                                        // TF2, this filters out other players and CBaseObjects
                                        COLLISION_GROUP_NPC,			// Generic NPC group
                                        COLLISION_GROUP_IN_VEHICLE,		// for any entity inside a vehicle
                                        COLLISION_GROUP_WEAPON,			// for any weapons that need collision detection
                                        COLLISION_GROUP_VEHICLE_CLIP,	// vehicle clip brush to restrict vehicle movement
                                        COLLISION_GROUP_PROJECTILE,		// Projectiles!
                                        COLLISION_GROUP_DOOR_BLOCKER,	// Blocks entities not permitted to get near moving doors
                                        COLLISION_GROUP_PASSABLE_DOOR,	// Doors that the player shouldn't collide with
                                        COLLISION_GROUP_DISSOLVING,		// Things that are dissolving are in this group
                                        COLLISION_GROUP_PUSHAWAY,		// Nonsolid on client and server, pushaway in player code

                                        COLLISION_GROUP_NPC_ACTOR,		// Used so NPCs in scripts ignore the player.
                                        COLLISION_GROUP_NPC_SCRIPTED,	// USed for NPCs in scripts that should not collide with each other
                                        COLLISION_GROUP_PZ_CLIP,

                                        COLLISION_GROUP_DEBRIS_BLOCK_PROJECTILE, // Only collides with bullets

                                        LAST_SHARED_COLLISION_GROUP
};


enum class ObsMode {
    None = 0,
    Deathcam,
    Freezecam,
    Fixed,
    InEye,
    Chase,
    Roaming
};

enum {
    OBS_MODE_NONE = 0,	// not in spectator mode
    OBS_MODE_DEATHCAM,	// special mode for death cam animation
    OBS_MODE_FREEZECAM,	// zooms to a target, and freeze-frames on them
    OBS_MODE_FIXED,		// view from a fixed camera position
    OBS_MODE_IN_EYE,	// follow a player in first person view
    OBS_MODE_CHASE,		// follow a player in third person view
    OBS_MODE_POI,		// PASSTIME point of interest - game objective, big fight, anything interesting; added in the middle of the enum due to tons of hard-coded "<ROAMING" enum compares
    OBS_MODE_ROAMING,	// free roaming

    NUM_OBSERVER_MODES,
};

class Collideable {
public:
    VIRTUAL_METHOD(const Vector&, obbMins, 1, (), (this))
    VIRTUAL_METHOD(const Vector&, obbMaxs, 2, (), (this))
};


#include "matrix3x4.h"

class BoneAccessor {
public:
    void* m_pAnimating;
    matrix3x4 m_pBones[256];
    int        m_ReadableBones;
    int        m_WritableBones;
};

class Entity {
public:
    VIRTUAL_METHOD(void, release, 1, (), (this + 8))
    VIRTUAL_METHOD(ClientClass*, getClientClass, 2, (), (this + 8))
    VIRTUAL_METHOD(void, preDataUpdate, 6, (int updateType), (this + 8, updateType))
    VIRTUAL_METHOD(void, postDataUpdate, 7, (int updateType), (this + 8, updateType))
    VIRTUAL_METHOD(bool, isDormant, 9, (), (this + 8))
    VIRTUAL_METHOD(int, index, 10, (), (this + 8))
    VIRTUAL_METHOD(void, setDestroyedOnRecreateEntities, 13, (), (this + 8))

    VIRTUAL_METHOD(void, DrawModel, 9, (int nFlags, uint8_t nAlpha), (this + 4, nFlags, nAlpha))
    VIRTUAL_METHOD(Vector&, getRenderOrigin, 1, (), (this + 4))

    VIRTUAL_METHOD(const Model*, getModel, 8, (), (this + 4))
    VIRTUAL_METHOD(const matrix3x4&, toWorldTransform, 32, (), (this + 4))

    VIRTUAL_METHOD(int&, handle, 2, (), (this))
    VIRTUAL_METHOD(Collideable*, getCollideable, 3, (), (this))
    VIRTUAL_METHOD(Vector&, getAbsOrigin, 10, (), (this))
    VIRTUAL_METHOD(Vector&, getAbsAngle, 11, (), (this))
    VIRTUAL_METHOD(void, setModelIndex, 75, (int index), (this, index))
    VIRTUAL_METHOD(int, health, 121, (), (this))
    VIRTUAL_METHOD(bool, isAlive, 155, (), (this))
    VIRTUAL_METHOD(bool, isPlayer, 157, (), (this))
    VIRTUAL_METHOD(bool, isWeapon, 165, (), (this))



    // void __fastcall DoExtraBoneProcessing(Entity* player, uint32_t, StudioHdr* hdr, Vector* pos, quaternion* q, matrix3x4* matrix, void* bone_list, void* context)
    //VIRTUAL_METHOD(bool, DoExtraBoneProcessing, 197, (), (this, uint32_t, StudioHdr* hdr, Vector pos, float[4]  q, matrix3x4* matrix, void* bone_list, void* context))
    VIRTUAL_METHOD(Entity*, getActiveWeapon, 267, (), (this))
    VIRTUAL_METHOD(int, getWeaponSubType, 281, (), (this))
    VIRTUAL_METHOD(ObsMode, getObserverMode, 293, (), (this))
    VIRTUAL_METHOD(Entity*, getObserverTarget, 294, (), (this))
    VIRTUAL_METHOD(WeaponType, getWeaponType, 454, (), (this))
    VIRTUAL_METHOD(WeaponInfo*, getWeaponData, 460, (), (this))
    VIRTUAL_METHOD(float, getInaccuracy, 482, (), (this))
    VIRTUAL_METHOD(void, UpdateClientSideAnimation, 223, (), (this))
        
    VIRTUAL_METHOD(void, UpdateAccuracyPenalty, 483, (), (this))
    VIRTUAL_METHOD(float, getSpread, 452, (), (this))


    VIRTUAL_METHOD(void, SetModelIndex, 75, (int nIndex), (this, nIndex))
    VIRTUAL_METHOD(bool, InitializeAsClientEntity, 96, (const char* modelName, int renderGroup),(this, modelName, renderGroup))






    constexpr auto isPistol() noexcept
    {
        return getWeaponType() == WeaponType::Pistol;
    }


    constexpr auto isPrimary() noexcept
    {
        auto type = getWeaponType();

        return (type == WeaponType::Rifle) || (type == WeaponType::Machinegun) || (type == WeaponType::SubMachinegun) || (type == WeaponType::Shotgun) || (type == WeaponType::SniperRifle);
    }

    constexpr auto isBomb() noexcept
    {
        return getWeaponType() == WeaponType::C4;
    }


    constexpr auto isRevolver() noexcept
    {
        return ((getWeaponType() == WeaponType::Pistol) && (itemDefinitionIndex2() == WeaponId::Revolver));
    }

    /* Modified File */
    constexpr auto isFullAuto() noexcept
    {
        const auto weaponData = getWeaponData();
        if (weaponData)
            return weaponData->fullAuto;
        return false;
    }

    constexpr auto isGrenade() noexcept
    {
        return getWeaponType() == WeaponType::Grenade;
    }

    constexpr auto isKnife() noexcept
    {
        return getWeaponType() == WeaponType::Knife;
    }



    bool hasBomb() noexcept
    {
            for (auto nWeapon : this->weapons()) {
                if (nWeapon) {
                    Entity* pWeapon = interfaces->entityList->getEntityFromHandle(nWeapon);
                    if (pWeapon && !pWeapon->isDormant() && pWeapon->isWeapon() && pWeapon->isBomb()) {
                        return true;
                    }
                }
            }
            return false;
    }

    bool hasRevolver() noexcept
    {
        for (auto nWeapon : this->weapons()) {
            if (nWeapon) {
                Entity* pWeapon = interfaces->entityList->getEntityFromHandle(nWeapon);
                if (pWeapon && !pWeapon->isDormant() && pWeapon->isWeapon() && pWeapon->isPistol() && pWeapon->isRevolver()) {
                    return true;
                }
            }
        }
        return false;
    }


    constexpr auto isSniperRifle() noexcept
    {
        return getWeaponType() == WeaponType::SniperRifle;
    }
    /*End Of Modified*/
    constexpr auto requiresRecoilControl() noexcept
    {
        const auto weaponData = getWeaponData();
        if (weaponData)
            return weaponData->recoilMagnitude < 35.0f && weaponData->recoveryTimeStand > weaponData->cycletime;
        return false;
    }

    bool setupBones(matrix3x4* out, int maxBones, int boneMask, float currentTime) noexcept
    {
        if (localPlayer && this == localPlayer.get() && localPlayer->isAlive())
        {
            uint32_t* effects = (uint32_t*)((uintptr_t)this + 0xF0);
            uint32_t* shouldskipframe = (uint32_t*)((uintptr_t)this + 0xA68);
            uint32_t backup_effects = *effects;
            uint32_t backup_shouldskipframe = *shouldskipframe;
            *shouldskipframe = 0;
            *effects |= 8;
            auto result = VirtualMethod::call<bool, 13>(this + 4, out, maxBones, boneMask, currentTime);
            *effects = backup_effects;
            *shouldskipframe = backup_shouldskipframe;
            return result;
        }

        if (config->misc.fixBoneMatrix) {
            int* render = reinterpret_cast<int*>(this + 0x274);
            int backup = *render;
            Vector absOrigin = getAbsOrigin();
            *render = 0;
            memory->setAbsOrigin(this, origin());
            auto result = VirtualMethod::call<bool, 13>(this + 4, out, maxBones, boneMask, currentTime);
            memory->setAbsOrigin(this, absOrigin);
            *render = backup;
            return result;
        }
        return VirtualMethod::call<bool, 13>(this + 4, out, maxBones, boneMask, currentTime);
    }




    Vector getBonePosition(int bone) noexcept
    {
        if (matrix3x4 boneMatrices[256]; setupBones(boneMatrices, 256, 256, 0.0f))
            return Vector{ boneMatrices[bone][0][3], boneMatrices[bone][1][3], boneMatrices[bone][2][3] };
        else
            return Vector{ };
    }

    auto getEyePosition() noexcept
    {
        Vector vec;
        VirtualMethod::call<void, 284>(this, std::ref(vec));
        return vec;
    }

    bool isVisible(const Vector& position = { }) noexcept
    {
        if (!localPlayer)
            return false;

        Trace trace;
        interfaces->engineTrace->traceRay({ localPlayer->getEyePosition(), position.notNull() ? position : getBonePosition(8) }, 0x46004009, { localPlayer.get() }, trace);
        return trace.entity == this || trace.fraction > 0.97f;
    }
    
    bool isOtherEnemy(Entity* other) noexcept;

    VarMap* getVarMap() noexcept
    {
        return reinterpret_cast<VarMap*>(this + 0x24);
    }
   
    AnimState* getAnimstate() noexcept
    {
        return *reinterpret_cast<AnimState**>(this + 0x3914);
    }

    float getMaxDesyncAngle() noexcept
    {
        const auto animState = getAnimstate();

        if (!animState)
            return 0.0f;

        float yawModifier = (animState->stopToFullRunningFraction * -0.3f - 0.2f) * std::clamp(animState->footSpeed, 0.0f, 1.0f) + 1.0f;

        if (animState->duckAmount > 0.0f)
            yawModifier += (animState->duckAmount * std::clamp(animState->footSpeed2, 0.0f, 1.0f) * (0.5f - yawModifier));

        return animState->velocitySubtractY * yawModifier;
    }

    bool isInReload() noexcept
    {
        return *reinterpret_cast<bool*>(uintptr_t(&clip()) + 0x41);
    }

    auto getAimPunch() noexcept
    {
        Vector vec;
        VirtualMethod::call<void, 345>(this, std::ref(vec));
        return vec;
    }

    auto getUserId() noexcept
    {
        if (PlayerInfo playerInfo; interfaces->engine->getPlayerInfo(index(), playerInfo))
            return playerInfo.userId;

        return -1;
    }

    bool isBot() noexcept {
        if (PlayerInfo playerInfo; interfaces->engine->getPlayerInfo(index(), playerInfo))
            return playerInfo.fakeplayer;

        return false;
    }


    [[nodiscard]] auto getPlayerName(bool normalize) noexcept
    {
        std::string playerName = "unknown";

        PlayerInfo playerInfo;
        if (!interfaces->engine->getPlayerInfo(index(), playerInfo))
            return playerName;

        playerName = playerInfo.name;

        if (normalize) {
            if (wchar_t wide[128]; MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, 128, wide, 128)) {
                if (wchar_t wideNormalized[128]; NormalizeString(NormalizationKC, wide, -1, wideNormalized, 128)) {
                    if (char nameNormalized[128]; WideCharToMultiByte(CP_UTF8, 0, wideNormalized, -1, nameNormalized, 128, nullptr, nullptr))
                        playerName = nameNormalized;
                }
            }
        }

        playerName.erase(std::remove(playerName.begin(), playerName.end(), '\n'), playerName.cend());
        return playerName;
    }
    int getAnimationLayerCount() noexcept
    {
        return *reinterpret_cast<int*>(this + 0x298C);
    }

    AnimationLayer* animOverlays()
    {
        return *reinterpret_cast<AnimationLayer**>(uintptr_t(this) + 0x2980);
    }

    AnimationLayer* getAnimationLayer(int overlay) noexcept
    {
        return &animOverlays()[overlay];
    }

    int getSequenceActivity(int sequence) noexcept
    {
        auto hdr = interfaces->modelInfo->getStudioModel(this->getModel());

        if (!hdr)
            return 0 ;

        return memory->getSequenceActivity(this, hdr, sequence);
    }


    StudioHdr* GetStudioHdr() {
        auto hdr = interfaces->modelInfo->getStudioModel(this->getModel());

        if (!hdr)
            return nullptr;

        return hdr;
    }


    std::array<float, 24>& pose_parameters()
    {
        return *reinterpret_cast<std::add_pointer_t<std::array<float, 24>>>((uintptr_t)this + netvars->operator[](fnv::hash("CBaseAnimating->m_flPoseParameter")));
    }

    void CreateState(AnimState* state)
    {
        static auto CreateAnimState = reinterpret_cast<void(__thiscall*)(AnimState*, Entity*)>(memory->CreateState);
        if (!CreateAnimState)
            return;

        CreateAnimState(state, this);
    }

    void UpdateState(AnimState* state, Vector angle) {
        if (!state || !(angle.notNull()))
            return;
        static auto UpdateAnimState = reinterpret_cast<void(__vectorcall*)(void*, void*, float, float, float, void*)>(memory->UpdateState);
        if (!UpdateAnimState)
            return;
        UpdateAnimState(state, nullptr, 0.0f, angle.y, angle.x, nullptr);
    }

    float spawnTime()
    {
        return *(float*)((uintptr_t)this + 0xA370);
    }

    void InvalidateBoneCache()
    {
        static auto invalidate_bone_cache = memory->InvalidateBoneCache;
        reinterpret_cast<void(__fastcall*) (void*)> (invalidate_bone_cache) (this);
    }


    BoneAccessor* GetBoneAccessor()
    {
        return (BoneAccessor*)((char*)this + (int)9888);
    }

    matrix3x4* GetBoneCache()
    {

        return GetBoneAccessor()->m_pBones;
    }

    int GetBoneCount()
    {
        return GetBoneAccessor()->m_ReadableBones;
    }

    void setAbsAngle(Vector& Vec)
    {
        memory->setAbsAngle(this, Vec);
    }

    

    Vector* getAbsVelocity()
    {
        return (Vector*)(this + 0x94);
    }

    void setAbsOrigin(Vector& vec) {
        memory->setAbsOrigin(this, vec);
    }

    Vector getRotatedBonePos(float rotation, int bone = 8) {
        Vector bone_pos = this->getBonePosition(bone);
        Vector origin = this->origin();
        Vector bone_offset = bone_pos - origin;
        float radius = bone_offset.length2D();
        float radian = rotation * 3.1415926536f;
        bone_offset.x = std::cos(radian) * radius;
        bone_offset.y = std::sin(radian) * radius;
        bone_offset += origin;
        return bone_offset;
    }

    uint32_t* getEffects()
    {
        return (uint32_t*)((uintptr_t)this + 0xF0);
    }


    bool canSeePoint(const Vector& position = { }) noexcept
    {
        Trace trace;
        interfaces->engineTrace->traceRay({ getEyePosition(), position}, 0x46004009, { this }, trace);
        if (trace.allSolid || trace.startSolid)
            return false;

        return (trace.fraction > 0.97f);
    }
    
    bool feetCanSeePoint(const Vector& position = { }) noexcept
    {
        Trace trace;
        Vector Org = origin();
        Org.z += 8;
        interfaces->engineTrace->traceRay({ Org, position }, 0x46004009, { this }, trace);
        if (trace.allSolid || trace.startSolid)
            return false;

        return (trace.fraction > 0.97f);
    }



    bool directPathToPoint(const Vector& position = { }) noexcept {
        return canSeePoint(position) && feetCanSeePoint(position);
    }



    /*
            Look, this aint good code. Its "temporary" ya know?
    
    */
    bool canSeePointRage(const Vector& position = { }) noexcept
    {
        Trace trace;
        bool visible = false;

        Vector ObbMin = origin();
        Vector ObbMax = origin();
        ObbMin += localPlayer->getCollideable()->obbMins().x * 2.0f;
        ObbMax += localPlayer->getCollideable()->obbMaxs().x * 2.0f;
        ObbMin += localPlayer->getCollideable()->obbMins().y * 2.0f;
        ObbMax += localPlayer->getCollideable()->obbMaxs().y * 2.0f;
        float mid = (localPlayer->getCollideable()->obbMaxs().z + localPlayer->getCollideable()->obbMins().z) / 2.0f;
        ObbMin.z += mid;
        ObbMax.z += mid;

        Vector points[] = { ObbMin, ObbMax };

        for (auto point : points) {
            interfaces->engineTrace->traceRay({ point , position }, 0x46004009, { this }, trace);
            if (trace.allSolid || trace.startSolid) {
                visible = false;
                continue;
            }
            if (trace.fraction > 0.97f) {
                visible = true;
                return visible;
            }
            visible = false;
        }
        return visible;
    }


    float PointToPoint(const Vector& positionSRC = { }, const Vector& positionDest = { }) noexcept
    {
        try {
            Trace trace;
            //0x46004009
            interfaces->engineTrace->traceRay({ positionSRC, positionDest }, 33570827, { this }, trace);
            if (trace.allSolid || trace.startSolid)
                return 0.0;


            return trace.fraction;
        } catch (std::exception& e){
            return 0.0;
        }
    }



    int findnearestWall() {
        
        float fraction = .97f;
        float temp_fraction = 2.1f;
        float offset = -1;
        for (int i = 90; i < 360; i += 180) {
            Vector point = getRotatedBonePos((i * 22.0) / (180.0 * 7.0));
            Vector WallCheck = point;
            WallCheck.y += getCollideable()->obbMaxs().y * 2.0;
            temp_fraction = PointToPoint(point, WallCheck);

            if (temp_fraction < fraction) {
                offset = i;
                fraction = temp_fraction;
                
            }
            WallCheck.y += getCollideable()->obbMins().y * 4.0;
            temp_fraction = PointToPoint(point, WallCheck);
            if (temp_fraction < fraction) {
                offset = i;
                fraction = temp_fraction;
            }
            
            WallCheck = point;
            WallCheck.x += getCollideable()->obbMaxs().x * 2.0;
            temp_fraction = PointToPoint(point, WallCheck);

            if (temp_fraction < fraction) {
                offset = i;
                fraction = temp_fraction;

            }
            WallCheck.x += getCollideable()->obbMins().x * 4.0;
            temp_fraction = PointToPoint(point, WallCheck);
            if (temp_fraction < fraction) {
                offset = i;
                fraction = temp_fraction;
            }
            
        }
        return offset;
    }
    using _BYTE = unsigned char;
    using _DWORD = uint32_t;
    void* GetPredDescMap() {
        void* map = (*(void*(__thiscall**)(_BYTE*))(*(_DWORD*)this + 68))((_BYTE*)this);
        return map;
    }

    int GetIntermediateDataSize()
    {
        (*(int(__thiscall**)(_BYTE*))(*(_DWORD*)this + 568))((_BYTE*)this);
        const void* map = GetPredDescMap();

        int size = *(_DWORD*)((_DWORD*)map + 16);

        // At least 4 bytes to avoid some really bad stuff
        return std::clamp(size, 4, INT_MAX);

    }

    _DWORD** GetOriginalFrame() {
        _DWORD* __this = (_DWORD*)this;
        return ((_DWORD**)__this + (_DWORD)2348);
    }

    void*  GetPredictedFrame(unsigned int a2)
    {
        void* result; // eax
        //
        _DWORD* __this = (_DWORD*)this;

        if (__this[587])
            result = (void*)__this[a2 % 0x96 + 286];
        else
            result = 0;
        return result;
    }

    float findnearestGap(Vector viewangles) {

        auto startPos = origin();
        startPos.z = (origin().z + getEyePosition().z) / 2;

        float fraction = 0.0f;
        float temp_fraction = 0.0f;
        float offset = viewangles.y;

        if (true) {
            Vector viewAngles{ std::cos(degreesToRadians(viewangles.x)) * std::cos(degreesToRadians(viewangles.y)) * 100.0,
                     std::cos(degreesToRadians(viewangles.x)) * std::sin(degreesToRadians(viewangles.y)) * 100.0,
                    -std::sin(degreesToRadians(viewangles.x)) };
            auto endPos = startPos + viewAngles;
            temp_fraction = PointToPoint(startPos, endPos);
            if (temp_fraction == 1.0f) {
                return viewangles.y;
            }
        }

        temp_fraction = 0.0f;

        for (int i = (viewangles.y-180); i < (viewangles.y + 180.0f); i+= 10.0f) {
            Vector viewAngles{ std::cos(degreesToRadians(viewangles.x )) * std::cos(degreesToRadians(viewangles.y + i)) * 100.0,
                                 std::cos(degreesToRadians(viewangles.x)) * std::sin(degreesToRadians(viewangles.y + i)) * 100.0,
                                -std::sin(degreesToRadians(viewangles.x)) };
            auto endPos = startPos + viewAngles;
            temp_fraction = PointToPoint(startPos, endPos);
            if (temp_fraction == 999)
                continue;
            if (temp_fraction == 1.0f)
                return i;
            else if (temp_fraction > fraction) {
                offset = i;
                fraction = temp_fraction;
            }

        }

        
        return offset;
    }


    

    void getPlayerName(char(&out)[128]) noexcept;
    bool canSee(Entity* other, const Vector& pos) noexcept;
    bool visibleTo(Entity* other) noexcept;

    NETVAR(ClientSideAnimation, "CBaseAnimating", "m_bClientSideAnimation", bool)

    NETVAR(body, "CBaseAnimating", "m_nBody", int)
    NETVAR(hitboxSet, "CBaseAnimating", "m_nHitboxSet", int)
    NETVAR(m_flCycle, "CBaseAnimating", "m_flCycle", float);
    NETVAR(m_nSequence, "CBaseAnimating", "m_flCycle", int);
    NETVAR(m_flModelScale, "CBaseAnimating", "m_flModelScale", float);
    NETVAR(modelIndex, "CBaseEntity", "m_nModelIndex", unsigned)
    NETVAR(origin, "CBaseEntity", "m_vecOrigin", Vector)
    NETVAR_OFFSET(moveType, "CBaseEntity", "m_nRenderMode", 1, MoveType)
    NETVAR(simulationTime, "CBaseEntity", "m_flSimulationTime", float)
    NETVAR(ownerEntity, "CBaseEntity", "m_hOwnerEntity", int)
    NETVAR(team, "CBaseEntity", "m_iTeamNum", int)
    NETVAR(spotted, "CBaseEntity", "m_bSpotted", bool)
    NETVAR(CoordinateFrame, "CBaseEntity", "m_rgflCoordinateFrame", matrix3x4)
    NETVAR(netOrigin, "CBaseEntity", "m_vecNetworkOrigin", Vector)
    //NETVAR(m_pOriginalData, "CBaseEntity", "m_pOriginalData", void*)
    NETVAR(weapons, "CBaseCombatCharacter", "m_hMyWeapons", int[64])
    PNETVAR(wearables, "CBaseCombatCharacter", "m_hMyWearables", int)

    NETVAR(viewModel, "CBasePlayer", "m_hViewModel[0]", int)
    NETVAR(fov, "CBasePlayer", "m_iFOV", int)
    NETVAR(fovStart, "CBasePlayer", "m_iFOVStart", int)
    NETVAR(defaultFov, "CBasePlayer", "m_iDefaultFOV", int)
    NETVAR(flags, "CBasePlayer", "m_fFlags", int)
    NETVAR(tickBase, "CBasePlayer", "m_nTickBase", int)
    NETVAR(aimPunchAngle, "CBasePlayer", "m_aimPunchAngle", Vector)
    NETVAR(viewPunchAngle, "CBasePlayer", "m_viewPunchAngle", Vector)
    NETVAR(m_vecViewOffset, "CBasePlayer", "m_vecViewOffset[0]", Vector);
    NETVAR(m_nTickBase, "CBasePlayer", "m_nTickBase", int32_t);
    NETVAR(lifeState, "CBasePlayer", "m_lifeState", int32_t);



 /*   
.rdata:00B11958 aCfogcontroller db 'CFogController',0   ; DATA XREF: .data:00D469CC↓o
.rdata:00B11967                 align 4
.rdata:00B11968 aMFogEnable     db 'm_fog.enable',0     ; DATA XREF: sub_21420+E1↑o
.rdata:00B11975                 align 4
.rdata:00B11978 aDtFogcontrolle db 'DT_FogController',0 ; DATA XREF: sub_21420+9A0↑o
.rdata:00B11989                 align 4
.rdata:00B1198C aMFogDirprimary db 'm_fog.dirPrimary',0 ; DATA XREF: sub_21420+1DF↑o
.rdata:00B1199D                 align 10h
.rdata:00B119A0 aMFogBlend      db 'm_fog.blend',0      ; DATA XREF: sub_21420+160↑o
.rdata:00B119AC aMFogColorsecon db 'm_fog.colorSecondary',0
.rdata:00B119AC                                         ; DATA XREF: sub_21420+2DD↑o
.rdata:00B119C1                 align 4
.rdata:00B119C4 aMFogColorprima db 'm_fog.colorPrimary',0
.rdata:00B119C4                                         ; DATA XREF: sub_21420+25E↑o
.rdata:00B119D7                 align 4
.rdata:00B119D8 aMFogEnd        db 'm_fog.end',0        ; DATA XREF: sub_21420+3DB↑o
.rdata:00B119E2                 align 4
.rdata:00B119E4 aMFogStart      db 'm_fog.start',0      ; DATA XREF: sub_21420+35C↑o
.rdata:00B119F0 aMFogMaxdensity db 'm_fog.maxdensity',0 ; DATA XREF: sub_21420+4D9↑o
.rdata:00B11A01                 align 4
.rdata:00B11A04 aMFogFarz       db 'm_fog.farz',0       ; DATA XREF: sub_21420+45A↑o
.rdata:00B11A0F                 align 10h
.rdata:00B11A10 aMFogColorsecon_0 db 'm_fog.colorSecondaryLerpTo',0
.rdata:00B11A10                                         ; DATA XREF: sub_21420+5D7↑o
.rdata:00B11A2B                 align 4
.rdata:00B11A2C aMFogColorprima_0 db 'm_fog.colorPrimaryLerpTo',0
.rdata:00B11A2C                                         ; DATA XREF: sub_21420+558↑o
.rdata:00B11A45                 align 4
.rdata:00B11A48 aMFogEndlerpto  db 'm_fog.endLerpTo',0  ; DATA XREF: sub_21420+6D5↑o
.rdata:00B11A58 aMFogStartlerpt db 'm_fog.startLerpTo',0
.rdata:00B11A58                                         ; DATA XREF: sub_21420+656↑o
.rdata:00B11A6A                 align 4
.rdata:00B11A6C aMFogLerptime   db 'm_fog.lerptime',0   ; DATA XREF: sub_21420+7D3↑o
.rdata:00B11A7B                 align 4
.rdata:00B11A7C aMFogMaxdensity_0 db 'm_fog.maxdensityLerpTo',0
.rdata:00B11A7C                                         ; DATA XREF: sub_21420+754↑o
.rdata:00B11A93                 align 4
.rdata:00B11A94 aMFogHdrcolorsc db 'm_fog.HDRColorScale',0
.rdata:00B11A94                                         ; DATA XREF: sub_21420+8D1↑o
.rdata:00B11AA8 aMFogDuration   db 'm_fog.duration',0   ; DATA XREF: sub_21420+852↑o
.rdata:00B11AB7                 align 4
.rdata:00B11AB8 aMFogZoomfogsca db 'm_fog.ZoomFogScale',0
.rdata:00B11AB8                                         ; DATA XREF: sub_21420+950↑o
.rdata:00B11ACB                 align 4
.rdata:00B11ACC

.....

.rdata:00B11AD8 ; const C_FogController::`vftable'
.rdata:00B11AD8 ??_7C_FogController@@6B@_0 dd offset sub_1A71F6
.rdata:00B11AD8                                         ; DATA XREF: sub_219AE0+22↑o
.rdata:00B11ADC                 dd offset sub_1E00C0
.rdata:00B11AE0                 dd offset sub_219A60
.rdata:00B11AE4                 dd offset sub_1E1850
.rdata:00B11AE8                 dd offset sub_1E38B0
.rdata:00B11AEC                 dd offset sub_1E38D0
.rdata:00B11AF0                 dd offset sub_1E18D0
.rdata:00B11AF4                 dd offset sub_1E2090
.rdata:00B11AF8                 dd offset sub_1E2610
.rdata:00B11AFC                 dd offset sub_1E43B0
.rdata:00B11B00                 dd offset sub_174EC0
.rdata:00B11B04                 dd offset sub_1E09A0
.rdata:00B11B08                 dd offset sub_1E0AD0
.rdata:00B11B0C                 dd offset sub_1E43C0


 BEGIN_SIMPLE_DATADESC( fogplayerparams_t )
    DEFINE_FIELD( m_hCtrl, FIELD_EHANDLE ),
    DEFINE_FIELD( m_flTransitionTime, FIELD_FLOAT ),
    DEFINE_FIELD( m_OldColor, FIELD_COLOR32 ),
    DEFINE_FIELD( m_flOldStart, FIELD_FLOAT ),
    DEFINE_FIELD( m_flOldEnd, FIELD_FLOAT ),
    DEFINE_FIELD( m_NewColor, FIELD_COLOR32 ),
    DEFINE_FIELD( m_flNewStart, FIELD_FLOAT ),
    DEFINE_FIELD( m_flNewEnd, FIELD_FLOAT ),
END_DATADESC()

BEGIN_SIMPLE_DATADESC( fogparams_t )
    DEFINE_FIELD( enable, FIELD_BOOLEAN ),
    DEFINE_FIELD( blend, FIELD_BOOLEAN ),
    DEFINE_FIELD( dirPrimary, FIELD_VECTOR ),
    DEFINE_FIELD( colorPrimary, FIELD_COLOR32 ),
    DEFINE_FIELD( colorSecondary, FIELD_COLOR32 ),
    DEFINE_FIELD( start, FIELD_FLOAT ),
    DEFINE_FIELD( end, FIELD_FLOAT ),
    DEFINE_FIELD( farz, FIELD_FLOAT ),
    DEFINE_FIELD( maxdensity, FIELD_FLOAT ),
    DEFINE_FIELD( colorPrimaryLerpTo, FIELD_COLOR32 ),
    DEFINE_FIELD( colorSecondaryLerpTo, FIELD_COLOR32 ),
    DEFINE_FIELD( startLerpTo, FIELD_FLOAT ),
    DEFINE_FIELD( endLerpTo, FIELD_FLOAT ),
    DEFINE_FIELD( lerptime, FIELD_TIME ),
    DEFINE_FIELD( duration, FIELD_FLOAT ),
END_DATADESC()

BEGIN_SIMPLE_DATADESC( sky3dparams_t )
    DEFINE_FIELD( scale, FIELD_INTEGER ),
    DEFINE_FIELD( origin, FIELD_VECTOR ),
    DEFINE_FIELD( area, FIELD_INTEGER ),
    DEFINE_EMBEDDED( fog ),
END_DATADESC()


*/
struct m_fog {
    bool enable;
    bool blend;
    Vector dirPrimary;
};

    NETVAR(m_fog_enable, "CFogController", "m_fog.enable", bool)
    NETVAR(m_fog_blend, "CFogController", "m_fog.blend", bool)
    NETVAR(m_fog_colorPrimary, "CFogController", "m_fog.colorPrimary", uint32_t)
    NETVAR(m_fog_maxdensity, "CFogController", "m_fog.maxdensity", float)
        //float m_flSunDistance;

    NETVAR(m_flSunDistance, "CSunlightShadowControl", "m_flSunDistance", float)

    NETVAR(velocity, "CBasePlayer", "m_vecVelocity[0]", Vector)

    NETVAR(lastPlaceName, "CBasePlayer", "m_szLastPlaceName", char[18])
    NETVAR(observer_handle, "CBasePlayer", "m_hObserverTarget", Entity*);
    NETVAR(m_iObserverMode, "CBasePlayer", "m_iObserverMode", int);
    NETVAR(armor, "CCSPlayer", "m_ArmorValue", int)
    NETVAR(eyeAngles, "CCSPlayer", "m_angEyeAngles", Vector)
    NETVAR(isScoped, "CCSPlayer", "m_bIsScoped", bool)
    NETVAR(isDefusing, "CCSPlayer", "m_bIsDefusing", bool)
    NETVAR_OFFSET(flashDuration, "CCSPlayer", "m_flFlashMaxAlpha", -8, float)
    NETVAR(flashMaxAlpha, "CCSPlayer", "m_flFlashMaxAlpha", float)
    NETVAR(gunGameImmunity, "CCSPlayer", "m_bGunGameImmunity", bool)
    NETVAR(account, "CCSPlayer", "m_iAccount", int)
    NETVAR(inBombZone, "CCSPlayer", "m_bInBombZone", bool)
    NETVAR(hasDefuser, "CCSPlayer", "m_bHasDefuser", bool)
    NETVAR(hasHelmet, "CCSPlayer", "m_bHasHelmet", bool)
    NETVAR(lby, "CCSPlayer", "m_flLowerBodyYawTarget", float)
    NETVAR(ragdoll, "CCSPlayer", "m_hRagdoll", int)
    NETVAR(shotsFired, "CCSPlayer", "m_iShotsFired", int)

    NETVAR(viewModelIndex, "CBaseCombatWeapon", "m_iViewModelIndex", int)
    NETVAR(worldModelIndex, "CBaseCombatWeapon", "m_iWorldModelIndex", int)
    NETVAR(worldDroppedModelIndex, "CBaseCombatWeapon", "m_iWorldDroppedModelIndex", int)
    NETVAR(weaponWorldModel, "CBaseCombatWeapon", "m_hWeaponWorldModel", int)
    NETVAR(clip, "CBaseCombatWeapon", "m_iClip1", int)
    NETVAR(reserveAmmoCount, "CBaseCombatWeapon", "m_iPrimaryReserveAmmoCount", int)
    NETVAR(nextPrimaryAttack, "CBaseCombatWeapon", "m_flNextPrimaryAttack", float)
    NETVAR(recoilIndex, "CBaseCombatWeapon", "m_flRecoilIndex", float)
    NETVAR(m_bPinPulled, "CBaseCSGrenade", "m_bPinPulled", bool)
    NETVAR(m_fThrowTime, "CBaseCSGrenade", "m_fThrowTime", float_t)


    NETVAR(nextAttack, "CBaseCombatCharacter", "m_flNextAttack", float)

    NETVAR(accountID, "CBaseAttributableItem", "m_iAccountID", int)
    NETVAR(itemDefinitionIndex, "CBaseAttributableItem", "m_iItemDefinitionIndex", short)
    NETVAR(itemDefinitionIndex2, "CBaseAttributableItem", "m_iItemDefinitionIndex", WeaponId)
    NETVAR(itemIDHigh, "CBaseAttributableItem", "m_iItemIDHigh", int)
    NETVAR(entityQuality, "CBaseAttributableItem", "m_iEntityQuality", int)
    NETVAR(customName, "CBaseAttributableItem", "m_szCustomName", char[32])
    NETVAR(fallbackPaintKit, "CBaseAttributableItem", "m_nFallbackPaintKit", unsigned)
    NETVAR(fallbackSeed, "CBaseAttributableItem", "m_nFallbackSeed", unsigned)
    NETVAR(fallbackWear, "CBaseAttributableItem", "m_flFallbackWear", float)
    NETVAR(fallbackStatTrak, "CBaseAttributableItem", "m_nFallbackStatTrak", unsigned)
    NETVAR(initialized, "CBaseAttributableItem", "m_bInitialized", bool)



    NETVAR(owner, "CBaseViewModel", "m_hOwner", int)
    NETVAR(weapon, "CBaseViewModel", "m_hWeapon", int)

    NETVAR(c4StartedArming, "CC4", "m_bStartedArming", bool)

    NETVAR(c4BlowTime, "CPlantedC4", "m_flC4Blow", float)
    NETVAR(c4BombSite, "CPlantedC4", "m_nBombSite", int)
    NETVAR(c4Ticking, "CPlantedC4", "m_bBombTicking", bool)
    NETVAR(c4DefuseCountDown, "CPlantedC4", "m_flDefuseCountDown", float)
    NETVAR(c4Defuser, "CPlantedC4", "m_hBombDefuser", int)

    NETVAR(tabletReceptionIsBlocked, "CTablet", "m_bTabletReceptionIsBlocked", bool)
    
    NETVAR(droneTarget, "CDrone", "m_hMoveToThisEntity", int)
    NETVAR(waitForNoAttack, "CCSPlayer", "m_bWaitForNoAttack", bool)

    
    NETVAR(totalRoundsPlayed, "CCSGameRulesProxy","m_totalRoundsPlayed", int)
    NETVAR(WarmupPeriod, "CCSGameRulesProxy", "m_bWarmupPeriod", bool)
    NETVAR(HasMatchStarted, "CCSGameRulesProxy", "m_bHasMatchStarted", bool)
    NETVAR(FreezePeriod, "CCSGameRulesProxy", "m_bFreezePeriod", bool)      
    NETVAR(m_bIsValveDS, "CCSGameRulesProxy", "m_bIsValveDS", bool)


    NETVAR_OFFSET(CompetitiveRanking, "CCSPlayerResource", "m_iCompetitiveRanking", 4, int);
    NETVAR_OFFSET(CompetitiveWins, "CCSPlayerResource", "m_iCompetitiveWins", 4, int);

    NETVAR(thrower, "CBaseGrenade", "m_hThrower", int)

    bool isFlashed() noexcept
    {
        return flashDuration() > 75.0f;
    }

    bool grenadeExploded() noexcept
    {
        return *reinterpret_cast<bool*>(this + 0x29E8);
    }

    bool isInThrow() noexcept {
        if (!m_bPinPulled()) {
            float throwTime = m_fThrowTime();
            if (throwTime > 0)
                return 1;
        }
        return 0;
    }

    bool IsThrowingGrenade() noexcept {
        if (getActiveWeapon() && getActiveWeapon()->isGrenade() && getActiveWeapon()->isInThrow()) {
            return true;
        }
        return false;
    }

    bool HasPinPulledGrenade() noexcept {
        if (getActiveWeapon() && getActiveWeapon()->isGrenade() && getActiveWeapon()->m_bPinPulled()) {
            return true;
        }
        return false;
    }

    Vector getVelocity() noexcept {
        return velocity();
    }

    matrix3x4 EntityToWorldTransform(){
        /*
        	Assert( CBaseEntity::IsAbsQueriesValid() );

	        if (IsEFlagSet(EFL_DIRTY_ABSTRANSFORM))
	        {
		        CalcAbsolutePosition(); // <---- Need to RE this function
	        }
        
        
        */

        return CoordinateFrame();
    }

};
#include "../../MemAlloc.h"

inline Entity* MakeBaseFlex(const char* szModelPath) {
    //std::uintptr_t callAddr = (std::uintptr_t)memory->findPattern_ex(L"client", "\xE8????\xC7\x87????????\x8D\x8F????\xC7\x07????");
    //std::uintptr_t funcAddr = callAddr + 1 + sizeof(std::uintptr_t) + *(std::uintptr_t*)(callAddr + 1);
    Entity* pBaseAnimating = reinterpret_cast<Entity*>(memory->memalloc->Alloc(15000));   

    if (!pBaseAnimating)
        return nullptr;
    memset(pBaseAnimating, 0, 15000);

    
    reinterpret_cast<void(__thiscall*)(Entity*)>(memory->CBaseAnimatingConstructor)(pBaseAnimating);

    //reinterpret_cast<void(__thiscall*)(void*)>(funcAddr)(pFlex);
    if (!pBaseAnimating->InitializeAsClientEntity(szModelPath, 1))
    {
        memory->memalloc->Free((void*)pBaseAnimating);
        return nullptr;
    }
    return pBaseAnimating;
}




enum Contents
{
    contents_empty = 0x0,
    contents_solid = 0x1,
    contents_window = 0x2,
    contents_aux = 0x4,
    contents_grate = 0x8,
    contents_slime = 0x10,
    contents_water = 0x20,
    contents_blocklos = 0x40,
    contents_opaque = 0x80,
    contents_testfogvolume = 0x100,
    contents_unused = 0x200,
    contents_blocklight = 0x400,
    contents_team1 = 0x800,
    contents_team2 = 0x1000,
    contents_ignore_nodraw_opaque = 0x2000,
    contents_moveable = 0x4000,
    contents_areaportal = 0x8000,
    contents_playerclip = 0x10000,
    contents_monsterclip = 0x20000,
    contents_current0 = 0x40000,
    contents_current90 = 0x80000,
    contents_current180 = 0x100000,
    contents_current270 = 0x200000,
    contents_current_up = 0x400000,
    contents_current_down = 0x800000,
    contents_origin = 0x1000000,
    contents_monster = 0x2000000,
    contents_debris = 0x4000000,
    contents_detail = 0x8000000,
    contents_translucent = 0x10000000,
    contents_ladder = 0x20000000,
    contents_hitbox = 0x40000000,

    last_visible_contents = contents_opaque,
    all_visible_contents = last_visible_contents | last_visible_contents - 1
};

enum surf
{
    surf_light = 0x1,
    surf_sky2d = 0x2,
    surf_sky = 0x4,
    surf_warp = 0x8,
    surf_trans = 0x10,
    surf_noportal = 0x20,
    surf_trigger = 0x40,
    surf_nodraw = 0x80,
    surf_hint = 0x100,
    surf_skip = 0x200,
    surf_nolight = 0x400,
    surf_bumplight = 0x800,
    surf_noshadows = 0x1000,
    surf_nodecals = 0x2000,
    surf_nopaint = surf_nodecals,
    surf_nochop = 0x4000,
    surf_hitbox = 0x8000
};

enum mask
{
    mask_all = 0xFFFFFFFF,
    mask_solid = (contents_solid | contents_moveable | contents_window | contents_monster | contents_grate),
    mask_playersolid = (contents_solid | contents_moveable | contents_playerclip | contents_window | contents_monster | contents_grate),
    mask_npcsolid = (contents_solid | contents_moveable | contents_monsterclip | contents_window | contents_monster | contents_grate),
    mask_npcfluid = (contents_solid | contents_moveable | contents_monsterclip | contents_window | contents_monster),
    mask_water = (contents_water | contents_moveable | contents_slime),
    mask_opaque = (contents_water | contents_moveable | contents_opaque),
    mask_opaque_npc = (mask_opaque | contents_monster),
    mask_blocklos = (contents_solid | contents_moveable | contents_slime),
    mask_blocklos_npc = (mask_blocklos | contents_monster),
    mask_visible = (mask_opaque | contents_ignore_nodraw_opaque),
    mask_visible_npc = (mask_opaque_npc | contents_ignore_nodraw_opaque),
    mask_shot = (contents_solid | contents_moveable | contents_monster | contents_window | contents_debris | contents_hitbox),
    mask_shot_brushonly = (contents_solid | contents_moveable | contents_window | contents_debris),
    mask_shot_hull = (contents_solid | contents_moveable | contents_monster | contents_window | contents_debris | contents_grate),
    mask_shot_portal = (contents_solid | contents_moveable | contents_window | contents_monster),
    mask_solid_brushonly = (contents_solid | contents_moveable | contents_window | contents_grate),
    mask_playersolid_brushonly = (contents_solid | contents_moveable | contents_window | contents_playerclip | contents_grate),
    mask_npcsolid_brushonly = (contents_solid | contents_moveable | contents_window | contents_monsterclip | contents_grate),
    mask_npcworldstatic = (contents_solid | contents_window | contents_monsterclip | contents_grate),
    mask_npcworldstatic_fluid = (contents_solid | contents_window | contents_monsterclip),
    mask_splitareaportal = (contents_water | contents_slime),
    mask_current = (contents_current0 | contents_current90 | contents_current180 | contents_current270 | contents_current_up | contents_current_down),
    mask_deadsolid = (contents_solid | contents_playerclip | contents_window | contents_grate)
};

enum PlayerFlags {
    ONGROUND = 1 << 0,
    DUCKING = 1 << 1,
    WATERJUMP = 1 << 2,
    ONTRAIN = 1 << 3,
    INRAIN = 1 << 4,
    FROZEN = 1 << 5,
    ATCONTROLS = 1 << 6,
    CLIENT = 1 << 7,
    FAKECLIENT = 1 << 8,
    INWATER = 1 << 9,
    FLY = 1 << 10,
    SWIM = 1 << 11,
    CONVEYOR = 1 << 12,
    NPC = 1 << 13,
    GODMODE = 1 << 14,
    NOTARGET = 1 << 15,
    AIMTARGET = 1 << 16,
    PARTIALGROUND = 1 << 17,
    STATICPROP = 1 << 18,
    GRAPHED = 1 << 19,
    GRENADE = 1 << 20,
    STEPMOVEMENT = 1 << 21,
    DONTTOUCH = 1 << 22,
    BASEVELOCITY = 1 << 23,
    WORLDBRUSH = 1 << 24,
    OBJECT = 1 << 25,
    KILLME = 1 << 26,
    ONFIRE = 1 << 27,
    DISSOLVING = 1 << 28,
    TRANSRAGDOLL = 1 << 29,
    UNBLOCKABLE_BY_PLAYER = 1 << 30
};


// Info about last 20 or so updates to the
class CPlayerCmdInfo
{
public:
    CPlayerCmdInfo() :
        m_flTime(0.0f), m_nNumCmds(0), m_nDroppedPackets(0)
    {
    }

    // realtime of sample
    float		m_flTime;
    // # of CUserCmds in this update
    int			m_nNumCmds;
    // # of dropped packets on the link
    int			m_nDroppedPackets;
};

class CPlayerSimInfo
{
public:
    CPlayerSimInfo() :
        m_flTime(0.0f), m_nNumCmds(0), m_nTicksCorrected(0), m_flFinalSimulationTime(0.0f), m_flGameSimulationTime(0.0f), m_flServerFrameTime(0.0f), m_vecAbsOrigin(0, 0, 0)
    {
    }

    // realtime of sample
    float		m_flTime;
    // # of CUserCmds in this update
    int			m_nNumCmds;
    // If clock needed correction, # of ticks added/removed
    int			m_nTicksCorrected; // +ve or -ve
    // player's m_flSimulationTime at end of frame
    float		m_flFinalSimulationTime;
    float		m_flGameSimulationTime;
    // estimate of server perf
    float		m_flServerFrameTime;
    Vector		m_vecAbsOrigin;
};