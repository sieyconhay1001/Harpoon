#include "Resolver.h"

#include "../../Config.h"
#include "../../Interfaces.h"
#include "../../Memory.h"
#include "../../Netvars.h"
#include "../Misc.h"
#include "../../SDK/OsirisSDK/ConVar.h"
#include "../../SDK/OsirisSDK/Surface.h"
#include "../../SDK/OsirisSDK/GlobalVars.h"
#include "../../SDK/OsirisSDK/NetworkChannel.h"
#include "../../SDK/OsirisSDK/WeaponData.h"
#include "../../Other/EnginePrediction.h"
#include "../../SDK/OsirisSDK/LocalPlayer.h"
#include "../../SDK/OsirisSDK/Entity.h"
#include "../../SDK/OsirisSDK/UserCmd.h"
#include "../../SDK/OsirisSDK/GameEvent.h"
#include "../../SDK/OsirisSDK/FrameStage.h"
#include "../../SDK/OsirisSDK/Client.h"
#include "../../SDK/OsirisSDK/ItemSchema.h"
#include "../../SDK/OsirisSDK/WeaponSystem.h"
#include "../../SDK/OsirisSDK/WeaponData.h"
#include "../../SDK/OsirisSDK/Vector.h"
#include "../../Helpers.h"
#include "../../SDK/OsirisSDK/ModelInfo.h"
#include "../Backtrack.h"
#include "../../SDK/OsirisSDK/AnimState.h"
#include "../../SDK/OsirisSDK/LocalPlayer.h"
#include "../../SDK/OsirisSDK/FrameStage.h"
#include <deque>


#include "../Other/Debug.h"
#include <mutex>
#include <numeric>
#include <sstream>
#include <codecvt>
#include <locale>
#include <string>
#include <iostream>
#include <cstddef>

const int MAX_RECORDS = 128;

std::vector<Resolver::Record> Resolver::PlayerRecords(65);
Resolver::Record Resolver::invalid_record;
Entity* Resolver::TargetedEntity;
Resolver::Record* Resolver::lastShotAt;
//#include "../Debug.h"
static void ResolverDebug(std::wstring str) {
    if (config->debug.resolverDebug) {
        Debug::LogItem item;
        item.PrintToScreen = false;
        item.text.push_back(str);
        Debug::LOG_OUT.push_back(item);
    }
}

static void ConOut(std::wstring text, bool Error = false) {
    memory->conColorMsg({ 0,250,0,255 }, "\n[HARPOON] ");
    const wchar_t* wcmd = text.c_str();
    size_t size = (wcslen(wcmd) + 1) * sizeof(wchar_t);
    char* cmd = new char[size];
    std::wcstombs(cmd, wcmd, size);
    if (Error) {
        memory->conColorMsg({ 255,0,0,255 }, cmd);
    }
    else {
       memory->conColorMsg({ 255,255,255,255 }, cmd);
    }
    memory->conColorMsg({ 0,250,0,255 }, " [HARPOON]\n");
    delete cmd;
}

static void ResolverOut(std::wstring str, bool Error = false) {
    if (config->debug.resolverDebug) {
        Debug::LogItem item;
        item.PrintToScreen = true;
        if (Error) { item.PrintToScreen = false; }
        item.PrintToConsole = false;
        item.text.push_back(str);
        item.time_of_creation = memory->globalVars->realtime;
        ConOut(str, Error);
        Debug::LOG_OUT.push_front(item);
        
    }

}

#include "../../SDK/SDKAddition/Utils/CapsuleMath.h"
#include "../../SDK/OsirisSDK/Beams.h"


std::vector<Vector> Hits;
bool ph = false;

static void OurShot(GameEvent* event) {

    Vector Pos;
    Pos.x = event->getFloat("x");
    Pos.y = event->getFloat("y");
    Pos.z = event->getFloat("z");


    //ResolverOut(L" Bullet Hit Detected ");
    Hits.push_back(Pos);


}

static int OurHit(GameEvent* event) {


    if ((interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("userid"))) != Resolver::lastShotAt->ent)) {
        return 0;
    }

   // ResolverOut(L" Player Hit Detected ");

    ph = true;
    return 1;
}



void Resolver::GetHits(GameEvent* event) {
    /* Calculate Spread Misses
       Calculate Hitbox hit
       Calculate Damage Done
       Whether to inc missed shots
       etc 
     */
    if (!Resolver::lastShotAt || Resolver::lastShotAt->invalid) {
        return;
    }

    //if (Resolver::lastShotAt->shots.front().processed) {
    //   return;
    //}

    switch (fnv::hashRuntime(event->getName())) {
        case fnv::hash("bullet_impact"):
            if ((interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("userid"))) == localPlayer.get())) {
                OurShot(event);
            }
            break;
        case fnv::hash("player_hurt"):
            if ((interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("attacker"))) == localPlayer.get())) {
                OurHit(event);
            }
            break;
    }

}

void Resolver::Reset(FrameStage stage) {


    for (int i = 0; i <= interfaces->engine->getMaxClients(); i++) {
        Entity* entity = interfaces->entityList->getEntity(i);
        if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive()
            || !entity->isOtherEnemy(localPlayer.get()))
            continue;

        Record* record = &PlayerRecords.at(entity->index());

        if (!record || record->invalid || !record->ResolveInfo.Original.Set)
            return;

        AnimState* Animstate = entity->getAnimstate();
        if (!Animstate)
            return;

        record->FiredUpon = false;


        Animstate->m_flGoalFeetYaw = record->ResolveInfo.Original.LBYAngle; /* Restore */
        entity->setAbsAngle(record->ResolveInfo.Original.ABSAngles);
        entity->eyeAngles() = record->ResolveInfo.Original.EyeAngles;
        entity->UpdateState(Animstate, entity->eyeAngles());


    }


}

#include "../../SDK/SDKAddition/Utils/MatrixMath.h"


/* Parse Hitbox rq lmaooo */
static bool ParseHitboxrq(Entity* entity, Resolver::shotdata* shot, Vector Pos) {

    const Model* model = entity->getModel();

    if (!model)
        return -10.f;

    StudioHdr* hdr = interfaces->modelInfo->getStudioModel(model);

    if (!hdr)
        return false;

    int hitBoxSet = entity->hitboxSet();

    if (hitBoxSet < 0)
        return false;


    matrix3x4 backup[256];
    auto boneCache = *(int**)(entity + 0x290F);
    auto countBones = *(int*)(entity + 0x291A);


    memcpy(backup, boneCache, countBones);
    entity->InvalidateBoneCache();
    memcpy(boneCache, shot->matrix, countBones);
    bool hit = false;

    Trace trace;
    
    interfaces->engineTrace->clipRayToEntity({ shot->EyePosition , Pos }, 0x4600400B,entity, trace);

    if (trace.entity == entity)
        hit = true;
    entity->InvalidateBoneCache();
    memcpy(boneCache, backup, countBones);

   // if (MatrixMath::DoesRayIntersectMatrix(shot->EyePosition, Pos, hdr, hitBoxSet, shot->matrix)) {
    //    return true;
    //}
    return hit;
}


