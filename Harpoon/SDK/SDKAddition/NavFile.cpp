#include "NavFile.h"


#include "../../Hacks/OTHER/Debug.h"
#include "../../Pendelum/D3D9Render.h"


#include "Utils/ScreenMath.h"
#include "../../Interfaces.h"
#include "../../Hooks.h"
const std::string NAV_FILE_PATH = "C:/Program Files (x86)/Steam/steamapps/common/Counter-Strike Global Offensive/csgo/maps/";





void DrawLine(Vector A, Vector B, RendererStructs::rCOLOR color) {
	Debug::fl_coords coords1, coords2;

	if (!ScreenMath::worldToScreen(A, coords1.x, coords1.y))
		return;

	if (!ScreenMath::worldToScreen(B, coords2.x, coords2.y))
		return;
	renderer->drawMultiLine({ {coords1.x, coords1.y}, {coords2.x, coords2.y} }, color);
	//interfaces->surface->drawLine(coords1.x, coords1.y, coords2.x, coords2.y);

}




namespace MapNavPaths {
	inline NavMesh::NavPathPositions de_dust2{
		1402,
		1572,
		7186,
		1803,
		{ 1572, 8075, 1287, 8985, 95, 1402, 4140, 4999, 4167, 1441, 7187 },
		"de_dust2"
	};

	inline NavMesh::NavPathPositions de_mirage{
		296,
		7,
		1,
		3,
		{73, 105, 7, 862, 59, 222, 797, 77, 1, 645, 82, 784, 98, 33, 135},
		"de_mirage"
	};

	inline NavMesh::NavPathPositions de_inferno{
		903,
		5,
		52,
		68,
		{20, 2633, 2626, 15, 71, 82, 208, 1668, 541, 89, 35},
		"de_inferno"
	};

	inline NavMesh::NavPathPositions de_overpass{
		7145,
		9106,
		272,
		12220,
		{12220, 7145, 9106, 272},
		"de_overpass"
	};

	inline NavMesh::NavPathPositions de_train{
		4122,
		1201,
		3973,
		3132,
		{ 4122, 3973, 1201, 3132},
		"de_train"
	};


	inline NavMesh::NavPathPositions de_nuke{
		213,
		119,
		3152,
		2913,
		{ 213, 119, 3152, 2913},
		"de_nuke"
	};

	inline NavMesh::NavPathPositions de_vertigo{
		4891,
		5247,
		4360,
		3027,
		{ 4891, 5247, 4360, 3027},
		"de_vertigo"
	};

	inline NavMesh::NavPathPositions de_ancient{
		2,
		180,
		1,
		14,
		{ 2, 14, 1, 180},
		"de_ancient"
	};



	inline NavMesh::NavPathPositions de_engage{
		526,
		1622,
		2,
		38,
		{ 526, 2, 38, 1622},
		"de_engage"
	};

	inline NavMesh::NavPathPositions de_cache{
		212,
		232,
		1,
		12,
		{ 212, 232, 1, 12},
	    "de_cache"
	};


	/*
		struct NavPathPositions {
		int A;
		int B;
		int T_Spawn;
		int CT_Spawn;
		std::vector<int> mainPath;
		std::string mapname;
	};
	
	
	*/

	inline std::unordered_map<std::string, NavMesh::NavPathPositions> MapPaths = {
		{"de_dust2" , de_dust2},
		{"de_mirage", de_mirage},
		{"de_inferno", de_inferno},
		{"de_overpass", de_overpass},
		{"de_train", de_train},
		{"de_cache", de_cache},
		{"de_ancient", de_ancient},
		{"de_vertigo", de_vertigo},
		{"de_engage", de_engage},
		{"de_nuke", de_nuke}
	};
	inline NavMesh::NavPathPositions Invalid_Path{ 0,0,0,0,{0}, "de_invalid" };
}



