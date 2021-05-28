#include "EntityListCull.hpp"
#include "../OsirisSDK/Entity.h"
#include "../OsirisSDK/Engine.h"
#include "../OsirisSDK/LocalPlayer.h"
#include "../../Interfaces.h"
#include "../../Config.h"
#include "../../SDK/SDKAddition/Utils/ScreenMath.h"

//EntityListCull entitylistculled;

/* For now this is just a clone of cullEntities that gets called on creation. I made add other things in the future here*/

//#include "../../Hacks/OTHER/Debug.h"


EntityListCull::EntityListCull() {    
    cullEntities();
}

EntityListCull::~EntityListCull() {
    if (!ents.empty())
        ents.clear();
}


void EntityListCull::cullEntities() {
    if (!interfaces->engine->isInGame()) {
        ents.clear();
        enemies.clear();
        return;
    }


    NearestPlayerDist = -1.f;

    //if (!localPlayer)
    //   return;

    if(!ents.empty())
        ents.clear();

    if (!enemies.empty())
        enemies.clear();

    if (!config)
        return;


    if (!interfaces->entityList)
        return;


    //Debug::QuickPrint(("interfaces->entityList = " + std::to_string((std::uintptr_t)interfaces->entityList)).c_str());


    for (int i = 0; i <= interfaces->engine->getMaxClients(); i++) {
        const auto entity = interfaces->entityList->getEntity(i);
        if (!entity || !entity->isPlayer()) // Removed isPlayer() cause i realized getMaxClient makes that redundant
            continue;

        EntityQuick entq;

        if (localPlayer.get() && (entity == localPlayer.get()))
            entq.m_bisLocalPlayer = true;

        if (localPlayer.get() && localPlayer->isAlive()) {
            float dist = entity->origin().distTo(localPlayer->origin());
            if (dist < NearestPlayerDist)
                NearestPlayerDist = dist;
        }


        entq.entity = entity;
        entq.index = entity->index();
        if (localPlayer.get()) {
            entq.m_bisEnemy = entity->isOtherEnemy(localPlayer.get());
        }
        else {
            entq.m_bisEnemy = true;
        }
        entq.m_bisDormant = entity->isDormant();
        entq.m_bisAlive = entity->isAlive();


        if (!entq.m_bisLocalPlayer && !entq.m_bisDormant && entq.m_bisAlive && entq.m_bisEnemy) {
            entq.m_bisVisible = entity->isVisible();
            entq.m_flDistanceFromCrosshair = ScreenMath::worldToScreenDistanceFromCenter(entity->getEyePosition());
            enemies.push_back(entq);
        }
        if (config->backtrack.enabled) {
            std::deque<Backtrack::Record>* records = &(Backtrack::records[entq.index]);
            entq.bhasBacktrack = true;
        }
        ents.push_back(entq);
    }
}

void EntityListCull::quickcullEntities() {
    enemies.clear();

    if (ents.empty())
        return;

    if (!interfaces->engine->isInGame()) {
        ents.clear();
        return;
    }



    for (int i = 0; i < ents.size(); i++) {
            EntityQuick* entq = &(ents.at(i));
            entq->entity = interfaces->entityList->getEntity(entq->index);
            if (!entq->entity) {
                std::swap(ents.at(i), ents.back());
                ents.pop_back();
                continue;
            }
            if (!entq->entity->isPlayer())
                return;

            
            if (localPlayer.get() && (entq->entity == localPlayer.get()))
                entq->m_bisLocalPlayer = true;
            entq->index = entq->entity->index();
            if (localPlayer.get()) {
                entq->m_bisEnemy = entq->entity->isOtherEnemy(localPlayer.get());
            }
            else {
                entq->m_bisEnemy = true;
            }
            entq->m_bisDormant = entq->entity->isDormant();
            entq->m_bisAlive = entq->entity->isAlive();

            if (config->backtrack.enabled) {
                std::deque<Backtrack::Record>* records = &(Backtrack::records[entq->index]);
                entq->bhasBacktrack = true;
            }

            if (!entq->m_bisLocalPlayer && !entq->m_bisDormant && entq->m_bisAlive && entq->m_bisEnemy)
                enemies.push_back(*entq);


        }
}


float EntityListCull::getDistToNearestPlayer() {
    return NearestPlayerDist;
}


void EntityListCull::sortEntities() {
    struct {
        bool operator()(EntityQuick entA, EntityQuick entB) const
        {
            if (!entA.m_bisLocalPlayer && entB.m_bisLocalPlayer)
                return true;
            if (!entB.m_bisLocalPlayer && entA.m_bisLocalPlayer)
                return false;
            
            if (!entA.m_bisDormant && entB.m_bisDormant)
                return true;
            if (!entB.m_bisDormant && entA.m_bisDormant)
                return false;

            if (entA.m_bisAlive && !entB.m_bisAlive)
                return true;
            if (entB.m_bisAlive && !entA.m_bisAlive)
                return false;

            if (entA.m_bisEnemy && !entB.m_bisEnemy)
                return true;
            if (!entB.m_bisEnemy && !entA.m_bisEnemy)
                return false;

            if (entB.index > entA.index)
                return false;
            if (entA.index > entB.index)
                return true;
        }
    } sortEnts;
    std::sort(ents.begin(), ents.end(), sortEnts);
}

Entity* EntityListCull::getEntNearestToCrosshair() {
    struct entity_sort {
        bool operator() (EntityQuick A, EntityQuick B) {
            float A_Dist = A.m_flDistanceFromCrosshair;
            float B_Dist = B.m_flDistanceFromCrosshair;
            if (A_Dist < B_Dist) {
                return true;
            }
            if (A_Dist > B_Dist) {
                return false;
            }

            if (A.index < B.index) { // On the off chance that they are equal, do this so we dont crash do to weak sorting or whatever the bullshit is called 
                return true;
            }
            return false;
        }
    } ent_sort;
    std::vector<EntityQuick> entQuicks = enemies;

    if (entQuicks.empty())
        return nullptr;
    else if (entQuicks.size() > 1) {
        std::sort(entQuicks.begin(), entQuicks.end(), ent_sort);
    }
    return entQuicks.front().entity;
}

Entity* EntityListCull::getEntNearestToCrosshairPrioritizeVis() {
    struct entity_sort {
        bool operator() (EntityQuick A, EntityQuick B) {

            if (A.m_bisVisible && !B.m_bisVisible) {
                return true;
            }

            if (!A.m_bisVisible && B.m_bisVisible) {
                return false;
            }

            if (A.m_flDistanceFromCrosshair < B.m_flDistanceFromCrosshair) {
                return true;
            }

            if (A.m_flDistanceFromCrosshair > B.m_flDistanceFromCrosshair) {
                return false;
            }

            if (A.index < B.index) { // On the off chance that they are equal, do this so we dont crash do to weak sorting or whatever the bullshit is called 
                return true;
            }
            return false;
        }
    } ent_sort;
    //std::vector<EntityQuick> entQuicks = enemies;

    if (enemies.empty()) {
        return localPlayer.get();
    }
    else if (enemies.size() > 1) {
        std::sort(enemies.begin(), enemies.end(), ent_sort);
    }
    return enemies.front().entity;
}