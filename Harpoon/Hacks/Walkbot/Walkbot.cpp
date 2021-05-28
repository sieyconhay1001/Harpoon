#include "Walkbot.h"
#include "WalkbotTargetingSystem.h"
#include "WalkbotAimBot.h"
#include "../../SDK/SDKAddition/Utils/VectorMath.h"
#include "../../SDK/OsirisSDK/LocalPlayer.h"
#include "../../SDK/OsirisSDK/Entity.h"
#include "../../COMPILERDEFINITIONS.h"
#include "WalkbotNetworking.h"
#include "../OTHER/Debug.h"
class UserCmd;

Walkbot::WalkbotData::WalkbotData() {
	TargetSys = new Walkbot::TargetingSystem(this);
}

Walkbot::WalkbotData::~WalkbotData() {
	delete TargetSys;
}

namespace WalkbotThink {

}

static inline void ClampViewAngles(Vector PrevAng, UserCmd* cmd) {
	cmd->viewangles.y = std::clamp(cmd->viewangles.y, PrevAng.y - 28.0f, PrevAng.y + 28.0f);
	cmd->viewangles.x = std::clamp(cmd->viewangles.x, PrevAng.x - 28.0f, PrevAng.x + 28.0f);
	cmd->viewangles.z = std::clamp(cmd->viewangles.z, PrevAng.z - 28.0f, PrevAng.z + 28.0f);
}

static inline void SetEyeAngles(UserCmd* cmd, Vector NavCenter) {
	float z = cmd->viewangles.z;
	float x = cmd->viewangles.x;
	Vector AngleToNavCenter = VectorMath::calculateRelativeAngle(localPlayer->getEyePosition(), NavCenter, cmd->viewangles);
	if ((Walkbot::WBot.Settings.LookSmoothing > 1) && (fabs(AngleToNavCenter.y) > 1.f)) {
		int PreserveSign = AngleToNavCenter.y / fabs(AngleToNavCenter.y);
		AngleToNavCenter.y = fabs(AngleToNavCenter.y);
		if (AngleToNavCenter.y > 28.0f) {
			float Difference = AngleToNavCenter.y - 28.0f;
			AngleToNavCenter -= Difference;
		}
		float SmoothFrac = 1.f-(static_cast<float>(Walkbot::WBot.Settings.LookSmoothing)/100.f);
		//SmoothFrac *= (28.0f / (AngleToNavCenter.y + .5));

		SmoothFrac *= 1.f - log((28.0f / (AngleToNavCenter.y + .5)))+1.f;

		SmoothFrac = std::clamp(SmoothFrac, 0.f, 1.f);
		AngleToNavCenter.y *= SmoothFrac;
		AngleToNavCenter.x *= SmoothFrac;
		AngleToNavCenter.y *= static_cast<float>(PreserveSign);
	}
	cmd->viewangles += AngleToNavCenter;
	cmd->viewangles.z = z;
	if (!Walkbot::WBot.UnlockX) {// Fix this
		if (fabs(cmd->viewangles.x) != 0.f) {
			int Sign = cmd->viewangles.x / fabs(cmd->viewangles.x);
			float Ang = fabs(0 - fabs(cmd->viewangles.x));
			Ang = std::clamp(Ang, 0.f, 28.f);
			if((Walkbot::WBot.Settings.LookSmoothing > 1))
				Ang /= static_cast<float>(Walkbot::WBot.Settings.LookSmoothing);
			Ang *= static_cast<float>(Sign);
			cmd->viewangles.x = Ang;
		}
		else {
			cmd->viewangles.x = x;
		}
	}
}

float ODEG2RAD(float degree) {
	return (float)(degree * 22.0 / (180.0 * 7.0));
}


#include "../Misc.h"

void CorrectMovement(float OldAngleY, UserCmd* pCmd, float fOldForward, float fOldSidemove)
{
	//side/forward move correction



	float deltaView = pCmd->viewangles.y - OldAngleY;
	float f1;
	float f2;

	if (OldAngleY < 0.f)
		f1 = 360.0f + OldAngleY;
	else
		f1 = OldAngleY;

	if (pCmd->viewangles.y < 0.0f)
		f2 = 360.0f + pCmd->viewangles.y;
	else
		f2 = pCmd->viewangles.y;

	if (f2 < f1)
		deltaView = abs(f2 - f1);
	else
		deltaView = 360.0f - abs(f1 - f2);
	deltaView = 360.0f - deltaView;

	pCmd->forwardmove = cos(ODEG2RAD(deltaView)) * fOldForward + cos(ODEG2RAD(deltaView + 90.f)) * fOldSidemove;
	pCmd->sidemove = sin(ODEG2RAD(deltaView)) * fOldForward + sin(ODEG2RAD(deltaView + 90.f)) * fOldSidemove;
}


static inline void SetupMovement(UserCmd* cmd, Vector NavCenter, int Forward, int Sideways, float Smoothing = 1.f) {
	Vector AngleToNavCenter = VectorMath::calculateRelativeAngle(localPlayer->getEyePosition(), NavCenter, cmd->viewangles);
	AngleToNavCenter /= Smoothing;
	Vector Ang = AngleToNavCenter + cmd->viewangles;
	CorrectMovement(Ang.y, cmd, Forward, Sideways);
}




static void CheckCollisions(UserCmd* cmd) {
	Vector maxs = localPlayer->origin();
	Vector mins = localPlayer->origin();

	maxs += localPlayer->getCollideable()->obbMaxs() * 1.5;
	mins.x += localPlayer->getCollideable()->obbMins().x * 1.5;
	mins.y += localPlayer->getCollideable()->obbMins().y * 1.5;



	std::vector<Vector> TestPoints;

	TestPoints.push_back(Vector{ (maxs.x + mins.x) / 2, maxs.y, maxs.z });
	TestPoints.push_back(Vector{ (maxs.x + mins.x) / 2, mins.y, maxs.z });
	TestPoints.push_back(Vector{ maxs.x, (maxs.y + mins.y) / 2, maxs.z });
	TestPoints.push_back(Vector{ mins.x, (maxs.y + mins.y) / 2, maxs.z });

	TestPoints.push_back(Vector{ (maxs.x + mins.x) / 2, maxs.y, ((((mins.z + maxs.z) / 2) / 2) / 2) });
	TestPoints.push_back(Vector{ (maxs.x + mins.x) / 2, mins.y, ((((mins.z + maxs.z) / 2) / 2) / 2) });
	TestPoints.push_back(Vector{ maxs.x, (maxs.y + mins.y) / 2, ((((mins.z + maxs.z) / 2) / 2) / 2) });
	TestPoints.push_back(Vector{ mins.x, (maxs.y + mins.y) / 2, ((((mins.z + maxs.z) / 2) / 2) / 2) });

	for (int i = 0; i < 2; i++) {
		for (int b = 0; b < 4; b++) {
			Vector Pos = (i == 0) ? localPlayer->getEyePosition() : localPlayer->origin();

			try {
				if (localPlayer->PointToPoint(Pos, TestPoints.at(((i + 1) * b))) < .997) {
					if (i == 1) {
						cmd->buttons |= UserCmd::IN_JUMP;
						return;
					}
					else {
						cmd->buttons |= UserCmd::IN_DUCK;
						return;
					}
				}
			}
			catch (std::exception& e) {
				//exception = e.what();
				return;
			}

		}
	}











}





