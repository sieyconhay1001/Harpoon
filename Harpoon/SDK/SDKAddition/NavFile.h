#pragma once
#include <string>
#include <vector>
#include <map>
#include <map>
#include <unordered_set>



#include "../../Resource/MicroPather/micropather.h"
#include "../../Resource/MicroPather/nav_file.h"
#include "../../Resource/MicroPather/nav_area.h"
#include "../../Resource/MicroPather/nav_structs.h"



#include "../OsirisSDK/Entity.h"

namespace NavMesh {
	struct NavPathPositions {
		~NavPathPositions() { return; }
		int A;
		int B;
		int T_Spawn;
		int CT_Spawn;
		std::vector<int> mainPath;
		std::string mapname;
	};


	struct MapInfo {
		nav_mesh::nav_file NavFile;
		std::string MapName = "";
		std::string NavFilePath = "";
		std::map<int, std::vector<nav_mesh::nav_area>> navset_backup;
		std::map<int, std::vector<nav_mesh::nav_area>> connections_backup;
		std::map<int, std::unordered_set<int>> connections_backup_int;

		int PreviousDepth = 0;
		bool Initialized = false;
		bool Failed = false;
	};

	struct PlayerNavInfo {
		Entity* entity = nullptr;
		std::vector<nav_mesh::nav_area> NearNavs;
		std::vector<int> NavIDs;
		int NavID = -1;
	};



	bool LoadNav(MapInfo& map, const char* caller);
	bool LoadCurrentMapNav(MapInfo& map, const char* caller);
	bool SameMap(MapInfo& map);
	NavPathPositions GetCurrentMapPathPositions(MapInfo& Map);

	/* Debug */
	void DrawNav(nav_mesh::nav_area area, float* in_color = &(config->visuals.DrawNavs.color[0]));
	void DrawNavs(std::vector<nav_mesh::nav_area> areas, float* in_color);


};



