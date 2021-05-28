#include "Renderables.h"
#include "OTHER/Debug.h"



#include "../SDK/SDKAddition/Utils/ScreenMath.h"
#include "../SDK/SDKAddition/EntityListCull.hpp"
/* Actually rewrote the entire SDK at this point. */
#include "../SDK/OsirisSDK/Entity.h"
#include "../SDK/OsirisSDK/Vector.h"
#include "../SDK/OsirisSDK/NetworkChannel.h"
#include "../Interfaces.h"

#include <vector>
#include <unordered_set>
#include <algorithm>
/* TODO: Make this work dynamically. but idgaf rn*/
const std::string NAV_FILE_PATH = "C:/Program Files (x86)/Steam/steamapps/common/Counter-Strike Global Offensive/csgo/maps/";



/* reinterpret_cast<std::string*>(reinterpret_cast<uintptr_t>(memory->clientState) + 0x288); */
NavMesh::MapInfo Renderables::NavDraw::Map;

std::vector<NavMesh::PlayerNavInfo> PlayerNavs(65);
#include "../Hooks.h"
#include "../Pendelum/D3D9Render.h"


#include "Walkbot/Walkbot.h"

#include "../COMPILERDEFINITIONS.h"

namespace Renderables {

	namespace NavDraw {

		std::vector<nav_mesh::nav_area> GetConnections(nav_mesh::nav_area area) {
			std::vector<nav_mesh::nav_connect_t> connections = area.get_connections();
			std::vector<nav_mesh::nav_area> tempAreas;
			for (nav_mesh::nav_connect_t connection : connections) {
				tempAreas.push_back(Map.NavFile.get_area_by_id(connection.id));
			}
			return tempAreas;
		}

		std::vector<nav_mesh::nav_area> GetConnections(int NavID) {
			std::map<int, std::vector<nav_mesh::nav_area>>::iterator iterator = Map.connections_backup.find(NavID);
			if (iterator != Map.connections_backup.end()) {
				return Map.connections_backup.at(NavID);
			}
			std::vector<nav_mesh::nav_area> Areas = GetConnections(Map.NavFile.get_area_by_id(NavID));
			Map.connections_backup.insert(std::pair<int, std::vector<nav_mesh::nav_area>>(NavID, Areas));
			return Areas;
		}


		std::unordered_set<int> GetConnectionsID(nav_mesh::nav_area area) {



			std::map<int, std::unordered_set<int>>::iterator iterator = Map.connections_backup_int.find(area.get_id());
			if (iterator != Map.connections_backup_int.end()) {
				return Map.connections_backup_int.at(area.get_id());
			}
			std::vector<nav_mesh::nav_connect_t> connections = area.get_connections();
			std::unordered_set<int> ids;
			for (nav_mesh::nav_connect_t connection : connections) {
				ids.insert(connection.id);
			}

			Map.connections_backup_int.insert(std::pair<int, std::unordered_set<int>>(area.get_id(), ids));
			return ids;
		}

		std::unordered_set<int> GetConnectionsID(int NavID) {
			return GetConnectionsID(Map.NavFile.get_area_by_id(NavID));
		}

		bool InitLoad() {

			char buffer[1024];
			snprintf(buffer, 1024, "Renderables::NavDraw::InitLoad Initalizing Map: %s", Map.MapName.c_str());
			Debug::QuickPrint(buffer);
			/* Yes I know a try{}catch(){} is not good to use here but idc at this moment in time. The default library already used std::exception. I kept with the status quo*/
			try {
				std::string NAVFILE{ NAV_FILE_PATH + Map.MapName + ".nav" };

				char buffer[1024];
				snprintf(buffer, 1024, "Renderables::NavDraw::InitLoad Loading Map Nav From Location: %s", NAVFILE.c_str());
				Debug::QuickPrint(buffer);

				Map.NavFile.load(NAVFILE);
				if (Map.NavFile.getNavAreas()->empty()) {
					throw std::exception("Renderables::NavDraw::InitLoad Nav Areas Empty after load");
				}

				Debug::QuickPrint("Renderables::NavDraw::InitLoad Loaded Map Successfully");
				
			}
			catch (std::exception& e) {
				Debug::QuickPrint({ 255,0,0,255 }, e.what());
				Map.Failed = true;
				return false;
			}

			if (config->visuals.DrawNavSettings.CalcAllConnections) {
				Debug::QuickPrint("Renderables::NavDraw::InitLoad Precalculating all connections for Nav Mesh");
				for (nav_mesh::nav_area area : *Map.NavFile.getNavAreas()) {
					GetConnections(area.get_id());
					GetConnectionsID(area);
				}
			}


			Map.Initialized = true;
			return true;
		}

