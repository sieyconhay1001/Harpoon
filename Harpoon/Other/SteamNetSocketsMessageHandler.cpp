#pragma once
#include "SteamNetSocketsMessageHandler.h"


#include "CrossCheatTalk.h"

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


#include "../../Resource/Protobuf/Headers/cstrike15_gcmessages.pb.h"
#include "../../Hacks/OTHER/Debug.h"
#include "../../Resource/Protobuf/Headers/steamnetworkingsockets_messages.pb.h"
//#include "../../Resource/Protobuf/Headers/steamnetworkingsockets_messages_certs.pb.h"
//#include "../../Resource/Protobuf/Headers/steamnetworkingsockets_messages_udp.pb.h"

#include "../../Resource/SteamWorks/steam/isteamnetworkingmessages.h"
#include <Windows.h>

#include <intrin.h>



#include "../Hacks/ESP.h"
#include "../Timing.h"
bool __stdcall HarpoonMessageHandler::ExploitOnMsgHandler(size_t nDataSize, const char* pMsg){


	ExploitOnMsg ExploitMsg;
	ExploitMsg.ParsePartialFromArray(pMsg, nDataSize);

	static bool bWasOn{ false };

	if (ExploitMsg.on()) {
		if (!bWasOn) {
			Timing::ExploitTiming.nNetworkedDeltaTick = memory->clientState->deltaTick;
			Timing::ExploitTiming.nExtraTicksToSimulate = ExploitMsg.speed();
			bWasOn = true;
		}
		Timing::ExploitTiming.m_bNetworkedFreeze = true;
	}

	if (!ExploitMsg.on()) {
		if (bWasOn) {
			Timing::ExploitTiming.m_bNetworkedFreeze = false;
			bWasOn = false;
		}
		memory->clientState->deltaTick = -1;
	}

	Debug::QuickWarning("HarpoonMessageHandler::ExploitOnMsgHandler Bitchass!");

	Debug::QuickPrint(std::to_string(Timing::ExploitTiming.nExtraTicksToSimulate).c_str());

	return true;
}
#include "../Other/CrossCheatTalk.h"
#include "../Hacks/SteamWorks/SteamWorks.h"
#include "../SDK/OsirisSDK/Vector.h"
#include "../SDK/OsirisSDK/matrix3x4.h"
#include "../SDK/OsirisSDK/EntityList.h"
#include "../SDK/OsirisSDK/Entity.h"
#include "../../SDK/SDKAddition/EntityListCull.hpp"
bool __stdcall HarpoonMessageHandler::OriginUpdateHandler(size_t nDataSize, const char* pMsg) {
	//Debug::QuickPrint("OriginUpdateHandler");
	ESP::VectorOriginsLock.lock();
	OriginUpdate OriginMsg;
	OriginMsg.ParsePartialFromArray(pMsg, nDataSize);
	Vector Origin;
	Origin.x = OriginMsg.x();
	Origin.y = OriginMsg.y();
	Origin.z = OriginMsg.z();

	
	int nSize = OriginMsg.hitboxes_size();

	//Debug::QuickWarning(std::to_string(nSize) + " Hitboxes Recieved From Network!");
#if 0
	const char* formatter = {
		"Origin Update : {%f, %f, %f}"
	};
	char buffer[4096];
	snprintf(buffer, 4096, formatter, Origin.x, Origin.y, Origin.z);
	Debug::QuickPrint(buffer);
#endif


	ESP::g_Denzil.m_nSteamID = OriginMsg.steamid();


	ESP::VectorOrigins.push_back(Origin);
	ESP::VectorOriginsLock.unlock();


	

	ESP::g_Denzil.m_vEyeAngles.x = OriginMsg.eyeangles().x();
	ESP::g_Denzil.m_vEyeAngles.y = OriginMsg.eyeangles().y();
	ESP::g_Denzil.m_vEyeAngles.z = OriginMsg.eyeangles().z();

	ESP::g_Denzil.m_vEyePosition.x = OriginMsg.eyeposition().x();
	ESP::g_Denzil.m_vEyePosition.y = OriginMsg.eyeposition().y();
	ESP::g_Denzil.m_vEyePosition.z = OriginMsg.eyeposition().z();


	if (nSize > 0) {
		for (int i = 0; i < 20; i++) {
			ESP::g_Denzil.Boxes[i].flRadius = 0.f;
			ESP::g_Denzil.Boxes[i].vMin = { 0.f , 0.f, 0.f };
			ESP::g_Denzil.Boxes[i].vMax = { 0.f , 0.f, 0.f };
		}
		ESP::g_Denzil.nNumberOfHitboxes = 0;
		for (int i = 0; i < nSize; i++) {
			ESP::g_Denzil.nNumberOfHitboxes++;
			HitBoxMsg hitbox = OriginMsg.hitboxes(i);
			ESP::g_Denzil.Boxes[i].flRadius = hitbox.radius();

			ESP::g_Denzil.Boxes[i].vMax.x = hitbox.maxs().x();
			ESP::g_Denzil.Boxes[i].vMax.y = hitbox.maxs().y();
			ESP::g_Denzil.Boxes[i].vMax.z = hitbox.maxs().z();

			ESP::g_Denzil.Boxes[i].vMin.x = hitbox.mins().x();
			ESP::g_Denzil.Boxes[i].vMin.y = hitbox.mins().y();
			ESP::g_Denzil.Boxes[i].vMin.z = hitbox.mins().z();

			ESP::g_Denzil.nLastMatrixUpdate = memory->globalVars->tickCount;
		}
	}


	/* GHETTO FIX */
	if (OriginMsg.has_matrix() && false) {
		for (int i = 0; i < interfaces->entityList->getHighestEntityIndex(); i++) {
			Entity* pEnt = interfaces->entityList->getEntity(i);
			if (!pEnt || pEnt->isDormant() || !pEnt->isAlive())
				continue;

			PlayerInfo PlayerInf;
			interfaces->engine->getPlayerInfo(i, PlayerInf);
			if (PlayerInf.friendsId != OriginMsg.steamid())
				continue;


			pEnt->InvalidateBoneCache();

			auto boneCache = *(int**)(pEnt + 0x290F);
			auto countBones = *(int*)(pEnt + 0x291A);


			memcpy(boneCache, OriginMsg.matrix().data(), OriginMsg.matrix().size());
			pEnt->setAbsOrigin(Origin);
			pEnt->origin() = Origin;

		}
	}


	return true;
}

