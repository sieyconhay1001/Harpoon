#include "Animations.h"

#include "../Memory.h"
#include "../MemAlloc.h"
#include "../Interfaces.h"
#include "../Hacks/AntiAim.h"
#include "../SDK/OsirisSDK/LocalPlayer.h"
#include "../SDK/OsirisSDK/Cvar.h"
#include "../SDK/OsirisSDK/GlobalVars.h"
#include "../SDK/OsirisSDK/Entity.h"
#include "../SDK/OsirisSDK/UserCmd.h"
#include "../SDK/OsirisSDK/ConVar.h"
#include "../SDK/OsirisSDK/Input.h"

#include "../SDK/SDKAddition/EntityListCull.hpp"

Animations::Datas Animations::data;

void Animations::update(UserCmd* cmd, bool& sendPacket) noexcept
{
    //if (!localPlayer || !localPlayer->isAlive())
    //   return;
    data.viewangles = cmd->viewangles;
    data.sendPacket = sendPacket;
}

void Animations::fake(UserCmd* cmd) noexcept
{
    static AnimState* fakeanimstate = nullptr;
    static bool updatefakeanim = true;
    static bool initfakeanim = true;
    static float spawnTime = 0.f;

    if (!interfaces->engine->isInGame())
    {
        updatefakeanim = true;
        initfakeanim = true;
        spawnTime = 0.f;
        fakeanimstate = nullptr;
    }

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (spawnTime != localPlayer.get()->spawnTime() || updatefakeanim)
    {
        spawnTime = localPlayer.get()->spawnTime();
        initfakeanim = false;
        updatefakeanim = false;
    }

    if (!initfakeanim)
    {
        fakeanimstate = static_cast<AnimState*>(memory->memalloc->Alloc(sizeof(AnimState)));

        if (fakeanimstate != nullptr)
            localPlayer.get()->CreateState(fakeanimstate);

        initfakeanim = true;
    }
    if (((cmd->tickCount != INT_MAX) || config->antiAim.airstuckonLBY) && ((data.sendPacket && (!AntiAim::lbyNextUpdatedPrevtick || localPlayer->velocity().length2D() > 1.125f)) || ((!AntiAim::lbyNextUpdatedPrevtick && AntiAim::lbyNextUpdated) || (AntiAim::lbyNextUpdatedPrevtick && AntiAim::lbyNextUpdated)) && localPlayer->velocity().length2D() < 1.125f))
    {
        std::array<AnimationLayer, 15> networked_layers;

        std::memcpy(&networked_layers, localPlayer.get()->animOverlays(), sizeof(AnimationLayer) * localPlayer->getAnimationLayerCount());
        auto backup_abs_angles = localPlayer.get()->getAbsAngle();
        auto backup_poses = localPlayer.get()->pose_parameters();

        *(uint32_t*)((uintptr_t)localPlayer.get() + 0xA68) = 0;

        localPlayer.get()->UpdateState(fakeanimstate, data.viewangles);
        localPlayer.get()->InvalidateBoneCache();
        memory->setAbsAngle(localPlayer.get(), Vector{ 0, fakeanimstate->m_flGoalFeetYaw, 0 });
        std::memcpy(localPlayer.get()->animOverlays(), &networked_layers, sizeof(AnimationLayer) * localPlayer->getAnimationLayerCount());

        data.gotMatrix = localPlayer.get()->setupBones(Animations::data.fakematrix, 256, 0x7FF00, memory->globalVars->currenttime);
        const auto origin = localPlayer.get()->getRenderOrigin();
        if (data.gotMatrix)
        {
            for (auto& i : data.fakematrix)
            {
                i[0][3] -= origin.x;
                i[1][3] -= origin.y;
                i[2][3] -= origin.z;
            }
        }
        std::memcpy(localPlayer.get()->animOverlays(), &networked_layers, sizeof(AnimationLayer) * localPlayer->getAnimationLayerCount());
        localPlayer.get()->pose_parameters() = backup_poses;
        memory->setAbsAngle(localPlayer.get(), Vector{ 0,backup_abs_angles.y,0 });
    }
}

