#pragma once
#include "Walkbot.h"

namespace TargetingSystemDefaultStructs {

}

namespace TargetingSystemTargetHandler {
	/*
	
	Handles Specific things needed by each Target Type

	The Correct Way is to use Classes and Virtual Functions. But Alas, I decide to Reinvent the wheel

	I realise now I just have reinvented classes......


	*/


	bool DefaultReturn(void*);

	bool VisibleEntity(void*);


}



namespace TargetingSystemPrivate {


	enum GameObjectiveTypes {
		goHasBomb,
		goBombPlanted,
	};

	enum TargetTypes {
		SoundQue,
		RadarQue,
		VisibleEnt,
		GObjective,
	};



	struct TargetData {
		int AreaID;
		Vector Position;
		TargetTypes Type;
		void* Executer;
	};

	struct SoundQues : TargetData {
		SoundQues(Vector Pos) {
			Executer = TargetingSystemTargetHandler::DefaultReturn;
			Position = Pos;
			Type = SoundQue;
		}


	};

	struct RadarQues : TargetData {
		RadarQues(Vector Pos) {
			Executer = TargetingSystemTargetHandler::DefaultReturn;
			Position = Pos;
			Type = RadarQue;
		}
	};

	struct VisibleEntity : TargetData {
		VisibleEntity(Vector Pos, Entity* entity){
			Player = entity;
			Position = Pos;
			Executer = TargetingSystemTargetHandler::VisibleEntity;
			Type = VisibleEnt;
		}
		Entity* Player;
	};

	struct FriendlyMolly : TargetData {




	};


	struct GameObjective : TargetData {
		GameObjective() {
			Type = GObjective;
		}
		GameObjectiveTypes ObjectiveType;

	};


	struct PlantedBomb : GameObjective {
		PlantedBomb() {
			ObjectiveType = GameObjectiveTypes::goBombPlanted;
			Type = GObjective;
		}
	};

	struct HasBomb : GameObjective {
		HasBomb() {
			ObjectiveType = GameObjectiveTypes::goHasBomb;
			Type = GObjective;
		}

	};

	struct BombOnGround : GameObjective {

	};

	struct HostageTaken : GameObjective {

	};

	struct HasHostage : GameObjective {

	};

	struct RescueHostage : GameObjective {

	};



}
#include "../OTHER/Debug.h"
#include "../../SDK/SDKAddition/EntityListCull.hpp"
class Walkbot::TargetingSystem {
public:
	TargetingSystem(WalkbotData*);
	void SetBotOwner(Walkbot::WalkbotData* _Bot) {
		Bot = _Bot;
	}

	bool IsHasBomb() {
		for (int i = 0; i < Targets.size(); i++) { /* Remove 1 Tick Only Events*/
			if (Targets.at(i) && (Targets.at(i)->Type == TargetingSystemPrivate::GObjective)) {
				if (reinterpret_cast<TargetingSystemPrivate::GameObjective*>(Targets.at(i).get())->ObjectiveType == TargetingSystemPrivate::goHasBomb) {
					//Debug::QuickPrint("Detected As Hvving Bomb Objective");
					return true;
				}

			}
		}
		return false;
	}

	bool IsNeedingToDefuse() {
		for (int i = 0; i < Targets.size(); i++) { /* Remove 1 Tick Only Events*/
			if (Targets.at(i) && (Targets.at(i)->Type == TargetingSystemPrivate::GObjective)) {
				if (reinterpret_cast<TargetingSystemPrivate::GameObjective*>(Targets.at(i).get())->ObjectiveType == TargetingSystemPrivate::goBombPlanted) {
					//Debug::QuickPrint("Detected As Hvving Bomb Objective");
					return true;
				}

			}
		}
		return false;
	}


	bool SortTargetsByWeight() {
		return false;
	}

	void Planted() {
		for (int i = 0; i < Targets.size(); i++) { /* Remove 1 Tick Only Events*/
			if (Targets.at(i)->Type == TargetingSystemPrivate::GObjective) {
				if (reinterpret_cast<TargetingSystemPrivate::GameObjective*>(Targets.at(i).get())->ObjectiveType == TargetingSystemPrivate::goHasBomb) {
					Targets.erase(Targets.begin() + i);
					//Bot->CurrentAct = WalkbotActs::Hide;
					return Planted();
				}

			}
		}
	}
	void Defused() {
		for (int i = 0; i < Targets.size(); i++) { /* Remove 1 Tick Only Events*/
			if (Targets.at(i)->Type == TargetingSystemPrivate::GObjective) {
				if (reinterpret_cast<TargetingSystemPrivate::GameObjective*>(Targets.at(i).get())->ObjectiveType == TargetingSystemPrivate::goBombPlanted) {
					Targets.erase(Targets.begin() + i);
					//Bot->CurrentAct = WalkbotActs::Hide;
					return Defused();
				}

			}
		}
	}
	bool hasBomb() {
		for (auto nWeapon : localPlayer->weapons()) {
			Entity* pWeapon = interfaces->entityList->getEntityFromHandle(nWeapon);
			if (pWeapon && (pWeapon->getWeaponType() == WeaponType::C4)) {
				return true;
			}
		}
		return false;
	}

