#pragma once

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
#include <string>
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

//#define SocketNetChannelDebug

#include "../Hacks/SteamWorks/SteamWorks.h"

#pragma pack(push, 1)
struct MsgHeader_t {
	char HARP[5];
	uint32_t nSize;
	uint32_t SteamID;
	CrossCheatMsgType nType;
	bool bVoice{ false };
};
#pragma pack(pop)


void CrossCheatTalk::SteamNetSocketsChannelMessageHandler::ProcessMessage(int nSize, CrossCheatMsgType nType, const char* pMsg) {
	MessageHandlerFunc pFunc = m_mapHandlerFunctions[nType];
	if (pFunc) {
		pFunc(nSize, pMsg);
	}
}

std::string hexDump3(const char* desc, void* addr, int len)
{
	if (len <= 0)
		return std::string("NOT VALID SIZE");

	char* buffer = (char*)malloc(((len * 20) * sizeof(char)));
	// DUMB ASS WAY TO DO THIS!!!

	if (!buffer)
		return std::string("BUFFER FAILED TO ALLOC");

	int i;
	unsigned char buff[17];
	unsigned char* pc = (unsigned char*)addr;

	char tempBuf[4096];

	// Output description if given.
	if (desc != NULL) {
		snprintf(tempBuf, 4096, "%s:\n", desc);
		strcat(buffer, tempBuf);
	}

	// Process every byte in the data.
	for (i = 0; i < len; i++) {
		// Multiple of 16 means new line (with line offset).

		if ((i % 16) == 0) {
			// Just don't print ASCII for the zeroth line.
			if (i != 0) {
				snprintf(tempBuf, 4096, "  %s\n", buff);
				strcat(buffer, tempBuf);
			}

			// Output the offset.
			snprintf(tempBuf, 4096, "  %04x ", i);
			strcat(buffer, tempBuf);
		}

		// Now the hex code for the specific character.
		snprintf(tempBuf, 4096, " %02x", pc[i]);
		strcat(buffer, tempBuf);

		// And store a printable ASCII character for later.
		if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
			buff[i % 16] = '.';
		}
		else {
			buff[i % 16] = pc[i];
		}

		buff[(i % 16) + 1] = '\0';
	}

	// Pad out last line if not exactly 16 characters.
	while ((i % 16) != 0) {
		snprintf(tempBuf, 4096, "   ");
		strcat(buffer, tempBuf);
		i++;
	}

	// And print the final ASCII bit.
	snprintf(tempBuf, 4096, "  %s\n", buff);
	strcat(buffer, tempBuf);

	std::string retStr{ buffer };
	free(buffer);
	return retStr;
}