void CalculateHits() {
    // || !Resolver::lastShotAt->FiredUpon
    if (!Resolver::lastShotAt || Resolver::lastShotAt->invalid  || !Resolver::lastShotAt->ent || Resolver::lastShotAt->ent->isDormant() || !Resolver::lastShotAt->ent->isAlive()) {
        return; //         Hits.clear();
    }

    if (Resolver::lastShotAt->shots.empty()) {
        if (Resolver::lastShotAt->FiredUpon) {
            ResolverOut(L"ERROR : !!!Player: " + Resolver::lastShotAt->PlayerName + L" Had No ShotData & Was Fired Upon!!! : ERROR", true);
        }
        return;
    }


    Resolver::shotdata* shot = &Resolver::lastShotAt->shots.front();

    if (shot->processed) {
        Hits.clear();
        return;
    }

    if (ph) {
        ph = false;
        ResolverOut(L"Player: " + Resolver::lastShotAt->PlayerName + L" Hit");
        Hits.clear();
        return;
    }

    int Intersects = 0;
    float dist = 0.0f;
    if (!Hits.empty()) {
        for (Vector Pos : Hits) {
            bool inter = false;
            if (CapsuleMath::HitBoxRayIntersection(shot->EyePosition, Pos, &shot->box, shot->matrix[shot->box.bone])) {
                inter = true;
            } else if(config->debug.parseALL){
                if (ParseHitboxrq(Resolver::lastShotAt->ent, shot, Pos)) {
                    inter = true;
                }
            }

            if (inter) {
                Intersects++;
            } else {
                if(dist < shot->EyePosition.distTo(Pos)){
                    dist = shot->EyePosition.distTo(Pos);
                }
            }


        }
        Hits.clear();
        shot->processed = true;
    }
    else {
        //ResolverOut(L"No bullet_hit registered (Shot On " + Resolver::lastShotAt->PlayerName + L") - Client Side Shot ");
        //Hits.clear();
        return;
    }

    
    if (Intersects > 0) {
        if (Resolver::lastShotAt->ent->health() == Resolver::lastShotAt->prevhealth) {
            Resolver::lastShotAt->missedshots++;
            ResolverOut(L"Missed Shot On " + Resolver::lastShotAt->PlayerName + L" Due To Bad Resolve " + std::to_wstring(Intersects) + L" Intersections");
            Resolver::lastShotAt->missedshotsthisinteraction++;
        }
        shot->processed = true;
        Hits.clear();
    }
    else {

        if (config->debug.animstatedebug.resolver.goforkill) {
            Resolver::lastShotAt->missedshots++;
        }

        if (dist > shot->EyePosition.distTo(shot->TargetedPosition)) {
            ResolverOut(L"Missed Shot Due To Spread On " + Resolver::lastShotAt->PlayerName);
        }
        else {
            ResolverOut(L"Missed Shot Due To Occlusion On " + Resolver::lastShotAt->PlayerName);
        }
        shot->processed = true;
        Hits.clear();
    }



}
/*



*/
/*
bool Resolver::LBY_UPDATE(Entity* entity, Resolver::Record* record) {

    bool returnval = false;
    float servertime = memory->globalVars->serverTime();

    if (record->velocity > 0.1f) { //LBY updates on any velocity
        record->lbyNextUpdate = 0.22f + servertime;
        return false;
    }

    if (record->lbyNextUpdate < servertime) {
        record->lbyNextUpdate = servertime + 1.1f;
        return true;
    }
    else {
        return false;
    }    
}
*/


bool Resolver::LBY_UPDATE(Entity* entity, Resolver::Record* record, bool UseAnim, int TicksToPredict){
    float servertime = memory->globalVars->serverTime();
    float Velocity;

    if (!(TicksToPredict == 0)) { servertime += memory->globalVars->intervalPerTick * TicksToPredict; }

    // if(!TicksToPredict){} <-- this is a shit way to determine whether to set lbyNextUpdate but oh well
    if (!UseAnim) {
        Velocity = entity->getVelocity().length2D();
    }
    else {
        AnimState* as_EntityAnimState = entity->getAnimstate();
        if (as_EntityAnimState) {
            //return false;
            try {
                //Velocity = as_EntityAnimState->speed_2d;
                Velocity = entity->getVelocity().length2D();
                /* So For some reason CS:GO Likes to give me pointers to random addresses when calling
                   getAnimstate. So to combat this, we just wont use it until I can find a fix.*/

            }
            catch (std::exception& e) { /* This likes to be thrown.. often. FIX THIS!*/
                ResolverDebug(std::wstring{ L"Error [Resolver.cpp:107]: as_EntityAnimState is an Invalid Pointer" });
                Velocity = entity->getVelocity().length2D();
            }
        }
        else {
            Velocity = entity->getVelocity().length2D();
        }
    }

    if (Velocity > 0.1f) { //LBY updates on any velocity
        if (TicksToPredict == 0) { record->lbyNextUpdate = 0.22f + servertime;}
        return false; // FALSE, as I dont want to run LBY breaking code on movement!, however it does update!!!
    }

    if ( record->lbyNextUpdate >= servertime) {
        return false;
    }
    else if ( record->lbyNextUpdate < servertime) { // LBY ipdates on no velocity, .22s after last velocity, 1.1s after previous no-velocity
        if (TicksToPredict == 0) { record->lbyNextUpdate = servertime + 1.1f;}
        return true;
    }

    return false;
}


void Resolver::Update() {
    if (!config->debug.animstatedebug.resolver.enabled)
        return;

    for (int i = 0; i < PlayerRecords.size(); i++) {



        if (!localPlayer) {
            //return;
            auto record = &PlayerRecords.at(i);
            if (!record)
                continue;
            record->invalid = true;
            //record->shots.clear();
            record->totalshots = 0;
            continue;
        }

        auto record = &PlayerRecords.at(i);

        if (!record)
            continue;

        record->move = false;
        record->noDesync = false;

        auto entity = interfaces->entityList->getEntity(i);
        
        if (entity == localPlayer.get()) {
            continue;
        }



        if (!entity || !entity->isPlayer() || entity->isDormant() || !entity->isAlive() ||!entity->isOtherEnemy(localPlayer.get()) || entity == localPlayer.get()) { //  ((entity != localPlayer.get()) && 
            if (record->FiredUpon || record->wasTargeted) {
                record->lastworkingshot = record->missedshots;
            }
            record->missedshotsthisinteraction = 0;
            record->totalshots = 0;
            record->invalid = true;
            record->ResolveInfo.Original.Set = false;
            record->ResolverMatrix = 0;

            if (!entity) {
            }
            //record->shots.clear();
            continue;
        }

        record = &PlayerRecords.at(entity->index());      

        if (!record)
            continue;

        if (entity != record->ent) {
        }





        record->ent = entity;
        record->prevVelocity = record->velocity;
        record->velocity = entity->getVelocity().length2D();
        if (record->invalid == true) {
            record->once = false;
            record->ResolverMatrices.clear();
            record->PlayerName = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(entity->getPlayerName(true));
            record->ResolveInfo.CurrentSet.Set = false;
            record->ResolveInfo.Original.Set = true;
            record->ResolveInfo.Original.ABSAngles = entity->getAbsAngle();
            record->ResolveInfo.Original.EyeAngles = entity->eyeAngles();

            record->missedshotsthisinteraction = 0;
            record->totalshots = 0;
            if (config->debug.animstatedebug.resolver.missedshotsreset) { 
                record->missedshots = config->debug.animstatedebug.resolver.missedoffset; 
            }
            record->prevhealth = entity->health();
            record->wasTargeted = false;
            record->FiredUpon = false;
            record->multiExpan = 2.0f;
            record->ResolverMatrix = 0;
        }
        
        if (record->targeted == 3) {
            record->ResolverMatrix = record->Original;
        }

        record->lbyUpdated = LBY_UPDATE(entity, record, true);
        //record->activeweapon = entity->getActiveWeapon();
        record->onshot = false;
        auto Animstate = entity->getAnimstate();
        if (Animstate && record->wasTargeted) {
            record->ResolveInfo.Original.LBYAngle = Animstate->m_flGoalFeetYaw; /* Save What We Get From The Server*/
            auto AnimLayerOne = entity->getAnimationLayer(1);
            if (AnimLayerOne) {
                auto currAct = entity->getSequenceActivity(AnimLayerOne->sequence);
                bool IN_FIRE_ACT = (currAct == ACT_CSGO_FIRE_PRIMARY) || (currAct == ACT_CSGO_FIRE_PRIMARY_OPT_1) || (currAct == ACT_CSGO_FIRE_PRIMARY_OPT_2);
                if (IN_FIRE_ACT && (AnimLayerOne->weight > (0.1f)) && (AnimLayerOne->cycle < .9f)) {
                    record->onshot = true;
                }

            }
        }

        if (!record->shots.empty()) {
            
            if ((entity->simulationTime()) > (record->shots.back().simtime + 4.0f)) {
                record->shots.pop_back();
            }
            
            if (record->shots.size() > 5) {
                record->shots.erase(record->shots.begin() + 5, record->shots.end());
            }
        }

        if (!record->FiredUpon) {
            record->invalid = false;
            continue;
        }
        else if (((record->prevhealth == entity->health()) && !record->invalid) || (config->debug.animstatedebug.resolver.goforkill) || ((record->shots.size() > 1) && record->shots.back().hitbox == 0)){
            record->lastworkingshot = -1;
            //record->missedshots =  ++(record->missedshots); // (record->missedshots >= 8 ? config->debug.animstatedebug.resolver.missedoffset )
            record->ResolveInfo.wasUpdated = false;
            Debug::LogItem item;
            //item.text.push_back(std::wstring{ L"Missed Shot On " + record->PlayerName + L" Fuck If I know why, GL resolving next time" });
            Debug::LOG_OUT.push_front(item);
        }
        else if (!record->shots.empty()) {
            record->shots.back().DamageDone = record->prevhealth - entity->health(); 
            record->totalshots++;
            if (config->debug.animstatedebug.resolver.missedshotsreset) {
                record->missedshots = 0;
            }
        }
        else {
            record->totalshots++;
            if (config->debug.animstatedebug.resolver.missedshotsreset) {
                record->missedshots = 0;
            }
        }
        


        record->prevhealth = entity->health();

        record->invalid = false;

        if (record->wasTargeted) {
            TargetedEntity = entity;
        }

    }
    CalculateHits();
}