namespace Walkbot {

	static bool IsInTargetArea(UserCmd* cmd, WalkbotData& Bot) {


		if (!(localPlayer->flags() & PlayerFlags::ONGROUND))
			return false;


		try {


			if (Bot.Settings.BBBox) {
				Vector maxs = localPlayer->origin();
				Vector mins = localPlayer->origin();
				maxs.x += localPlayer->getCollideable()->obbMaxs().x * .5f;
				maxs.y += localPlayer->getCollideable()->obbMaxs().y * .5f;
				mins.x += localPlayer->getCollideable()->obbMins().x * .5f;
				mins.y += localPlayer->getCollideable()->obbMins().y * .5f;
				try {
					if (Bot.Map.NavFile.inArea(maxs, Bot.CurrentPath.front())) {
						Bot.UnlockX = false;
						return true;
					}
				}
				catch (std::exception& e) {}
				try {
					if (Bot.Map.NavFile.inArea(mins, Bot.CurrentPath.front())) {
						Bot.UnlockX = false;
						return true;
					}
				}
				catch (std::exception& e) {}


				try {
					if (Bot.Map.NavFile.inArea(Vector{ mins.x, maxs.y }, Bot.CurrentPath.front())) {
						Bot.UnlockX = false;
						return true;
					}
				}
				catch (std::exception& e) {}


				try {
					if (Bot.Map.NavFile.inArea(Vector{ mins.y, maxs.x }, Bot.CurrentPath.front())) {
						Bot.UnlockX = false;
						return true;
					}
				}
				catch (std::exception& e) {}



			}


			if (Bot.Map.NavFile.inArea(localPlayer->origin(), Bot.CurrentPath.front())) {
				Bot.UnlockX = false;
				return true;
			}

			Bot.UnlockX = false;
			Bot.PathingStatus = InPath;
		}
		catch (std::exception& e) {
			/* So this is a Ghetto Method, but it does work. Essentially I need someway to determine what exception gets thrown */
			/* So I use StrStr() */
			if (strstr(e.what(), "NFE")) { /* Nav File Read Error */
				Debug::QuickPrint("Walkbot::IsInArea Failed To Find Any Areas In Map\n", false);
				cmd->forwardmove = 150;
				Bot.Map.Failed = true;
				Bot.Map.Initialized = false;
			}
			else if (strstr(e.what(), "FTFA")) { /* This dumb ass bot as left the area */
				Bot.PathingStatus = OutOfNavBounds;
				cmd->forwardmove = 150;

				if (fabs(Bot.CurrentPath.front().z - (Bot.Map.NavFile.nearestArea(localPlayer->origin()).z)) > 50.f) {
					Bot.NextPosition = Bot.CurrentPath.front().toVector();
				}
				else {
					Bot.NextPosition = Bot.Map.NavFile.nearestArea(localPlayer->origin());
				}



				//Debug::QuickPrint("Walkbot::IsInArea Failed To Find Area\n", false);
				Bot.UnlockX = true;
				return false;
			}
		}
		return false;
	}



	static bool IsInTargetAreaVecRefrence(UserCmd* cmd, WalkbotData& Bot, Vector Area) {


		if (!localPlayer || !localPlayer->isAlive() || localPlayer->isDormant())
			return true;

		if (!(localPlayer->flags() & PlayerFlags::ONGROUND))
			return false;


		try {

			int AreaID = Bot.Map.NavFile.get_area_by_position(Area).get_id();

			if (Bot.Settings.BBBox) {
				Vector maxs = localPlayer->origin();
				Vector mins = localPlayer->origin();
				maxs.x += localPlayer->getCollideable()->obbMaxs().x * .5f;
				maxs.y += localPlayer->getCollideable()->obbMaxs().y * .5f;
				mins.x += localPlayer->getCollideable()->obbMins().x * .5f;
				mins.y += localPlayer->getCollideable()->obbMins().y * .5f;
				try {
					if (Bot.Map.NavFile.inArea(maxs, AreaID)) {
						Bot.UnlockX = false;
						return true;
					}
				}
				catch (std::exception& e) {}
				try {
					if (Bot.Map.NavFile.inArea(mins, AreaID)) {
						Bot.UnlockX = false;
						return true;
					}
				}
				catch (std::exception& e) {}


				try {
					if (Bot.Map.NavFile.inArea(Vector{ mins.x, maxs.y }, AreaID)) {
						Bot.UnlockX = false;
						return true;
					}
				}
				catch (std::exception& e) {}


				try {
					if (Bot.Map.NavFile.inArea(Vector{ mins.y, maxs.x }, AreaID)) {
						Bot.UnlockX = false;
						return true;
					}
				}
				catch (std::exception& e) {}



			}


			if (Bot.Map.NavFile.inArea(localPlayer->origin(), AreaID)) {
				Bot.UnlockX = false;
				return true;
			}

			Bot.UnlockX = false;
			Bot.PathingStatus = InPath;
		}
		catch (std::exception& e) {
			/* So this is a Ghetto Method, but it does work. Essentially I need someway to determine what exception gets thrown */
			/* So I use StrStr() */
			if (strstr(e.what(), "NFE")) { /* Nav File Read Error */
				Debug::QuickPrint("Walkbot::IsInArea Failed To Find Any Areas In Map\n", false);
				cmd->forwardmove = 150;
				Bot.Map.Failed = true;
				Bot.Map.Initialized = false;
			}
			else if (strstr(e.what(), "FTFA")) { /* This dumb ass bot as left the area */
				Bot.PathingStatus = OutOfNavBounds;
				cmd->forwardmove = 150;

				if (fabs(Bot.CurrentPath.front().z - (Bot.Map.NavFile.nearestArea(localPlayer->origin()).z)) > 50.f) {
					if (!Bot.CurrentPath.empty()) {
						Bot.NextPosition = Bot.CurrentPath.front().toVector();
					}
				}
				else {
					Bot.NextPosition = Bot.Map.NavFile.nearestArea(localPlayer->origin());
				}



				//Debug::QuickPrint("Walkbot::IsInArea Failed To Find Area\n", false);
				Bot.UnlockX = true;
				return false;
			}
		}
		return false;
	}