void CrossCheatTalk::SteamNetSocketsNetChannel::OnNewFrame() {

	SteamNetworkingMessage_t* pMsg{ nullptr };
	SteamNetworkingMessage_t* pMsgArray{ nullptr };
	static float flLastPrintTime{ 0.f };
	for (int nOpenPort : m_vOpenPorts) {
		if (flLastPrintTime < memory->globalVars->realtime) {
			flLastPrintTime = memory->globalVars->realtime + 5.f;
			//Debug::QuickPrint((" [CrossCheatTalk::SteamNetSocketsNetChannel::OnNewFrame] Parsing Messages On Port" + std::to_string(nOpenPort)).c_str());
		}
		int nMessagesRecieved = steamNetworkingMessages->ReceiveMessagesOnChannel(nOpenPort, &pMsgArray, 1); 
		if (nMessagesRecieved > 0) {
			//Debug::QuickPrint((" [CrossCheatTalk::SteamNetSocketsNetChannel::OnNewFrame] Parsing " + std::to_string(nMessagesRecieved) + " Messages on Port " + std::to_string(nOpenPort)).c_str());
		}
		for(int i = 0; i < nMessagesRecieved; i++) {
			pMsg = &(pMsgArray[i]);
			if (!pMsg)
				continue;

			size_t nDataSize = pMsg->GetSize();
			if (nDataSize < sizeof(MsgHeader_t)) { // Obviously Not Valid
				//Debug::QuickPrint("Invalid DataSize");
				pMsg->Release();
				continue;
			}

			MsgHeader_t* pHeader = reinterpret_cast<MsgHeader_t*>((void*)pMsg->GetData());		
			if (pHeader->nSize > (nDataSize - sizeof(MsgHeader_t))) { // Malicious Client? Or Fucked up Recieving code
				//Debug::QuickPrint("Invalid pHeader->nSize");
				pMsg->Release();
				continue;
			}

			if (strcmp("HARP", pHeader->HARP)) {
				Debug::QuickPrint(" [CrossCheatTalk::SteamNetSocketsNetChannel::OnNewFrame] Message Recieved With Invalid Header!");
				pMsg->Release();
				continue;
			}

			char* pBuffer = (char*)malloc(nDataSize);
			std::memcpy(pBuffer, pMsg->GetData(), nDataSize);
			pHeader = reinterpret_cast<MsgHeader_t*>(pBuffer);

			if (!pBuffer) {
				pMsg->Release();
				continue;
			}

#ifdef SocketNetChannelDebug
			const char* formatter = {
				" [CrossCheatTalk::SteamNetSocketsNetChannel::OnNewFrame] SteamNetSocketsNetChannel Recieved Message at %f, Type : %d, Size : %d, Voice : %d"
			};
			char buffer[4096];
			snprintf(buffer, 4096, formatter, pMsg->m_usecTimeReceived, pHeader->nType, pHeader->nSize, pHeader->bVoice);
			Debug::QuickPrint(buffer);
			//Debug::QuickPrint(hexDump3(" [CrossCheatTalk::SteamNetSocketsNetChannel::OnNewFrame] Message Data: ", pBuffer, nDataSize).c_str());
#endif



			if (pHeader->bVoice) {
				//Debug::QuickPrint((" [HarpoonVoice] Recieved Voice Data at " + std::to_string(memory->globalVars->realtime) + " from " + std::to_string(pHeader->SteamID)).c_str());
				size_t nSize = 20000;
				size_t nBytesWritten = 0;
				void* pVoiceBuffer = (void*)malloc(nSize);
				while (g_pSteamUser->DecompressVoice(pBuffer, pHeader->nSize, pVoiceBuffer, nSize, &nBytesWritten, 48000) == k_EVoiceResultBufferTooSmall) {
					pVoiceBuffer = realloc(pVoiceBuffer, nBytesWritten);
					if (!pVoiceBuffer) {
						free(pVoiceBuffer);
						pMsg->Release();
					}

				}
				free(pVoiceBuffer);
			} else if (pHeader->nType == CrossCheatMsgType::k_CrossCheatInitMsg) {
				Debug::QuickPrint((" [CrossCheatTalk::SteamNetSocketsNetChannel] k_CrossCheatInitMsg Recieved From " + std::to_string(pHeader->SteamID) + " at " + std::to_string(memory->globalVars->realtime)).c_str());
				ClientConnection NewConn;
				NewConn.nSteamID = pMsg->m_identityPeer.GetSteamID().GetAccountID();
				NewConn.nVirtualDataPort = 58;
				AddNewConnection(NewConn);
			}
			else {
				m_pMsgHandler->ProcessMessage(pHeader->nSize, pHeader->nType, (const char*)(pBuffer + sizeof(MsgHeader_t)));
			}
			free(pBuffer);
			pMsg->Release();
		}
	}
}

void CrossCheatTalk::SteamNetSocketsNetChannel::SendMessageToUser(CrossCheatMsgType nType, ::google::protobuf::Message* pMsg, int nVirtualPort, uint32_t SteamID) {
	size_t nMessageSize = pMsg->ByteSize();
	size_t nTotalSize = nMessageSize + sizeof(MsgHeader_t);
	char* pBuffer = (char*)malloc(nTotalSize);
	pMsg->SerializePartialToArray((void*)(pBuffer + sizeof(MsgHeader_t)), nMessageSize);
	MsgHeader_t* pHeader = reinterpret_cast<MsgHeader_t*>(pBuffer);
	pHeader->nSize = nMessageSize;
	pHeader->nType = nType;	
	pHeader->SteamID = g_pSteamUser->GetSteamID().GetAccountID();
	pHeader->bVoice = false;
	strcpy(pHeader->HARP, "HARP");
	SteamNetworkingIdentity Iden;
	CSteamID SteamIDFull(SteamID, k_EUniversePublic, k_EAccountTypeIndividual);
	Iden.SetSteamID64(SteamIDFull.ConvertToUint64());	
	steamNetworkingMessages->SendMessageToUser(Iden, pBuffer, nTotalSize, k_nSteamNetworkingSend_ReliableNoNagle | k_nSteamNetworkingSend_AutoRestartBrokenSession, 58);
}


#include "../../SDK/SDKAddition/EntityListCull.hpp"
#include "../../SDK/OsirisSDK/Entity.h"
#include "../../GameData.h"
void CrossCheatTalk::SteamNetSocketsNetChannel::BroadCastMessage(CrossCheatMsgType nType, ::google::protobuf::Message* pMsg, int nVirtualPort) {
	//Debug::QuickPrint("[CrossCheatTalk::SteamNetSocketsNetChannel::BroadCastMessage] Broadcasting Message");
	for (EntityQuick& Entq : entitylistculled->getEntities()) {
		if (Entq.m_bisLocalPlayer || Entq.entity->isBot())
			continue;

		if (!config->debug.EnemySend && Entq.m_bisEnemy) // Currently We Don't Broadcast to Enemies, but we may want to potentially
			continue;


		PlayerInfo playerInfo;
		interfaces->engine->getPlayerInfo(Entq.index, playerInfo);
		uint32_t nSteamID = playerInfo.friendsId;
		
		// Eventually It'll make sense to set up the message beforehand
		//std::string DebugStr(" [CrossCheatTalk::SteamNetSocketsNetChannel::BroadCastMessage] Sending Message To : " + Entq.entity->getPlayerName(false) + " SteamID : " + std::to_string(nSteamID));
		//Debug::QuickPrint(DebugStr.c_str());
		SendMessageToUser(nType, pMsg, nVirtualPort, nSteamID);
	}
}

