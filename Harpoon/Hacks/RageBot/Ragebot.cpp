#include "Ragebot.h"
#include <mutex>
#include <numeric>
#include <sstream>
#include <codecvt>
#include <locale>

#include "../Other/Debug.h"
#include "../../Config.h"
#include "../../SDK/OsirisSDK/ConVar.h"
#include "../../Interfaces.h"
#include "../../Memory.h"
#include "../../SDK/OsirisSDK/Entity.h"
#include "../../SDK/OsirisSDK/UserCmd.h"
#include "../../SDK/OsirisSDK/Vector.h"
#include "../../SDK/OsirisSDK/WeaponId.h"
#include "../../SDK/OsirisSDK/GlobalVars.h"
#include "../../SDK/OsirisSDK/PhysicsSurfaceProps.h"
#include "../../SDK/OsirisSDK/WeaponData.h"
#include "../../SDK/OsirisSDK/ModelInfo.h"
#include "../../SDK/OsirisSDK/matrix3x4.h"
#include "../../SDK/OsirisSDK/Math.h"
#include "../../Other/Animations.h"
#include "../../GameData.h"
#include "../Backtrack.h"
#include "Resolver.h"
#include "../../SDK/OsirisSDK/Angle.h"
#include "../../Other/AutoWall.h"
#include "../../SDK/SDKAddition/Utils/VectorMath.h"
#include "Multipoints.h"
//DEBUG
#include <fstream>
#include <iostream>
#include <cstddef>
#include <thread>
#include <future>

#include <math.h>