	namespace Smoothing {

		/* TODO: Figure This Out */


		struct line {
			Vector Start;
			Vector End;
		};

		bool GetLineClipPoint(nav_mesh::nav_area Nav, nav_mesh::vec3_t& Point, line Line) {
			//if (Nav.is_within(Line.Start) && Nav.is_within(Line.End))
			//	return false;

			
			return false;

		}





		static bool Intersects(line l1, line l2, Vector& intersection)
		{
			Vector a1 = l1.Start;
			Vector a2 = l1.End;
			Vector b1 = l2.Start;
			Vector b2 = l2.End;

			intersection = {0,0,0};

			Vector b = a2 - a1;
			Vector d = b2 - b1;
			float bDotDPerp = b.x * d.y - b.y * d.x;

			// if b dot d == 0, it means the lines are parallel so have infinite intersection points
			if (bDotDPerp == 0)
				return false;

			Vector c = b1 - a1;
			float t = (c.x * d.y - c.y * d.x) / bDotDPerp;
			if (t < 0 || t > 1)
				return false;

			float u = (c.x * b.y - c.y * b.x) / bDotDPerp;
			if (u < 0 || u > 1)
				return false;

			intersection = a1 + b * t;

			return true;
		}


		static bool ClipLineToNav(nav_mesh::nav_area Nav, line Line, std::vector<nav_mesh::vec3_t>& Intersections) {

			nav_mesh::vec3_t helper;
			if (Nav.is_within(helper.convertVector(Line.Start)) && Nav.is_within(helper.convertVector(Line.End))) {
				return false;
			}

			auto Bounds = Nav.GetBounds();

			Vector Intersection;

			if (Intersects(line{ Bounds.ne, Bounds.nw }, Line, Intersection)) {
				Intersections.push_back(helper.convertVector(Intersection));
			}

			if (Intersects(line{ Bounds.ne, Bounds.se }, Line, Intersection)) {
				Intersections.push_back(helper.convertVector(Intersection));
			}


			if (Intersects(line{ Bounds.se, Bounds.sw }, Line, Intersection)) {
				Intersections.push_back(helper.convertVector(Intersection));
			}

			if (Intersects(line{ Bounds.sw, Bounds.nw }, Line, Intersection)) {
				Intersections.push_back(helper.convertVector(Intersection));
			}


			if (Intersections.size() > 0) {
				return true;
			}
			else {
				return false;
			}


		}

		static bool ClipLinetoNavs(std::vector<nav_mesh::nav_area> Areas, line Line) {
			std::vector<nav_mesh::vec3_t> Intersections;


			for (auto Area : Areas) {
				if (ClipLineToNav(Area, Line, Intersections)) {

				}
			}




		}





		static void SmoothPath(WalkbotData& Bot) {


			if (Bot.CurrentPath.size() < 1) {
				SetupPath(Bot);
				Bot.CurrentPath.erase(Bot.CurrentPath.begin());
				return;
			}

			if (Bot.CurrentPath.empty())
				return;


			if (!localPlayer->directPathToPoint(Bot.Map.NavFile.get_area_by_position(Bot.CurrentPath.at(0)).get_center().toVector()))
				return;

			nav_mesh::vec3_t endSave = Bot.CurrentPath.back();

			std::vector<nav_mesh::vec3_t> SmoothedPath;
			bool ran = false;
			SmoothedPath.push_back(Bot.Map.NavFile.get_area_by_position(Bot.CurrentPath.at(0)).get_center());
			for (int i = 0; i < (Bot.CurrentPath.size()); i++) {
				nav_mesh::nav_area Start = Bot.Map.NavFile.get_area_by_position(Bot.CurrentPath.at(i));
				nav_mesh::nav_area Previous = Start;
				if (!ran) {
					for (int j = (i + 1); j < (Bot.CurrentPath.size()); j++) {
						nav_mesh::nav_area End = Bot.Map.NavFile.get_area_by_position(Bot.CurrentPath.at(j));
						Vector Point = End.get_center().toVector();
						Point.z += 5;
						if (!localPlayer->directPathToPoint(Point) && (i != j)) {
							i += (j - 1);
							SmoothedPath.push_back(Previous.get_center());
							break;
						}
						Previous = End;
					}
					//if(!ran)


				}
				else {
					SmoothedPath.push_back(Start.get_center());
				}
				ran = true;

				

			}

			Bot.CurrentPath = SmoothedPath;
			Bot.CurrentPath.push_back(endSave);



		}
	}

	static bool SetupPath(WalkbotData& Bot) {

		if (!localPlayer || !localPlayer->isAlive())
			return false;


		if (Bot.MainPath.size() < 2) {
			Bot.MainPath = Bot.MapPaths.mainPath;
		}


		if (Bot.MainPath.empty())
			return false;

		Bot.PathingStatus = CalculatingPath;

		//Debug::QuickPrint("Walkbot::SetupPath Calculating New Path For Bot");
		try {
			if (Bot.TargetedEntity && (Bot.TargetedEntity != localPlayer.get()) && !Bot.TargetedEntity->isDormant() && Bot.TargetedEntity->isAlive()) {
				try {
					Bot.CurrentPath = Bot.Map.NavFile.find_path(nav_mesh::vec3_t::convertSDKVector(localPlayer->origin()), nav_mesh::vec3_t::convertSDKVector(Bot.TargetedEntity->origin()));
					return false;
				}
				catch (std::exception& e) {

				}

				Bot.CurrentPath = Bot.Map.NavFile.find_path(nav_mesh::vec3_t::convertSDKVector(localPlayer->origin()), Bot.MainPath.front());

			}
			else {
				Bot.CurrentPath = Bot.Map.NavFile.find_path(nav_mesh::vec3_t::convertSDKVector(localPlayer->origin()), Bot.MainPath.front());
			}
			Bot.MainPath.erase(Bot.MainPath.begin());
		}
		catch (std::exception& e) {
			//Debug::QuickPrint(("SetupPath" + std::to_string(Bot.MainPath.front())).c_str());
			//Debug::QuickPrint(e.what());
			//Debug::QuickPrint(e.what());
			nav_mesh::nav_area* area = 0;
			try {
				area = Bot.Map.NavFile.nearestAreaReturnAreaPtr(localPlayer->origin());
				if (!area)
					return false;
			}
			catch (std::exception& e) {
				return false;
			}


			try {
				if (Bot.CurrentPath.size() > 0) {

					Bot.CurrentPath = Bot.Map.NavFile.find_path(area->get_center(), Bot.MainPath.front());
				}
				else {
					// Access Violation at location <<<<



					Bot.CurrentPath = Bot.Map.NavFile.find_path(area->get_center(), Bot.Map.NavFile.get_area_by_position(Bot.NextPosition).get_id());
				}
			}
			catch (std::exception& e) {
				//Debug::QuickPrint(e.what());
				Bot.PathingStatus = OutOfNavBounds;
				Bot.NextPosition = Bot.Map.NavFile.nearestArea(localPlayer->origin());
				Bot.UnlockX = true;
				return false;
			}
		}

		//Bot.CurrentPath = 


		return false;
	}