		bool AppendConnections(std::unordered_set<int>& navSet, int& Depth, int MaxDepth) {
			Depth++;
			if (Depth >= MaxDepth) {
				return false;
			}
			std::vector< std::unordered_set<int>> sets;
			for (int NavID : navSet) {
				std::unordered_set<int> set = GetConnectionsID(NavID);
				if (!AppendConnections(set, Depth, MaxDepth)) {
					Depth--;
					sets.push_back(set);
				}
			}
			for (std::unordered_set<int> set : sets) {
				navSet.insert(set.begin(), set.end());
			}
			return true;
		}
		/*
		std::vector<nav_mesh::nav_area> GetNestedConnections(nav_mesh::nav_area area, int MaxDepth) {
			 std::unordered_set<int> areas = GetConnectionsID(area);
			 int DepthCounter = 0;
			 AppendConnections(areas, DepthCounter, MaxDepth);

			 return areas;

		}
		*/
		std::vector<nav_mesh::nav_area> GetNestedConnections(int NavID, int MaxDepth) {
			std::unordered_set<int> areas = GetConnectionsID(NavID);
			int DepthCounter = 0;
			AppendConnections(areas, DepthCounter, MaxDepth);
			std::vector<nav_mesh::nav_area> temp;

			for (int area : areas) {
				temp.push_back(Map.NavFile.get_area_by_id(area));
			}

			return temp;
		}

		bool SetPlayerNavs(NavMesh::PlayerNavInfo& PlayerNav) {
			int NavID;
			try {
				NavID = Map.NavFile.getAreaID(PlayerNav.entity->origin());
			}
			catch(std::exception &e){
				return true;
			}
			if (NavID == PlayerNav.NavID) {
				return true;
			}
			PlayerNav.NearNavs.clear();

			if (config->visuals.DrawNavSettings.DrawNavDepth == 0) {
				PlayerNav.NearNavs.push_back(Map.NavFile.get_area_by_id(NavID));
				return false;
			}

			std::map<int, std::vector<nav_mesh::nav_area>>::iterator iterator = Map.navset_backup.find(NavID);
			if (iterator != Map.navset_backup.end()) {
				PlayerNav.NearNavs = Map.navset_backup.at(NavID);
				return false;
			}

			PlayerNav.NearNavs = GetNestedConnections(NavID, (config->visuals.DrawNavSettings.DrawNavDepth));
			Map.navset_backup.insert(std::pair<int, std::vector<nav_mesh::nav_area>>(NavID, PlayerNav.NearNavs));
			return false;
		}







		void Run() {


			if (!interfaces->engine->isConnected() || !interfaces->engine->isInGame())
				return;

			if (!localPlayer.get() || !config->visuals.DrawNavs.enabled)
				return;

			if (!interfaces->engine->isInGame())
				return;

			std::string lvlName = std::string(memory->clientState->levelNameShort);
			//Debug::QuickPrint(lvlName.c_str());
			if (lvlName.empty()) {
				lvlName = std::string(memory->clientState->levelName);
				//Debug::QuickPrint(lvlName.c_str());
				if (lvlName.empty()) {
					Debug::QuickPrint({ 255,0,0,255 }, "Renderables::NavDraw::Run Unable to Retrieve Map Name from Client State");
					Map.Failed = true;
					return;
				}
			}

			if (!Map.Initialized || Map.MapName.compare(lvlName)) {

				if (Map.Initialized) {
					if (!Map.MapName.compare(lvlName)) {
						Debug::QuickPrint("Renderables::NavDraw::Run Map Names Do No Match");
					}
				}
				else {
					Debug::QuickPrint("Renderables::NavDraw::Run Initial Load Of Map NAV");
				}

				Map.MapName = lvlName;

				if (!InitLoad()) {
					Map.Failed = true;
					return;
				}
			}

			if (Map.PreviousDepth != config->visuals.DrawNavSettings.DrawNavDepth) {
				Map.PreviousDepth = config->visuals.DrawNavSettings.DrawNavDepth;
				Map.navset_backup.clear();

				if (config->visuals.DrawNavSettings.CalculateAllNavVectors) {
					Debug::QuickPrint("Renderables::NavDraw::Run Precalculating all Nav Sets for Nav Mesh");
					for (nav_mesh::nav_area area: *Map.NavFile.getNavAreas()) {
						int NavID = area.get_id();
						Map.navset_backup.insert(std::pair<int, std::vector<nav_mesh::nav_area>>(NavID, GetNestedConnections(NavID, (config->visuals.DrawNavSettings.DrawNavDepth))));
					}
				}


			}


			interfaces->surface->setDrawColor(config->visuals.DrawNavs.color[0]*255, config->visuals.DrawNavs.color[1] * 255, config->visuals.DrawNavs.color[2] * 255, config->visuals.DrawNavs.color[3] * 255);
			for (EntityQuick EntQuick : entitylistculled->getEnemies()) {
				PlayerNavs.at(EntQuick.index).entity = EntQuick.entity;
				SetPlayerNavs(PlayerNavs.at(EntQuick.index));
				float color[4];
				color[0] = config->visuals.DrawNavs.color.at(0);
				color[1] = config->visuals.DrawNavs.color.at(1) ;
				color[2] = config->visuals.DrawNavs.color.at(2) ;
				float* valcol = reinterpret_cast<float*>(config->visuals.DrawNavs.color.data());
				color[3] = valcol[3];
				NavMesh::DrawNavs(PlayerNavs.at(EntQuick.index).NearNavs, color);
			}

			return;
		}