static void setRandomSeed(int seed) noexcept
{
    using randomSeedFn = void(*)(int);
    static auto randomSeed{ reinterpret_cast<randomSeedFn>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomSeed")) };
    randomSeed(seed);
}

static float getRandom(float min, float max) noexcept
{
    using randomFloatFn = float(*)(float, float);
    static auto randomFloat{ reinterpret_cast<randomFloatFn>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat")) };
    return randomFloat(min, max);
}

#include "../../ClientHooks.h"
#include "../Aimbot.h"
#include "../../Timing.h"
#include "../../SDK/SDKAddition/EntityListCull.hpp"

extern void SendZeroSizedPacket(volatile NetworkChannel* network);

#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define ROUND_TO_TICKS( t )		( TICK_INTERVAL * TIME_TO_TICKS( t ) )
bool ForcePeak()
{

    NetworkChannel* pNetchan = interfaces->engine->getNetworkChannel();

    if (!pNetchan)
        return false;

    if (localPlayer->velocity().length2D() > 5.f)
        return false;


    bool bVisFound = false;
    for (EntityQuick& entq : entitylistculled->getEnemies())
    {
        if (entq.m_bisVisible)
        {
            bVisFound = true;
            break;
        }
    }

    if (bVisFound)
        return false;

    bool bShouldForcePeak = false;

    for (EntityQuick& entq : entitylistculled->getEnemies())
    {
        if (entq.m_bisDormant || !entq.m_bisAlive)
            continue;

        if (entq.m_bDidForcePeak)
        {
            entq.m_bDidForcePeak = false;
            return false;
        }

        Vector vecVelocity = entq.entity->getVelocity();

        float flBehind = (pNetchan->getLatency(FLOW_AVG)*2.f) + (memory->globalVars->intervalPerTick * 6.f);


        Vector vecPredictedPos = (vecVelocity * flBehind) + entq.entity->getEyePosition();

        //Do autowall here in the future
        if (localPlayer->canSeePoint(vecPredictedPos))
        {
            entq.m_bDidForcePeak = true;
            bShouldForcePeak = true;
            break;
        }
    }

    if (!bShouldForcePeak)
        return false;

    for(int i = 0; i < config->mmlagger.blippackets; i++)
        SendZeroSizedPacket((volatile NetworkChannel*)pNetchan);
    
    Timing::ExploitTiming.m_bDidBlip = true;

    Timing::ExploitTiming.m_nTicksLeftToDrop = config->mmlagger.blippackets;

    return true;
}



static float RageBot::hitChance(Entity* localPlayer, Entity* entity, Entity* weaponData, const Vector& destination, const UserCmd* cmd, const int hitChance) noexcept
{

    if (!hitChance)
        return 100.0f;

    Debug::LogItem item;
    item.PrintToConsole = true;

    int maxSeed = 256;

    const Angle angles(destination + cmd->viewangles);

    int hits = 0;
    const int hitsNeed = static_cast<int>(static_cast<float>(maxSeed) * (static_cast<float>(hitChance) / 100.f));
    auto calc_hitchance = [](int hits, int hitsNeed) {return ((static_cast<float>(hits) / static_cast<float>(hitsNeed) * 100.f)); };
    const auto weapSpread = weaponData->getSpread();
    const auto weapInaccuracy = weaponData->getInaccuracy();
    const auto localEyePosition = localPlayer->getEyePosition();
    const auto range = weaponData->getWeaponData()->range;

    for (int i = 0; i < maxSeed; ++i)
    {
        setRandomSeed(i + 1);
        float inaccuracy = getRandom(0.f, 1.f);
        float spread = getRandom(0.f, 1.f);
        const float spreadX = getRandom(0.f, 2.f * static_cast<float>(M_PI));
        const float spreadY = getRandom(0.f, 2.f * static_cast<float>(M_PI));

        const auto weaponIndex = weaponData->itemDefinitionIndex2();
        const auto recoilIndex = weaponData->recoilIndex();
        if (weaponIndex == WeaponId::Revolver)
        {
            if (cmd->buttons & UserCmd::IN_ATTACK2)
            {
                inaccuracy = 1.f - inaccuracy * inaccuracy;
                spread = 1.f - spread * spread;
            }
        }
        else if (weaponIndex == WeaponId::Negev && recoilIndex < 3.f)
        {
            for (int i = 3; i > recoilIndex; --i)
            {
                inaccuracy *= inaccuracy;
                spread *= spread;
            }

            inaccuracy = 1.f - inaccuracy;
            spread = 1.f - spread;
        }

        inaccuracy *= weapInaccuracy;
        spread *= weapSpread;

        Vector spreadView{ (cosf(spreadX) * inaccuracy) + (cosf(spreadY) * spread),
                           (sinf(spreadX) * inaccuracy) + (sinf(spreadY) * spread) };
        Vector direction{ (angles.forward + (angles.right * spreadView.x) + (angles.up * spreadView.y)) * range };

        static Trace trace;
        interfaces->engineTrace->clipRayToEntity({ localEyePosition, localEyePosition + direction }, 0x4600400B, entity, trace);
        if (trace.entity == entity)
            ++hits;



        if (hits >= hitsNeed) {
            if (config->debug.aimbotcoutdebug) {
                item.PrintToScreen = false;
                item.text.push_back(L"Hitchance Completed With " + std::to_wstring(calc_hitchance(hits, hitsNeed)) + L" Likelihood of hitting, " + std::to_wstring(hits) + L" Hits");
                Debug::LOG_OUT.push_front(item);
            }
            return calc_hitchance(hits, hitsNeed);
        }

        if ((maxSeed - i + hits) < hitsNeed) {
            if (config->debug.aimbotcoutdebug) {
                item.PrintToScreen = false;
                item.text.push_back(L"Hitchance Completed With " + std::to_wstring(calc_hitchance(hits, hitsNeed)) + L" Likelihood of hitting, " + std::to_wstring(hits) + L" Hits");
                Debug::LOG_OUT.push_front(item);
            }
            return -1.0f;
        }
    }
    if (config->debug.aimbotcoutdebug) {
        item.PrintToScreen = false;
        item.text.push_back(L"Hitchance Completed With " + std::to_wstring(calc_hitchance(hits, hitsNeed)) + L" Likelihood of hitting, " + std::to_wstring(hits) + L" Hits");
        Debug::LOG_OUT.push_front(item);
    }
    return -1.0f;
}

#include "../../SDK/SDKAddition/Utils/CapsuleMath.h"
#include "../../SDK/SDKAddition/Utils/MatrixMath.h"
float RageBot::NewhitChance(Entity* localPlayer, Entity* entity, Entity* weaponData, const int iweaponIndex, const Vector& destination, const UserCmd* cmd, const int hitChance, matrix3x4* matrix, int hitbox) noexcept
{

    if (!hitChance)
        return 100.0f;


    const Model* model = entity->getModel();

    if (!model)
        return -10.f;

    StudioHdr* hdr = interfaces->modelInfo->getStudioModel(model);

    if (!hdr)
        return -20.f;

    int hitBoxSet = entity->hitboxSet();

    if (hitBoxSet < 0)
        return -30.f;

    StudioHitboxSet* hitBoxSetPtr = hdr->getHitboxSet(hitBoxSet);

    if (!hitBoxSetPtr)
        return -40.f;


    StudioBbox* box = hitBoxSetPtr->getHitbox(hitbox);

    Debug::LogItem item;
    item.PrintToConsole = true;

    int maxSeed = 256;

    const Angle angles(destination + cmd->viewangles);

    int hits = 0;
    const int hitsNeed = static_cast<int>(static_cast<float>(maxSeed) * (static_cast<float>(hitChance) / 100.f));

    const auto weapSpread = weaponData->getSpread();
    const auto weapInaccuracy = weaponData->getInaccuracy();
    const auto localEyePosition = localPlayer->getEyePosition();
    const auto range = weaponData->getWeaponData()->range;
    auto calc_hitchance = [](int hits, int hitsNeed) {return ((static_cast<float>(hits) / static_cast<float>(hitsNeed)) * 100.f); };

    for (int i = 0; i < maxSeed; ++i)
    {
        setRandomSeed(i + 1);
        float inaccuracy = getRandom(0.f, 1.f);
        float spread = getRandom(0.f, 1.f);
        const float spreadX = getRandom(0.f, 2.f * static_cast<float>(M_PI));
        const float spreadY = getRandom(0.f, 2.f * static_cast<float>(M_PI));

        const auto weaponIndex = weaponData->itemDefinitionIndex2();
        const auto recoilIndex = weaponData->recoilIndex();
        if (weaponIndex == WeaponId::Revolver)
        {
            if (cmd->buttons & UserCmd::IN_ATTACK2)
            {
                inaccuracy = 1.f - inaccuracy * inaccuracy;
                spread = 1.f - spread * spread;
            }
        }
        else if (weaponIndex == WeaponId::Negev && recoilIndex < 3.f)
        {
            for (int i = 3; i > recoilIndex; --i)
            {
                inaccuracy *= inaccuracy;
                spread *= spread;
            }

            inaccuracy = 1.f - inaccuracy;
            spread = 1.f - spread;
        }

        inaccuracy *= weapInaccuracy;
        spread *= weapSpread;

        Vector spreadView{ (cosf(spreadX) * inaccuracy) + (cosf(spreadY) * spread),
                           (sinf(spreadX) * inaccuracy) + (sinf(spreadY) * spread) };
        Vector direction{ (angles.forward + (angles.right * spreadView.x) + (angles.up * spreadView.y)) * range };

        /*
        static Trace trace;

        interfaces->engineTrace->clipRayToEntity({ localEyePosition, localEyePosition + direction }, 0x4600400B, entity, trace);
        if (trace.entity == entity)
            ++hits;
        */
        if (config->RageBot[iweaponIndex].Async) {
            if (CapsuleMath::HitBoxRayIntersectionAsync(localEyePosition, localEyePosition + direction, *box, matrix[box->bone])) {
                ++hits;
            }
            else {
                if (!config->RageBot[iweaponIndex].ParseHitboxOnly) {
                    if (MatrixMath::DoesRayIntersectMatrixAsync(localEyePosition, localEyePosition + direction, hdr, hitBoxSet, matrix)) {
                        ++hits;
                    }
                }
            }
        }
        else {
            if (CapsuleMath::HitBoxRayIntersection(localEyePosition, localEyePosition + direction, box, matrix[box->bone])) {
                ++hits;
            }
            else {
                if (!config->RageBot[iweaponIndex].ParseHitboxOnly) {
                    if (MatrixMath::DoesRayIntersectMatrix(localEyePosition, localEyePosition + direction, hdr, hitBoxSet, matrix)) {
                        ++hits;
                    }
                }
            }
        }

        

        if (hits >= hitsNeed) {
            /*
            if (config->debug.aimbotcoutdebug) {
                item.PrintToScreen = false;
                item.text.push_back(L"Hitchance Completed With " + std::to_wstring(calc_hitchance(hits,hitsNeed)) + L" Likelihood of hitting, " + std::to_wstring(hits) + L" Hits") ;
                Debug::LOG_OUT.push_front(item);
            }*/
            return calc_hitchance(hits, hitsNeed);
        }

        if ((maxSeed - i + hits) < hitsNeed) {
            /*
            if (config->debug.aimbotcoutdebug) {
                item.PrintToScreen = false;
                item.text.push_back(L"Hitchance Completed With " + std::to_wstring(calc_hitchance(hits, hitsNeed)) + L" Likelihood of hitting, " + std::to_wstring(hits) + L" Hits");
                Debug::LOG_OUT.push_front(item);
            }*/
            return calc_hitchance(hits, hitsNeed);
        }
    }


    if (config->debug.aimbotcoutdebug) {
        /*
        item.PrintToScreen = false;
        item.text.push_back(L"Hitchance Completed With " + std::to_wstring(calc_hitchance(hits, hitsNeed)) + L" Likelihood of hitting, " + std::to_wstring(hits) + L" Hits");
        Debug::LOG_OUT.push_front(item);
        */
    }
    return calc_hitchance(hits, hitsNeed);
}

void RageBot::Autostop(UserCmd* cmd) noexcept
{

    if (!localPlayer || !localPlayer->isAlive())
        return;

    Vector Velocity = localPlayer->velocity();

    if (Velocity.length2D() == 0)
        return;

    static float Speed = 450.f;

    Vector Direction;
    Vector RealView;
    Math::VectorAngles(Velocity, Direction);
    interfaces->engine->getViewAngles(RealView);
    Direction.y = RealView.y - Direction.y;

    Vector Forward;
    Math::AngleVectors(Direction, &Forward);
    Vector NegativeDirection = Forward * -Speed;

    cmd->forwardmove = NegativeDirection.x;
    cmd->sidemove = NegativeDirection.y;
}

inline float DistanceToEntity(Entity* Enemy)
{
    return sqrt(pow(localPlayer->origin().x - Enemy->origin().x, 2) + pow(localPlayer->origin().y - Enemy->origin().y, 2) + pow(localPlayer->origin().z - Enemy->origin().z, 2));
}

#include "../../SDK/OsirisSDK/Surface.h"
static float worldToScreen(const Vector& in) noexcept
{
    const auto& matrix = GameData::toScreenMatrix();

    const auto w = matrix._41 * in.x + matrix._42 * in.y + matrix._43 * in.z + matrix._44;



    //auto out = interfaces->surface / 2.0f;
    //auto tester = StreamProofESP::ScreenSize / 2.0f;

    auto [ww, h] = interfaces->surface->getScreenSize();

    struct coord {
        float x;
        float y;
    } out, tester;

    out.x = ww / 2;
    tester.x = ww / 2;
    out.y = h / 2;
    tester.y = h / 2;


    out.x *= 1.0f + (matrix._11 * in.x + matrix._12 * in.y + matrix._13 * in.z + matrix._14) / w;
    out.y *= 1.0f - (matrix._21 * in.x + matrix._22 * in.y + matrix._23 * in.z + matrix._24) / w;

    if (w < 0.001f) {
        return 99999.0f;
    }

    return (std::abs(out.x - tester.x) + std::abs(out.y - tester.y)) / 2;
}

#include "../../SDK/SDKAddition/EntityListCull.hpp"
#include "../../SDK/SDKAddition/Utils/ScreenMath.h"


static std::vector<RageBot::EntityVal> SetUpSortedEnemies() noexcept {
    std::vector<RageBot::EntityVal> enemies = {};

    struct entity_sort {
        bool operator() (RageBot::EntityVal A, RageBot::EntityVal B) {

            if ((A.distance < 20.0f) && (B.distance > 20.0f)) {
                return true;
            }
            if ((A.distance > 20.0f) && (B.distance < 20.0f)) {
                return false;
            }
            if (A.isVisible && !B.isVisible) {
                return true;
            }
            else if (!A.isVisible && B.isVisible) {
                return false;
            }
            if (A.wasTargeted && !B.wasTargeted) {
                return true;
            }
            else if (!A.wasTargeted && B.wasTargeted) {
                return false;
            }

            else {
                return false;
            }
            if (A.entity < B.entity) { // WTF IS THIS
                return true;
            }

        }
    } ent_sort;


    static auto frameRate = 1.0f;
    frameRate = 1 / (0.9f * frameRate + 0.1f * memory->globalVars->absoluteFrameTime);


    bool visFound = false;
    for (EntityQuick EntQuick : entitylistculled->getEnemies()) {
        Entity* entity = EntQuick.entity;
        if (entity->gunGameImmunity()) {
            Resolver::PlayerRecords.at(EntQuick.index).wasTargeted = false;
            continue;
        }
        RageBot::EntityVal curr_ent;
        curr_ent.set = true;
        curr_ent.distance = ScreenMath::worldToScreenDistanceFromCenter(entity->getEyePosition());
        curr_ent.entity = EntQuick.index;
        curr_ent.health = entity->health();
        curr_ent.entity_ptr = entity;
        Resolver::Record* record = &Resolver::PlayerRecords.at(entity->index());
        curr_ent.btRecords = &Backtrack::records[entity->index()]; // Get Backtrack Record
        if (record && !record->invalid) {
            curr_ent.wasTargeted = record->wasTargeted;
            curr_ent.record = record;
            curr_ent.prevVelocity = record->prevVelocity;
        }
        else {
            curr_ent.wasTargeted = false;
            curr_ent.record = &Resolver::invalid_record;
        }

        if (!visFound) {
            curr_ent.isVisible = entity->isVisible();
            visFound = true;
        }
        else {
            curr_ent.isVisible = false;
        }
        enemies.push_back(curr_ent);
    } // set up entities


    Debug::DamageQuick = enemies.size();

    if (enemies.empty()) {
        return enemies;
    }


    if (enemies.size() > 1) {
        std::sort(enemies.begin(), enemies.end(), ent_sort);
    }

    

    if (!enemies.front().wasTargeted) {
        enemies.front().wasTargeted = true;
    }


    return enemies;

}

bool RageBot::SetupHitboxSelection(std::vector<Multipoints::Hitbox>& Hitboxes, InfoContainer& info, int weaponIndex) noexcept{
    bool baimTriggered = false; // set up baim 
    Resolver::Record* record = info.enemy.record;
    if (record && !record->invalid) {
        if (info.enemy.bt && config->RageBot[weaponIndex].baim && !info.enemy.onshot) { // if going for bt, just baim
            baimTriggered = true;
        }
        //if (info.enemy.entity_ptr->velocity().length2D() < 10.0f) {
        //    if ((record->missedshotsthisinteraction > config->RageBot[weaponIndex].baimshots) && (!info.enemy.onshot)) {
        //        baimTriggered = true;
         //   }
         //   else if ((info.enemy.health < config->RageBot[weaponIndex].minDamage) && (!info.enemy.onshot))
          //      baimTriggered = true;
        //}
    }


    for (int i = 0; i < (HITBOX_MAX-2); i++) { // setup those hitboxes
            if (config->RageBot[weaponIndex].hitboxes[i] == true) {
                Hitboxes.push_back((Multipoints::Hitbox)i);
            }
    }
    if (config->RageBot[weaponIndex].respectHitbox)
        return true;


    if (baimTriggered) {
        Hitboxes = {
                Multipoints::HITBOX_PELVIS,
                Multipoints::HITBOX_BODY,
                Multipoints::HITBOX_UPPER_CHEST,
                Multipoints::HITBOX_THORAX,
        };
    }

    if (info.enemy.isVisible) { // if we can see the bastard, why would we shoot at his fuckin' ankles? 

        Hitboxes = {
            Multipoints::HITBOX_HEAD,
            Multipoints::HITBOX_NECK,
            Multipoints::HITBOX_PELVIS,
            Multipoints::HITBOX_BODY,
            Multipoints::HITBOX_UPPER_CHEST,
            Multipoints::HITBOX_THORAX,
        };

    }
    else if (info.enemy.onshot) { // gimmie that head
        Hitboxes = {
                Multipoints::HITBOX_HEAD,
                Multipoints::HITBOX_NECK,

        };
    }
    else {
        /*
        Hitboxes = {
                    Multipoints::HITBOX_HEAD,
                    Multipoints::HITBOX_NECK,
                    Multipoints::HITBOX_PELVIS,
                    Multipoints::HITBOX_LEFT_THIGH,
                    Multipoints::HITBOX_RIGHT_THIGH,
                    Multipoints::HITBOX_LEFT_HAND,
                    Multipoints::HITBOX_RIGHT_HAND,
                    Multipoints::HITBOX_LEFT_FOREARM,
                    Multipoints::HITBOX_RIGHT_FOREARM
        };
        */
    }

    if (!info.enemy.wasTargeted) { // if we werent targeting this guy, lets just do these hitboxes
        Hitboxes = {
            Multipoints::HITBOX_HEAD,
            Multipoints::HITBOX_NECK,
            Multipoints::HITBOX_PELVIS,
            Multipoints::HITBOX_BODY,
            Multipoints::HITBOX_UPPER_CHEST,
            Multipoints::HITBOX_THORAX,
        };
    }


    if (config->RageBot[weaponIndex].pelvisAimOnLBYUpdate && info.enemy.lbyUpdated) {
        Hitboxes = {
                Multipoints::HITBOX_PELVIS,
                Multipoints::HITBOX_HEAD,
        };
    }


    if (config->RageBot[weaponIndex].PrioritizeHeadOnNoDesync && record->noDesync) {
        Hitboxes = {
            Multipoints::HITBOX_HEAD,
            Multipoints::HITBOX_NECK,
            Multipoints::HITBOX_PELVIS,
        };
    }

    if (config->RageBot[weaponIndex].OnshotOrDesyncless && (!info.enemy.onshot || !record->noDesync || !record->move)) {
        Hitboxes = {
            Multipoints::HITBOX_PELVIS,
        };
    }


    if (Hitboxes.empty()) {
        return false;
    }

    return true;
}

void RageBot::SetupBacktrack(InfoContainer& info, int weaponIndex) noexcept {
    std::deque<Backtrack::Record>* brecord = info.enemy.btRecords;
    RageBot::EntityVal enemy = info.enemy;

    if (config->backtrack.enabled && !brecord->empty() && (brecord->size() > 0) && Backtrack::valid(brecord->front().simulationTime) && (enemy.wasTargeted /*|| Backtrack::ImportantTick(*brecord)*/)) { // Sanity // 

        bool ImportantTick = Backtrack::ImportantTick(*brecord);
        if ((!enemy.isVisible || ImportantTick /*|| (enemy.entity_ptr->velocity().length2D() > 170.0f)*/)) { // if Enemy isn't visible, or their velocity is  > 100, we go for bt
            for (int i = ((ImportantTick) ? 0 : 3); i < (brecord->size() - 1); i++) { // Ignore the last 2 records, i've noticed the likely hood of hitting those is essentially nonexistant
                if (i >= brecord->size()) {
                    break;
                }
                Backtrack::Record* c_record = &(brecord->at(i));
                if (!c_record || !Backtrack::valid(c_record->simulationTime)) {
                    continue;
                }
                else {
                    if ((localPlayer->canSeePoint(c_record->head)) || (c_record->onshot && config->RageBot[weaponIndex].onshot) || c_record->lbyUpdated) { // Visible backtrack, onshot, or lbyUpdated? Lets go for it!
                        if ((c_record->onshot && config->RageBot[weaponIndex].onshot) || (c_record->lbyUpdated && config->RageBot[weaponIndex].ShootForLBY)) { /* If it's an onshot or lbyUpdated Tick, we set override*/
                            Debug::LogItem item2;
                            item2.PrintToScreen = false;
                            enemy.onshot = c_record->onshot;
                            enemy.lbyUpdated = c_record->lbyUpdated;
                            if (enemy.lbyUpdated && config->RageBot[weaponIndex].ShootForLBY) {
                                item2.text.push_back(std::wstring{ L"[RageBot] Going for BT LBY Update for entity " + std::to_wstring(enemy.entity_ptr->index()) });
                            }
                            else if (enemy.onshot && config->RageBot[weaponIndex].onshot) {
                                item2.text.push_back(std::wstring{ L"[RageBot] Going for BT Onshot for entity " + std::to_wstring(enemy.entity_ptr->index()) });
                            }
                            if (config->debug.backtrackCount) { Debug::LOG_OUT.push_front(item2); }
                            Backtrack::SetOverride(enemy.entity_ptr, *c_record);

                        }
                        else {
                            if (ImportantTick && config->RageBot[weaponIndex].prioritizeEventBT) {
                                continue;
                            }
                        }
                        enemy.bt = true;
                        c_record->btTargeted = true;
                        Animations::setup(enemy.entity_ptr, *c_record);
                        info.backup_b_rec = c_record;
                        Backtrack::SetOverride(enemy.entity_ptr, *c_record);
                        break;
                    }
                    else {
                        continue;
                    }

                }

            }
        }
    }
}


bool RageBot::SetupMultipoints(InfoContainer& info, Multipoints::EntityStudioInfo& save) {

    /* So We Aren't Running SetupBones 1 million times*/
 



    if ((!info.enemy.bt && !Animations::data.player[info.enemy.entity_ptr->index()].hasBackup) && !config->debug.animstatedebug.resolver.enabled || config->debug.forcesetupBones) {
        if (!(Multipoints::MultipointSetup(info.enemy.entity_ptr, save))){   
            return false;
        }
    }
    else if (info.enemy.bt) {
        if (!Multipoints::MultipointSetup(info.enemy.entity_ptr, save, info.backup_b_rec->matrix, true)) {           
            return false;
        }
    }
    else if(config->debug.animstatedebug.resolver.enabled && info.enemy.record->ResolverMatrix){
        if (!Multipoints::MultipointSetup(info.enemy.entity_ptr, save, info.enemy.record->ResolverMatrix, true)) {
            return false;
        }
    }
    else if (Animations::data.player[info.enemy.entity_ptr->index()].hasBackup) {
        
        if (!Multipoints::MultipointSetup(info.enemy.entity_ptr, save, Animations::data.player[info.enemy.entity_ptr->index()].matrix, true)) {
            return false;
        }
    }
    else {
        if (!Multipoints::MultipointSetup(info.enemy.entity_ptr, save)) {
            return false;
        }
    }
    if (config->debug.aimbotcoutdebug) {
        //Debug::LogItem item;
        //item.PrintToScreen = false;
       // item.text.push_back(std::wstring{ L"Made it Past Multipoint set for entity " + std::to_wstring(info.enemy.entity_ptr->index()) });
       // Debug::LOG_OUT.push_front(item);
    }
    return true;
}



bool DoesPointWork(Entity* entity, bool visible, int weaponIndex, float& BestDamage, Vector Point) {
    Vector I;
    float damage = Autowall->Damage(Point, I, 0.0f);

    if (((damage > config->RageBot[weaponIndex].minDamage) || ((damage > entity->health()+5)))) {
        if (damage > BestDamage) {
            BestDamage = damage;
            return true;
        }
    }
    return false;
}

bool RageBot::ParsePoints(InfoContainer& info, Multipoints::HitboxPoints& MP, bool CenterOnly) {


    Entity* entity = info.enemy.entity_ptr;
    bool Visible = info.enemy.isVisible;
    if (DoesPointWork(entity, Visible, info.weaponIndex, info.bestDamage, MP.Origin.Point)) {
        info.BestPoint = MP.Origin.Point;
        info.TargetFound = true;
        return true;
    }


    if (CenterOnly)
        return false;

    for (Multipoints::Point point : MP.Points) {
        if (DoesPointWork(entity, Visible, info.weaponIndex, info.bestDamage, point.Point)) {
            info.BestPoint = point.Point;
            info.TargetFound = true;
            return true;
        }
    }

    return false;


}

void RageBot::New(UserCmd* cmd) noexcept {
    //if (localPlayer->nextAttack() > memory->globalVars->serverTime()) {
    //    return;
    //}

    //Get the active weapon
    const auto activeWeapon = localPlayer->getActiveWeapon();
    if (!activeWeapon || !activeWeapon->clip()) // does active weapon exist and have a clip in
        return;

    auto weaponIndex = getWeaponIndex(activeWeapon->itemDefinitionIndex2()); // get weapon index
    if (!weaponIndex) // no weapon index? then exit
        return;

    // get weapon class
    auto weaponClass = getWeaponClass(activeWeapon->itemDefinitionIndex2());
    if (!config->RageBot[weaponIndex].enabled) //if weaponIndex is not enabled
        weaponIndex = weaponClass; // weaponClass is now weaponIndex

    if (!config->RageBot[weaponIndex].enabled) // If weaponClass isnt set
        weaponIndex = 0; // weaponClass/Index is now 0 (ALL setting)


    // BEYOND HERE REALLY AINT NEEDED UNLESS YOU WANNA BE PERFECT ABOUT WHEN ITS ON OR NOT
    if (!config->RageBot[weaponIndex].betweenShots && (activeWeapon->nextPrimaryAttack() > memory->globalVars->serverTime())) {
        return;
    }


    if (activeWeapon->isKnife() || activeWeapon->isGrenade()) {
        return;
    }

    if (!config->RageBot[weaponIndex].autoShot && !(cmd->buttons & UserCmd::IN_ATTACK) && !(config->RageBot[weaponIndex].aimlock)) {
        return;
    }



    std::vector<EntityVal> enemies = {};
    enemies = SetUpSortedEnemies();

    if (enemies.empty())
        return;

    if (config->RageBot[weaponIndex].m_bForcePeak)
    {
        if (ForcePeak())
            return;
    }

    Debug::DamageQuick = enemies.size();

    InfoContainer info_save;
    info_save.weaponIndex = weaponIndex;
    for (EntityVal enemy : enemies) {

        info_save.enemy = enemy;
        info_save.bestDamage = 0;
        if (config->debug.movefix) {
            //Animations::setupMoveFix(enemy.entity_ptr);
        }


        std::vector<Multipoints::Hitbox> hitboxes;

        SetupBacktrack(info_save, weaponIndex);
        if (!SetupHitboxSelection(hitboxes, info_save, weaponIndex))
            continue;


        Multipoints::EntityStudioInfo msave;

        
        if (!SetupMultipoints(info_save, msave))
            continue;

        info_save.ESI = msave;

        Multipoints::PointScales scales;


        if(!info_save.enemy.bt)
            Animations::setupMatrix(enemy.entity_ptr, msave.bones);
        


        for (Multipoints::Hitbox hitbox : hitboxes) {
            Multipoints::HitboxPoints MP;

            if (config->RageBot[weaponIndex].safepoint && enemy.record->wasTargeted && !enemy.record->ResolverMatrices.empty()) {
                if (!Multipoints::retrieveSafePoint(msave, MP, scales, hitbox, enemy.record->Original))
                    continue;
            }
            else {
                if (!Multipoints::retrieveHitbox(msave, MP, scales, hitbox, -1.f, !info_save.enemy.wasTargeted || info_save.enemy.isVisible))
                   continue;
            }


            if (ParsePoints(info_save, MP, !info_save.enemy.wasTargeted)) {
                info_save.box = *msave.hitBoxSetPtr->getHitbox(hitbox);
                info_save.hitbox_save = hitbox;
                if (config->aimbot[weaponIndex].bone == 1) { /* Best Damage goes through all points, only exiting on Kill */
                    if (info_save.bestDamage > (info_save.enemy.health + 5)) {
                        break;
                    }
                    else {
                        continue;
                    }
                }
                else {
                    break;
                }
            }
        }



        if (info_save.TargetFound)
            break;

    }





    for (int i = 0; i < Resolver::PlayerRecords.size(); i++) {
        Resolver::Record* record_b = &Resolver::PlayerRecords.at(i);
        if (record_b && !record_b->invalid) {
            record_b->wasTargeted = false;
            record_b->FiredUpon = false;
        }
    }

    Debug::TargetVec = info_save.BestPoint;

    if (info_save.TargetFound) {
        Debug::TargetVec = info_save.BestPoint;
        //Debug::ViewAngles = cmd->viewangles;
        std::wstring name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(info_save.enemy.entity_ptr->getPlayerName(true));
        if (config->debug.aimbotcoutdebug) {
            Debug::LogItem item3;
            item3.PrintToScreen = false;
            std::wstring name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(info_save.enemy.entity_ptr->getPlayerName(true));
            item3.text.push_back(std::wstring{ L"Selected " + name + L"As Target" });
            Debug::LOG_OUT.push_front(item3);
        }

        Entity* Target = info_save.enemy.entity_ptr;
        Resolver::Record* record = info_save.enemy.record;
        Vector BestPoint = info_save.BestPoint;

        if (!(record->invalid) && config->debug.animstatedebug.resolver.enabled) {
            record->wasTargeted = true;
            Resolver::TargetedEntity = Target;
        }
        //Vector Angle = Math::CalcAngle(localPlayer->getEyePosition(), BestPoint);
        static float MinimumVelocity = 0.0f;
        MinimumVelocity = localPlayer->getActiveWeapon()->getWeaponData()->maxSpeedAlt * 0.34f;
        const auto aimPunch = activeWeapon->requiresRecoilControl() ? /*localPlayer->getAimPunch()*/ (localPlayer->aimPunchAngle() * interfaces->cvar->findVar("weapon_recoil_scale")->getFloat()) : Vector{ };


        Vector ViewAng = cmd->viewangles;
        if (config->RageBot[weaponIndex].considerRecoilInHC) {
            ViewAng += aimPunch;
        }
        auto angle = VectorMath::calculateRelativeAngle(localPlayer->getEyePosition(), BestPoint, ViewAng);
        //angle -= (localPlayer->aimPunchAngle() * interfaces->cvar->findVar("weapon_recoil_scale")->getFloat());
        //angle /= config->RageBot[weaponIndex].smooth;

        bool clamped{ false };

        /*
        if (fabs(angle.x) > config->misc.maxAngleDelta || fabs(angle.y) > config->misc.maxAngleDelta) {
            angle.x = std::clamp(angle.x, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);
            angle.y = std::clamp(angle.y, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);
            clamped = true;
        }
        */
        if (localPlayer->velocity().length() >= MinimumVelocity && config->RageBot[weaponIndex].autoStop && (localPlayer->flags() & PlayerFlags::ONGROUND) && localPlayer->isScoped()) {
            Autostop(cmd); //Auto Stop
        }
        if ((activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime()) && activeWeapon->isSniperRifle() && !localPlayer->isScoped())
        {
            cmd->buttons |= UserCmd::IN_ATTACK2; //Auto Scope
        }
        //Angle -= (localPlayer->aimPunchAngle() * interfaces->cvar->findVar("weapon_recoil_scale")->getFloat());
        float hitchance = 0.0f;

        if (config->RageBot[weaponIndex].hitChance > 0) {
            if (config->RageBot[weaponIndex].ClipRay) {
                hitchance = hitChance(localPlayer.get(), Target, activeWeapon, angle, cmd, config->RageBot[weaponIndex].hitChance);
            }   
            else {
                hitchance = NewhitChance(localPlayer.get(), Target, activeWeapon, weaponIndex, angle, cmd, config->RageBot[weaponIndex].hitChance, info_save.ESI.bones, info_save.hitbox_save);
            }

        }
        else {
            hitchance = config->RageBot[weaponIndex].maxAimInaccuracy >= activeWeapon->getInaccuracy() * 100;
        }


        if (config->debug.aimbotcoutdebug && (hitchance > 0.f)) {
            Debug::LogItem item;
            item.PrintToScreen = false;
            item.text.push_back(L"Hitchance Completed With " + std::to_wstring(hitchance) + L" Likelihood of hitting on player " + name + L" Best Damage is " + std::to_wstring(info_save.bestDamage));
            Debug::LOG_OUT.push_front(item);
        }


        if ((((hitchance > config->RageBot[weaponIndex].hitChance) || config->RageBot[weaponIndex].ensureHC)) && (activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime()))
        {
            if (!config->RageBot[weaponIndex].considerRecoilInHC) {
                angle = VectorMath::calculateRelativeAngle(localPlayer->getEyePosition(), BestPoint, cmd->viewangles + aimPunch);
            }
            cmd->viewangles += angle; //Set Angles
            if (config->RageBot[weaponIndex].smooth > 1) {
                angle /= config->RageBot[weaponIndex].smooth;
            }
            if (!config->RageBot[weaponIndex].silent) {
                interfaces->engine->setViewAngles(cmd->viewangles);
            }


            if (config->RageBot[weaponIndex].autoShot) {
                cmd->buttons |= UserCmd::IN_ATTACK; //shoot
                record->totalshots++;
                if (!record->invalid) {
                    record->FiredUpon = true;
                    record->wasTargeted = true;
                    Resolver::lastShotAt = record;
                }
            }


            if (cmd->buttons & UserCmd::IN_ATTACK) {
                Resolver::lastShotAt = record;
                Resolver::shotdata shotdata;
                shotdata.simtime = Target->simulationTime();
                shotdata.EyePosition = localPlayer->getEyePosition();
                shotdata.viewangles = cmd->viewangles;
                shotdata.TargetedPosition = BestPoint;
                shotdata.hitbox = info_save.hitbox_save;
                shotdata.box = info_save.box;
                memcpy(shotdata.matrix, info_save.ESI.bones, sizeof(matrix3x4) * 256);
                record->shots.push_front(shotdata);
                if (config->debug.aimbotcoutdebug) {
                    Debug::LogItem item3;
                    item3.PrintToScreen = false;
                    std::wstring name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(info_save.enemy.entity_ptr->getPlayerName(true));
                    item3.text.push_back(std::wstring{ L"Taking Shot At " + name });
                    Debug::LOG_OUT.push_front(item3);
                }

                //if (info_save.enemy.bt && info_save.backup_b_rec && !Backtrack::valid(info_save.backup_b_rec->simulationTime)) {
                //    memcpy(shotdata.matrix, info_save.backup_b_rec->matrix, sizeof(matrix3x4) * std::clamp(info_save.backup_b_rec->countBones, 0, 256));
                //    record->shots.push_front(shotdata);
                //}
                //else if (Animations::data.player[Target->index()].hasBackup) {
                //    memcpy(shotdata.matrix, Animations::data.player[Target->index()].matrix, sizeof(matrix3x4) * std::clamp(Animations::data.player[Target->index()].countBones, 0, 256));

                //}
                //else {
                //    Target->setupBones(shotdata.matrix, 256, 0x7FF00, memory->globalVars->currenttime); /* TODO: Don't Call Setup Bones Here, back it up somewhere*/
                //    record->shots.push_front(shotdata);
                //}

            }
        }
        else {
            if (!enemies.empty()) {
                //enemies.front().record->wasTargeted = true;
                //Resolver::TargetedEntity = enemies.front().entity_ptr;
                record->wasTargeted = true;
            }
        }

        for (auto enemy : enemies) {
            if (enemy.bt == true) {
                Animations::finishSetup(enemy.entity_ptr);
            }
        }

        if (info_save.enemy.entity_ptr && !info_save.enemy.bt && config->debug.animstatedebug.resolver.enabled) {
            Animations::setupMatrix(info_save.enemy.entity_ptr, info_save.enemy.record->Original);
        }


    }
 else {
 if (info_save.enemy.entity_ptr && !info_save.enemy.bt && config->debug.animstatedebug.resolver.enabled) {
     Animations::setupMatrix(info_save.enemy.entity_ptr, info_save.enemy.record->Original);
 }

     if (!enemies.empty()) {
         enemies.front().record->wasTargeted = true;
     }
     if ((enemies.size() > 1) && (info_save.bestDamage < 5.0)) {
         if (enemies.at(1).distance < enemies.front().distance) {
             enemies.front().record->wasTargeted = true;
             enemies.at(1).record->wasTargeted = true;
             enemies.at(1).record->FiredUpon = false;
             enemies.front().record->FiredUpon = false;
             Resolver::TargetedEntity = enemies.at(1).entity_ptr;
         }
     }
     else if (enemies.size() == 1) {
         enemies.front().record->wasTargeted = true;
         enemies.front().record->FiredUpon = false;
         Resolver::TargetedEntity = enemies.front().entity_ptr;
     }

     for (auto enemy : enemies) {
         if (enemy.bt == true) {
             Animations::finishSetup(enemy.entity_ptr);
         }
     }

     return;
}




}


void RageBot::Run(UserCmd* cmd) noexcept {
    //int& bestDamage_save; int& bestHitchance; Vector& wallbangVector;
    // Does local player exists


   // if (localPlayer->nextAttack() > memory->globalVars->serverTime() ) {
   //     return;
   // }


    int bestDamage_save;
    int bestHitchance;
    Vector wallbangVector;

    //Get the active weapon
    const auto activeWeapon = localPlayer->getActiveWeapon();
    if (!activeWeapon || !activeWeapon->clip()) // does active weapon exist and have a clip in
        return;

    auto weaponIndex = getWeaponIndex(activeWeapon->itemDefinitionIndex2()); // get weapon index
    if (!weaponIndex) // no weapon index? then exit
        return;

    // get weapon class
    auto weaponClass = getWeaponClass(activeWeapon->itemDefinitionIndex2());
    if (!config->RageBot[weaponIndex].enabled) //if weaponIndex is not enabled
        weaponIndex = weaponClass; // weaponClass is now weaponIndex

    if (!config->RageBot[weaponIndex].enabled) // If weaponClass isnt set
        weaponIndex = 0; // weaponClass/Index is now 0 (ALL setting)


    // BEYOND HERE REALLY AINT NEEDED UNLESS YOU WANNA BE PERFECT ABOUT WHEN ITS ON OR NOT
    if (!config->RageBot[weaponIndex].betweenShots && (activeWeapon->nextPrimaryAttack() > memory->globalVars->serverTime())) {
        return;
    }


    /*
    if (activeWeapon->getInaccuracy() >= config->RageBot[weaponIndex].maxTriggerInaccuracy)
    {
        //oldstyle(cmd);
        //return;
    }
    */

    if (activeWeapon->isKnife() || activeWeapon->isGrenade()) {
        return;
    }

    if (!config->RageBot[weaponIndex].autoShot && !(cmd->buttons & UserCmd::IN_ATTACK) && !(config->RageBot[weaponIndex].aimlock)) {
        return;
    }


    struct EntityVal {
        Entity* entity_ptr;
        int entity = 0;
        float distance = 0;
        int health = 0;
        bool isVisible = false;
        bool wasTargeted = false;
        Resolver::Record* record = &Resolver::invalid_record;
        bool bt = false;
        bool onshot = false;
        float prevVelocity = 999.0f;
        bool lbyUpdated = false;
    };
    std::vector<EntityVal> enemies;

    //Debug::LogItem item;


    struct entity_sort {
        bool operator() (EntityVal A, EntityVal B) {

            if ((A.distance < 20.0f) && (B.distance > 20.0f)) {
                return true;
            }
            if ((A.distance > 20.0f) && (B.distance < 20.0f)) {
                return false;
            }
            if (A.isVisible && !B.isVisible) {
                return true;
            }
            else if (!A.isVisible && B.isVisible) {
                return false;
            }
            if (A.wasTargeted && !B.wasTargeted) {
                return true;
            }
            else if (!A.wasTargeted && B.wasTargeted) {
                return false;
            }

            else {
                return false;
            }
            if (A.entity < B.entity) { // WTF IS THIS
                return true;
            }

        }
    } ent_sort;


    static auto frameRate = 1.0f;
    frameRate = 1 / (0.9f * frameRate + 0.1f * memory->globalVars->absoluteFrameTime);


    bool visFound = false;
    for (EntityQuick EntQuick : entitylistculled->getEnemies()) {
        Entity* entity = EntQuick.entity;
        if (entity->gunGameImmunity()){
            Resolver::PlayerRecords.at(EntQuick.index).wasTargeted = false;
            continue;
        }
        EntityVal curr_ent;
        //curr_ent.distance = DistanceToEntity(entity);
        curr_ent.distance = worldToScreen(entity->getEyePosition());
        curr_ent.entity = entity->index();
        curr_ent.health = entity->health();
        curr_ent.entity_ptr = entity;
        Resolver::Record* record = &Resolver::PlayerRecords.at(EntQuick.index);
        if (record && !record->invalid) {
            curr_ent.wasTargeted = record->wasTargeted;
            curr_ent.record = record;
            curr_ent.prevVelocity = record->prevVelocity;
        }
        else {
            curr_ent.wasTargeted = false;
            curr_ent.record = &Resolver::invalid_record;
        }

        if (!visFound) {
            curr_ent.isVisible = entity->isVisible();
            visFound = true;
        }
        else {
            curr_ent.isVisible = false;
        }
        /*
        if (config->optimization.nearestonly.enabled && (frameRate < config->optimzation.nearestonly.frames)) {
            if (!(curr_ent.isVisible) && !(curr_ent.wasTargeted) curr_ent.distance < 2000) {
                continue;
            }
        }
        */

        enemies.push_back(curr_ent);
    } // set up entities

    if (enemies.empty()) {
        return;
    }


    if (enemies.size() > 1) {
        std::sort(enemies.begin(), enemies.end(), ent_sort);
    }

    Resolver::Record* record = &Resolver::invalid_record;

    if (!enemies.front().wasTargeted) {
        enemies.front().wasTargeted = true;
    }
    //else if (enemies.size() > 2){
    //    enemies.at(1).wasTargeted = true;
    //}

    // config->debug.TraceLimit limits the amount of entities in the vector to 3 if under 20fps, hopefully saving your frames
    if ((20 > frameRate) && config->debug.TraceLimit && (enemies.size() > 3)) {
        enemies.erase((enemies.begin() + 2), enemies.end());
    }

    //if (enemies.size() > 3 && enemies.front().wasTargeted && enemies.front().record->FiredUpon) {
    //    enemies.erase((enemies.begin() + 1), enemies.end());
    //}




    // math time
    float bestDamage = -1.0f;
    Vector firstWallToPen;
    std::vector<VectorAndDamage> AimPoints;
    Entity* Target = localPlayer.get();//interfaces->entityList->getEntity(localPlayer->index());
    Vector BestPoint;
    Backtrack::Record* backup_rec = nullptr;
    EntityVal Save; // Replace Entity* Target with this!
    int hitbox_save = -1;
    for (auto enemy : enemies) {


        record = enemy.record;
        // Get ALL the multipoints
        float multiplier = record->multiExpan;


        if (config->debug.movefix) {
            Animations::setupMoveFix(enemy.entity_ptr);
        }
        /*

            IIIIIIIIIIIIIIIIII
            I                I
            I       h  .3    I
            I                I
            IIIIIIIIIIIIIIIIII <- 2.0
                           1.9


        */
        auto brecord = &Backtrack::records[enemy.entity]; // Get Backtrack Record



        if (config->backtrack.enabled && !brecord->empty() && (brecord->size() > 0) && Backtrack::valid(brecord->front().simulationTime) && (enemy.wasTargeted /*|| Backtrack::ImportantTick(*brecord)*/)) { // Sanity // 

            bool ImportantTick = Backtrack::ImportantTick(*brecord);
            if ((!enemy.isVisible || ImportantTick /*|| (enemy.entity_ptr->velocity().length2D() > 170.0f)*/)) { // if Enemy isn't visible, or their velocity is  > 100, we go for bt
                for (int i = ((ImportantTick) ? 0 : 3); i < (brecord->size() - 1); i++) { // Ignore the last 2 records, i've noticed the likely hood of hitting those is essentially nonexistant
                    if (i >= brecord->size()) {
                        break;
                    }
                    Backtrack::Record* c_record = &(brecord->at(i));
                    if (!c_record || !Backtrack::valid(c_record->simulationTime)) {
                        continue;
                    }
                    else {
                        if ((localPlayer->canSeePoint(c_record->head)) || (c_record->onshot && config->RageBot[weaponIndex].onshot) || c_record->lbyUpdated) { // Visible backtrack, onshot, or lbyUpdated? Lets go for it!
                            if ((c_record->onshot && config->RageBot[weaponIndex].onshot) || (c_record->lbyUpdated && config->RageBot[weaponIndex].ShootForLBY)) { /* If it's an onshot or lbyUpdated Tick, we set override*/
                                Debug::LogItem item2;
                                item2.PrintToScreen = false;
                                enemy.onshot = c_record->onshot;
                                enemy.lbyUpdated = c_record->lbyUpdated;
                                if (enemy.lbyUpdated && config->RageBot[weaponIndex].ShootForLBY) {
                                    item2.text.push_back(std::wstring{ L"[RageBot] Going for BT LBY Update for entity " + std::to_wstring(enemy.entity_ptr->index()) });
                                }
                                else if (enemy.onshot && config->RageBot[weaponIndex].onshot) {
                                    item2.text.push_back(std::wstring{ L"[RageBot] Going for BT Onshot for entity " + std::to_wstring(enemy.entity_ptr->index()) });
                                }
                                if (config->debug.backtrackCount) { Debug::LOG_OUT.push_front(item2); }
                                Backtrack::SetOverride(enemy.entity_ptr, *c_record);

                            }
                            else {
                                if (ImportantTick && config->RageBot[weaponIndex].prioritizeEventBT) {
                                    continue;
                                }
                            }
                            enemy.bt = true;
                            c_record->btTargeted = true;
                            Animations::setup(enemy.entity_ptr, *c_record);
                            backup_rec = c_record;
                            Backtrack::SetOverride(enemy.entity_ptr, *c_record);
                            break;
                        }
                        else {
                            continue;
                        }

                    }

                }
            }
        } /* For Months I spent pissed off my shit would not shoot and always got insta tapped.... had this code AFTER I got the multipoints. Fuck my life up dawg */

        /* If the enemy hasnt moved, we move the points in from *2.0 to slightly below multidistance*/
        if (enemy.wasTargeted) {
            if (/*(enemy.entity_ptr->velocity().length2D() < 1.01) && (enemy.record->prevVelocity < 1.01) &&*/ config->RageBot[weaponIndex].dynamicpoints) {
                if (enemy.record->prevhealth == enemy.health) {
                    if (record->multiExpan == config->RageBot[weaponIndex].extrapointdist) {
                        enemy.record->multiExpan = config->RageBot[weaponIndex].extrapointdist - .1;
                    }
                    else {
                        multiplier = enemy.record->multiExpan - (config->RageBot[weaponIndex].extrapointdist * .1);
                        enemy.record->multiExpan = multiplier;
                    }

                    if ((multiplier > config->RageBot[weaponIndex].extrapointdist) || (multiplier < .1)) {
                        multiplier = config->RageBot[weaponIndex].extrapointdist;
                        enemy.record->multiExpan = multiplier;
                    }
                }
            }
            else if ((enemy.entity_ptr->velocity().length2D() > 1.01) || (enemy.record->prevVelocity > 1.01)) {

                //enemy.record->multiExpan = config->RageBot[weaponIndex].extrapointdist;
                //multiplier = config->RageBot[weaponIndex].extrapointdist;


            }
        }
        Vector bonePointsMulti[Multipoints::HITBOX_MAX][Multipoints::MULTIPOINTS_MAX];





        /* So We Aren't Running SetupBones 1 million times*/

        
        //if (enemy.bt || (!record->invalid && !record->ResolverMatrices.empty())) {
            if ((!enemy.bt && !Animations::data.player[enemy.entity_ptr->index()].hasBackup) || config->debug.forcesetupBones) {
                if (!Multipoints::retrieveAll(enemy.entity_ptr, config->RageBot[weaponIndex].multidistance, multiplier, bonePointsMulti, nullptr, false)) {
                    continue;
                }
            }
            else if (enemy.bt) {
                if (!Multipoints::retrieveAll(enemy.entity_ptr, config->RageBot[weaponIndex].multidistance, multiplier, bonePointsMulti, backup_rec->matrix, true)) {
                    continue;
                }
            }
            else if (Animations::data.player[enemy.entity_ptr->index()].hasBackup) {
                if (!Multipoints::retrieveAll(enemy.entity_ptr, config->RageBot[weaponIndex].multidistance, multiplier, bonePointsMulti, Animations::data.player[enemy.entity_ptr->index()].matrix, true)) {
                    continue;
                }
            }
            else {
                if (!Multipoints::retrieveAll(enemy.entity_ptr, config->RageBot[weaponIndex].multidistance, multiplier, bonePointsMulti, nullptr, false)) {
                    continue;
                }
            }
            if (config->debug.aimbotcoutdebug) {
                Debug::LogItem item;
                item.PrintToScreen = false;
                item.text.push_back(std::wstring{ L"Made it Past Multipoint set for entity " + std::to_wstring(enemy.entity_ptr->index()) });
                Debug::LOG_OUT.push_front(item);
            }
        //}
        //else {
            

           //if (!Multipoints::retrieveAll(enemy.entity_ptr, config->RageBot[weaponIndex].multidistance, multiplier, bonePointsMulti, record->ResolverMatrices.at(record->targeted).Matrix, true)) {
           //         continue;
           //}

       // }


        // So first off we want to calculate what side we are closest to, I.E. which side of the enemy player model is peaked at us

        /*to do this we first make sure our angle is between 0, 360*/
        /*
        float NewEyeYaw = enemy.entity_ptr->eyeAngles().y;
        if (!(NewEyeYaw > 180)) { // make sure we are in -180,180 range and not 0-360 range already
            NewEyeYaw += 180; // Easy as fuck
        }
        */
        // Since eye yaw is in relation to the world, we can calculate which side faces us pretty easy, ignoring z though
        // I believe the correct way would be to find the angle at which the enemy is the player, then compare that to the
        // view angles 0-360 of the enemy. And then have set defined ranges for certain values, but this works well enough
        /*
               Basically we find the average distance between 2 points, one on x, one on z, to the player. Then whichever had a greater average amount, is farther


               ^^^^^^ Above ended up not getting implemented, would certainly fix the issue of tilted hitboxes though ^^^^^^^^^^

        */

        /*

            To save on processesing time, and not autowalling to  152 points (9 points per box, 18 boxes) on a player we divy them up into sets of 3 points
            To decide which to use, we use the following idea:


                               minmaxmaxmax
                         *---------------------* (max, max)
                         |                     |
                         |                     |
            minminminmax |                     | maxmaxmaxmin
                         |                     |
                         |                     |
                         *---------------------*
                (min, min)     minminmaxmin


            defined in Multipoints.cpp as:

            locations[1] = Vector{min.x, (min.y + max.y) * .5f, ((min.z * max.z) * .5f) };  --> minminminmax
            locations[2] = Vector{(min.x + max.x) * .5f, max.y, ((min.z * max.z) * .5f) };  --> minmaxmaxmax
            locations[3] = Vector{max.x, (min.y + max.y), ((min.z * max.z) * .5f) };        --> maxmaxmaxmin
            locations[4] = Vector{(min.x + max.x) * .5f, min.y, ((min.z * max.z) * .5f) };  --> minminmaxmin

            (min,min) set
            locations[5] = Vector{ min.x, min.y, min.z, };
            locations[6] = Vector{ min.x, min.y, max.z, };
            (max,max) set
            locations[7] = Vector{ max.x, max.y, max.z, };
            locations[8] = Vector{ max.x, max.y, min.z, };
            (max,min) set
            locations[9] = Vector{ max.x, min.y, min.z, };
            locations[10] = Vector{ max.x, min.y, max.z, };
            (min,max) set
            locations[11] = Vector{ min.x, max.y, min.z, };
            locations[12] = Vector{ min.x, max.y, max.z, };

            with additional points declared as having an expansion of 2.0f as:


            locations[13] = Vector{ minexpan.x, minexpan.y,minexpan.z, };
            locations[14] = Vector{ minexpan.x, minexpan.y, maxexpan.z, };

            locations[15] = Vector{ maxexpan.x, maxexpan.y, maxexpan.z, };
            locations[16] = Vector{ maxexpan.x, maxexpan.y, minexpan.z, };

            locations[17] = Vector{ maxexpan.x, minexpan.y, minexpan.z, };
            locations[18] = Vector{ maxexpan.x, minexpan.y, maxexpan.z, };

            locations[19] = Vector{ minexpan.x, maxexpan.y, minexpan.z, };
            locations[20] = Vector{ minexpan.x, maxexpan.y, maxexpan.z, };

            Add +8 to original index to access

                                ------ TODO ------
            In the future I need to write a bypass for this if the enemy is visible.

            By checking what the current roatation is on the enemies axis, and the side nearest to player
            you'll be able to process multiple sets of multipoints if the enemy is at say an angle where i.e. (dist to max, max, and min, min line up. With min,max being nearest)

            Currently checking if max,max and min, min are within a certain range of each other

        */







        int closest = 0;
        if (!enemy.isVisible) {
            float mindist = 99999;
            for (int i = 1; i <= 4; i++) { // Find Point Set with lowest distance
                float distance = localPlayer->origin().distTo(bonePointsMulti[Multipoints::HITBOX_UPPER_CHEST][i]);
                if (distance < mindist) {
                    mindist = distance;
                    closest = i;
                }
            }
        }
        if ((closest == 0) && !enemy.isVisible) { // if nothing was ever found???? Continue obviously.
            continue;
        }




        //if (!enemy.onshot && config->RageBot[weaponIndex].onshot && (enemy.entity_ptr->getVelocity().length2D() < 5.0f) && (enemy.health < 60)) // if the enemy isnt shooting & onshot is on, and we dont believe its better to override, continue
        //    continue;

        bool baimTriggered = false; // set up baim 
        record = enemy.record;
        if (record && !record->invalid) {
            if (enemy.bt && config->RageBot[weaponIndex].baim && !enemy.onshot) { // if going for bt, just baim
                baimTriggered = true;
            }
            if (enemy.entity_ptr->velocity().length2D() < 10.0f) {
                if ((record->missedshotsthisinteraction > config->RageBot[weaponIndex].baimshots) && (!enemy.onshot)) {
                    baimTriggered = true;
                }
                else if ((enemy.health < config->RageBot[weaponIndex].minDamage) && (!enemy.onshot))
                    baimTriggered = true;
            }
        }


        std::vector<int> Hitboxes;
        std::vector<int> Points = { 0 }; // always check go for center of hitbox first, and the middle spot of the hitbox


        if (baimTriggered) {
            Hitboxes = {
                    Multipoints::HITBOX_PELVIS,
                    Multipoints::HITBOX_BODY,
                    Multipoints::HITBOX_UPPER_CHEST,
                    Multipoints::HITBOX_THORAX,
            };
        }
        else if (config->RageBot[weaponIndex].bone == 8) { // if we are in hitbox mode
            for (int i = 0; i < HITBOX_MAX; i++) { // setup those hitboxes
                if (config->RageBot[weaponIndex].hitboxes[i] == true) {
                    Hitboxes.push_back(i);
                }
            }
        }
        else if (enemy.isVisible) { // if we can see the bastard, why would we shoot at his fuckin' ankles? 

            Hitboxes = {
                Multipoints::HITBOX_HEAD,
                Multipoints::HITBOX_NECK,
                Multipoints::HITBOX_PELVIS,
                Multipoints::HITBOX_BODY,
                Multipoints::HITBOX_UPPER_CHEST,
                Multipoints::HITBOX_THORAX,

            };

        }
        else if (enemy.onshot) { // gimmie that head
            Hitboxes = {
                    Multipoints::HITBOX_HEAD,
                    Multipoints::HITBOX_NECK,

            };
        }
        else {
            Hitboxes = {
                        Multipoints::HITBOX_HEAD,
                        Multipoints::HITBOX_NECK,
                        Multipoints::HITBOX_PELVIS,
                        Multipoints::HITBOX_LEFT_THIGH,
                        Multipoints::HITBOX_RIGHT_THIGH,
                        Multipoints::HITBOX_LEFT_HAND,
                        Multipoints::HITBOX_RIGHT_HAND,
                        Multipoints::HITBOX_LEFT_FOREARM,
                        Multipoints::HITBOX_RIGHT_FOREARM
            };
        }

        if (!enemy.wasTargeted) { // if we werent targeting this guy, lets just do these hitboxes
            Hitboxes = {
                Multipoints::HITBOX_HEAD,
                Multipoints::HITBOX_PELVIS,

            };
        }


        if (config->RageBot[weaponIndex].pelvisAimOnLBYUpdate && enemy.lbyUpdated) {
            Hitboxes = {
                    Multipoints::HITBOX_PELVIS,

                    Multipoints::HITBOX_HEAD,
            };
        }


        if (config->RageBot[weaponIndex].PrioritizeHeadOnNoDesync && record->noDesync) {
            Hitboxes = {
                Multipoints::HITBOX_HEAD,
                Multipoints::HITBOX_NECK,

                Multipoints::HITBOX_PELVIS,

            };
        }

        if (config->RageBot[weaponIndex].OnshotOrDesyncless && (!enemy.onshot || !record->noDesync || !record->move)) {
            Hitboxes = {

                Multipoints::HITBOX_PELVIS,
            };
        }


        if (Hitboxes.empty()) {
            continue;
        }
        if (config->debug.aimbotcoutdebug) {

            Debug::LogItem item2;
            item2.PrintToScreen = false;
            item2.text.push_back(std::wstring{ L"Made it Past Hitbox set for entity " + std::to_wstring(enemy.entity_ptr->index()) });
            Debug::LOG_OUT.push_front(item2);
        }
        // Here's the fun shit, deciding which point sets to shoot at
        // --- TODO --- Take into account for hitboxes at an angle
        if (!enemy.isVisible && enemy.wasTargeted) { // Look, if we are starin this dude in the eyes. Who cares about scanning the corner of his ear, when we can just shoot him in between the eyes.
                                                   // Do what Dahmer couldn't, and lobotomize him
                                                   // Also fuck all this if he isnt our main target, we will just scan for base points, m'kay?
            switch (closest) {
            case 1:
                Points.push_back(5);
                Points.push_back(6);
                Points.push_back(11);
                Points.push_back(12);
                break;
            case 2:
                Points.push_back(12);
                Points.push_back(11);
                Points.push_back(7);
                Points.push_back(8);
                break;
            case 3:
                Points.push_back(7);
                Points.push_back(8);
                Points.push_back(9);
                Points.push_back(10);
                break;
            case 4:
                Points.push_back(9);
                Points.push_back(10);
                Points.push_back(5);
                Points.push_back(6);
                break;
            default:
                break;
            }
        }
        else {
            Points = { 0 };
        }
        //float minminmaxmaxdeviation = 
        if (config->debug.aimbotcoutdebug) {
            Debug::LogItem item3;
            item3.PrintToScreen = false;
            item3.text.push_back(std::wstring{ L"Made it Past Point set for entity " + std::to_wstring(enemy.entity_ptr->index()) });
            Debug::LOG_OUT.push_front(item3);
        }

        if (config->RageBot[weaponIndex].veloPointSort) {
            // This code here will sort these points based on the players velocity, and what portion of the body is expected to peak first
            Vector Velocity = enemy.entity_ptr->getVelocity();

            if ((Velocity.length2D() > 30.01f) && (Points.size() > 1)) {
                bool axis = ((abs(Velocity.y) - abs(Velocity.x)) > 0) ? 1 : 0; // True for Y Axis, False for X Axis

                int side = axis ? (Velocity.y > 1) : (Velocity.x > 1); // 1 for Pos, 0 for neg
                side = side ? 1 : -1; // Shit if 0 to -1


                /* Takes the Chest Hitbox*/
                /* If you pass a point that is less than 0 or greater than 21, you'll crash. So dont do that*/
                struct point_sort {
                private:
                    bool axis;
                    int side;
                    std::array<Vector, Multipoints::MULTIPOINTS_MAX>Box;
                public:
                    point_sort(bool ax, int si, std::array<Vector, Multipoints::MULTIPOINTS_MAX>Vec) {
                        this->axis = ax;
                        this->side = si;
                        std::array<Vector, Multipoints::MULTIPOINTS_MAX> Box = Vec;
                        this->Box = Box;
                    }
                    bool operator() (int A, int B) {
                        float a = side ? Box[A].y : Box[A].x; // Get the correct Axis
                        float b = side ? Box[B].y : Box[B].x;
                        if ((A == 0) && (B != 0)) // Sort middle of hitbox to always be first
                            return true;
                        if ((B == 0) && (A != 0))
                            return false;

                        if ((b * side) > (a * side)) // Get best point; 1000 > 50; but -50 > -1000
                            return true;
                        if ((b * side) < (a * side))
                            return false;

                        return (A > B);
                    }
                };
                std::array<Vector, Multipoints::MULTIPOINTS_MAX> PointArray;
                std::copy(std::begin(bonePointsMulti[Multipoints::HITBOX_UPPER_CHEST]), std::end(bonePointsMulti[Multipoints::HITBOX_UPPER_CHEST]), std::begin(PointArray));
                std::sort(Points.begin(), Points.end(), point_sort(axis, side, PointArray));
            }
            if (config->debug.aimbotcoutdebug) {
                Debug::LogItem item4;
                item4.PrintToScreen = false;
                item4.text.push_back(std::wstring{ L"Made it Past Velo Point Sort for entity " + std::to_wstring(enemy.entity_ptr->index()) });
                Debug::LOG_OUT.push_front(item4);
            }
        }

        bestDamage = -1.0f;
        bool killFound = false;
        bool visFound = false;
        bool breakLoop = false;
        int totalPointsScanned = 0;

        float minDamage = config->RageBot[weaponIndex].minDamage;
        if (enemy.isVisible) {
            minDamage = config->RageBot[weaponIndex].minDamageVis;
        }

        if (minDamage > enemy.health + 5) {
            minDamage = enemy.health + 5;
        }

        if (config->RageBot[weaponIndex].OnshotOrDesyncless && (!enemy.onshot || !record->noDesync || !record->move)) {
            minDamage = enemy.health + 10;
        }
        for (int l = 0; l <= 1; l++) { /* For Velo Point Sort*/
            std::vector<int>Points_Sp;
            if (Points.size() == 1) {
                Points_Sp = { Points.at(0) };
            }
            else {
                Points_Sp = { Points.at(0 + (l * 2)) , Points.at(1 + (l * 2)) }; // This is a shit solution, need to rewrite
            }

            for (int i = 0; i < Hitboxes.size(); i++) { /*  Iterate hitboxes */

                auto hitbox = Hitboxes[i];

                for (int j = 0; ((j < ((config->RageBot[weaponIndex].multiincrease == true) ? 2 : 1))); j++) { /* For Extended Multipoints */
                    for (auto point : Points_Sp) { /* Iterate Points*/
                        totalPointsScanned++;
                        if ((totalPointsScanned > config->RageBot[weaponIndex].pointstoScan) && (bestDamage < 1)) {
                            breakLoop = true;
                            break;
                        }

                        Vector vPoint = bonePointsMulti[hitbox][point + (((j == 1) || (point == 0)) ? 0 : 8)];
                        float damage = Autowall->Damage(vPoint, wallbangVector, 0.0f);


                        if (config->debug.aimbotcoutdebug) {
                            Debug::LogItem item;
                            item.PrintToScreen = false;
                            item.text.push_back(std::wstring{ L"Ran AutoWall on entity " + std::to_wstring(enemy.entity_ptr->index()) + L" Got damage: " + std::to_wstring(damage) + L" Best Damage: " + std::to_wstring(bestDamage) });
                            Debug::LOG_OUT.push_front(item);
                        }

                        if ((damage > minDamage)) {

                            if (config->RageBot[weaponIndex].ensureHC) {
                                const auto aimPunch = activeWeapon->requiresRecoilControl() ? localPlayer->getAimPunch() : Vector{ };
                                auto angle = VectorMath::calculateRelativeAngle(localPlayer->getEyePosition(), vPoint, cmd->viewangles + aimPunch);

                                if (fabs(angle.x) > config->misc.maxAngleDelta || fabs(angle.y) > config->misc.maxAngleDelta) {
                                    angle.x = std::clamp(angle.x, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);
                                    angle.y = std::clamp(angle.y, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);

                                }
                                if (!hitChance(localPlayer.get(), enemy.entity_ptr, activeWeapon, angle, cmd, config->RageBot[weaponIndex].hitChance) > config->RageBot[weaponIndex].hitChance) {
                                    continue;
                                }
                            }

                            if (damage > bestDamage) {
                                bestDamage = damage;
                                BestPoint = vPoint;
                                Target = enemy.entity_ptr;
                                hitbox_save = hitbox;
                                if ((config->RageBot[weaponIndex].bone == 0) || localPlayer->canSeePoint(vPoint)) {
                                    breakLoop = true;
                                    if (config->debug.aimbotcoutdebug) { Debug::LogItem item3; item3.PrintToScreen = false; item3.text.push_back(std::wstring{ L"[1384] Breaking Loop " + std::to_wstring(enemy.entity_ptr->index()) }); Debug::LOG_OUT.push_front(item3); }
                                }
                                break;
                            }



                        }
                        else if (damage > (enemy.health + 5)) {
                            const auto aimPunch = activeWeapon->requiresRecoilControl() ? localPlayer->getAimPunch() : Vector{ };
                            auto angle = VectorMath::calculateRelativeAngle(localPlayer->getEyePosition(), vPoint, cmd->viewangles + aimPunch);

                            if (fabs(angle.x) > config->misc.maxAngleDelta || fabs(angle.y) > config->misc.maxAngleDelta) {
                                angle.x = std::clamp(angle.x, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);
                                angle.y = std::clamp(angle.y, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);

                            }
                            if (hitChance(localPlayer.get(), enemy.entity_ptr, activeWeapon, angle, cmd, config->RageBot[weaponIndex].hitChance) > 80.f) {
                                if (damage > bestDamage) {
                                    bestDamage = damage;
                                    BestPoint = vPoint;
                                    Target = enemy.entity_ptr;
                                    hitbox_save = hitbox;
                                    if ((config->RageBot[weaponIndex].bone == 0) || localPlayer->canSeePoint(vPoint)) {
                                        if (config->debug.aimbotcoutdebug) { Debug::LogItem item3; item3.PrintToScreen = false; item3.text.push_back(std::wstring{ L"[1406] Breaking Loop " + std::to_wstring(enemy.entity_ptr->index()) }); Debug::LOG_OUT.push_front(item3); }
                                        breakLoop = true;
                                    }
                                    break;
                                }
                            }

                        }

                        if ((bestDamage > 0) && (hitbox != 0)) {
                            break;
                        }
                        if (breakLoop) { break; } /* Is this the one use-case of the goto? Guess I still shouldn't use it, huh? Maybe Use a lambda in the future?*/
                    }
                    if (breakLoop) { break; }
                }
                if ((i > 13) && (config->RageBot[weaponIndex].optimize)) {
                    if (config->debug.aimbotcoutdebug) { Debug::LogItem item3; item3.PrintToScreen = false; item3.text.push_back(std::wstring{ L"[1423] Breaking Loop " + std::to_wstring(enemy.entity_ptr->index()) }); Debug::LOG_OUT.push_front(item3); }
                    breakLoop = true;
                    break;
                }

                if (breakLoop) { break; }
                if ((hitbox != 0) &&
                    (hitbox != 1) &&
                    (hitbox != 2) &&
                    (hitbox != 3) &&
                    (hitbox != 4) &&
                    (hitbox != 5) &&
                    (hitbox != 12) &&
                    (hitbox != 13) &&
                    (hitbox != 16) &&
                    (hitbox != 18) && config->RageBot[weaponIndex].optimize) {
                    breakLoop = true;
                    break;
                }
            }
            if (breakLoop) { break; }
            if (Points.size() == 1) {
                if (config->debug.aimbotcoutdebug) { Debug::LogItem item3; item3.PrintToScreen = false; item3.text.push_back(std::wstring{ L"[1445] Breaking Loop " + std::to_wstring(enemy.entity_ptr->index()) }); Debug::LOG_OUT.push_front(item3); }
                break;
            }

        }

        Save = enemy;
        if (Target != localPlayer.get()) {
            break;
        }




    }

    for (int i = 0; i < Resolver::PlayerRecords.size(); i++) {
        Resolver::Record* record_b = &Resolver::PlayerRecords.at(i);
        if (record_b && !record_b->invalid) {
            record_b->wasTargeted = false;
            record_b->FiredUpon = false;
        }
    }

    if (Target != nullptr) {
        if (Target == localPlayer.get()) {
            if (!enemies.empty()) {
                enemies.front().record->wasTargeted = true;
            }
            if ((enemies.size() > 1) && (bestDamage < 5.0)) {
                if (enemies.at(1).distance < enemies.front().distance) {
                    enemies.front().record->wasTargeted = true;
                    enemies.at(1).record->wasTargeted = true;
                    enemies.at(1).record->FiredUpon = false;
                    enemies.front().record->FiredUpon = false;
                    Resolver::TargetedEntity = enemies.at(1).entity_ptr;
                }
            }
            else if (enemies.size() == 1) {
                enemies.front().record->wasTargeted = true;
                enemies.front().record->FiredUpon = false;
                Resolver::TargetedEntity = enemies.front().entity_ptr;
            }

            for (auto enemy : enemies) {
                if (enemy.bt == true) {
                    Animations::finishSetup(enemy.entity_ptr);
                }
            }

            return;
        }

        if (config->debug.aimbotcoutdebug) {
            Debug::LogItem item3;
            item3.PrintToScreen = false;
            std::wstring name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(Target->getPlayerName(true));
            item3.text.push_back(std::wstring{ L"Selected " + name + L"As Target" });
            Debug::LOG_OUT.push_front(item3);
        }

        if (!(record->invalid) && config->debug.animstatedebug.resolver.enabled) {
            record->wasTargeted = true;
            Resolver::TargetedEntity = Target;
        }
        //Vector Angle = Math::CalcAngle(localPlayer->getEyePosition(), BestPoint);
        static float MinimumVelocity = 0.0f;
        MinimumVelocity = localPlayer->getActiveWeapon()->getWeaponData()->maxSpeedAlt * 0.34f;
        const auto aimPunch = activeWeapon->requiresRecoilControl() ? /*localPlayer->getAimPunch()*/ (localPlayer->aimPunchAngle() * interfaces->cvar->findVar("weapon_recoil_scale")->getFloat()) : Vector{ };


        Vector ViewAng = cmd->viewangles;
        if (config->RageBot[weaponIndex].considerRecoilInHC) {
            ViewAng += aimPunch;
        }
        auto angle = VectorMath::calculateRelativeAngle(localPlayer->getEyePosition(), BestPoint, ViewAng);
        //angle -= (localPlayer->aimPunchAngle() * interfaces->cvar->findVar("weapon_recoil_scale")->getFloat());
        //angle /= config->RageBot[weaponIndex].smooth;

        bool clamped{ false };

        /*
        if (fabs(angle.x) > config->misc.maxAngleDelta || fabs(angle.y) > config->misc.maxAngleDelta) {
            angle.x = std::clamp(angle.x, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);
            angle.y = std::clamp(angle.y, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);
            clamped = true;
        }
        */
        if (localPlayer->velocity().length() >= MinimumVelocity && config->RageBot[weaponIndex].autoStop && (localPlayer->flags() & PlayerFlags::ONGROUND) && localPlayer->isScoped()) {
            Autostop(cmd); //Auto Stop
        }
        if ((activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime()) && activeWeapon->isSniperRifle() && !localPlayer->isScoped())
        {
            cmd->buttons |= UserCmd::IN_ATTACK2; //Auto Scope
        }
        //Angle -= (localPlayer->aimPunchAngle() * interfaces->cvar->findVar("weapon_recoil_scale")->getFloat());
        float hitchance = 0.0f;

        if (config->RageBot[weaponIndex].hitChance != 0) {
            hitchance = hitChance(localPlayer.get(), Target, activeWeapon, angle, cmd, config->RageBot[weaponIndex].hitChance);
        }
        else {
            hitchance = config->RageBot[weaponIndex].maxAimInaccuracy >= activeWeapon->getInaccuracy() * 100;
        }

        if ((((hitchance > 0.0f) || config->RageBot[weaponIndex].ensureHC)) && (activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime()))
        {
            if (!config->RageBot[weaponIndex].considerRecoilInHC) {
                auto angle = VectorMath::calculateRelativeAngle(localPlayer->getEyePosition(), BestPoint, cmd->viewangles + aimPunch);
            }
            cmd->viewangles += angle; //Set Angles
            if (config->RageBot[weaponIndex].smooth > 1) {
                angle /= config->RageBot[weaponIndex].smooth;
            }
            if (!config->RageBot[weaponIndex].silent) {
                interfaces->engine->setViewAngles(cmd->viewangles);
            }


            if (config->RageBot[weaponIndex].autoShot) {
                cmd->buttons |= UserCmd::IN_ATTACK; //shoot
                record->totalshots++;
                if (!record->invalid) {
                    record->FiredUpon = true;
                    record->wasTargeted = true;
                }
            }


            if (cmd->buttons & UserCmd::IN_ATTACK) {
                Resolver::shotdata shotdata;
                shotdata.simtime = Target->simulationTime();
                shotdata.EyePosition = localPlayer->getEyePosition();
                shotdata.viewangles = cmd->viewangles;
                shotdata.TargetedPosition = BestPoint;
                shotdata.hitbox = hitbox_save;

                if (Save.bt && backup_rec && !Backtrack::valid(backup_rec->simulationTime)) {
                    memcpy(shotdata.matrix, backup_rec->matrix, sizeof(matrix3x4) * std::clamp(backup_rec->countBones, 0, 256));
                    record->shots.push_front(shotdata);
                }
                else if (Animations::data.player[Target->index()].hasBackup) {
                    memcpy(shotdata.matrix, Animations::data.player[Target->index()].matrix, sizeof(matrix3x4) * std::clamp(Animations::data.player[Target->index()].countBones, 0, 256));
                    record->shots.push_front(shotdata);
                }
                else {
                    Target->setupBones(shotdata.matrix, 256, 0x7FF00, memory->globalVars->currenttime); /* TODO: Don't Call Setup Bones Here, back it up somewhere*/
                    record->shots.push_front(shotdata);
                }

            }
        }
        else {
            if (!enemies.empty()) {
                //enemies.front().record->wasTargeted = true;
                //Resolver::TargetedEntity = enemies.front().entity_ptr;
                record->wasTargeted = true;
            }
        }

        for (auto enemy : enemies) {
            if (enemy.bt == true) {
                Animations::finishSetup(enemy.entity_ptr);
            }
        }


        //if ((Target->velocity().length2D() > 170) && (!config->backtrack.enabled) && config->debug.movefix)
        //   Animations::finishSetup(Target);

    }



}