	/* Shoulda Used A Deque */
	static bool SetupRelativePath(WalkbotData& Bot) {

		if (Bot.CurrentPath.size() < 2) {
			SetupPath(Bot);
			return false;
		}

		//Debug::QuickPrint("Walkbot::SetupPath Calculating New Path For Bot");
		try {

			std::vector<nav_mesh::vec3_t> PathBackup = Bot.CurrentPath;



			Bot.CurrentPath = Bot.Map.NavFile.find_path(nav_mesh::vec3_t::convertSDKVector(localPlayer->origin()), Bot.CurrentPath.front());

			for (auto Point : PathBackup) {
				Bot.CurrentPath.push_back(Point);
			}


		}
		catch (std::exception& e) {
			//SetupPath(Bot);
			//Debug::QuickPrint("SetupRelativePath Err");
			Bot.PathingStatus = OutOfNavBounds;
			Bot.NextPosition = Bot.Map.NavFile.nearestArea(localPlayer->origin());
			Bot.UnlockX = true;
			return false;
		}

		//Bot.CurrentPath = 


		return false;
	}

	static bool CanReload(UserCmd* cmd, WalkbotData& Bot) {
		const auto activeWeapon = localPlayer->getActiveWeapon();
		if (!activeWeapon)
			return true;

		auto weaponInfo = activeWeapon->getWeaponData();
		if (!weaponInfo)
			return true;


		float nDist = entitylistculled->getDistToNearestPlayer();

	

		bool should = (nDist < 100.f) || (nDist == -1.f);

		if (!(Bot.CurrentAct == PlayerLink) || (Bot.Status == WalkbotStatus::InEngagement)) {
			if (((weaponInfo->maxClip * .60) > activeWeapon->clip()) && ((activeWeapon->reserveAmmoCount() + activeWeapon->clip()) > (weaponInfo->maxClip * .60)) && should && !activeWeapon->isKnife()) {
				cmd->buttons &= ~(UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2);
				cmd->buttons |= UserCmd::IN_RELOAD;
				return false;
			}
			else if (((!activeWeapon->isInReload() && !activeWeapon->isPrimary() && !activeWeapon->c4StartedArming()) || activeWeapon->isKnife()) && (!entitylistculled->getEnemies().empty())) {
				Entity* Pistol = 0;
				for (auto i = 0; i < 64; ++i)
				{
					auto Weapon = interfaces->entityList->getEntityFromHandle(localPlayer->weapons()[i]);
					if (Weapon && Weapon->isPrimary() && (Weapon->clip() || Weapon->reserveAmmoCount()))
					{
						cmd->weaponselect = Weapon->index();
						cmd->weaponsubtype = Weapon->getWeaponSubType();
						return false;
					}
					if (Weapon && Weapon->isPistol())
						Pistol = Weapon;
				}
				if (Pistol) {
					cmd->weaponselect = Pistol->index();
					cmd->weaponsubtype = Pistol->getWeaponSubType();
					return false;
				}
			}
			else if (activeWeapon->isKnife() && (!entitylistculled->getEnemies().empty())) {
				Entity* Pistol = 0;
				for (auto i = 0; i < 64; ++i)
				{
					auto Weapon = interfaces->entityList->getEntityFromHandle(localPlayer->weapons()[i]);
					if (Weapon && Weapon->isPrimary() && (Weapon->clip() || Weapon->reserveAmmoCount()))
					{
						cmd->weaponselect = Weapon->index();
						cmd->weaponsubtype = Weapon->getWeaponSubType();
						return false;
					}
					if (Weapon && Weapon->isPistol())
						Pistol = Weapon;
				}
				if (Pistol) {
					cmd->weaponselect = Pistol->index();
					cmd->weaponsubtype = Pistol->getWeaponSubType();
					return false;
				}
			}
		}


		if ((entitylistculled->getEnemies().empty() && !activeWeapon->isInReload()) || (Bot.CurrentAct == PlayerLink)) {
			for (auto i = 0; i < 64; ++i)
			{
				auto Weapon = interfaces->entityList->getEntityFromHandle(localPlayer->weapons()[i]);
				if (Weapon && Weapon->isKnife());
				{
					cmd->weaponselect = Weapon->index();
					cmd->weaponsubtype = Weapon->getWeaponSubType();
					return false;
				}
			}
		}




		return false;

	}



	static bool TraversePath(UserCmd* cmd, WalkbotData& Bot){

		if (!localPlayer->isAlive()) {
			Bot.CurrentPath.clear();
			return true;
		}

		if (Bot.TicksStuck > 15) {
			Bot.CurrentPath.clear();
			Bot.TicksStuck = 0;
		}


		Bot.forwardMove = 450;

		//if(!config->walkbot.pathonly)
		//	cmd->forwardmove = 450;

		CanReload(cmd, Bot);

		if (!Bot.CurrentPath.empty()) {
			Bot.NextPosition = Bot.Map.NavFile.get_area_by_position(Bot.CurrentPath.front()).get_center().toVector();
		}

		if (Bot.CurrentPath.empty()) {
			SetupPath(Bot);
			return true;
		}

		/* Check to see if we ran into anything*/
		if (localPlayer->velocity().length2D() < 2.0f) {
			Bot.TicksStuck++;
			CheckCollisions(cmd);
		}



		/* Check to see if we are in an area */
		Bot.NextPosition = Bot.Map.NavFile.get_area_by_position(Bot.CurrentPath.front()).get_center().toVector();
		if (!IsInTargetArea(cmd, Bot)) {
			SetEyeAngles(cmd, Bot.NextPosition);
			if (!Bot.CurrentPath.empty() && config->walkbot.smoothPath) {
				Vector Point = Bot.CurrentPath.front().toVector();
				Point.z += 5;
				if (!localPlayer->canSeePoint(Point)) {
					if (Bot.cmdsSinceSeen > 64) {
						SetupRelativePath(Bot);
						Bot.cmdsSinceSeen = 0;
					}
					else {
						Bot.cmdsSinceSeen++;
					}
				}
			}
			return false;
		} 

		//Debug::QuickPrint("We In The Target Area");

		Bot.CurrentPath.erase(Bot.CurrentPath.begin());

		if (config->walkbot.smoothPath) {
			//Debug::QuickPrint("Smoothing Path");
			Smoothing::SmoothPath(Bot);
		}
		if (Bot.CurrentPath.empty()) {
			SetupPath(Bot);
		}	
	}