		void DrawWholeMesh() {

			if (!interfaces->engine->isConnected() || !interfaces->engine->isInGame())
				return;


			if (!localPlayer.get() || !config->visuals.AllDrawNavs.enabled)
				return;

			if (!interfaces->engine->isInGame())
				return;

			std::string lvlName = std::string(memory->clientState->levelNameShort);
			if (!Map.Initialized || Map.MapName.compare(lvlName)) {
				if (Map.Initialized) {
					if (Map.MapName.compare(lvlName)) {
						Debug::QuickPrint("Renderables::NavDraw::DrawWholeMesh Map Names Do No Match");
					}
				}
				else {
					Debug::QuickPrint("Renderables::NavDraw::DrawWholeMesh Initial Load Of Map NAV");
				}
				Map.MapName = lvlName;
				if (!InitLoad())
					return;

			}


			interfaces->surface->setDrawColor(config->visuals.AllDrawNavs.color[0] * 255, config->visuals.AllDrawNavs.color[1] * 255, config->visuals.AllDrawNavs.color[2] * 255, config->visuals.AllDrawNavs.color[3] * 255);
			for (nav_mesh::nav_area area : *Map.NavFile.getNavAreas()) {
				NavMesh::DrawNav(area);
			}

		}

		void DrawLine(Vector A, Vector B) {
			Debug::fl_coords coords1, coords2;

			if (!ScreenMath::worldToScreen(A, coords1.x, coords1.y))
				return;

			if (!ScreenMath::worldToScreen(B, coords2.x, coords2.y))
				return;
			interfaces->surface->drawLine(coords1.x, coords1.y, coords2.x, coords2.y);

		}



		void DrawWalkbotPath() {

#ifndef WALKBOT_BUILD
			if (!config->walkbot.enabled)
				return;

			if(!config->walkbot.DrawPath)
				return;
#endif
			if (!localPlayer || localPlayer->isDormant() || !localPlayer->isAlive())
				return;



			if (config->walkbot.NoRender) {
				for (EntityQuick entq : entitylistculled->getEnemies()) {
					float x, y;
					if (ScreenMath::worldToScreen(entq.entity->getEyePosition(), x, y)) {
						interfaces->surface->setDrawColor(0, 0, 255, 255);
						interfaces->surface->drawCircle(x, y, 1, 25 * ((100 / localPlayer->origin().distTo(entq.entity->getEyePosition()))));
					}

				}
			}


			if (Walkbot::WBot.CurrentPath.size() < 2) {
				if (!Walkbot::WBot.CurrentPath.empty()) {
					interfaces->surface->setDrawColor(0, 255, 0, 255);
					NavMesh::DrawNav(Walkbot::WBot.Map.NavFile.get_area_by_position(Walkbot::WBot.CurrentPath.back()));
				}
				return;
			}

			nav_mesh::vec3_t Prev_Point = { 0,0,0 };
			bool set = false;
			for (nav_mesh::vec3_t Next_Point : Walkbot::WBot.CurrentPath) {

				if (!set) {
					set = true;
					Prev_Point = Next_Point;
					continue;
				}

				interfaces->surface->setDrawColor(255, 0, 0, 255);
				DrawLine(Prev_Point.toVector(), Next_Point.toVector());

				float x, y;
				if (ScreenMath::worldToScreen(Next_Point.toVector(), x, y)) {
					interfaces->surface->drawCircle(x, y, 1, 20  * ((100/localPlayer->origin().distTo(Next_Point.toVector()))));
				}

				Prev_Point = Next_Point;
			}
			NavMesh::DrawNav(Walkbot::WBot.Map.NavFile.get_area_by_position(Walkbot::WBot.CurrentPath.back()));
		}



	};








};