	bool CullTargets() {
		for (int i = 0; i < Targets.size(); i++) { /* Remove 1 Tick Only Events*/
			if (Targets.at(i)->Type == TargetingSystemPrivate::VisibleEnt) {
				Targets.at(i).release();
				Targets.erase(Targets.begin() + i);
				return CullTargets();
			}
			else if (Targets.at(i)->Type == TargetingSystemPrivate::GObjective) {
				if (reinterpret_cast<TargetingSystemPrivate::GameObjective*>(Targets.at(i).get())->ObjectiveType == TargetingSystemPrivate::goHasBomb) {
					if (localPlayer.get() && !localPlayer->hasBomb()) {
						Targets.erase(Targets.begin() + i);
						return CullTargets();
					}

				}

				if (reinterpret_cast<TargetingSystemPrivate::GameObjective*>(Targets.at(i).get())->ObjectiveType == TargetingSystemPrivate::goBombPlanted) {
					bool bombFound = true;
					for (int i = 0; i < interfaces->entityList->getHighestEntityIndex(); i++)
					{
						Entity* pEnt = interfaces->entityList->getEntity(i);
						if (!pEnt)
							continue;

						if (!pEnt->isBomb())
						{
							bombFound = false;
						} 
					}

					if (!bombFound)
					{
						Targets.erase(Targets.begin() + i);
						return CullTargets();
					}
				}

			}
		}
		Targets.shrink_to_fit();
		return true;

	}

	bool ManageTargets() {
		CullTargets();

		if (WalkbotActs::PlayerLink == Bot->CurrentAct)
			return false;


		if (IsHasBomb()) {
			Bot->CurrentAct = WalkbotActs::Plant;
			//Debug::QuickPrint("Lets Go Plant");
		}
		else if (IsNeedingToDefuse())
		{
			Bot->CurrentAct = WalkbotActs::Defuse;
		}
		else {
			Bot->CurrentAct = WalkbotActs::FollowPath;
		}
		//SortTargetsByWeight()

		return false;

	}




	bool SetTarget() {

		if (entitylistculled->getEnemies().empty())
			Bot->TargetedEntity = localPlayer.get();
		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		if (!Bot->TargetedEntity || (Bot->TargetedEntity == localPlayer.get()) || Bot->TargetedEntity->isDormant() || !Bot->TargetedEntity->isPlayer() || !Bot->TargetedEntity->isAlive() || !Bot->TargetedEntity->isVisible()) {
			for (EntityQuick entq : entitylistculled->getEnemies()) {
				if (entq.entity && entq.entity->isVisible()) {
					Targets.push_back(std::make_unique<TargetingSystemPrivate::VisibleEntity>(entq.entity->origin(), entq.entity));
					Bot->TargetedEntity = entq.entity;
				}
			}
		}


		if (!(WalkbotActs::PlayerLink == Bot->CurrentAct)) {
			if (!IsHasBomb() && localPlayer->hasBomb()) {
				//Debug::QuickPrint("Adding Bomb Objective");
				Targets.push_back(std::make_unique<TargetingSystemPrivate::HasBomb>());
			}
		}

		if (!(WalkbotActs::PlayerLink == Bot->CurrentAct) && (localPlayer->team() == 1)) {
			for (int i = 0; i < interfaces->entityList->getHighestEntityIndex(); i++)
			{
				Entity* pEnt = interfaces->entityList->getEntity(i);
				if (!pEnt)
					continue;

				if (pEnt->isBomb())
				{
					CON("We Should Be Defusing YO!\n");
					Targets.push_back(std::make_unique<TargetingSystemPrivate::PlantedBomb>());
				}
			}
		}

		ManageTargets();

		return false;

	}

	//~TargetingSystem();
protected:


	std::vector<std::unique_ptr<TargetingSystemPrivate::TargetData>> Targets;
	Walkbot::WalkbotData* Bot;
};