	void SetupBotSettings(WalkbotData& Bot) {

#ifdef WALKBOT_BUILD
		Bot.Settings.LookSmoothing = 95.f;
		Bot.Settings.AimbotParams.FOV = 100;
		Bot.Settings.AimbotParams.AimSmoothing = 10.f;
		Bot.Settings.AimbotParams.hitchance = config->walkbot.hitchance;
		Bot.Settings.AimbotParams.VariableSmoothing = true;
		Bot.Settings.AimbotParams.ignoreHead = config->walkbot.ignoreHead;
		Bot.Settings.BBBox = true;
		Bot.Settings.WalkSmoothing = true;
#endif

#ifndef WALKBOT_BUILD
		Bot.Settings.LookSmoothing = config->walkbot.lookSmoothing;
		Bot.Settings.AimbotParams.FOV = config->walkbot.AimFov;
		Bot.Settings.AimbotParams.AimSmoothing = config->walkbot.AimSmoothing;
		Bot.Settings.AimbotParams.hitchance = config->walkbot.hitchance;
		Bot.Settings.AimbotParams.VariableSmoothing = config->walkbot.variableSmooth;
		Bot.Settings.AimbotParams.ignoreHead = config->walkbot.ignoreHead;
		Bot.Settings.BBBox = config->walkbot.boundingbox;
		Bot.Settings.WalkSmoothing = config->walkbot.walkSmoothing;
#endif

#ifdef TROLL_BUILD
		Bot.Settings.AimbotParams.FOV = 100;
		Bot.Settings.AimbotParams.AimSmoothing = 1.f;
		Bot.Settings.AimbotParams.VariableSmoothing = false;
#endif


	}


	static bool DoHide(UserCmd* cmd, WalkbotData& Bot) {


		return false;
	}



	int PlantSpot = -1;
	bool set = false;
	static bool DoPlant(UserCmd* cmd, WalkbotData& Bot) {
		if (!set || (PlantSpot < 0)) {
			int randnum = (std::rand() % 100) + 1;
			if (randnum > 50) {
				PlantSpot = Bot.MapPaths.A;
			}
			else {
				PlantSpot = Bot.MapPaths.B;
			}
			set = true;
		}
		try {
			if (!Bot.Map.NavFile.inArea(localPlayer->origin(), PlantSpot)) {
				Bot.MainPath.clear();
				Bot.MainPath.push_back(PlantSpot);
				Bot.MainPath.push_back(PlantSpot);
				return TraversePath(cmd, Bot);
			}
			else {

				Bot.forwardMove = 0;
				Bot.sideMove = 0;

				if (!localPlayer->hasBomb()) {
					Bot.TargetSys->Planted();
					set = false;
				}

				cmd->viewangles.x = 89;
				cmd->buttons |= UserCmd::IN_USE;
			}
		}
		catch (std::exception& e) {
			Bot.MainPath.clear();
			Bot.MainPath.push_back(PlantSpot);
			Bot.MainPath.push_back(PlantSpot);
			return TraversePath(cmd, Bot);
		}
	
		return false;
	}

	Vector BombPosition;
	static bool DoDefuse(UserCmd* cmd, WalkbotData& Bot) {
		if (!set || (PlantSpot < 0)) {
			for (int i = 0; i < interfaces->entityList->getHighestEntityIndex(); i++)
			{
				Entity* pEnt = interfaces->entityList->getEntity(i);
				if (!pEnt)
					continue;

				if (pEnt->isBomb())
				{
					BombPosition = pEnt->getAbsOrigin();
					PlantSpot = Bot.Map.NavFile.nearestAreaReturnAreaPtr(BombPosition)->get_id();
				}
			}
		}
		try {
			if (!Bot.Map.NavFile.inArea(localPlayer->origin(), PlantSpot)) {
				Bot.MainPath.clear();
				Bot.MainPath.push_back(PlantSpot);
				Bot.MainPath.push_back(PlantSpot);
				return TraversePath(cmd, Bot);
			}
			else {

				Bot.forwardMove = 0;
				Bot.sideMove = 0;

				static bool bHasBeenDefusing{ false };
				if (!localPlayer->isDefusing()) {
					Bot.GoalPoint = BombPosition;
					if (bHasBeenDefusing)
					{
						bHasBeenDefusing = false;
						set = false;
						Bot.TargetSys->Defused();
					}

				}
				else {
					bHasBeenDefusing = true;
				}


				cmd->viewangles.x = 89;
				cmd->buttons |= UserCmd::IN_USE;
			}
		}
		catch (std::exception& e) {
			Bot.MainPath.clear();
			Bot.MainPath.push_back(PlantSpot);
			Bot.MainPath.push_back(PlantSpot);
			return TraversePath(cmd, Bot);
		}

		return false;
	}



	static bool ToPoint(UserCmd* cmd, WalkbotData& Bot) {

		Bot.MainPath.clear();

		nav_mesh::vec3_t Pos;



		int Point = 0;//Bot.Map.NavFile.get_area_by_position(Pos.convertSDKVector(Bot.GoalPoint)).get_id();

		try {
			Point = Bot.Map.NavFile.get_area_by_position(Pos.convertSDKVector(Bot.GoalPoint)).get_id();
		}
		catch (std::exception& e) {
			try {
				Point = Bot.Map.NavFile.get_area_by_position(Bot.Map.NavFile.nearestArea(Bot.GoalPoint)).get_id();
			}
			catch (std::exception& u) {
				return true;
			}
		}

		try {
			Bot.CurrentPath.clear();
			if (Bot.CurrentPath.empty()) {
				Bot.MainPath.push_back(Point);
				Bot.MainPath.push_back(Point);
				Bot.MainPath.push_back(Point);
				Bot.MainPath.push_back(Point);
				SetupPath(Bot);
			}


			if (!IsInTargetAreaVecRefrence(cmd, Bot, Bot.GoalPoint)) {
				SetupPath(Bot);
				Bot.MainPath.clear();
				Bot.MainPath.push_back(Point);
				Bot.MainPath.push_back(Point);
				Bot.MainPath.push_back(Point);
				Bot.MainPath.push_back(Point);
				return TraversePath(cmd, Bot);
			}
			else {
				Bot.NextPosition = Bot.GoalPoint;
				if (localPlayer->origin().distTo(Bot.GoalPoint) < 10.f){
					Bot.forwardMove = 0;
					Bot.sideMove = 0;
					Bot.Status = IdleStandBy;
				}
				return false;
			}
		}
		catch (std::exception& e) {
			Bot.MainPath.clear();
			Bot.MainPath.push_back(Point);
			Bot.MainPath.push_back(Point);
			Bot.MainPath.push_back(Point);
			Bot.MainPath.push_back(Point);
			return TraversePath(cmd, Bot);
		}
	}