void Animations::real() noexcept
{
    static auto jigglebones = interfaces->cvar->findVar("r_jiggle_bones");
    jigglebones->setValue(0);

    if (!localPlayer)
        return;

    if (!localPlayer->isAlive())
    {
        localPlayer.get()->ClientSideAnimation() = true;
        return;
    }

    static auto backup_poses = localPlayer.get()->pose_parameters();
    static auto backup_abs = localPlayer.get()->getAnimstate()->m_flGoalFeetYaw;

    if (!memory->input->isCameraInThirdPerson) {
        localPlayer.get()->ClientSideAnimation() = true;
        localPlayer.get()->UpdateClientSideAnimation();
        localPlayer.get()->ClientSideAnimation() = false;
        return;
    }

    static std::array<AnimationLayer, 15> networked_layers;

    while (localPlayer.get()->getAnimstate()->m_iLastClientSideAnimationUpdateFramecount == memory->globalVars->framecount)
        localPlayer.get()->getAnimstate()->m_iLastClientSideAnimationUpdateFramecount -= 1;
    while (localPlayer->getAnimstate()->m_iLastClientSideAnimationUpdateFramecount == memory->globalVars->framecount)
        localPlayer->getAnimstate()->m_iLastClientSideAnimationUpdateFramecount -= 1;

    static int old_tick = 0;
    if (old_tick != memory->globalVars->tickCount)
    {
        old_tick = memory->globalVars->tickCount;
        std::memcpy(&networked_layers, localPlayer.get()->animOverlays(), sizeof(AnimationLayer) * localPlayer->getAnimationLayerCount());
        localPlayer.get()->ClientSideAnimation() = true;
        localPlayer.get()->UpdateState(localPlayer->getAnimstate(), data.viewangles);
        localPlayer.get()->UpdateClientSideAnimation();
        localPlayer.get()->ClientSideAnimation() = false;
        if ((data.sendPacket && (!AntiAim::lbyNextUpdatedPrevtick && !AntiAim::lbyNextUpdated)) || (data.sendPacket && (localPlayer->velocity().length2D() > 1.1f))) /*&& ( ^ !AntiAim::lbyNextUpdated))*/
        {
            backup_poses = localPlayer.get()->pose_parameters();
            backup_abs = localPlayer.get()->getAnimstate()->m_flGoalFeetYaw;
        }
    }
    localPlayer.get()->getAnimstate()->m_flFeetYawRate = 0.f;
    memory->setAbsAngle(localPlayer.get(), Vector{ 0,backup_abs,0 });
    localPlayer.get()->getAnimstate()->UnknownFraction = 0.f;
    std::memcpy(localPlayer.get()->animOverlays(), &networked_layers, sizeof(AnimationLayer) * localPlayer->getAnimationLayerCount());
    localPlayer.get()->pose_parameters() = backup_poses;
}


// Animfix


class Quaternion {
public:
    float x, y, z, w;
};






static bool BuildPlayerBones(Entity* entity, int mask, matrix3x4* Bones) {
    Vector		     pos[128];
    Quaternion       q[128];
    Vector           backup_origin;
    Vector           backup_angles;
    float            backup_poses[24];
    AnimationLayer        backup_layers[13];

    StudioHdr* hdr = entity->GetStudioHdr();

    if (!hdr)
        return false;



}