/*

    struct shotdata {
        matrix3x4 matrix[256];
        float simtime;
        Vector EyePosition;
        Vector TargetedPosition;
        Vector viewangles;
        int DamageDone;
        int shotCount;
    };

    struct LBYResolveBackup {
        bool Prev = false;
        float EyeAngle;
        Vector AbsAngle;
    };

    struct ResolveBackupData{
        float PreviousEyeAngle = 0.0f;
        float eyeAnglesOnUpdate = 0.0f;
        float PreviousDesyncAng = 0.0f;
        float originalLBY = 0.0f;
    }

    struct Record {
        float prevSimTime = 0.0f;
        int prevhealth = 0;
        int lastworkingshot = -1;
        int missedshots = 0;
        bool wasTargeted = false;
        bool invalid = true;
        bool FiredUpon = false;
        bool autowall = false;
        float velocity = -1.0f;
        float prevVelocity = -1.0f;
        float multiExpan = 2.0f;
        float lbyNextUpdate = 0.0f;
        bool lbyUpdated = false;
        int missedshotsthisinteraction = 0;
        int totalshots = 0;
        //Entity* activeweapon;
        std::deque<shotdata> shots;
        bool wasUpdated = false;
        LBYResolveBackup lbybackup;
        ResolveBackupData ResolveInfo;
    };



*/

#include "../../Other/Animations.h"
#include "../../Hooks.h"
#include <string>


#include "../../COMPILERDEFINITIONS.h"

static void PrintVal(bool val) {
    if (val) {
        memory->conColorMsg({ 0,255,0,255 }, "Y\n");
    }
    else {
        memory->conColorMsg({ 255,0,0,255 }, "N\n");
    }
}
#include "../../Hooks.h"


struct MatrixBuildInfo {
    AnimState* anim;
    Entity* entity;
    Resolver::Record* record;
    Resolver::matrixWrapper* wrapper;
    float GFY;
    Vector ABSOrigin;
    Vector Origin;
    Vector ABSAngle;
    bool valid = false;
    matrix3x4 original[256];
};

static Resolver::matrixWrapper QuickBuildMatrix(MatrixBuildInfo info) {

    Entity* entity = info.entity;
    Resolver::Record* record = info.record;
    auto boneCache = *(int**)(entity + 0x290F);
    auto countBones = *(int*)(entity + 0x291A);

    Resolver::matrixWrapper wrapper;


    if (!boneCache) {
        return wrapper;
    }

    static auto jigglebones = interfaces->cvar->findVar("r_jiggle_bones");
    jigglebones->setValue(0);
    record->hook.init(entity);

    entity->ClientSideAnimation() = true;

    AnimState backup;
    memcpy(&backup, info.anim, sizeof(AnimState));

    info.anim->m_flGoalFeetYaw = info.GFY;
    entity->setAbsAngle(info.ABSAngle);
    entity->setAbsOrigin(info.ABSOrigin);
    entity->origin() = info.Origin;

    //entity->UpdateState(info.anim, info.ABSAngle);
    info.anim->m_iLastClientSideAnimationUpdateFramecount = memory->globalVars->framecount - 1;
    info.anim->m_flLastClientSideAnimationUpdateTime = memory->globalVars->currenttime - 1;
    entity->UpdateClientSideAnimation();

    float backup_frame = memory->globalVars->framecount;

    memory->globalVars->framecount = INT_MAX;

    entity->InvalidateBoneCache();
    entity->setupBones(wrapper.Matrix, 256, 0x7FF00, memory->globalVars->currenttime);

    entity->InvalidateBoneCache();
    info.anim->m_iLastClientSideAnimationUpdateFramecount = memory->globalVars->framecount - 1;
    info.anim->m_flLastClientSideAnimationUpdateTime = memory->globalVars->currenttime - 1;

    memcpy(info.anim, &backup, sizeof(AnimState));
    entity->UpdateClientSideAnimation();
    entity->InvalidateBoneCache();
    memcpy(boneCache, record->Original, sizeof(matrix3x4) * std::clamp(countBones, 0, 256));



    memory->globalVars->framecount = backup_frame;





    


}