	bool ShouldPlay = true;
	int PlayDude = 0;


	bool wasDead = false;
	bool calledvote = false;
	static bool LinkToPlayer(UserCmd* cmd, WalkbotData& Bot) {

		//Debug::QuickPrint("In Link To Player");
		Entity* entity = interfaces->entityList->getEntity(Bot.LinkEntity);
		//Debug::QuickPrint("Aquired Entity Handle");


		

		if (!entity || entity->isDormant() || !entity->isAlive() || (entity == localPlayer.get())) {
			Bot.Status = WalkbotStatus::IdleStandBy;
			Bot.CurrentAct = WalkbotActs::Hide;
			wasDead = true;
			if ((interfaces->cvar->findVar("game_type")->getInt() == 1) && (interfaces->cvar->findVar("game_mode")->getInt() == 2)) {
				if (entity && entity->isPlayer() && !entity->isOtherEnemy(localPlayer.get())) {
					return false;
				}
			}

			WBot.CurrentAct = WalkbotActs::FindPlayerToLink;
			WBot.Status = ToPosition;
			//Debug::QuickPrint("Aint Live No More");
			ShouldPlay = true;
			return false;
		}


		if (ShouldPlay) {

			int num = rand() % 100;

			if (num < 50) {
				//SoundSystem::soundPlayer->Play("MINE");
			}
			else {
				//SoundSystem::soundPlayer->Play("REED");
			}
			ShouldPlay = false;
		}

		if ((cmd->tickCount > 20000) && !calledvote && Misc::freezeTime) {
			calledvote = true;
			interfaces->engine->clientCmdUnrestricted("callvote starttimeout");
			SoundSystem::soundPlayer->Play("BTG");
		}

		PlayDude++;
		if (!(cmd->tickCount % 1024)) {
			interfaces->engine->clientCmdUnrestricted("chatwheel_ping");
		}


		if (!(cmd->tickCount % 512)) {
			interfaces->engine->clientCmdUnrestricted("getout");
		}


		if (PlayDude > (64 * 15)) {
			PlayDude = 0;
			//SoundSystem::soundPlayer->Play("DUDE");
		}

		Bot.Status = WalkbotStatus::ToPosition;

		Vector EntityPos = entity->origin();


		if (!((interfaces->cvar->findVar("game_type")->getInt() == 1) && (interfaces->cvar->findVar("game_mode")->getInt() == 2))) {
			if ((localPlayer->origin().distTo(EntityPos) < 50.f) && (entity->velocity().length2D() < 250.f)) {
				Vector viewAngles = Vector::fromAngle(entity->eyeAngles()) * 5;
					Vector startPos = entity->getEyePosition();
					Vector endPos = startPos + viewAngles;

					Trace trace;
					interfaces->engineTrace->traceRay({ startPos, endPos }, 0x46004009, localPlayer.get(), trace);

					if ((trace.fraction > .80) || (trace.entity == localPlayer.get())) {
						endPos.z = EntityPos.z;
							EntityPos = endPos;
					}
			}
		}

		nav_mesh::vec3_t Pos;
		int Point = 0;
		try {
			Point = Bot.Map.NavFile.get_area_by_position(Pos.convertSDKVector(EntityPos)).get_id();
		}
		catch(std::exception& e) {
			try {
				Point = Bot.Map.NavFile.get_area_by_position(Bot.Map.NavFile.nearestArea(EntityPos)).get_id();
			}
			catch (std::exception& u) {



				return true;
			}
		}
		//Bot.CurrentPath.clear();
		Bot.MainPath.clear();
		Bot.MainPath.push_back(Point);
		Bot.MainPath.push_back(Point);

		try {




			if (Bot.CurrentPath.empty() || wasDead) {
				Bot.CurrentPath.clear();
				wasDead = false;
				Bot.MainPath.push_back(Point);
				Bot.MainPath.push_back(Point);
				SetupPath(Bot);
			}


			if (!IsInTargetAreaVecRefrence(cmd, Bot, EntityPos)) {


				for (auto i = 0; i < 64; ++i)
				{
					auto Weapon = interfaces->entityList->getEntityFromHandle(localPlayer->weapons()[i]);
					if (Weapon && Weapon->isKnife())
					{
						cmd->weaponselect = Weapon->index();
						cmd->weaponsubtype = Weapon->getWeaponSubType();
					}
				}


				//Debug::QuickPrint("Not In Target Area");
				Bot.MainPath.push_back(Point);
				Bot.MainPath.push_back(Point);
				return TraversePath(cmd, Bot);
			}
			else {
				Bot.CurrentPath.clear();
				//Debug::QuickPrint("In Target Area");
				Bot.NextPosition = EntityPos;
				//cmd->forwardmove = 450;
				Bot.forwardMove = 450;
				if (localPlayer->getActiveWeapon() && localPlayer->getActiveWeapon()->isWeapon()) {
					if ((entity->velocity().length2D() < 20.f) && !localPlayer->getActiveWeapon()->isRevolver()) {
						for (auto i = 0; i < 64; ++i)
						{
							auto Weapon = interfaces->entityList->getEntityFromHandle(localPlayer->weapons()[i]);
							if (Weapon && Weapon->isRevolver())
							{
								cmd->weaponselect = Weapon->index();
								cmd->weaponsubtype = Weapon->getWeaponSubType();
							}
						}
					}
				}

				Misc::prepareRevolver(cmd);





				return false;
			}
		}
		catch (std::exception& e) {
			/*
			Debug::QuickPrint(e.what());
			if (Bot.CurrentPath.empty()) {
				Bot.NextPosition = EntityPos;
			}
			else {
				Bot.NextPosition = Bot.CurrentPath.front().toVector();
			}
			Bot.MainPath.clear();
			Bot.MainPath.push_back(Point);
			Bot.MainPath.push_back(Point);
			cmd->forwardmove = 450;
			return TraversePath(cmd, Bot);
			*/
		}


		return false;
	}




