#pragma once
#include <cstdint>

#include <vector>

class ISteamUser;
inline ISteamUser* g_pSteamUser;
class ISteamFriends;
inline ISteamFriends* steamFriends;
class ISteamMatchmaking;
inline ISteamMatchmaking* g_pSteamMatchmaking;
class ISteamUtils;
inline ISteamUtils* g_pSteamUtils;
class ISteamHTTP;
inline ISteamHTTP* g_pSteamHTTP;
class ISteamNetworking;
inline ISteamNetworking* g_pSteamNetworking;
class ISteamNetworkingSockets;
inline ISteamNetworkingSockets* g_pSteamNetworkingSockets;
#include "../../Other/Networking/HarpoonNetworkingSockets.h"


namespace OurSteamWorks {
	inline std::uintptr_t steam_api;
	//const auto game_coordinator = steam_client->GetISteamGenericInterface(user, pipe, STEAMGAMECOORDINATOR_INTERFACE_VERSION);

	struct Param {
		void* ecx;
		void* edx;
		uint32_t unMsgType;
		const void* pubData;
		uint32_t cubData;
	};



	inline std::vector<uint32_t> m_vIDs;

	void Init();
	int RegionPicker(Param);

	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/312446-choosing-datacenter-play.html
	int __fastcall hk_SendMessage(void* ecx, void* edx, std::uint32_t unMsgType, const void* pubData, std::uint32_t cubData);
	int __fastcall hk_RetrieveMessage(void* ecx, void* edx, uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize);
	int64_t __fastcall SNP_GetNextThinkTime(void*, void*, int64_t usecNow);
	int __fastcall CSteamNetworkConnectionBase__SNP_ClampSendRate(void* _this, void* edx);

	bool CheckPingUpToDate(bool);
	int hk_SendMessageToConnection(void* ecx, void* edx, uint32_t hConn, const void* pData, uint32_t cbData, int nSendFlags, int64_t* pOutMessageNumber);
	int SendAbandon();
	void ConnectToPlayersMatch();
	void PlayerDOS();
	__declspec(noinline) void InviteSpam();
}