#pragma once
#include <deque>
#include <map>
#include "../Headers/CrossCheatTalkMessages.pb.h"
//#include "../../Resource/SteamWorks/steam/isteamnetworkingutils.h"
//#include "../../Resource/SteamWorks/steam/isteamnetworkingsockets.h"
//#include "../../Resource/SteamWorks/steam/isteamnetworkingmessages.h"


namespace HarpoonMessageHandler {
	bool __stdcall ExploitOnMsgHandler(size_t nDataSize, const char* pMsg);
	bool __stdcall OriginUpdateHandler(size_t nDataSize, const char* pMsg);
	bool __stdcall SharedEspUpdateHandler(size_t nDataSize, const char* pMsg);
	bool __stdcall HarpoonChatHandler(size_t nDataSize, const char* pMsg);
}