void CrossCheatTalk::SteamNetSocketsNetChannel::BroadCastMessageToConnectedClients(CrossCheatMsgType nType, ::google::protobuf::Message* pMsg, int nVirtualPort) {
	//Debug::QuickPrint("[CrossCheatTalk::SteamNetSocketsNetChannel::BroadCastMessage] Broadcasting Message");
	for (ClientConnection Conn : m_vConnections) {
		uint32_t nSteamID = Conn.nSteamID;
		//std::string DebugStr(" [CrossCheatTalk::SteamNetSocketsNetChannel::BroadCastMessage] Sending Message To SteamID : " + std::to_string(nSteamID));
		SendMessageToUser(nType, pMsg, nVirtualPort, nSteamID);
	}
}

//FnSteamNetworkingMessagesSessionRequest 
void CrossCheatTalk::SteamNetSocketsNetChannel::AddNewConnection(ClientConnection Conn) {
	m_vConnections.push_back(Conn);
}
#include "../Hacks/SteamWorks/SteamWorks.h"
void CrossCheatTalk::SessionRequestHandler(SteamNetworkingMessagesSessionRequest_t* pRequest) {
	Debug::QuickPrint((" [CrossCheatTalk::SessionRequestHandler] Session Request From " + std::to_string(pRequest->m_identityRemote.GetSteamID().GetAccountID())).c_str());
	ClientConnection NewConn;
	NewConn.nSteamID = pRequest->m_identityRemote.GetSteamID().GetAccountID();
	NewConn.nVirtualDataPort = 58;
	g_pSteamNetSocketsNetChannel->AddNewConnection(NewConn);
	g_pSteamNetSocketsNetChannel->AddOpenPort(58);
	steamNetworkingMessages->AcceptSessionWithUser(pRequest->m_identityRemote);
	CrossCheatInitMsg NewConnectionInit;
	NewConnectionInit.set_steamid(g_pSteamUser->GetSteamID().GetAccountID());
	g_pSteamNetSocketsNetChannel->SendMessageToUser(CrossCheatMsgType::k_CrossCheatInitMsg, &NewConnectionInit, NewConn.nSteamID);

	// Now Lets Call The Original Callback Func
	// E8 ? ? ? ? FF 35 ? ? ? ? B9 ? ? ? ?
	//static FnSteamNetworkingMessagesSessionRequest oCallBack = *reinterpret_cast<FnSteamNetworkingMessagesSessionRequest*>(memory->oHandleSessionRequest);
	//if (oCallBack) {
	//	Debug::QuickPrint(" [CrossCheatTalk::SessionRequestHandler] Calling oCallBack");
		//oCallBack(pRequest);
	//}
}
#include "SteamNetSocketsMessageHandler.h"



void SessionFailedHandler(SteamNetworkingMessagesSessionFailed_t* pSessionFailed) {	

	/*
		// The connection to the remote host timed out, but we
		// don't know if the problem is on our end, in the middle,
		// or on their end.
		k_ESteamNetConnectionEnd_Misc_Timeout = 5003,	
	*/

	Debug::QuickWarning((" [CrossCheatTalk::SessionFailedHandler] Session Request Failure Reason: " + std::to_string(pSessionFailed->m_info.m_eEndReason) + " SteamID : " + std::to_string(pSessionFailed->m_info.m_identityRemote.GetSteamID().GetAccountID())).c_str());
}



void CrossCheatTalk::Init() {
	g_pSteamNetSocketsNetChannel = new SteamNetSocketsNetChannel;
	SteamNetSocketsChannelMessageHandler* pNewHandler = new SteamNetSocketsChannelMessageHandler;
	pNewHandler->AddHandlerFuncton(CrossCheatMsgType::k_ExploitOnMsg, &HarpoonMessageHandler::ExploitOnMsgHandler);
	pNewHandler->AddHandlerFuncton(CrossCheatMsgType::k_OriginUpdate, &HarpoonMessageHandler::OriginUpdateHandler);
	pNewHandler->AddHandlerFuncton(CrossCheatMsgType::k_SharedESPUpdate, &HarpoonMessageHandler::SharedEspUpdateHandler);
	pNewHandler->AddHandlerFuncton(CrossCheatMsgType::k_HarpoonChat, &HarpoonMessageHandler::HarpoonChatHandler);
	g_pSteamNetSocketsNetChannel->SetMessageHandler(pNewHandler);
	g_pSteamNetSocketsNetChannel->AddOpenPort(58);
	steamNetworkingUtils->SetGlobalCallback_MessagesSessionRequest(&CrossCheatTalk::SessionRequestHandler);
	steamNetworkingUtils->SetGlobalCallback_MessagesSessionFailed(&SessionFailedHandler);

	

}
//SetGlobalCallback_MessagesSessionRequest



