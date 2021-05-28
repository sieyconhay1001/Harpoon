#pragma once
#include <vector>
#include "../../Hacks/Backtrack.h"
class Entity;

struct EntityQuick {
	Entity* entity = nullptr;
	int index = 0;
	bool m_bisEnemy = false;
	bool m_bisAlive = false;
	bool m_bisDormant = true;
	bool m_bisLocalPlayer = false;
	bool m_bisVisible = false;
	bool m_bDidForcePeak = false;
	float m_flDistanceFromCrosshair = 0.0f;
	std::deque<Backtrack::Record>* backtrackRecords = nullptr;
	bool bhasBacktrack = false;
};



/* A Compressed List to quickly Go through entities, and decrease the amount of needed Sanity Checks*/
class EntityListCull {
public:
	EntityListCull();
	~EntityListCull();
	/* If entity Does not exist, we will Not include that. Simple*/
	void cullEntities();
	/* Only Culls Entities already in ents vector */
	void quickcullEntities();
	/* Sort Entities so Dormant, Dead, and Teammates come last*/
	void sortEntities();
	/* Return All Valid Players*/
	std::vector<EntityQuick> getEntities() { return ents; }
	/* Return Alive, Not Dormant, Enemies */
	std::vector<EntityQuick> getEnemies() { return enemies; }
	/* Self Explanatory */
	Entity* getEntNearestToCrosshair();
	Entity* getEntNearestToCrosshairPrioritizeVis();
	float getDistToNearestPlayer();
private:
	std::vector<EntityQuick> ents = std::vector<EntityQuick>{};
	std::vector<EntityQuick> enemies = std::vector<EntityQuick>{};
	int NumberOfPlayers;
	float NearestPlayerDist = -1.f;
};

inline std::unique_ptr<EntityListCull> entitylistculled;