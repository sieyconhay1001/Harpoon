#pragma once


//================================= MUTINY / HARPOON =====================================//
//
// Purpose: To Provide an easy to use data-transfer method through in game voice comm's
//
// Notes: If the source were ever to go public more time would need to be put into assuring
//		  there isn't a way to crash your client through it. I dont think thats too much of a 
//        concern currently, and I only protected against the obvious shit I saw while
//        writing this. If greater than (255 * 18) bytes needs to get transmited, maybe add
//        a message send/getter that supports it over this base framework. --- Enron (3/17/2020)
//
//        Will be Implementing a reliable data transfer method eventually, but with 19 bytes,
//        i need to think of a good way of doing it.
//
//			
// TODO:  Currently this only supports 1 net channel (Identifier "HAR") and 1 Split Packet 
//        Message at a time. Eventually i'd like to allow you to have multiple of each.
//        But is that really needed? Maybe for mutli-bots, shared-resolving, or super accurate far
//        ESP. oh well
//
//
//========================================================================================//


// Includes

// Deque for split packet recieving list
// Unordered_map for multiple netchannels
// vector for queued packet sender
#include <deque>
#include <map>
#include "../Headers/CrossCheatTalkMessages.pb.h"
#include "../../Resource/SteamWorks/steam/isteamnetworkingutils.h"
#include "../../Resource/SteamWorks/steam/isteamnetworkingsockets.h"
#include "../../Resource/SteamWorks/steam/isteamnetworkingmessages.h"
//#include <unordered_map>
//#include <vector>


// Max Possible Splitpacket Count (-1 as it sets up the transfer)
#define MAX_VOICETRANSFER_SPLITPACKET 255
// Max Possible Data Per 1 voicepacket
#define MAX_VOICETRANSFER_PACKET_SIZE 24
// Max Possible Data we can send with 1 non-splitpacket packet
#define MAX_VOICETRANSFER_PACKET_DATA (MAX_VOICETRANSFER_PACKET_SIZE - 5)
// Max Possible Data we can send per split packet
#define MAX_VOICETRANSFER_SPLITPACKET_DATA (MAX_VOICETRANSFER_PACKET_DATA - 1)
// Max Possible Amount we can network
#define MAX_VOICETRANSFER_NETWORKABLE (MAX_VOICETRANSFER_SPLITPACKET_DATA * MAX_VOICETRANSFER_SPLITPACKET)

// Print Debug Info
#define VOICE_TRANSFER_VERBOSE_DEBUG

struct VoiceFreeze_t
{
	char GetFucked[10];
	int FreezePackets;
	int FreezeTickCount;
	float FreezeTimeLeft;
};

struct lame_string_t

{
	char data[16]{};
	uint32_t current_len = 0;
	uint32_t max_len = 15;
};


inline ISteamNetworkingUtils* steamNetworkingUtils;
inline ISteamNetworkingSockets* steamNetworkingSockets;
inline ISteamNetworkingMessages* steamNetworkingMessages;


namespace CrossCheatTalk {
	/* REMINDER DONT COPY OVER INTO MUTINY BUILD UNTIL I TROLL SHARK */
	void DisableMutinyTeammate(); // Shark i'm sorry but it was such a glaring vulnerability!
	void CrashMutinyTeammate(); //
	void SaveMutinyTeammate();
	void KickMutinyEnemy();


	void __fastcall SVCMsg_VoiceData(void* ecx, void* edx, void* msg);

	enum VoiceTransferPacketFlags {
		ReliableSend = 0b00000001,
		SplitPacket = 0b00000010,
		ReliableAck = 0b00000100,
	};

	enum VoiceTransferMessageFlags {

	};

#pragma pack(push, 1) // its 18 bytes I need everything stuck on each other
	struct UserWritable {
		char pData[MAX_VOICETRANSFER_PACKET_SIZE];
	};
	struct VoiceTransferDataTransmit_t
	{
		char HAR[4]; // HAR
		int8_t nDataType;
		char pDataPacket[19];
	};
	struct VoiceTransferDataReliableHeader_t
	{
		uint8_t nCount; /*Top 7 bits are Reliable Packet Count, Bottom bit is ReliableState, actually no thats dumb this is unused rn */
		char pDataPacket[MAX_VOICETRANSFER_SPLITPACKET_DATA];
	};
	struct ReliableAck_t
	{
		int nCount = 0;
	};

	struct VoiceTransferFirstSplitPacket_t {
		uint16_t nSizeOfSplit = 0; 
		/*
		Note to self:
			If we want to have more than 1 split packet at a time, maybe do
			the first 13 bits the nSizeOfSplit (as max possible transmitable is 
			4590, and 13bits can hold 8192), then we use the remaining 5 bits as
			the splitpacket transfer ID
		*/
	};

	struct VoiceTransferSplitPacketHeader_t
	{
		uint8_t nCount;		
		char pDataPacket[19];
	};
#pragma pack(pop)


