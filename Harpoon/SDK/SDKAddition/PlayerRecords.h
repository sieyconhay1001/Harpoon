/* Since I keep So Many Records Of Players for different features, I've decided to Condense Them */



#include "../../Hacks/RageBot/Resolver.h"
#include "../../Hacks/Grief.h"
#include "../../Hacks/Backtrack.h"
#include "../../Other/Animations.h"
#include "../../GameData.h"


namespace RecordKeeper {
	struct GeneralInfo {
		Vector LastVelocity;


	};


	struct PlayerRecord {
		bool valid = false;
		std::uint32_t SteamID;
		Resolver::Record ResolverRecord;
		std::deque<Backtrack::Record> BacktrackRecords;
		Grief::G_Player GriefRecord;
		Animations::Players AnimationsRecord;
		
		PlayerData;
		PlayerInfo PlayerInfo;
		GeneralInfo GeneralRecords;
	};

	inline PlayerRecord invalidRecord;

	class PlayerRecords {
	public:
		PlayerRecords() { Records.reserve(65); }
		~PlayerRecords() {}

		std::vector<PlayerRecord> GetRecords() { return Records; };
		//void Update();
		//PlayerRecords getPlayerRecord(Entity*);
		//PlayerRecords getPlayerRecord(int Index);
	private:
		std::vector<PlayerRecord> Records;


	};

}