void Animations::players() noexcept
{

    if (!config->debug.Animfix)
        return;

    //if (!localPlayer)
    //    return;

    for (EntityQuick entq : entitylistculled->getEntities())//entitylistculled->getEnemies())
    {

        if (!entq.m_bisAlive || entq.m_bisDormant)
            continue;

        if (entq.m_bisLocalPlayer && false) {
            NetworkChannel* netchann = interfaces->engine->getNetworkChannel();
            if (!netchann)
                continue;


            if (lpPlayerUpdate.size() > 2000 * 64) {
                lpPlayerUpdate.clear();
            }
            if (lpPlayerUpdateExtended.size() > 2000 * 64) {
                lpPlayerUpdateExtended.clear();
            }


            float latency = (netchann->getLatency(FLOW_OUTGOING) * 1000.f) - 200.f; /* 200 because of backtrack window */
            while ((lpPlayerUpdate.size() > 0) && ((lpPlayerUpdate.back().updateTime + (std::clamp(latency * 2.f, 0.f, 5000.f)) > (memory->globalVars->currenttime)))) {
                if(latency > 300)
                    lpPlayerUpdateExtended.push_front(lpPlayerUpdate.back());
                lpPlayerUpdate.pop_back();            
            }

            while ((lpPlayerUpdateExtended.size() > 0) && ((lpPlayerUpdateExtended.back().updateTime + (std::clamp(latency * 2.f, 0.f, 5000.f)) > (memory->globalVars->currenttime)))) {
                lpPlayerUpdateExtended.pop_back();
            }


            Entity* entity = entq.entity;
            
            Players lpData;
            lpData.updateTime = memory->globalVars->currenttime;
            lpData.simtime = entity->simulationTime();

            entity->InvalidateBoneCache();
            memory->setAbsOrigin(entity, entity->origin());
            *entity->getEffects() &= ~0x1000;
            entity->ClientSideAnimation() = true;
            entity->UpdateClientSideAnimation();
            entity->ClientSideAnimation() = false;

            entity->setupBones(lpData.matrix, 256, 0x7FF00, memory->globalVars->currenttime);
            lpPlayerUpdate.push_front(lpData);
            continue;
        }

        static auto jigglebones = interfaces->cvar->findVar("cl_extrapolate");
        jigglebones->setValue(0);
        jigglebones->onChangeCallbacks.size = 0;

        if (!entq.entity->getAnimstate())
            continue;

        Entity* entity = entq.entity;
        int i = entq.index;
        if (!data.player[i].once)
        {
            data.player[i].poses = entity->pose_parameters();
            data.player[i].abs = entity->getAnimstate()->m_flGoalFeetYaw;
            data.player[i].simtime = 0;
            data.player[i].once = true;
        } 
        if (data.player[i].simtime != entity->simulationTime()) {
             entity->setupBones(data.player[i].originalMatrix, 256, 0x7FF00, memory->globalVars->currenttime);
        }
        

        if (data.player[i].networked_layers.empty())
            std::memcpy(&data.player[i].networked_layers, entity->animOverlays(), sizeof(AnimationLayer) * entity->getAnimationLayerCount());

        //while (entity->getAnimstate()->m_flLastClientSideAnimationUpdateTime == memory->globalVars->currenttime)
        //    entity->getAnimstate()->m_flLastClientSideAnimationUpdateTime -= memory->globalVars->intervalPerTick;
        while (entity->getAnimstate()->m_iLastClientSideAnimationUpdateFramecount == memory->globalVars->framecount)
            entity->getAnimstate()->m_iLastClientSideAnimationUpdateFramecount -= 1;


        entity->InvalidateBoneCache();
        memory->setAbsOrigin(entity, entity->origin());
        *entity->getEffects() &= ~0x1000;

        std::memcpy(&data.player[i].networked_layers, entity->animOverlays(), sizeof(AnimationLayer) * entity->getAnimationLayerCount());
        entity->ClientSideAnimation() = true;

        entity->UpdateClientSideAnimation();
        entity->ClientSideAnimation() = false;


        if (data.player[i].simtime != entity->simulationTime())
        {
            data.player[i].chokedPackets = static_cast<int>((entity->simulationTime() - data.player[i].simtime) / memory->globalVars->intervalPerTick) - 1;
            data.player[i].simtime = entity->simulationTime();
            data.player[i].poses = entity->pose_parameters();
            data.player[i].abs = entity->getAnimstate()->m_flGoalFeetYaw;  
            data.player[i].accel = entity->velocity() - data.player[i].velocity;
            data.player[i].PredictedVelocity = entity->velocity();
            data.player[i].prevOrigin = entity->getAbsOrigin();
            data.player[i].velocity = entity->velocity();

        }
        else {   
            //data.player[i].PredictedVelocity = data.player[i].velocity + (data.player[i].accel * (memory->globalVars->intervalPerTick * data.player[i].chokedPackets));
            //data.player[i].PredictedVelocity.x = std::clamp(data.player[i].PredictedVelocity.x, 0.f, 255.f);
            //data.player[i].PredictedVelocity.y = std::clamp(data.player[i].PredictedVelocity.y, 0.f, 255.f);
            //data.player[i].PredictedVelocity.z = std::clamp(data.player[i].PredictedVelocity.z, 0.f, 255.f);
            //Vector PredOrigin = data.player[i].prevOrigin + (data.player[i].PredictedVelocity * (memory->globalVars->intervalPerTick * data.player[i].chokedPackets));
            entity->setAbsOrigin(data.player[i].prevOrigin);
            entity->origin() = data.player[i].prevOrigin;

        }


        entity->getAnimstate()->m_flFeetYawRate = 0.f;
        memory->setAbsAngle(entity, Vector{ 0,data.player[i].abs,0 });
        data.player[i].networked_layers[12].weight = std::numeric_limits<float>::epsilon();
        std::memcpy(entity->animOverlays(), &data.player[i].networked_layers, sizeof(AnimationLayer) * entity->getAnimationLayerCount());
        entity->pose_parameters() = data.player[i].poses;
        entity->InvalidateBoneCache();

        auto backup = data.lastest[i];
        auto boneCache = entity->GetBoneCache();
        auto countBones = entity->GetBoneCount();


        entity->setupBones(data.player[i].matrix, 256, 0x7FF00, memory->globalVars->currenttime);




        data.player[i].hasBackup = true;   


        data.player[i].countBones = countBones;

        backup.boneCache = (int*)boneCache;
        backup.countBones = countBones;
        backup.mins = entity->getCollideable()->obbMins();
        backup.max = entity->getCollideable()->obbMaxs();
        backup.origin = entity->getAbsOrigin();
    }
}