void Resolver::NewAnimStateResolver(Entity* entity) {
    // __ MAKE INTO FUNCTION __
    return;
    if (!interfaces->engine->isInGame() || !localPlayer || !localPlayer->isAlive())
        return;

    
    

    config->debug.indicators.resolver = false;
    if (!entity || !entity->isPlayer() || entity->isDormant() || !entity->isAlive() || !entity->isOtherEnemy(localPlayer.get()))
        return;

    if (PlayerRecords.empty())
        return;



    std::wstring name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(entity->getPlayerName(true));

    if ((PlayerRecords.size() < entity->index()) || (entity->index() == -1))
        return;

    auto record = &PlayerRecords.at(entity->index());

    if (!record || record->invalid) {
        //ResolverDebug(std::wstring{ L"[Resolver.cpp:321] Exiting, No record/invalid record" });
        //record->FiredUpon = false;
        return;
    }

    if (record->lastTickCount != memory->globalVars->tickCount) {
        record->lastTickCount = memory->globalVars->tickCount;
        if (entity->simulationTime() != record->simulationTime) {
            record->simulationTime = entity->simulationTime();
            record->chokedPackets = 0;
            record->ticksSinceChoked++;
            if (!config->debug.Animfix || !Animations::data.player[entity->index()].hasBackup) {
                //entity->setupBones(record->Original, 256, 0x7FF00, memory->globalVars->currenttime);
                memcpy(record->Original, entity->GetBoneCache(), entity->GetBoneCount());
            }
            else {
                memcpy(record->Original, Animations::data.player[entity->index()].matrix, sizeof(matrix3x4) * 256);

            }
        }
    }


    if (record->onshot) {
        record->ResolverMatrices.clear();
        record->ResolverMatrix = record->Original;
        //ResolverDebug(std::wstring{ L"[Resolver.cpp:314] Exiting, player onshot [" + name + L"] -> " + std::to_wstring(memory->globalVars->currenttime) });
        record->FiredUpon = false;
        record->once = false;
        return;
    }

    auto Animstate = entity->getAnimstate();

    if (!Animstate) {
        record->ResolverMatrices.clear();
        ResolverDebug(std::wstring{ L"[Resolver.cpp:321] Exiting, No anim state [" + name + L"] -> " + std::to_wstring(memory->globalVars->currenttime) });
        record->FiredUpon = false;
        return;
    }

    auto AnimLayerThree = entity->getAnimationLayer(3);
    if (!AnimLayerThree) {
        record->ResolverMatrices.clear();
        ResolverDebug(std::wstring{ L"[Resolver.cpp:321] Exiting, No AnimLayerThree was set [" + name + L"] -> " + std::to_wstring(memory->globalVars->currenttime) });
        record->FiredUpon = false;
        return;
    }

    auto currAct = entity->getSequenceActivity(AnimLayerThree->sequence);
    bool IN_LBY_ACT = (currAct == ACT_CSGO_IDLE_TURN_BALANCEADJUST);

    float Velocity = entity->velocity().length2D();

    bool hasDesyncHadTimeToLBY = false;
    bool hasDesyncHadTimeToDesync = false;
    bool PresumedNoDesync = false;
    bool move = true;
    //bool btFLAG = false;
    float DesyncFrac = 1.0f;



    if ((Velocity < 140) && (Animstate->m_flTimeSinceStartedMoving <= 1.2f)) {
        if (Animstate->m_flTimeSinceStartedMoving <= 1.2f) {
            if (Animstate->m_flTimeSinceStoppedMoving >= .22f) {
                hasDesyncHadTimeToLBY = true;
                if (Animstate->m_flTimeSinceStoppedMoving >= .8f) {
                    hasDesyncHadTimeToDesync = true;
                }
            }
        }
        else if (Animstate->m_flTimeSinceStartedMoving > 0) {
            if (Velocity > 100) {
                DesyncFrac = 1.1f - (Animstate->m_flTimeSinceStartedMoving / 1.1f);
                move = true;
                hasDesyncHadTimeToDesync = true;
            }
        }
    }
    else {
        PresumedNoDesync = true;
    }








    if (record->chokedPackets > 8) {
         PresumedNoDesync = true;
    }






    record->noDesync = PresumedNoDesync;
    record->move = move;


    if (PresumedNoDesync || move) {
        std::deque<Backtrack::Record>* bt_record = &(Backtrack::records[entity->index()]);
        if (bt_record && !bt_record->empty()) {
            bt_record->front().move = record->move;
            bt_record->front().noDesync = record->noDesync;
        }
    }



    if (!(hasDesyncHadTimeToDesync || hasDesyncHadTimeToLBY) && PresumedNoDesync) {
        //ResolverDebug(std::wstring{ L"[Resolver.cpp:459] Exiting, Presumed No Desync [" + name + L"] -> " + std::to_wstring(memory->globalVars->currenttime) });
        record->FiredUpon = false;
        record->ResolverMatrices.clear();
        record->ResolverMatrix = record->Original;
        record->once = false;
        //ResolverDebug(std::wstring{ L"[Resolver.cpp:321] Exiting, No No Desync [" + name + L"] -> " + std::to_wstring(memory->globalVars->currenttime) });
        return;
    }

    float DesyncAng = 0.0f;
    float LBYAng = 0.0f;


    if (!record->lbyUpdated || ((AnimLayerThree->cycle > .9f) && IN_LBY_ACT) || !IN_LBY_ACT || record->FiredUpon || !record->once) { /* Simple Little Brute Force Method */
        Vector _ABSSave = entity->getAbsAngle();
        Vector _EyeAnglesSave = entity->eyeAngles();

            if ((AnimLayerThree->cycle < .9f) && hasDesyncHadTimeToLBY) {
                LBYAng = (record->totalshots % 2) ? 58 : 0;

                switch (record->missedshots) {
                case 0:
                    DesyncAng += 116;
                    break;
                case 1:
                    DesyncAng += 58.f;
                    record->missedshots = 0;
                    break;
                default:
                    DesyncAng += 0;
                    record->missedshots = 0;
                    break;
                }
                DesyncAng += DesyncAng + LBYAng;

            }
            else if (move) {
                DesyncAng = 58;
                DesyncAng *= DesyncFrac;
            }
            else { /*Micromoving(?), leading me to believe they are not Extended LBY breaking, and possibly sub Max Desync*/
                switch (record->missedshots / 2) {
                case 0:
                    DesyncAng += 58;
                    break;
                case 1:
                    DesyncAng += (58 / 2);
                    break;
                case 2:
                    if (!IN_LBY_ACT) {
                        DesyncAng -= 58;
                        record->missedshots = 1;
                        break;
                    }
                    //DesyncAng += 116;
                    //break;
                default:
                    DesyncAng += 0;
                    record->missedshots = 0;
                    break;
                }

            }
       




        int prevTargeted = record->targeted;
        if (record->missedshots != record->prevMissed) {
                record->targeted++;
                record->prevMissed = record->missedshots;
                if (record->targeted > 2) {
                    record->targeted = 0;
                }
         }      
        else {

        }
        //record->once = true;
        

        //entity->InvalidateBoneCache();
        //auto boneCache = *(int**)(entity + 0x290F);
       // auto countBones = *(int*)(entity + 0x291A);
        auto boneCache = entity->GetBoneCache();
        auto countBones = entity->GetBoneCount();
        sizeof(BoneAccessor);

        if (!boneCache) {
            if (true || !boneCache)
            {
               // ResolverDebug(std::wstring{ L"[Resolver.cpp:933] Exiting, No bonecache for t [" + name + L"] -> " + std::to_wstring(memory->globalVars->currenttime) });

            }
            return;
        }
        static auto jigglebones = interfaces->cvar->findVar("r_jiggle_bones");
        jigglebones->setValue(0);
        record->hook.init(entity);

        entity->ClientSideAnimation() = true;

        record->ResolverMatrices.clear();




        
        matrixWrapper bones;
        Vector ABS = entity->getAbsAngle();
        Vector EyeAngles = entity->eyeAngles();
        float GFY = Animstate->m_flGoalFeetYaw;
        Vector ABSSave = ABS;
        ABS.y += DesyncAng;
        Animstate->m_flGoalFeetYaw += DesyncAng;
        entity->setAbsAngle(ABS);
        bones.Angle = ABS.y;
        entity->InvalidateBoneCache();
        entity->setupBones(bones.Matrix, 256, 0x7FF00, memory->globalVars->currenttime);
        record->ResolverMatrices.push_back(bones);
        memcpy(boneCache, record->Original, sizeof(matrix3x4) * std::clamp(countBones, 0, 256));

        entity->UpdateState(Animstate, EyeAngles);
        entity->UpdateClientSideAnimation();
        ABS = ABSSave;
        Animstate->m_flGoalFeetYaw = GFY;




        ABS.y -= DesyncAng;;
        Animstate->m_flGoalFeetYaw -= DesyncAng;
        entity->setAbsAngle(ABS);
        bones.Angle = ABS.y;
        entity->UpdateState(Animstate, entity->eyeAngles());
        entity->UpdateClientSideAnimation();
        entity->InvalidateBoneCache();
        entity->setupBones(bones.Matrix, 256, 0x7FF00, memory->globalVars->currenttime);
        entity->InvalidateBoneCache();
        memcpy(boneCache, record->Original, sizeof(matrix3x4) * std::clamp(countBones, 0, 256));
        record->ResolverMatrices.push_back(bones);
        entity->setAbsAngle(ABSSave);
        Animstate->m_flGoalFeetYaw = GFY;
        entity->UpdateState(Animstate, EyeAngles);
        entity->UpdateClientSideAnimation();
        entity->ClientSideAnimation() = false;
        
        //entity->UpdateClientSideAnimation();


        if (record->FiredUpon || !record->once || true) {
            record->once = true;
            if (record->targeted == 2) {
                record->ResolverMatrix = record->Original;
                record->ResolverMatrices.at(1).targeted = false;
                record->ResolverMatrices.at(0).targeted = false;
            }if (record->targeted == 1) {
                record->ResolverMatrices.at(0).targeted = false;
                record->ResolverMatrix = record->ResolverMatrices.at(1).Matrix;
            }
            else {
                record->ResolverMatrices.at(1).targeted = false;
                record->ResolverMatrix = record->ResolverMatrices.at(0).Matrix;
            }
            record->ResolverMatrices.at(record->targeted).targeted = true;
        }

#ifdef _EXTRA_DEBUG
        if (record->FiredUpon || (prevTargeted != record->targeted)) {
            record->LastAng = DesyncAng;
            /*
            std::string micromove_str{ (AnimLayerThree->cycle > .9f) ? std::string("Y") : std::string("N") };
            std::string lby_str{ (record->lbyUpdated) ? std::string("Y") : std::string("N") };
            std::string move_str{ (move) ? std::string("Y") : std::string("N") };
            std::string DesyT{ (hasDesyncHadTimeToDesync) ? std::string("Y") : std::string("N") };
            std::string LBYT{ (hasDesyncHadTimeToLBY) ? std::string("Y") : std::string("N") };
            std::string LBYA{ (IN_LBY_ACT) ? std::string("Y") : std::string("N") };
            */

            char buffer[2048];
            memory->conColorMsg({ 0,255,255,255 }, "\n        [HARPOON NEW ANIMSTATE RESOLVER]\n");
            memory->conColorMsg({ 0,255,255,255 }, "------------------------------------------------\n");


            
            memory->conColorMsg({ 0,255,255,255 }, "    Resolver Record For: ");
            memory->conColorMsg({ 255,255,255,255 }, entity->getPlayerName(true).c_str());

            memory->conColorMsg({ 0,255,255,255 }, "\n            Micromoving?   : ");
            PrintVal((AnimLayerThree->cycle > .9f) && (5.f > Velocity) );

            memory->conColorMsg({ 0,255,255,255 }, "            LBY Updated?   : ");
            PrintVal((record->lbyUpdated));

            memory->conColorMsg({ 0,255,255,255 }, "            Moving         : ");
            PrintVal(move);

            memory->conColorMsg({ 0,255,255,255 }, "            Time To Desync : ");
            PrintVal(hasDesyncHadTimeToDesync);

            memory->conColorMsg({ 0,255,255,255 }, "            Time To ExLBY  : ");
            PrintVal(hasDesyncHadTimeToLBY);


            memory->conColorMsg({ 0,255,255,255 }, "            979 Animation  : ");
            PrintVal(IN_LBY_ACT);

            snprintf(buffer, 1024, "%f\n", DesyncFrac);
            memory->conColorMsg({ 0,255,255,255 }, "            Desync Frac    : " );
            memory->conColorMsg({ 255,255,255,255 }, buffer);

            snprintf(buffer, 1024, "%f\n", DesyncAng);
            memory->conColorMsg({ 0,255,255,255 }, "            Desync Ang     : ");
            memory->conColorMsg({ 255,255,255,255 }, buffer);

            snprintf(buffer, 1024, "%d\n", record->missedshots);
            memory->conColorMsg({ 0,255,255,255 }, "            Missed Shots   : ");
            memory->conColorMsg({ 255,255,255,255 }, buffer);

            snprintf(buffer, 1024, "%d\n", record->targeted);
            memory->conColorMsg({ 0,255,255,255 }, "            Targeted Mat   : ");
            memory->conColorMsg({ 255,255,255,255 }, buffer);
            snprintf(buffer, 1024, "%f\n", memory->globalVars->currenttime);
            memory->conColorMsg({ 0,255,255,255 }, "\n    CurrTime : ");
            memory->conColorMsg({ 255,255,255,255 }, buffer);
           
            memory->conColorMsg({ 0,255,255,255 }, "\n------------------------------------------------\n");

            /*
            const char* formatter =
                "         [HARPOON NEW ANIMSTATE RESOLVER]\n"
                "-----------------------------------------------\n "
                "    Resolver Record For %s:\n "
                "\n "
                "            Micromoving?   : %s\n "
                "            LBY Updated?   : %s\n "
                "            Moving         : %s\n "
                "            Time To Desync : %s\n "
                "            Time To ExLBY  : %s\n "
                "            979 Animation  : %s\n "
                "            Desync Frac    : %f\n "
                "            Desync Ang     : %f\n "
                "            Missed Shots   : %d\n "
                "\n   CurrTime: %f\n"
                "------------------------------------------------\n";

            
            snprintf(buffer, 1024, formatter,
                
                micromove_str.c_str(),
                lby_str.c_str(),
                move_str.c_str(),
                DesyT.c_str(),
                LBYT.c_str(),
                LBYA.c_str(),
                DesyncFrac,
                DesyncAng,
                record->missedshots,
                memory->globalVars->currenttime
            );

            memory->conColorMsg({ 255,0,0,255 }, buffer);

            */












        }
#endif
       
        record->FiredUpon = false;
        memcpy(entity->GetBoneCache(), record->Original, entity->GetBoneCount());

        entity->setAbsAngle(_ABSSave);
        entity->eyeAngles() = _EyeAnglesSave;

        //item2.text.push_back(std::wstring{ L"[Resolver] Attempting To Resolve " + name + L" With Angles: " + std::to_wstring(DesyncAng) + L" " + micromove_str + lby_str + move_str + DesyT + LBYT + LBYA });
        //item2.color = { (int)config->debug.ResolverOut.color[0] * 255,(int)config->debug.ResolverOut.color[1] * 255, (int)config->debug.ResolverOut.color[2] * 255 };
       // Debug::LOG_OUT.push_front(item2);


    } else {
     record->ResolverMatrices.clear();
    }




}