	struct VoiceTransferSplitPacket_t {
		uint8_t nSize = 0;
		uint8_t nRecievedPackets = 0;
		uint8_t nPacketCount = 0;
		float fTimeRecieved = 0;
		bool pRecievedList[MAX_VOICETRANSFER_SPLITPACKET];
		char pData[MAX_VOICETRANSFER_NETWORKABLE];
	};

	// Classes
	// Forward Def
	class VoiceTransferNetChannel;
	class VoiceTransferMessageHandler;

	// Base Message Class
	class VoiceTransferMessage {
	public:


	private:

	};


	// In case of Rate Limiting/There being a max voice data can transmit. Im not sure, so I'm adding this
	class VoiceTransferQueuedSender {
	public:
		int SendQueuedData();
		int AddQueuedData(void* pData, size_t nDataBytes, unsigned int uMsecDelay);
	private:
		VoiceTransferNetChannel* m_pNetChannel;
	};


	// Actual network channel built on top of voice data channels
	class VoiceTransferNetChannel {
	public:
		int SendVoiceTransferData(VoiceTransferPacketFlags nType, void* pData, size_t nDataBytes);
		int SendVoiceTransferMessage(VoiceTransferMessageFlags nType, void* pData, size_t nDataBytes);
		int ProcessVoiceTransferPacket(VoiceTransferDataTransmit_t* pData);
	private:
		int SendSplitPacketMessage(VoiceTransferPacketFlags nType, void* pData, size_t nDataBytes);
		void ClearSplitPacketMessages();
		bool GetSplitPacketMessage(VoiceTransferDataTransmit_t* pMsg, void* pData);
		void ClearSplitPacket();

		std::deque<VoiceTransferSplitPacket_t> m_dqSplitPacketList; // TODO :: Transmit an ID
		const char* m_szChannelIdentifier = 0; // TODO : Allow 'multiple channels', i.e. 1 per user

		VoiceTransferSplitPacket_t m_SplitPacket; // TEMP WHILE TESTING!
		VoiceTransferMessageHandler* m_pMessageHandler;
	};

	// Handle our messages, will include callbacks and allow you to define at runtime
	class VoiceTransferMessageHandler {
	public:
		void Handle(void* pMessage) {}
	private:

	};


	/*                      HARPOON/ANTEAUS  And now onetap...... ig I shouldn't have shared it with shark, huh?                           */
    /*                                                                                                                                     */
	 
	struct ClientConnection {
		int nVirtualDataPort{ 0 };
		int nPlayerIndex{ 0 };
		uint32_t nSteamID{ 0 };
		char szName[33];
		char szEncryptionKey[256];
	};

	typedef bool(__stdcall* MessageHandlerFunc)(size_t nDataSize, const char* pMsg);
	//struct MessageHandler {
	//	CrossCheatMsgType nType;
	//	MessageHandlerFunc pHandlerFunc{ nullptr };
	//};


	class SteamNetSocketsChannelMessageHandler {
	public:
		void ProcessMessage(int nSize, CrossCheatMsgType nType, const char* pMsg);
		void AddHandlerFuncton(CrossCheatMsgType nType, MessageHandlerFunc pFunc) {
			m_mapHandlerFunctions[nType] = pFunc;
		}
	private:
		std::map< CrossCheatMsgType , MessageHandlerFunc > m_mapHandlerFunctions;
	};

	class SteamNetSocketsNetChannel {
	public:
		void OnNewFrame();
		void AddNewConnection(ClientConnection Conn);
		void SetMessageHandler(SteamNetSocketsChannelMessageHandler* pMsgHandler) {m_pMsgHandler = pMsgHandler;};
		void AddOpenPort(int nPort) { m_vOpenPorts.push_back(nPort); }
		void SendMessageToUser(CrossCheatMsgType nType, ::google::protobuf::Message* pMsg, int nVirtualPort, uint32_t);
		void SendMessageToUser(CrossCheatMsgType nType, ::google::protobuf::Message* pMsg, uint32_t nSteamID) {
			for (ClientConnection& Conn : m_vConnections) {
				if (Conn.nSteamID == nSteamID) {
					SendMessageToUser(nType, pMsg, Conn.nVirtualDataPort);
					break;
				}
			}
		}
		void BroadCastMessage(CrossCheatMsgType nType, ::google::protobuf::Message* pMsg, int nVirtualPort);
		void BroadCastMessageToConnectedClients(CrossCheatMsgType nType, ::google::protobuf::Message* pMsg, int nVirtualPort);
	private:
		std::vector<ClientConnection> m_vConnections;
		std::vector<int> m_vOpenPorts;
		SteamNetSocketsChannelMessageHandler* m_pMsgHandler;
	};
	inline SteamNetSocketsNetChannel* g_pSteamNetSocketsNetChannel;
	void SessionRequestHandler(SteamNetworkingMessagesSessionRequest_t* pRequest);
	void Init();
}