	bool BindToTeammate(UserCmd* cmd, WalkbotData& Bot) {


		for (EntityQuick entq : entitylistculled->getEntities()) {

			if (entq.m_bisEnemy )//|| entq.entity->isBot())
				continue;

			if (entq.entity->isAlive() && !entq.m_bisDormant &&  !(entq.entity == localPlayer.get()) && !strstr(entq.entity->getPlayerName(false).c_str(), "Generic")) {
				Bot.LinkEntity = entq.entity->index();
				Bot.CurrentAct = WalkbotActs::PlayerLink;		
				Bot.Status = WalkbotStatus::ToPosition;
				break;
			}
			else {
				continue;
			}
		}


		return false;

	}



	static bool DoActivities(UserCmd* cmd, WalkbotData& Bot) {

		//if(Bot.Settings.)

		switch (Bot.Status) {
		case IdleStandBy:
		{
			//Debug::QuickPrint("Exiting Due To Idle Status");
			return false;
		}
			break;
		default:
			break;

		}

		//Debug::QuickPrint((" DoActivities - Status: " + std::to_string(Bot.Status) + " Activities: " + std::to_string(Bot.CurrentAct)).c_str());

		switch (Bot.CurrentAct) {
			case WalkbotActs::FollowPath:
			{
				Bot.Status = ActiveStandby;
				return TraversePath(cmd, Bot);
				break;
			}
			case WalkbotActs::Hide:
			{
				Bot.Status = Idle;
				return DoHide(cmd, Bot);
				break;
			}
			case WalkbotActs::Plant:
			{
				Bot.Status = CompletingObjective;
				return DoPlant(cmd, Bot);
				break;
			}
			case WalkbotActs::Defuse:
			{
				Bot.Status = CompletingObjective;
				return DoDefuse(cmd, Bot);
				break;
			}
			case WalkbotActs::HeadToPoint:
			{
				return ToPoint(cmd, Bot);
				break;
			}
			case WalkbotActs::PlayerLink:
			{
				Bot.Status = ToPosition;
				return LinkToPlayer(cmd, Bot);
				break;
			}
			case WalkbotActs::FindPlayerToLink:
			{
				return BindToTeammate(cmd, Bot);
				break;
			}
			default:
				return false;
		}







	}


	/*
	
			static const auto mp_buytime = interfaces->cvar->findVar("mp_buytime");

		if ((memory->globalVars->realtime < (freezeEnd + mp_buytime->getFloat())) && !localPlayer->gunGameImmunity()) {

			if ((localPlayer->team() == 2) && (localPlayer->getActiveWeapon()->isPistol()) && ((localPlayer->account() >= 800) || (localPlayer->gunGameImmunity() && (localPlayer->getActiveWeapon()->isPistol())))) {
				std::string cmd = "buy vest; buy vesthelm; buy gsg08; buy awp; buy ak47; buy p90; buy deagle";
				interfaces->engine->clientCmdUnrestricted(cmd.c_str());
	}
			else if ((localPlayer->team() == 3) && (localPlayer->getActiveWeapon()->isPistol()) && ((localPlayer->account() >= 800) || (localPlayer->gunGameImmunity() && (localPlayer->getActiveWeapon()->isPistol())))) {
				std::string cmd = "buy vest; buy defuser; buy scar20; buy awp; buy m4a1_silencer; buy p90; buy deagle";
				interfaces->engine->clientCmdUnrestricted(cmd.c_str());
			}
			else if ((localPlayer->armor() < 90) && (localPlayer->account() > 650) && !localPlayer->gunGameImmunity() && (freezeEnd != 0.0f)) {
				std::string cmd = "buy vest;";
				interfaces->engine->clientCmdUnrestricted(cmd.c_str());
			}
			else


	*/
	bool boughtthislife = false;
	void SetupBuy() {

		if (!localPlayer->gunGameImmunity() && !Misc::freezeTime) {
			//Debug::QuickPrint("Not FreezeTime");
			boughtthislife = false;
			return;
		}

		if (boughtthislife)
			return;

		if ((Walkbot::WBot.CurrentAct == WalkbotActs::PlayerLink) && Misc::freezeTime) {
			if ((localPlayer->account() >= 600) && !localPlayer->hasRevolver()) {
				interfaces->engine->clientCmdUnrestricted("buy deagle");
				boughtthislife = true;
			}
			return;
		}




		if (localPlayer->gunGameImmunity() && !boughtthislife) {
			if (localPlayer->getActiveWeapon() && !localPlayer->getActiveWeapon()->isPrimary()) {
				if ((localPlayer->team() == 2)) {
					std::string cmd = "buy ak47";
					interfaces->engine->clientCmdUnrestricted(cmd.c_str());
					boughtthislife = true;
					return;
				}
				else if ((localPlayer->team() == 3)) {
					std::string cmd = "buy m4a1_silencer";
					interfaces->engine->clientCmdUnrestricted(cmd.c_str());
					boughtthislife = true;
					return;
				}
			}
		}

		/*

		float Walkbot::freezeEnd = 0.0f;

		void Walkbot::SetupTime(GameEvent* event) {

			if (!localPlayer)
				return;
				switch (fnv::hashRuntime(event->getName())) {
				case fnv::hash("round_start"):
					freezeEnd = memory->globalVars->realtime;
					break;
				case fnv::hash("round_freeze_end"):
					freezeEnd = 0.0f;
					break;
			}
		}

		static const auto mp_buytime = interfaces->cvar->findVar("mp_buytime");

		if ((memory->globalVars->realtime < (freezeEnd + mp_buytime->getFloat())) && !localPlayer->gunGameImmunity()) {

			if ((localPlayer->team() == 2) && (localPlayer->getActiveWeapon()->isPistol()) && ((localPlayer->account() >= 800) || (localPlayer->gunGameImmunity() && (localPlayer->getActiveWeapon()->isPistol())))) {
				std::string cmd = "buy vest; buy vesthelm; buy gsg08; buy awp; buy ak47; buy p90; buy deagle";
				interfaces->engine->clientCmdUnrestricted(cmd.c_str());
			}
			else if ((localPlayer->team() == 3) && (localPlayer->getActiveWeapon()->isPistol()) && ((localPlayer->account() >= 800) || (localPlayer->gunGameImmunity() && (localPlayer->getActiveWeapon()->isPistol())))) {
				std::string cmd = "buy vest; buy defuser; buy scar20; buy awp; buy m4a1_silencer; buy p90; buy deagle";
				interfaces->engine->clientCmdUnrestricted(cmd.c_str());
			}
			else if ((localPlayer->armor() < 90) && (localPlayer->account() > 650) && !localPlayer->gunGameImmunity() && (freezeEnd != 0.0f)) {
				std::string cmd = "buy vest;";
				interfaces->engine->clientCmdUnrestricted(cmd.c_str());
			}


		}
		*/

	}