void Animations::setupMoveFix(Entity* entity) noexcept
{
    if (!config->debug.movefix)
        return;

    if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive())
        return;

    if (entity->velocity().length2D() < 170.0f) {
        return;
    }

    auto backup = data.backup[entity->index()];
    auto boneCache = entity->GetBoneCache();
    auto countBones = entity->GetBoneCount();

    backup.boneCache = (int*)boneCache;
    backup.countBones = countBones;
    backup.mins = entity->getCollideable()->obbMins();
    backup.max = entity->getCollideable()->obbMaxs();
    backup.origin = entity->getAbsOrigin();


    Vector origin = entity->getAbsOrigin();

    entity->InvalidateBoneCache();
    memcpy(boneCache, backup.boneCache, sizeof(matrix3x4) * std::clamp(backup.countBones, 0, 256));


    if (entity->velocity().length2D() > 170.0f) {
        origin.x -= (entity->velocity().x * (3 / 64));
        origin.y -= (entity->velocity().y * (3 / 64));
        origin.z -= (entity->velocity().z * (3 / 64));
    }

    memory->setAbsOrigin(entity, origin);

}

void Animations::setup(Entity* entity, Backtrack::Record record) noexcept
{
    if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive())
        return;
    auto backup = data.backup[entity->index()];
    auto boneCache = entity->GetBoneCache();
    auto countBones = entity->GetBoneCount();


    backup.boneCache = (int*)boneCache;
    backup.countBones = countBones;
    backup.mins = entity->getCollideable()->obbMins();
    backup.max = entity->getCollideable()->obbMaxs();
    backup.origin = entity->getAbsOrigin();
    entity->InvalidateBoneCache();
    memcpy(boneCache, record.matrix, sizeof(matrix3x4) * std::clamp(countBones, 0, 256));

    entity->getCollideable()->obbMins() = record.mins;
    entity->getCollideable()->obbMaxs() = record.max;

    if (config->debug.movefix) {
        if (entity->velocity().length2D() > 170.0f) {
            record.origin.x -= (entity->velocity().x * (3 / 64));
            record.origin.y -= (entity->velocity().y * (3 / 64));
            record.origin.z -= (entity->velocity().z * (3 / 64));
        }
    }

    memory->setAbsOrigin(entity, record.origin);
    backup.hasBackup = true;
}

#include "../Hacks/OTHER/Debug.h"
void Animations::setupMatrix(Entity* entity, matrix3x4* mat) noexcept
{
    if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive())
        return;

    auto boneCache = entity->GetBoneCache();
    auto countBones = entity->GetBoneCount();
    //auto boneCache = *(int**)(entity + 0x290C);
    //auto countBones = *(int*)(entity + 0x291B);

    if (!boneCache)
    {
        //Debug::QuickPrint("INVALID BONE CACHE!");
        return;
    }
    entity->InvalidateBoneCache();
    memcpy(boneCache, mat, sizeof(matrix3x4) * std::clamp(countBones, 0, 256));
}


void Animations::finishSetup(Entity* entity) noexcept
{
    if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive())
        return;

    auto backup = data.lastest[entity->index()];
    if (data.backup[entity->index()].hasBackup)
    {
        auto boneCache = entity->GetBoneCache();
        auto countBones = entity->GetBoneCount();
        entity->InvalidateBoneCache();
        memcpy(boneCache, backup.boneCache, sizeof(matrix3x4) * std::clamp(backup.countBones, 0, 256));

        entity->getCollideable()->obbMins() = backup.mins;
        entity->getCollideable()->obbMaxs() = backup.max;

        memory->setAbsOrigin(entity, backup.origin);
        data.backup[entity->index()].hasBackup = false;
    }
    else if (data.backupResolver[entity->index()].hasBackup)
    {
        auto boneCache = entity->GetBoneCache();
        auto countBones = entity->GetBoneCount();
        entity->InvalidateBoneCache();
        memcpy(boneCache, backup.boneCache, sizeof(matrix3x4) * std::clamp(backup.countBones, 0, 256));

        entity->getCollideable()->obbMins() = backup.mins;
        entity->getCollideable()->obbMaxs() = backup.max;

        memory->setAbsOrigin(entity, backup.origin);
        data.backupResolver[entity->index()].hasBackup = false;
    }
}