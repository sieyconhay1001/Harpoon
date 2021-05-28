#include "GOTVConnect.h"
#include "../../COMPILERDEFINITIONS.h"
#include "../../Memory.h"
#include "../../Interfaces.h"
#include "../../SDK/OsirisSDK/NetworkChannel.h"

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
#include "../../Resource/SteamWorks/steam/steamdatagram_tickets.h"
#include "../../Hooks.h"
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

#pragma comment(lib,"C:\\Users\\user\\source\\repos\\Harpoon\\Harpoon\\Resource\\SteamWorks\\redistributable_bin\\steam_api.dll")
#include "../../Resource/Protobuf/Headers/cstrike15_gcmessages.pb.h"


typedef void* (__thiscall* UNK2Fn)(void*, void*, int, int, char, int);
// 





#if 0

enum PeerToPeerAddressType_t
{
	P2P_STEAMID,
};



class CPeerToPeerAddress
{
public:
	CSteamID m_steamID;
	int m_steamChannel; // SteamID channel (like a port number to disambiguate multiple connections)

	PeerToPeerAddressType_t m_AddrType;
};

enum NetworkSystemAddressType_t
{
	NSAT_NETADR,
	NSAT_P2P,
	NSAT_PROXIED_GAMESERVER,	// Client proxied through Steam Datagram Transport
	NSAT_PROXIED_CLIENT,		// Client proxied through Steam Datagram Transport
};



struct ns_address
{
	netadr_tt m_adr; // ip:port and network type (NULL/IP/BROADCAST/etc).
	CPeerToPeerAddress m_steamID; // SteamID destination
	NetworkSystemAddressType_t m_AddrType;
};

int lol = sizeof(netadr_ss);
#endif


void DumpNSAddressInfo(ns_address addr) {
	Debug::QuickPrint( "       --- Dumping ns_address Info ---\n", false);
	Debug::QuickPrint(("ns_addr.m_AddrType : " + std::to_string(addr.m_AddrType) + "\n").c_str(), false);
	Debug::QuickPrint( "ns_addr.m_steamID \n", false);
	Debug::QuickPrint(("	    m_steamID.m_AddrType         : " + std::to_string(addr.m_steamID.m_AddrType) + "\n").c_str(), false);
	Debug::QuickPrint(("	    m_steamID.m_steamChannel     : " + std::to_string(addr.m_steamID.m_steamChannel) + "\n").c_str(), false);
	Debug::QuickPrint( "	    m_steamID.CSteamID \n", false);
	//Debug::QuickPrint(("	    	    m_steamID.CSteamID.AccountID : " + std::to_string(((CSteamID*)&addr)->m_steamID.m_steamID.GetAccountID()) + "\n").c_str(), false);
	Debug::QuickPrint(("ns_addr.addr \n"), false);
	Debug::QuickPrint(("	    ns_addr.m_adr.type  : " + std::to_string(addr.m_adr.type) + "\n").c_str(), false);
	Debug::QuickPrint(("        ns_addr.m_adr.ip      : " + std::to_string(*(uint32*)addr.m_adr.ip) + "\n").c_str(), false);
	Debug::QuickPrint(("        ns_addr.m_adr.port    : " + std::to_string(addr.m_adr.port) + "\n").c_str(), false);


}

typedef int(__thiscall* SCPo)(void*, const ns_address*, int, int, uint64_t, bool);
int __fastcall GOTVConnect::CBaseClientState_SendConnectPacket(void* CBaseClientState, void* edx, const ns_address* netAdrRemote, int challengeNr, int authProtocol, uint64_t unGSSteamID, bool bGSSecure) {
	Debug::QuickPrint("CBaseClientState::SendConnectPacket");
	Debug::QuickPrint(("unGSSteamID : " + std::to_string(unGSSteamID)).c_str());
	Debug::QuickPrint(("challengeNr : " + std::to_string(challengeNr)).c_str());
	Debug::QuickPrint(("authProtocol : " + std::to_string(authProtocol)).c_str());
	Debug::QuickPrint(("bGSSecure : " + std::to_string(bGSSecure)).c_str());
	DumpNSAddressInfo(*netAdrRemote);
	return ((SCPo)memory->oCBaseClientState_SendConnectPacket)(CBaseClientState, netAdrRemote, challengeNr, authProtocol, unGSSteamID, bGSSecure);
}