	void Run(UserCmd* cmd) {
#ifndef WALKBOT_BUILD
		if (!config->walkbot.enabled) {
			Walkbot::WBot.TargetedEntity = 0;
			return;
		}
#endif

		if (cmd->buttons & UserCmd::IN_ATTACK)
			return;

		if (!localPlayer.get() || localPlayer->isDormant()) {
			Walkbot::WBot.TargetedEntity = 0;
			boughtthislife = false;
			Debug::QuickPrint("No LP");
			return;
		}

		if (!localPlayer->isAlive()) {
			boughtthislife = false;
			Walkbot::WBot.TargetedEntity = localPlayer.get();
		}

		if (!WBot.Map.Initialized) {// https://pastebin.com/
			if (NavMesh::LoadCurrentMapNav(WBot.Map, "Walkbot::Run")) {
				Debug::QuickPrint("Load Map Error");
				return;
			}
			WBot.MapPaths = NavMesh::GetCurrentMapPathPositions(WBot.Map);
		}
		else {
			if (NavMesh::SameMap(WBot.Map)) {
				WBot.Map.Initialized = false;
				Debug::QuickPrint("Sampe Map");
				return;
			}
		}

		if (!interfaces->engine->isInGame() || !interfaces->engine->isConnected()) {
			//Debug::QuickPrint("Not In Game Return");
			if (localPlayer.get()) {
				Walkbot::WBot.TargetedEntity = localPlayer.get();
			}
			else {
				Walkbot::WBot.TargetedEntity = nullptr;
			}
			return;
		}

//#ifdef WALKBOT_BUILD
		if(localPlayer.get() && localPlayer->isAlive() && !localPlayer->isDormant())
			SetupBuy();
//#endif




		entitylistculled->cullEntities();
		SetupBotSettings(WBot);
		Vector PrevAng = cmd->viewangles;


		if ((!(WBot.CurrentAct == WalkbotActs::PlayerLink)) && !(WBot.CurrentAct == WalkbotActs::FindPlayerToLink)){//|| ((interfaces->cvar->findVar("game_type")->getInt() == 1) && (interfaces->cvar->findVar("game_mode")->getInt() == 2))) { // || interfaces->cvar->findVar("game_type");																										
			Walkbot::WBot.TargetSys->SetTarget();
		}


		//if (!(WBot.CurrentAct == WalkbotActs::FindPlayerToLink)) {
		//	WBot.CurrentAct = WalkbotActs::PlayerLink;
		//}



		


		
		/*
		if (WalkbotNetworking::NDMutex.try_lock_shared()) {
			if (WalkbotNetworking::NetworkData.shouldRead) {
				Debug::QuickPrint("Reading From Data Container");
				WBot.Status = (Walkbot::WalkbotStatus)WalkbotNetworking::NetworkData.Status;
				WBot.CurrentAct = (Walkbot::WalkbotActs)WalkbotNetworking::NetworkData.Act;
				if (WalkbotNetworking::NetworkData.hasPos) {
					Debug::QuickPrint("Setting Position");
					WBot.GoalPoint = WalkbotNetworking::NetworkData.Pos;
					WBot.CurrentPath.clear();
					WBot.MainPath.clear();
				}
				WalkbotNetworking::NDMutex.unlock_shared();
				if (WalkbotNetworking::NDMutex.try_lock()) {
					Debug::QuickPrint("Setting shouldRead and hasPos");
					WalkbotNetworking::NetworkData.shouldRead = false;
					WalkbotNetworking::NetworkData.hasPos = false;
					WalkbotNetworking::NDMutex.unlock();
				}
				else {
					Debug::QuickPrint("Unable To Acquire WalkbotNetworking::NDMutex Lock (try_lock)");
				}

			} else {
				//Debug::QuickPrint("No Data To Read");
				WalkbotNetworking::NDMutex.unlock_shared();
			}

		}
		else {
			Debug::QuickPrint("Unable To Acquire WalkbotNetworking::NDMutex Shared Lock (try_lock_shared)");
		}
		*/
		



		if(localPlayer && localPlayer.get() && localPlayer->isAlive())
			WalkbotAimbot::Run(Walkbot::WBot, cmd);

		if (Walkbot::WBot.ShouldReturn) {
			//Debug::QuickPrint("Exiting Due tO Should Return");
			WBot.Status = InEngagement;
			Walkbot::WBot.CurrentPath.clear();
			ClampViewAngles(PrevAng, cmd); // You Can Never Be too careful
			return;
		}

		//if ((!(WBot.LinkEntity) && (!(WBot.CurrentAct == WalkbotActs::PlayerLink)) )){//&& (!(WBot.Status == WalkbotStatus::InEngagement)))) {
		//	WBot.CurrentAct = WalkbotActs::FindPlayerToLink;
		//	WBot.Status = ToPosition;
		//}

		if (DoActivities(cmd, WBot)) {
			//Debug::QuickPrint("Do Activites Exit");
			return;
		}


		ClampViewAngles(PrevAng, cmd);
		interfaces->engine->setViewAngles(cmd->viewangles);

		if ((WBot.Settings.WalkSmoothing > 1) && (WBot.UnlockX || !(localPlayer->flags() & ONGROUND))) {
			SetupMovement(cmd, WBot.NextPosition, WBot.forwardMove, WBot.sideMove, WBot.Settings.WalkSmoothing/2);
		}
		else {
			SetupMovement(cmd, WBot.NextPosition, WBot.forwardMove, WBot.sideMove, WBot.Settings.WalkSmoothing);
		}

		static auto wasLastTimeOnGround{ localPlayer->flags() & 1 };
		if (config->walkbot.bunnyhop) {
			if ((localPlayer->velocity().length2D() > 215.f) || ((localPlayer->velocity().length2D() > 80.f) && (localPlayer->flags() & ONGROUND) && !wasLastTimeOnGround)) {
				cmd->buttons |= UserCmd::IN_JUMP;
				Misc::bunnyHop(cmd);
				Misc::autoStrafe(cmd);
			}
		}
		wasLastTimeOnGround = localPlayer->flags() & 1;


		//Debug::QuickPrint("Natural Return");


	}

	
}