void Resolver::AnimStateResolver(Entity* entity) {

    // __ MAKE INTO FUNCTION __
    config->debug.indicators.resolver = false;
    if (!localPlayer || !localPlayer->isAlive() || !entity || !entity->isPlayer() || entity->isDormant() || !entity->isAlive() || !entity->isOtherEnemy(localPlayer.get()))
        return;

    if (PlayerRecords.empty())
        return;

    std::wstring name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(entity->getPlayerName(true));
    auto record = &PlayerRecords.at(entity->index());

    if (!record || record->invalid) {
        //ResolverDebug(std::wstring{ L"[Resolver.cpp:321] Exiting, No record/invalid record" });
        record->FiredUpon = false;
        return;
    }


    if (record->onshot) {
        ResolverDebug(std::wstring{ L"[Resolver.cpp:314] Exiting, player onshot [" + name + L"] -> " + std::to_wstring(memory->globalVars->currenttime) });
        record->FiredUpon = false;
        return;
    }

    auto Animstate = entity->getAnimstate();

    if (!Animstate) {
        ResolverDebug(std::wstring{L"[Resolver.cpp:321] Exiting, No anim state [" + name + L"] -> " + std::to_wstring(memory->globalVars->currenttime) });
        record->FiredUpon = false;
        return;
    }

    auto AnimLayerThree = entity->getAnimationLayer(3);
    if (!AnimLayerThree) {
        ResolverDebug(std::wstring{ L"[Resolver.cpp:321] Exiting, No AnimLayerThree was set [" + name + L"] -> " + std::to_wstring(memory->globalVars->currenttime) });
        record->FiredUpon = false;
        return;
    }

    auto currAct = entity->getSequenceActivity(AnimLayerThree->sequence);
    bool IN_LBY_ACT = (currAct == ACT_CSGO_IDLE_TURN_BALANCEADJUST);

    float Velocity = entity->velocity().length2D();

    bool hasDesyncHadTimeToLBY = false;
    bool hasDesyncHadTimeToDesync = false;
    bool PresumedNoDesync = false;
    bool move = true;
    //bool btFLAG = false;
    float DesyncFrac = 1.0f;
    if ((Velocity < 140) && (Animstate->m_flTimeSinceStartedMoving <= 1.2f)) {
        if (Animstate->m_flTimeSinceStartedMoving <= 1.2f) {

            if (Animstate->m_flTimeSinceStoppedMoving >= .22f) {
                hasDesyncHadTimeToLBY = true;
                if (Animstate->m_flTimeSinceStoppedMoving >= .8f) {
                    hasDesyncHadTimeToDesync = true;
                }
            }
        } else if (Animstate->m_flTimeSinceStartedMoving > 0) {
            if (Velocity > 100) {
                DesyncFrac = 1.1f - (Animstate->m_flTimeSinceStartedMoving / 1.1f);
                move = true;
                hasDesyncHadTimeToDesync = true;
            } 
        }
    }
    else{
        PresumedNoDesync = true;
    }

    record->noDesync = PresumedNoDesync;
    record->move = move;


    if (PresumedNoDesync || move) {
        std::deque<Backtrack::Record>* bt_record = &(Backtrack::records[entity->index()]);
        if (bt_record && !bt_record->empty()) {
            bt_record->front().move = record->move;
            bt_record->front().noDesync = record->noDesync;
        }
    }


    if ((record->ResolveInfo.prevSimTime != entity->simulationTime()) && (entity->eyeAngles().y != record->ResolveInfo.Original.EyeAngles.y)) { /* On Character Update */
        record->ResolveInfo.prevSimTime = entity->simulationTime();
        entity->UpdateState(Animstate, entity->eyeAngles());

        if (!record->ResolveInfo.wasUpdated && !record->lbybackup.Prev) { /* If we didn't update it, we set it up. This is to prevent spinning characters*/
            record->ResolveInfo.Original.LBYAngle = Animstate->m_flGoalFeetYaw; /* Save What We Get From The Server*/
            record->ResolveInfo.Original.ABSAngles = entity->getAbsAngle();
            record->ResolveInfo.Original.EyeAngles = entity->eyeAngles();
            record->ResolveInfo.Original.Set = true;
            
            if (!PresumedNoDesync) {
                Animstate->m_flGoalFeetYaw += record->ResolveInfo.ResolveAddition.LBYAngle; /* Add on the Additions */
                entity->getAbsAngle() += record->ResolveInfo.ResolveAddition.ABSAngles;
                entity->eyeAngles() += record->ResolveInfo.ResolveAddition.EyeAngles;
            }

            record->ResolveInfo.CurrentSet.Set = true;
            record->ResolveInfo.CurrentSet.LBYAngle = Animstate->m_flGoalFeetYaw; /* Set Current Angles */
            record->ResolveInfo.CurrentSet.ABSAngles = entity->getAbsAngle();
            record->ResolveInfo.CurrentSet.EyeAngles = entity->eyeAngles();
            record->ResolveInfo.wasUpdated = false;

        }
        else if (record->lbybackup.Prev) {
            if (record->ResolveInfo.CurrentSet.Set) { /* Restore After LBY Tick*/
                if (hasDesyncHadTimeToLBY || hasDesyncHadTimeToDesync) {
                    Animstate->m_flGoalFeetYaw = record->ResolveInfo.CurrentSet.LBYAngle; /* Restore */
                    entity->getAbsAngle() = record->ResolveInfo.CurrentSet.ABSAngles;
                    entity->eyeAngles() = record->ResolveInfo.CurrentSet.EyeAngles;
                }
                entity->UpdateState(Animstate, entity->eyeAngles());
            }
            record->lbybackup.Prev = false;     
        }
        else {
            record->ResolveInfo.Original.LBYAngle = Animstate->m_flGoalFeetYaw; /* Save What We Get From The Server*/
            record->ResolveInfo.Original.ABSAngles = entity->getAbsAngle();
            record->ResolveInfo.Original.EyeAngles = entity->eyeAngles();
            record->ResolveInfo.Original.Set = true;
        }

        entity->UpdateState(Animstate, entity->eyeAngles());
    }
    else {
        if (record->ResolveInfo.CurrentSet.Set) {
            if (hasDesyncHadTimeToDesync || hasDesyncHadTimeToLBY) {
                Animstate->m_flGoalFeetYaw = record->ResolveInfo.CurrentSet.LBYAngle; /* Restore */
                entity->getAbsAngle() = record->ResolveInfo.CurrentSet.ABSAngles;
                entity->eyeAngles() = record->ResolveInfo.CurrentSet.EyeAngles;
                entity->UpdateState(Animstate, entity->eyeAngles());
            }
        }
    }


    if (!(hasDesyncHadTimeToDesync || hasDesyncHadTimeToLBY) && PresumedNoDesync) {
        ResolverDebug(std::wstring{ L"[Resolver.cpp:459] Exiting, Presumed No Desync [" + name + L"] -> " + std::to_wstring(memory->globalVars->currenttime) });
        record->FiredUpon = false;
        return;
    }

    if (!record->ResolveInfo.Original.Set) {
        record->FiredUpon = false;
        ResolverDebug(std::wstring{ L"[Resolver.cpp:392] Exiting, ResolveInfo.Original.Set == false [" + name + L"] -> " + std::to_wstring(memory->globalVars->currenttime)});
        return;
    }

    float DesyncAng = 0.0f;

    if ((record->lbyUpdated && (AnimLayerThree->cycle < .9f) && IN_LBY_ACT) && !record->FiredUpon){
        DesyncAng += (record->totalshots % 3) ? 0 : (record->totalshots % 2) ? -58 : 58;
        Vector ABS = record->ResolveInfo.Original.ABSAngles;
        //Vector EyeAngles = record->ResolveInfo.Original.EyeAngles;
        float LBYAngles = record->ResolveInfo.Original.LBYAngle;
        ABS.y += DesyncAng;
        //EyeAngles.y += DesyncAng;
        LBYAngles += DesyncAng;
        entity->setAbsAngle(ABS);
        //entity->eyeAngles() = EyeAngles;
        Animstate->m_flCurrentFeetYaw = LBYAngles;
        record->lbybackup.Prev = true;
        record->ResolveInfo.wasUpdated = true;
        entity->UpdateState(Animstate, entity->eyeAngles());
        if (record->wasTargeted) {
            Debug::LogItem item;
            item.PrintToScreen = false;           
            //item.text.push_back(L" ");
            //item.text.push_back(L" ");
            item.text.push_back(std::wstring{ L"[Resolver] Assumed LBY Update for " + name + L" at current time " + std::to_wstring(memory->globalVars->currenttime) + L" setting Angles to " + std::to_wstring(DesyncAng) });
            Debug::LOG_OUT.push_front(item);
        }

        return;
    }


    float LBYAng = 0.0f;

    if (!record->lbyUpdated || ((AnimLayerThree->cycle > .9f) && IN_LBY_ACT) || !IN_LBY_ACT || record->FiredUpon) { /* Simple Little Brute Force Method */
        record->FiredUpon = false;
        if ((AnimLayerThree->cycle < .9f) && hasDesyncHadTimeToLBY) {
            LBYAng = (record->totalshots % 2) ? 58 : 0;

            switch (record->missedshots) {
            case 0:
                DesyncAng += 58;
                break;
            case 1:
                DesyncAng += -58.f;
                break;
            case 2:
                DesyncAng = 0;
                break;
            case 3:
                DesyncAng -= 29.0f;
                break;
            case 4:
                DesyncAng += 29.0f;
                record->missedshots = 0;
                break;
            default:
                DesyncAng += 0;
                record->missedshots = 0;
                break;
            }
            DesyncAng += DesyncAng;

        } else if (move) {
            switch ((record->totalshots%2)) {
            case 0:
                DesyncAng -= 58;
                break;
            case 1:
                DesyncAng += 58;
                break;
            }
            DesyncAng *= DesyncFrac;




        } else  { /*Micromoving(?), leading me to believe they are not Extended LBY breaking, and possibly sub Max Desync*/
            switch (record->missedshots) {
            case 0:
                DesyncAng -= 58;
                break;
            case 1:
                DesyncAng += 58;
                break;
            case 2:
                DesyncAng = 0;
                break;
            case 3:
                DesyncAng -= (58 / 2);
                break;
            case 4:
                DesyncAng += (58 / 2);
                break;
            case 5:
                if (!IN_LBY_ACT) {
                    DesyncAng -= 58;
                    record->missedshots = 1;
                    break;
                }
                DesyncAng += 116;         
                break;
            case 6:
                DesyncAng -= 116;
                record->missedshots = 0;
                break;
            default:
                DesyncAng += 0;
                record->missedshots = 0;
                break;
            }

        }

        if (DesyncAng == record->ResolveInfo.ResolveAddition.LBYAngle) {
            if (record->wasTargeted) {
                Debug::LogItem item;
                item.PrintToScreen = false;
                if (config->debug.movefix) {
                    item.text.push_back(std::wstring{ L"Exiting [467] Desync Ang: [" + std::to_wstring(DesyncAng) + L"] ResolveInfo.ResolveAddition.LBYAngle: [" + std::to_wstring(record->ResolveInfo.ResolveAddition.LBYAngle) + L"]" });
                    Debug::LOG_OUT.push_front(item);
                }
                return;
            }
            else {
                if (config->debug.movefix) {
                    std::wstring name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(entity->getPlayerName(true));
                    ResolverDebug(std::wstring{ L"Exiting [467] Desync Ang: [" + std::to_wstring(DesyncAng) + L"] ResolveInfo.ResolveAddition.LBYAngle: [" + std::to_wstring(record->ResolveInfo.ResolveAddition.LBYAngle) + L"] for entity " + name });
                }
                return;
            }
        }

        Animstate->m_flGoalFeetYaw = record->ResolveInfo.Original.LBYAngle; /* Restore */
        entity->setAbsAngle(record->ResolveInfo.Original.ABSAngles);
        entity->eyeAngles() = record->ResolveInfo.Original.EyeAngles;

        entity->UpdateState(Animstate, entity->eyeAngles());

        record->ResolveInfo.ResolveAddition.ABSAngles = { 0, DesyncAng, 0 };
        //record->ResolveInfo.ResolveAddition.EyeAngles = { 0, DesyncAng, 0 };//= { 0, DesyncAng, 0 };
        record->ResolveInfo.ResolveAddition.EyeAngles = { 0,0,0 };
        record->ResolveInfo.ResolveAddition.LBYAngle = DesyncAng;

        record->ResolveInfo.CurrentSet.Set = true;
        record->ResolveInfo.CurrentSet.LBYAngle = Animstate->m_flGoalFeetYaw + record->ResolveInfo.ResolveAddition.LBYAngle; /* Set Current Angles */
        record->ResolveInfo.CurrentSet.ABSAngles = (entity->getAbsAngle() + record->ResolveInfo.ResolveAddition.ABSAngles);
        record->ResolveInfo.CurrentSet.EyeAngles = entity->eyeAngles() + record->ResolveInfo.ResolveAddition.EyeAngles; ;

        Animstate->m_flGoalFeetYaw = record->ResolveInfo.CurrentSet.LBYAngle; /* Add on the Additions */
        entity->setAbsAngle(record->ResolveInfo.CurrentSet.ABSAngles);
        entity->eyeAngles() = record->ResolveInfo.CurrentSet.EyeAngles;

        entity->UpdateState(Animstate, entity->eyeAngles());

        Debug::LogItem item;
        item.PrintToScreen = false;


        std::wstring name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(entity->getPlayerName(true));
        //item.text.push_back(L" ");
        //item.text.push_back(L" ");
        if (config->debug.resolverDebug) {
            item.text.push_back(std::wstring{ L"[Resolver] Attempting to Resolve " + name + L" setting (Y) Angles to " + std::to_wstring(record->ResolveInfo.CurrentSet.EyeAngles.y) + L" Offset " + std::to_wstring(DesyncAng) + L" Original LBY/EyeAngles Were " + std::to_wstring(record->ResolveInfo.Original.LBYAngle) + L"/" + std::to_wstring(record->ResolveInfo.Original.LBYAngle) });
            std::wstring lby_str = std::wstring{ ((record->lbyUpdated) ? L" We Believe LBY Was Updated " : L" We Believe LBY Was Not Updated ") };
            item.text.push_back(std::wstring{ L"[Resolver]" + lby_str + L"at current time " + std::to_wstring(memory->globalVars->currenttime) });
            std::wstring micromove_str = std::wstring{ (AnimLayerThree->cycle > .9f) ? L"[Resolver] Players Cycle is >.9, Assumed to Be Micro-Moving" : L"[Resolver] Players Cycle is <.9, Assumed to not Be Micro-Moving, extended LBY-Desync?" };
            item.text.push_back(micromove_str);
            Debug::LOG_OUT.push_front(item);
        }

        if (config->debug.ResolverOut.enabled) {
            Debug::LogItem item2;
            item2.PrintToConsole = false;

            /*
            
                Resolver Flags:

                MM = Micro-Move
                EL = Extended LBY

                (LBY THIS TICK)
                UL = Updated LBY
                NU = No Updated LBY
            
                M: Moving (No to less desync)
                S: Still

                DT: Has had time to Desync
                ND: Has not had time to Desync

                LT: Has had time to break-LBY
                NL: Has not had time to break-LBY

                LA: In 979 Animation Act
                NA: Not in 979 Animation Act
            
            */



            std::wstring micromove_str = std::wstring{ (AnimLayerThree->cycle > .9f) ? L"MM:" : L"EL:"  };
            std::wstring lby_str = std::wstring{ ((record->lbyUpdated) ? L"UL:" : L"NU:")               };
            std::wstring move_str = std::wstring{ ((move) ? L"M:" : L"S:")                              };
            std::wstring DesyT = std::wstring{ ((hasDesyncHadTimeToDesync) ? L"DT:" : L"ND:")           };
            std::wstring LBYT = std::wstring{ ((hasDesyncHadTimeToLBY) ? L"LT:" : L"NL:")               };
            std::wstring LBYA = std::wstring{ ((IN_LBY_ACT) ? L"LA" : L"NA")                            };




            item2.text.push_back(std::wstring{ L"[Resolver] Attempting To Resolve " + name + L" With Angles: " + std::to_wstring(DesyncAng) + L" " + micromove_str + lby_str + move_str + DesyT + LBYT + LBYA});
            item2.color = { (int)config->debug.ResolverOut.color[0] * 255,(int)config->debug.ResolverOut.color[1] * 255, (int)config->debug.ResolverOut.color[2] * 255 };
            Debug::LOG_OUT.push_front(item2);
        }
        record->ResolveInfo.wasUpdated = true;
        record->FiredUpon = false;
        return;
    }
    else {
        Debug::LogItem item;
        item.PrintToScreen = false;
        item.text.push_back(std::wstring{ L"Error [Resolver.cpp:511]: Exiting, no conditions met for resolve" });
        Debug::LOG_OUT.push_front(item);
        record->FiredUpon = false;
    }


    record->FiredUpon = false;

    ResolverDebug(std::wstring{ L"[Resolver.cpp:542] Exiting, End of function" });

    






    // __MAKE INTO FUNCTION__




}