namespace NavMesh {
	bool LoadNav(MapInfo& Map, const char* caller = "NO_CALLER_SPECIFIED"){

		char buffer[1024];
		snprintf(buffer, 1024, "%s Initalizing Map: %s", caller, Map.MapName.c_str());
		Debug::QuickPrint(buffer);
		/* Yes I know a try{}catch(){} is not good to use here but idc at this moment in time. The default library already used std::exception. I kept with the status quo*/
		try {
			std::string NAVFILE{ NAV_FILE_PATH + Map.MapName + ".nav" };

			char buffer[1024];
			snprintf(buffer, 1024, "%s Loading Map Nav From Location: %s", caller, NAVFILE.c_str());
			Debug::QuickPrint(buffer);

			Map.NavFile.load(NAVFILE);
			if (Map.NavFile.getNavAreas()->empty()) {
				throw std::exception("Nav Areas Empty after load");
			}

			//Debug::QuickPrint("%s Loaded Map Successfully", caller);

		}
		catch (std::exception& e) {

			Debug::QuickPrint({ 255,0,0,255 }, e.what());
			Map.Failed = true;
			return false;
		}

		Map.Initialized = true;
		return true;
	}

	bool LoadCurrentMapNav(MapInfo& Map, const char* caller = "(LoadCurrentMapNav)NO_CALLER_SPECIFIED") {

		std::string lvlName = std::string(memory->clientState->levelNameShort);
		//Debug::QuickPrint(lvlName.c_str());
		if (lvlName.empty()) {
			lvlName = std::string(memory->clientState->levelName);
			//Debug::QuickPrint(lvlName.c_str());
			if (lvlName.empty()) {
				Debug::QuickPrint({ 255,0,0,255 }, "NavMesh::LoadCurrentMapNav Unable to Retrieve Map Name from Client State");
				Map.Failed = true;
				return true;
			}
		}

		if (!Map.Initialized || Map.MapName.compare(lvlName) || Map.Failed) {

			if (Map.Initialized) {
				if (!Map.MapName.compare(lvlName)) {
					Debug::QuickPrint("NavMesh::LoadCurrentMapNav Map Names Do No Match");
				}
			}
			else {
				Debug::QuickPrint("NavMesh::LoadCurrentMapNav Initial Load Of Map NAV");
			}

			Map.MapName = lvlName;

			if (!LoadNav(Map, caller)) {
				Map.Failed = true;
				return true;
			}
		}
		return false;
	}

	bool SameMap(MapInfo& Map) {
		std::string lvlName = std::string(memory->clientState->levelNameShort);
		//Debug::QuickPrint(lvlName.c_str());
		if (lvlName.empty()) {
			lvlName = std::string(memory->clientState->levelName);
			if (lvlName.empty()) {
				Map.Failed = true;
				return false;
			}
		}
		return (0 != Map.MapName.compare(lvlName));
	}

	NavPathPositions GetCurrentMapPathPositions(MapInfo& Map) {
		return MapNavPaths::MapPaths[Map.MapName];
	}

	void DrawNav(nav_mesh::nav_area area, float* in_color) {
		nav_mesh::Bounds corners;

		corners = area.GetBounds();

		RendererStructs::rCOLOR color;
		color.r = static_cast<int>(in_color[0] * 255);
		color.g = static_cast<int>(in_color[1] * 255);
		color.b = static_cast<int>(in_color[2] * 255);
		color.a = static_cast<int>(in_color[3] * 255);

		DrawLine(corners.ne, corners.se, color);
		DrawLine(corners.se, corners.sw, color);
		DrawLine(corners.sw, corners.nw, color);
		DrawLine(corners.nw, corners.ne, color);

		if (config->visuals.DrawNavSettings.DrawCenterLines) {
			DrawLine(corners.se, corners.nw, color);
		}

		if (config->visuals.DrawNavSettings.DrawNavsRect) {
			Debug::fl_coords coords1, coords2, coords3, coords4;

			RendererStructs::coordinate coords[4];

			if (!ScreenMath::worldToScreen(corners.se, coords[0].x, coords[0].y))
				return;
			if (!ScreenMath::worldToScreen(corners.ne, coords[1].x, coords[1].y))
				return;

			if (!ScreenMath::worldToScreen(corners.sw, coords[2].x, coords[2].y))
				return;
			if (!ScreenMath::worldToScreen(corners.nw, coords[3].x, coords[3].y))
				return;


			renderer->drawFilledRectangle2D(coords, color);
			//renderer->drawFilledTriangle(&coords[0], color);
			//renderer->drawFilledTriangle(&coords[1], color);
		}
	}


	void DrawNavs(std::vector<nav_mesh::nav_area> areas, float* in_color = NULL) {
		for (nav_mesh::nav_area area : areas) {
			DrawNav(area, in_color);
		}
	}

}