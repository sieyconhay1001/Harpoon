#pragma once
#include <cstdint>

#include "Resource/GameSockets/CSteamNetworkingMessage.h"
#include "../../Resource/Protobuf/Headers/steamnetworkingsockets_messages.pb.h"
//#define RCE_EXPLOIT

class ISteamUser;
namespace SteamNetHooks {
	std::int64_t __fastcall CSteamNetworkConnectionBase__SNP_SendMessage(void* _this, void* edx, CSteamNetworkingMessage* pSendMessage, std::int64_t usecNow, bool* pbThinkImmediately);
	bool __fastcall InternalRecievedP2PSignal(void* pCSteamNetworkingSockets, void* edx, const void* pMsg, int cbMsg, ISteamNetworkingSignalingRecvContext* pContext, bool bDefaultSignaling);
	bool ConnectionP2P_ProcessSignal(void* _this, void* edx, const CMsgSteamNetworkingP2PRendezvous& msg, SteamNetworkingMicroseconds usecNow);
	int __fastcall SteamP2PHandler(void* pCSteamNetworkingSocketsSteamBase, void* edx, const char* pMsg);
#ifdef RCE_EXPLOIT


#endif
}