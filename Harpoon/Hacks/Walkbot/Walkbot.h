#pragma once
#include "../../SDK/SDKAddition/NavFile.h"
#include "../../SDK/OsirisSDK/UserCmd.h"
#include "../../SDK/OsirisSDK/Vector.h"




namespace Walkbot {





	enum WalkbotFlags {
		FollowTeamMates = 0x00000001,
		FollowBomb      = 0x00000010,
		UseGrenades     = 0x00000100, // Experiemental
		TauntInChat     = 0x00001000,
		ShouldBuy       = 0x00010000,
		ShouldBHOP      = 0x00100000,
		InspectWeapon   = 0x01000000,
		HeadAimOnly     = 0x10000000,
	};


	enum WalkbotActs {
		FollowPath,
		Hide,	
		Defuse,
		Plant,
		HeadToPoint,
		PlayerLink,
		FindPlayerToLink,
	};


	enum WalkbotStatus {
		InEngagement,
		CompletingObjective,
		ActiveStandby,
		IdleStandBy,
		Idle,
		ToPosition,
	};

	enum WalkbotPathFindingStatus {
		CalculatingPath,
		InPath,
		OutOfNavBounds,
		NoPath,
	};

	struct weights {
		float Plant{ 0.f };
		float Defuse{ 0.f };
		float AttackEnemy{ 0.f };
	};

	struct AimbotSettings {
		float FOV = 80.f;
		float AimSmoothing = 10.f;
		float hitchance = 40.f;
		bool VariableSmoothing = false;
		bool ignoreHead = false;

	};


	struct WalkbotParameters {
		int Aggression = 1; 
		int GoalOrientated = 1;

		WalkbotFlags Flags;


		float LookSmoothing = 1.f;
		float WalkSmoothing = 1.f;
		bool BBBox = false;
		AimbotSettings AimbotParams;


	};

	/*
	Its all random number generators...
	0-10

	Aggression = Liklihood of tracking down and murdering enemies
	GoalOrientated = Liklihood of choosing to go and defuse/plant

	Flags

	FollowTeamMates = What do you think this does?

	Additional Settings




	*/

	class TargetingSystem;
	class WalkbotData {
	public:
		WalkbotData();
		~WalkbotData();
		bool ShouldReturn = false;
		NavMesh::MapInfo Map;
		std::vector<nav_mesh::vec3_t> CurrentPath;
		std::vector<int> MainPath;
		NavMesh::NavPathPositions MapPaths;
		Vector GoalPoint = { 0,0,0 };
		WalkbotParameters Settings;
		WalkbotActs CurrentAct = FollowPath;
		WalkbotStatus Status = Idle;
		WalkbotPathFindingStatus PathingStatus = NoPath;
		Vector NextPosition = { 0,0,0 };
		bool UnlockX = false;
		int TicksStuck = 0;
		int cmdsSinceSeen = 0;

		int forwardMove = 0;
		int sideMove = 0;

		Entity* TargetedEntity;

		int LinkEntity = 0;


		TargetingSystem* TargetSys;

	};


	inline WalkbotData WBot;


	void Run(UserCmd* cmd);

	static bool SetupPath(WalkbotData& Bot);


}