#include "../Hacks/ESP.h"
#include "../SDK/OsirisSDK/EntityList.h"
#include "../SDK/OsirisSDK/Entity.h"
#include "../Config.h"
bool __stdcall HarpoonMessageHandler::SharedEspUpdateHandler(size_t nDataSize, const char* pMsg) {
	Debug::QuickPrint("SharedEspUpdateHandler");

	if (!config->esp.shared_esp.bEnabled)
		return true;

	SharedESPUpdate UpdateMsg;
	UpdateMsg.ParseFromArray(pMsg, nDataSize);

	int nEntityPackets = UpdateMsg.entinfo_size();
	for (int i = 0; i < nEntityPackets; i++) {
		EntityPacketMsg EntityMsg = UpdateMsg.entinfo(i);
		Entity* pEnt = interfaces->entityList->getEntity(EntityMsg.serverindex());
		if (!pEnt || !pEnt->isDormant() || !pEnt->isPlayer()) {
			continue;
		}
		PlayerInfo PlayerInf;
		interfaces->engine->getPlayerInfo(pEnt->index(), PlayerInf);
		if (PlayerInf.friendsId != EntityMsg.steamid()) {
			const char* formatter = {
				"EntityMsg for index %d , SteamID Does Not Match The ID We Have For Player (%d != %d)"
			};
			char buffer[4096];
			snprintf(buffer, 4096, formatter, EntityMsg.serverindex(), PlayerInf.friendsId, EntityMsg.steamid());
			Debug::QuickWarning(buffer);
			continue;
		}
		ESP::SharedESPPlayer* pEntry = &(ESP::g_arrSharedESP[pEnt->index()]);
		pEntry->m_nArmour = EntityMsg.playerarmour();
		pEntry->m_nHealth = EntityMsg.playerhealth();
		pEntry->m_nLastESPTick = memory->globalVars->tickCount;
		pEntry->m_flLastUpdateTime = memory->globalVars->realtime;
		pEntry->m_vOrigin.x = EntityMsg.x();
		pEntry->m_vOrigin.y = EntityMsg.y();
		pEntry->m_vOrigin.z = EntityMsg.z();
		Debug::QuickPrint("Setting Shared ESP Data");
	}

	return true;
}

#include "cstrike15_usermessages.pb.h"

#include "../SDK/OsirisSDK/Client.h"
#include "../SDK/SDK/ClientModeShared.h"
#include "Memory.h"

bool __stdcall HarpoonMessageHandler::HarpoonChatHandler(size_t nDataSize, const char* pMsg) {
	Debug::QuickPrint("HarpoonChatHandler");
	HarpoonChat Message;
	Message.ParseFromArray(pMsg, nDataSize);
	((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sHS%s] %s%s%s : %s", PurpleChat, WhiteChat, PurpleChat, Message.name().c_str(), WhiteChat, Message.text().c_str());
	return true;
}

