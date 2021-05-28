





//#define WalkbotNetworking 
#include <mutex>
#include <shared_mutex>
#define INTERGRITY_VAL 1991811
#include "../../SDK/OsirisSDK/Vector.h"
namespace WalkbotNetworkingStructs {

	struct TaskData {

	};


	struct OutData {

	};


	struct NetworkVector {
		NetworkVector() { ZeroMemory(this, sizeof(NetworkVector)); }
		float x, y, z;
	};

	enum Operation {
		VOIDOP,
		CommandLine,
		ClientIDSetup,
		InGame,
		GetLocation,
		IsAlive,
		GameUpdate,
		ControllerDataRequest,
		ControllerCommandRequest,
		StatusChange,
		GoToPosition,
	};


	enum ControllerOperation {
		VOIDCOP,
		JoinTOP,
		JoinCTOP,
		VoiceRecordOn,
		VoiceRecordOff,
		ChangeStatus,
		DispatchToPoint,
	};

	struct ControllerCommand {
		ControllerOperation OP;
		NetworkVector Vec;
		int EnumVar;
	};

	enum ClientIdentification {
		VOIDCI,
		Slave,
		Master,
		Controller,
		Standby
	};



	struct UpdateData {
		NetworkVector MapPosition;
		int NavID = 0;
		int Health = 0;
		int Armor = 0;
		bool isInGame = false;
	};

	struct WalkbotHeaderData {
		std::uint16_t Intergrity = 1991811;
		std::uint16_t clientID = 0;
		std::uint32_t steamID = 0;
		Operation OperationCode = VOIDOP; // Cast from Operation
		ClientIdentification ClientType = VOIDCI;
	};


	struct DataContainer {
		DataContainer() { ZeroMemory(data, (sizeof(std::uint32_t) * 8)); }
		char data[sizeof(std::uint32_t) * 8];

		/*
		std::uint32_t data1 = 0;
		std::uint32_t data2 = 0;
		std::uint32_t data3 = 0;
		std::uint32_t data4 = 0;
		std::uint32_t data5 = 0;
		std::uint32_t data6 = 0;
		std::uint32_t data7 = 0;
		std::uint32_t data8 = 0;
		*/

	};

	struct DataSet {
		WalkbotHeaderData Header;
		DataContainer Datas;
	};




}




namespace WalkbotNetworking{

	enum WalkbotDesignation {
		Slave,
		Master, 
		Controller
	};


	
	struct DataShare {
		WalkbotNetworkingStructs::TaskData CurrentOperation;
		WalkbotNetworkingStructs::OutData DataOut;
		int Status;
		int Act;
		WalkbotDesignation WalkbotSpecifier;
		bool shouldRead = false;
		bool hasPos = false;
		Vector Pos;
	};

	extern std::shared_mutex NDMutex;
	extern DataShare NetworkData;

    /* Connects to Master Server*/

	struct WalkbotNetworkContainer;
	class WalkNetworkAccesor;


	class WalkbotNetworkListener {
	public:
		WalkbotNetworkListener(const char* s_IPAddress, u_short us_Port, void* Accessor);
		~WalkbotNetworkListener() {
			WSACleanup();
		}
		UINT_PTR ConnectToServer(char* s_IPAddress, u_short us_Port, bool pass = false); /* Reminder: Make this overloader, default to use member variables*/
		void MainLoop();


		// Make Private
		bool SendToServer(WalkbotNetworkingStructs::DataSet);
		bool RecvFromServer(WalkbotNetworkingStructs::DataSet&);




	private:
		void NegotiateClass();
		void UpdateServer();

		UINT_PTR Server;
		char m_sIPAddress[40]; // Maybe i'll allow IPv6 support. Hence the 39 (but 40 for null terminator) length
		u_short m_usPort;
		std::uint32_t WalkbotID;
		WalkbotNetworkingStructs::ClientIdentification OurID = WalkbotNetworkingStructs::ClientIdentification::VOIDCI;
		
	};





	class WalkNetworkAccesor {
		friend WalkbotNetworkListener;
	public:
		WalkNetworkAccesor(WalkbotNetworkContainer* OurContainer) {
			Container = OurContainer;
			return;
		}
		void JoinCTSide() {
			JoinTeam(3);
		}
		void JoinTSide() {
			JoinTeam(2);
		}
		void TurnVoiceRecordOn();
		void TurnVoiceRecordOff();

		void SendToPosition(Vector Pos);
		
		void SetStatusToIdle() {
			SendStatus(3);//Walkbot::WalkbotStatus::Idle)
		}

		void SetStatusToActiveStandby() {
			SendStatus(2);// Walkbot::WalkbotStatus::ActiveStandby);
		}

		void SetRead();
	private:
		void SendStatus(int Status);
		void JoinTeam(int Team);
		void SendCommandToServer(WalkbotNetworkingStructs::ControllerOperation Op);



		WalkbotNetworkContainer* Container;


	};

	struct WalkbotNetworkContainer {
		std::shared_mutex WNLMutex;
		std::unique_ptr<WalkNetworkAccesor> WalkbotNetworkAccess;
		std::unique_ptr<WalkbotNetworkListener> WalkbotNetwork;
	};

	inline WalkbotNetworkContainer NetworkContainer;

	void OpenConnection();

}


/*








*/