/*
struct GameServerChangeRequested_t
{
	enum { k_iCallback = k_iSteamFriendsCallbacks + 32 };
	char m_rgchServer[64];		// server address ("127.0.0.1:27015", "tf2.valvesoftware.com")
	char m_rgchPassword[64];	// server password, if any
};

*/

typedef int(__thiscall* OGSCRo)(void*, GameServerChangeRequested_t*);
int __fastcall GOTVConnect::CSteam3Client_OnGameServerChangeRequested(void* CSteam3Client, void* edx, void* GameServerChangeRequested)
{
	Debug::QuickPrint("CSteam3ClientOnGameServerChangeRequested");
	GameServerChangeRequested_t* pGameServerChangeRequested = reinterpret_cast<GameServerChangeRequested_t*>(GameServerChangeRequested);
	Debug::QuickPrint("GameServerChangeRequested_t");
	Debug::QuickPrint(("m_rgchServer : " + std::string(pGameServerChangeRequested->m_rgchServer)).c_str());
	Debug::QuickPrint(("m_rgchPassword : " + std::string(pGameServerChangeRequested->m_rgchPassword)).c_str());
	return ((OGSCRo)memory->CSteam3Client_OnGameServerChangeRequested)(CSteam3Client, pGameServerChangeRequested);
}
//int __fastcall SendConnectPacket(int a1_ecx, void* edx, int a3, int a4, int a5, int a6, int a7, char a8) {
//
//}

// 55 8B EC 83 EC 14 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? FF 75 08 8D 4D EC 89 45 FC E8 ? ? ? ? 8B 45 FC FF 70 0C



void GOTVConnect::ConnectToServer(void* matchInfo, void* packet) {


	return;
#if 0
	Debug::QuickPrint("GOTVConnect::ConectToServer");
	WatchableMatchInfo* pMatchInfo = reinterpret_cast<WatchableMatchInfo*>(matchInfo);
	ISteamNetworkingSockets* steamNetworkingSockets = ((ISteamNetworkingSockets * (*)())GetProcAddress(GetModuleHandleA("steamnetworkingsockets.dll"), "SteamNetworkingSockets_Lib"))();
	SteamNetworkingIdentity Identity;
	Identity.SetSteamID64(pMatchInfo->server_id());
	SteamDatagramRelayAuthTicket ourAuthTicked;
	Debug::QuickPrint("GOTVConnect::ConectToServer Calling FindRelayAuthTicketForServer");
	int ret = steamNetworkingSockets->FindRelayAuthTicketForServer(Identity, 3, &ourAuthTicked);
	if (true) {
		if (!ret) {
			Debug::QuickPrint("No Authentication Tick For Server Found!");
		}

		Debug::QuickPrint("GOTVConnect::ConectToServer Calling ConnectToHostedDedicatedServer");
		int ret = steamNetworkingSockets->ConnectToHostedDedicatedServer(Identity, pMatchInfo->tv_port(), 0, 0);
		Debug::QuickPrint(("Return of ConnectToHostedDedicatedServer is - " + std::to_string(ret)).c_str());
		if (ret) {
			typedef void* (__cdecl* UIComponentFn)();
			void* ecx = ((UIComponentFn)memory->CreateMatchInfoUIComponent)();
			//((UNK2Fn)memory->oUnknownGOTVFunction2)(ecx, pMatchInfo, 0, 0, 0, 0);
			//const ns_address addr;
			//CBaseClientState_SendConnectPacket(memory->clientState, 0, &addr, 3, 3, pMatchInfo->server_id(), true);
		}
	}
#endif
	//CDataGCCStrike15_v2_MatchInfo* MatchInfo;

	
	//interfaces->matchFramework->GetMatchSession();


}

/* Executes on every WatchUserInfo Thing so it's worthless */
typedef void(__thiscall* handler)(void*, void*);
void __fastcall GOTVConnect::UnknownFunction(void* ecx , void* edx , void* watchUserInfo) {
	Debug::QuickPrint("Unknown GOTV Function Called");
	return ((handler)memory->oWatchUserInfoProcess)(ecx, watchUserInfo);
}

/*  */
// 55 8B EC 81 EC ? ? ? ? 53 56 57 8B 7D 08 89 4D FC 

void* __fastcall GOTVConnect::UnknownFunction2(void* ecx, void* edx, void* a2, int a3, int a4, char a5, int a6)
{
	Debug::QuickPrint("Unknown GOTV Function 2 Called");
	return ((UNK2Fn)memory->oUnknownGOTVFunction2)(ecx, a2, a3, a4, a5, a6);
}