/*
void Resolver::BasicResolver(Entity* entity, int missed_shots) {

    if (!config->debug.animstatedebug.resolver.basicResolver)
        return;

    config->debug.indicators.resolver = false;
    if (!localPlayer || !localPlayer->isAlive() || !entity || entity->isDormant() || !entity->isAlive())
        return;

    if (PlayerRecords.empty())
        return;

    auto record = &PlayerRecords.at(entity->index());

    if (!record || record->invalid)
        return;

    if (entity->getVelocity().length2D() > 5.0f) {
        record->PreviousEyeAngle = entity->eyeAngles().y;
        return;
    }

    auto b_record = Backtrack::records[entity->index()];
    if ((b_record.size() > 0) && !b_record.empty() && Backtrack::valid(b_record.front().simulationTime))
    {
        if (b_record.front().onshot)
            return;
    }


    auto Animstate = entity->getAnimstate();

    if (!Animstate)
        return;


    bool lby = record->lbyUpdated;

    if (!record->FiredUpon || !record->wasTargeted) { // || !record->wasTargeted
        for (int b = 0; b < entity->getAnimationLayerCount(); b++) {
            auto AnimLayer = entity->getAnimationLayer(b);

            if (entity->getSequenceActivity(AnimLayer->sequence) == ACT_CSGO_FIRE_PRIMARY && (AnimLayer->weight > (0.5f)) && (AnimLayer->cycle < .8f)) {
                return;
            }

        };

        entity->UpdateState(Animstate, entity->eyeAngles());
        if ((record->wasUpdated == false) && (entity->eyeAngles().y != record->PreviousEyeAngle) && (record->prevSimTime != entity->simulationTime())) {
            record->originalLBY = Animstate->m_flGoalFeetYaw;
            //record->PreviousEyeAngle = entity->eyeAngles().y;
            record->eyeAnglesOnUpdate = entity->eyeAngles().y;
            record->prevSimTime = entity->simulationTime();
            record->PreviousEyeAngle = entity->eyeAngles().y + (record->PreviousEyeAngle - record->PreviousDesyncAng);
            record->wasUpdated = true;
        } 





        
        if ((record->wasUpdated == true) && (entity->eyeAngles().y != record->PreviousEyeAngle) && (record->prevSimTime != entity->simulationTime())){
            //record->PreviousEyeAngle = entity->eyeAngles().y;

        }
        


        if (lby) {
            record->lbybackup.AbsAngle = entity->getAbsAngle();
            record->lbybackup.EyeAngle = entity->eyeAngles().y;
            record->lbybackup.Prev = true;
        } else if (!lby) {
            if (record->lbybackup.Prev == true) {
                record->lbybackup.Prev = false;
                entity->setAbsAngle(record->lbybackup.AbsAngle);
                entity->eyeAngles().y = record->lbybackup.EyeAngle;
                entity->UpdateState(Animstate, entity->eyeAngles());
            }
        }

        entity->eyeAngles().y = record->PreviousEyeAngle;
        entity->UpdateState(Animstate, entity->eyeAngles());

        
    }


    float DesyncAng = record->originalLBY;

    //if (record->wasUpdated == true)
    //   return;

    entity->UpdateState(Animstate, entity->eyeAngles());

    config->debug.indicators.resolver = true;

    int missed = record->missedshots;

    if (config->debug.animstatedebug.resolver.missed_shots > 0) {
        missed = config->debug.animstatedebug.resolver.missed_shots;
    }

    if (record->lastworkingshot != -1) {
        missed = record->lastworkingshot;
        record->lastworkingshot = -1;
    }

    if (!lby) {
        switch (missed) {
        case 1: // POP all those pasted Polak AA's
            if (record->originalLBY == 0) {
                record->originalLBY = Animstate->m_flGoalFeetYaw;
            }
            DesyncAng = record->originalLBY;
            break;
        case 2:
            DesyncAng += 58.0f;
            break;
        case 3:
            DesyncAng -= 58.0f;
            break;
        case 4:
            DesyncAng += 29.0f;
            break;
        case 5:
            DesyncAng -= 29.0f;
            break;
        case 7:
            DesyncAng += -116.0f;
            break;
        case 8:
            DesyncAng += +116.0f;
            break;
  DesyncAng += 15;
        default:
            DesyncAng += 5;
            record->missedshots = 0;
            break;
        }
    }
    else {
        DesyncAng += (record->totalshots % 3) ? 0 : (record->totalshots % 2) ? -(entity->getMaxDesyncAngle()) : (entity->getMaxDesyncAngle());
    }


    if (DesyncAng > 180) {
        float off = DesyncAng - 180;
        DesyncAng = -180 + off;
    }
    else  if (DesyncAng < -180) {
        DesyncAng = (DesyncAng + 180) + 180;
    }

    Debug::LogItem item;
    item.PrintToScreen = false;


    //std::wstring name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(entity->getPlayerName(true));
    //item.text.push_back(std::wstring{ L"[Resolver] Attempting to Resolve " + name + L" setting (Y) Angles to " + std::to_wstring(DesyncAng) + L" Offset " + std::to_wstring(DesyncAng - record->originalLBY) + L" Original LBY/EyeAngles Were " + std::to_wstring(record->originalLBY) + L"/" + std::to_wstring(record->PreviousEyeAngle)});
    //std::wstring lby_str = std::wstring{ ((lby) ? L" We Believe LBY Was Updated " : L" We Believe LBY Was Not Updated ") };
    //item.text.push_back(std::wstring{L"[Resolver]" + lby_str + L"at current time " + std::to_wstring(memory->globalVars->currenttime)});


    entity->eyeAngles().y = DesyncAng;
    entity->setAbsAngle(entity->eyeAngles());

    //record->missedshots = missed;

    Animstate->m_flGoalFeetYaw = DesyncAng;

    entity->UpdateState(Animstate, entity->eyeAngles());


        std::wstring name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(entity->getPlayerName(true));
        item.text.push_back(std::wstring{ L"[Resolver] Attempting to Resolve " + name + L" setting (Y) Angles to " + std::to_wstring(DesyncAng) + L" Offset " + std::to_wstring(DesyncAng - record->originalLBY) + L" Original LBY/EyeAngles Were " + std::to_wstring(record->originalLBY) + L"/" + std::to_wstring(record->PreviousEyeAngle) });
        std::wstring lby_str = std::wstring{ ((lby) ? L" We Believe LBY Was Updated " : L" We Believe LBY Was Not Updated ") };
        item.text.push_back(std::wstring{ L"[Resolver]" + lby_str + L" at current time " + std::to_wstring(memory->globalVars->currenttime) });

        if (!lby) {
            record->PreviousEyeAngle = entity->eyeAngles().y;

            if (record->PreviousEyeAngle > 180) {
                float off = record->PreviousEyeAngle - 180;
                record->PreviousEyeAngle = -180 + off;
            }
            else  if (DesyncAng < -180) {
                record->PreviousEyeAngle = (record->PreviousEyeAngle + 180) + 180;
            }
            record->PreviousDesyncAng = DesyncAng;
            record->wasTargeted = true;
            record->FiredUpon = false;
            record->wasUpdated = true;
        }
        
    Debug::LOG_OUT.push_front(item);
    return;

}
*/