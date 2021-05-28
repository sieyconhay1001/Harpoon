#pragma once
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")

#include "../Hacks/SendNetMSG/SendNetMsg.h"
#include "../Hacks/Other/Debug.h"

#include "../Config.h"
#include "../EngineHooks.h"
#include "../ClientHooks.h"
#include "../Timing.h"
#include "../Hooks.h"
#include "../Memory.h"

#ifndef _DEBUG
#pragma comment(lib,"C:\\Users\\user\\source\\repos\\Harpoon\\Harpoon\\Resource\\Protobuf\\libprotobuf.lib")
#endif
#ifdef _DEBUG
#pragma comment(lib,"C:\\vcpkg\\installed\\x86-windows-static-md\\debug\\lib\\libprotobufd.lib")
#endif

#include "../Resource/Protobuf/Headers/cstrike15_gcmessages.pb.h"
#include "../Resource/Protobuf/Headers/netmessages.pb.h"
#include "../SDK/SDK/CNetMessageBinder.h"
#include "../SDK/SDK/Tier0/utldelegate.h"
#include "../SDK/OsirisSDK/Prediction.h"

#include "../SDK/SDK/clzss.h"
#include "../SDK/SDK/IceKey.h"
#include "MessageSender.h"
#include "CrossCheatTalk.h"
#include "../SDK/OsirisSDK/LocalPlayer.h"
#include "../SDK/OsirisSDK/Entity.h"


void send_data_msg(VoiceDataCustom* data)
{
	if(!GetAsyncKeyState(VK_NUMLOCK))
		return;
	// net_showmsg CSVCMsg_VoiceData 
	// use this for debugging

	CCLCMsg_VoiceData_Legacy msg;
	memset(&msg, 0, sizeof(msg));

	// call constructor ( called in CL_SendVoicePacket )
	// 56 57 8B F9 8D 4F 08 C7 07 ? ? ? ? E8 ? ? ? ? C7
	// _DWORD *__thiscall ConstructVoiceMessage(int ecx)

	static DWORD m_construct_voice_message = (DWORD)memory->findPattern_ex(L"engine", "\x56\x57\x8B\xF9\x8D\x4F\x08\xC7\x07????\xE8????\xC7");

	auto func = (uint32_t(__fastcall*)(void*, void*))m_construct_voice_message;
	func((void*)&msg, nullptr);

	// set our data
	msg.set_data(data);

	// mad!
	lame_string_t lame_string;

	// set rest
	msg.data = &lame_string;
	msg.format = 0; // VoiceFormat_Steam
	msg.flags = 63; // all flags!

	// send it
	interfaces->engine->getNetworkChannel()->SendNetMsg((NetworkMessage*)&msg, false, true);

	Debug::QuickPrint("Sending Voice Message!!!!!");

}

std::string hexDump2(const char* desc, void* addr, int len)
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




#pragma pack(push, 1)
struct OneTapSharedESP {
	char kcuf[4];
	uint8_t UserID;
	uint8_t Counter;
	int16_t x;
	int16_t y;
	int16_t z;
};
#pragma pack(pop, 1)


#if 0
public static short Encode(double value) {
	int cnt = 0;
	while (value != Math.Floor(value)) {
		value *= 10.0;
		cnt++;
	}
	return (short)((cnt << 12) + (int)value);
}
#endif

#if 0
static double Decode(short value) {
	int cnt = value >> 12;
	double result = value & 0xfff;
	while (cnt > 0) {
		result /= 10.0;
		cnt--;
	}
	return result;
}
#endif


static double Decode(short value) {
	return static_cast<double>(value / 100.0);
}


Vector Unpack(byte b) {
	Vector v;
	v.x = ((b & 0x30) >> 4) - 1;    // 0x30 == 0011 0000
	v.y = ((b & 0xC) >> 2) - 1;     // 0xC == 0000 1100
	v.z = (b & 0x3) - 1;     // 0x3 == 0000 0011
	return v;
}


static int nLastUserID = 0;

typedef void(__thiscall* SMVDo)(void*, void*);
void __fastcall CrossCheatTalk::SVCMsg_VoiceData(void* ecx, void* edx, void* msg)
{

	if (!msg)
		return;

	CSVCMsg_VoiceData_Legacy* m = (CSVCMsg_VoiceData_Legacy*)msg;

	// not correct format!
	//if (config->debug.dumpVoiceData)
	//	Debug::QuickPrint(hexDump2("Voice Data Dump\n", &data, sizeof(VoiceDataCustom)).c_str());

	if (m->format != 0)
	{
		//Debug::QuickPrint("Not right format");
		hooks->hclientstate.callOriginal<void, 24, void*>(msg);
		return;
	}

	VoiceDataCustom data = m->get_data();




	int sender_index = m->client + 1;

	if (!localPlayer.get()) {
		hooks->hclientstate.callOriginal<void, 24, void*>(msg);
		return;
	}

	// message from local player...
	if (localPlayer->index() == sender_index)
	{
		hooks->hclientstate.callOriginal<void, 24, void*>(msg);
		return;
	}

	// check if its empty
	if (data.section_number == 0 && data.sequence_bytes == 0 && data.uncompressed_sample_offset == 0)
	{
		hooks->hclientstate.callOriginal<void, 24, void*>(msg);
		return;
	}

	if (config->debug.dumpVoiceDataFormat /*&& !(memory->globalVars->tickCount % 20)*/) {
		/*
		struct CSVCMsg_VoiceData_Legacy
		{
			char pad_0000[8]; //0x0000
			int32_t client; //0x0008
			int32_t audible_mask; //0x000C
			uint32_t xuid_low{};
			uint32_t xuid_high{};
			void* voide_data_; //0x0018
			int32_t proximity; //0x001C
			//int32_t caster; //0x0020
			int32_t format; //0x0020
			int32_t sequence_bytes; //0x0024
			uint32_t section_number; //0x0028
			uint32_t uncompressed_sample_offset; //0x002C

			__forceinline VoiceDataCustom get_data()
			{
				VoiceDataCustom cdata;
				cdata.xuid_low = xuid_low;
				cdata.xuid_high = xuid_high;
				cdata.sequence_bytes = sequence_bytes;
				cdata.section_number = section_number;
				cdata.uncompressed_sample_offset = uncompressed_sample_offset;
				return cdata;
			}
		};
		*/
#if 0
		const char* formatter = {
			"CSVCMsg_VoiceData_Legacy : \n"
			"   client                     : %d\n"
			"   audible_mask               : %d\n"
			"   xuid_low                   : %d\n"
			"   xuid_high                  : %d\n"
			"   proximity                  : %d\n"
			"   format                     : %d\n"
			"   sequence_bytes             : %d\n"
			"   section_number             : %d\n"
			"   uncompressed_sample_offset : %d\n"
		};
#endif
		char buffer[4096 * 2];

		//snprintf(buffer, 4096 * 2, formatter, m->client, m->audible_mask, m->xuid_low, m->xuid_high, m->proximity, m->format, m->sequence_bytes, m->section_number, m->uncompressed_sample_offset);


		//std::string newStr = hexDump2("Voice Data Dump\n", &data, sizeof(VoiceDataCustom));

		//memory->conColorMsg({ 255,0,255,255 }, buffer);
		//if(newStr.c_str())
		//	memory->conColorMsg({ 255,0,255,255 }, newStr.c_str());



		const char* formatter2 = {
			"Onetap Shared ESP\n"
			"    TickCount %d \n"
			"userid  : %d\n"
			"counter : %d\n"
			"x       : %d   -   %f\n"
			"y       : %d   -   %f\n"
			"z       : %d   -   %f\n"
			"%s\n"
		};

		OneTapSharedESP* OT_ESP = reinterpret_cast<OneTapSharedESP*>(&data);

		Entity* pEnt = (Entity*)interfaces->engine->getPlayerForUserID(OT_ESP->UserID);

		if (pEnt && !(memory->globalVars->tickCount % 120)) {
			Debug::QuickWarning(pEnt->getPlayerName(false));
			//PlayerInfo pInfo;
			//interfaces->engine->getPlayerInfo(pEnt->index(), pInfo);
		}

		nLastUserID = OT_ESP->UserID;

		snprintf(buffer, 4096 * 2, formatter2, memory->globalVars->tickCount, OT_ESP->UserID, OT_ESP->Counter, OT_ESP->x, Decode(OT_ESP->x), OT_ESP->y, Decode(OT_ESP->y), OT_ESP->z, Decode(OT_ESP->z));

		memory->conColorMsg({ 255,0,255,255 }, buffer);
		//memory->conColorMsg({ 255,0,255,255 }, hexDump2("Voice Data Dump\n", m->voide_data_, m->sequence_bytes).c_str());
	}


	VoiceFreeze_t* packet = (VoiceFreeze_t*)data.get_raw_data();

	if (!strcmp(packet->GetFucked, "kcuf")) {
		Debug::QuickPrint("OT Shared ESP Packet!");
		OneTapSharedESP* OT_ESP = reinterpret_cast<OneTapSharedESP*>(&data);
		nLastUserID = OT_ESP->UserID;
	}


	// not our handshake!
#ifdef DEV_SECRET_BUILD
	if (!config->debug.DontRecieveVoice) {



		if (strcmp(packet->GetFucked, "GetFucked") && strcmp(packet->GetFucked, "HAR"))
		{
			hooks->hclientstate.callOriginal<void, 24, void*>(msg);
			return;
		}

		if (!strcmp(packet->GetFucked, "HAR")) {
			Debug::QuickPrint("Got VoiceTransfer Packet!");

		}

		if (!strcmp(packet->GetFucked, "GetFucked")) {
			Debug::QuickWarning("Mutiny User HIT THE PING KEY!");

			if (packet->FreezePackets <= 0) {
				Timing::ExploitTiming.m_bNetworkedFreeze = true;
				Timing::TimingSet_s.m_bInPredictionMode = true;
				Timing::TimingSet_s.m_bShouldPredictMore = true;
				Timing::ExploitTiming.m_fFreezeTimeLeft = 999999.f;
				//config->mmlagger.packets = packet->FreezePackets;
				config->mmlagger.ticksToSimulate = packet->FreezePackets;
			}
			else {
				Timing::ExploitTiming.m_bNetworkedFreeze = false;
				Timing::TimingSet_s.m_bInPredictionMode = false;
				Timing::TimingSet_s.m_bShouldPredictMore = false;
				Timing::ExploitTiming.m_fFreezeTimeLeft = 0;
				//config->mmlagger.packets = packet->FreezePackets;
				config->mmlagger.ticksToSimulate = packet->FreezePackets;
				memory->clientState->deltaTick = -1;
			}
		}
	}
#endif
	hooks->hclientstate.callOriginal<void, 24, void*>(msg);
}


#ifdef DEV_SECRET_BUILD
void CrossCheatTalk::CrashMutinyTeammate() { // LMAO
	   



#if 0
	if (interfaces->engine->getNetworkChannel()) {
		VoiceDataCustom data;
		const char* OTHeader = "kcuf";
		//memcpy(data.get_raw_data(), OTHeader, strlen(OTHeader) - 1);
		*(uint32_t*)data.get_raw_data() = 0x6b637566;
		//strcpy((char*)data.get_raw_data(), OTHeader);
		*(int8_t*)(data.get_raw_data() + 5) = nLastUserID;
		*(int8_t*)(data.get_raw_data() + 6) = (int8_t)(memory->globalVars->tickCount % 0xFF);
		*(int16_t*)(data.get_raw_data() + 7) = (int16_t)(std::rand() % 0xFFFF);
		*(int16_t*)(data.get_raw_data() + 9) = (int16_t)(std::rand() % 0xFFFF);
		*(int16_t*)(data.get_raw_data() + 11) = (int16_t)(std::rand() % 0xFFFF);
		send_data_msg(&data);
	}
#endif
#if 0
	if (interfaces->engine->getNetworkChannel()) {
		Debug::QuickWarning("Get Fucked");
		VoiceFreeze_t packet;
		strcpy(packet.GetFucked, "GetFucked"); // Yeah Really Tho
		packet.FreezeTickCount = memory->globalVars->tickCount;
		packet.FreezePackets = INT_MAX; // AND SHE'S GONE FOLKS!
		packet.FreezeTimeLeft = -1.0f;
		VoiceDataCustom data;
		memcpy(data.get_raw_data(), &packet, sizeof(packet));
		send_data_msg(&data);
	}
#endif
}

void CrossCheatTalk::DisableMutinyTeammate() { // LMAO
	if (interfaces->engine->getNetworkChannel()) {
		Debug::QuickWarning("Get Fucked");
		VoiceFreeze_t packet;
		strcpy(packet.GetFucked, "GetFucked"); // Yeah Really Tho
		packet.FreezeTickCount = memory->globalVars->tickCount;
		packet.FreezePackets = 50; // ZIP-ZIP-ZIP AND AWAY
		packet.FreezeTimeLeft = -1.0f;
		VoiceDataCustom data;
		memcpy(data.get_raw_data(), &packet, sizeof(packet));
		send_data_msg(&data);
	}
}

void CrossCheatTalk::SaveMutinyTeammate() { // LMAO
	if (interfaces->engine->getNetworkChannel()) {
		Debug::QuickWarning("Get Fucked");
		VoiceFreeze_t packet;
		strcpy(packet.GetFucked, "GetFucked"); 
		packet.FreezeTickCount = memory->globalVars->tickCount;
		packet.FreezePackets = 50;
		packet.FreezeTimeLeft = 0.1f; // Okay Fine its cool you can stay
		VoiceDataCustom data;
		memcpy(data.get_raw_data(), &packet, sizeof(packet));
		send_data_msg(&data);
	}
}

void CrossCheatTalk::KickMutinyEnemy() { // Coming Soon!

}


namespace CrossCheatTalk {

	int VoiceTransferNetChannel::SendVoiceTransferData(VoiceTransferPacketFlags nType, void* pData, size_t nDataBytes) {



		if (nDataBytes > MAX_VOICETRANSFER_PACKET_DATA) {
#ifdef VOICE_TRANSFER_VERBOSE_DEBUG
			Debug::QuickPrint("Passing Too Many Bytes to VoiceTransfer Message");
#endif
			return -1;
		}
		VoiceTransferDataTransmit_t packet;
		memset(&packet, 0, sizeof(VoiceTransferDataTransmit_t));
		strcpy(packet.HAR, "HAR"); // Yeah Really Tho
		memcpy(packet.pDataPacket, pData, nDataBytes);
		packet.nDataType |= nType;
		VoiceDataCustom data;
		memcpy(data.get_raw_data(), &packet, sizeof(packet));
		send_data_msg(&data);
		return 0;
	}
	// https://www.youtube.com/watch?v=jFbsypcQPm0 <--- Just a link for me to find later - Enron



	int VoiceTransferNetChannel::SendSplitPacketMessage(VoiceTransferPacketFlags nType, void* pData, size_t nDataBytes) {
#ifdef VOICE_TRANSFER_VERBOSE_DEBUG
		int nRemainder = nDataBytes % MAX_VOICETRANSFER_SPLITPACKET_DATA;
		if (nRemainder) {
			int nNeeded = MAX_VOICETRANSFER_SPLITPACKET_DATA - nRemainder;
			const char* formatter = {
				"VoiceTransfer::SendSplitPacketMessage Sending Data with nRemainder of %d, nNeeded is %d, and nDataBytes is %d"
			};
			char buffer[4096];
			snprintf(buffer, 4096, formatter, nRemainder, nNeeded, nDataBytes);
			Debug::QuickWarning(buffer);
		}
#endif


		if (nDataBytes > MAX_VOICETRANSFER_NETWORKABLE) {
#ifdef VOICE_TRANSFER_VERBOSE_DEBUG
			Debug::QuickWarning("SendVoiceTransferMessage Is Unable To SendSplitPacketMessage Send Message, nDataBytes > MAX_VOICETRANSFER_NETWORKABLE");
#endif
			return 0;
		}

		char* buffer[MAX_VOICETRANSFER_PACKET_DATA];
		memset(buffer, 0, MAX_VOICETRANSFER_PACKET_DATA);
		VoiceTransferSplitPacketHeader_t* pSplitPacket = reinterpret_cast<VoiceTransferSplitPacketHeader_t*>(buffer);

		// Send First Packet Going "HEY WE GOT A SPLIT PACKET COMING BRO!"
		pSplitPacket->nCount = 0;
		VoiceTransferFirstSplitPacket_t InitializeTransfer;
		InitializeTransfer.nSizeOfSplit = (uint8_t)nDataBytes; // Let them know how much we are sending out 
		memcpy(pSplitPacket->pDataPacket, &InitializeTransfer, sizeof(VoiceTransferFirstSplitPacket_t)); // Copy over the Transfer Initializer
		SendVoiceTransferData(VoiceTransferPacketFlags::SplitPacket, pSplitPacket, MAX_VOICETRANSFER_PACKET_DATA); // Send that out

		int nDataBytesSent = 0;
		int nDataBytesLeft = nDataBytes;
		for (int i = 0; i < MAX_VOICETRANSFER_SPLITPACKET; i++) { // Now we Send out the split packets
			pSplitPacket->nCount++;
			memset(pSplitPacket->pDataPacket, 0, MAX_VOICETRANSFER_PACKET_DATA);	// Ensure Data is Zero'd
			int nBytes = min(sizeof(MAX_VOICETRANSFER_SPLITPACKET_DATA), nDataBytesLeft);
			memcpy(pSplitPacket->pDataPacket, &pData + nBytes, MAX_VOICETRANSFER_SPLITPACKET_DATA); // Copy Data Chunck
			SendVoiceTransferData(VoiceTransferPacketFlags::SplitPacket, pSplitPacket, nBytes); // Send Data Chunk
			nDataBytesLeft -= nBytes;
			nDataBytesSent += nBytes;
		}
		return nDataBytesSent;
	}


	int VoiceTransferNetChannel::SendVoiceTransferMessage(VoiceTransferMessageFlags nType, void* pData, size_t nDataBytes) {

		return 0;
	}

#ifdef EXTENDED // Not uses for now
	void VoiceTransferNetChannel::ClearSplitPacketMessages() {
		while (!m_dqSplitPacketList.empty()) {
			free(m_dqSplitPacketList.back().pData);
			free(m_dqSplitPacketList.back().pRecievedList);
		}
	}
#endif

	void VoiceTransferNetChannel::ClearSplitPacket() {

		memset(m_SplitPacket.pData, 0, sizeof(m_SplitPacket.pData));
		memset(m_SplitPacket.pRecievedList, 0, sizeof(m_SplitPacket.pRecievedList));
		m_SplitPacket.fTimeRecieved = 0;
		m_SplitPacket.nSize = 0;
		m_SplitPacket.nPacketCount = 0;
		m_SplitPacket.nRecievedPackets = 0;
	}

	bool VoiceTransferNetChannel::GetSplitPacketMessage(VoiceTransferDataTransmit_t* pMsg, void* pData) {

		VoiceTransferSplitPacketHeader_t* pHeader = reinterpret_cast<VoiceTransferSplitPacketHeader_t*>(pMsg->pDataPacket);
		// Temp code
		if (!pHeader->nCount) {
			memset(m_SplitPacket.pData, 0, sizeof(m_SplitPacket.pData));
			memset(m_SplitPacket.pRecievedList, 0, sizeof(m_SplitPacket.pRecievedList));
			m_SplitPacket.fTimeRecieved = memory->globalVars->realtime;
			m_SplitPacket.nSize = reinterpret_cast<VoiceTransferFirstSplitPacket_t*>(pHeader->pDataPacket)->nSizeOfSplit;
			m_SplitPacket.nPacketCount = (m_SplitPacket.nSize / MAX_VOICETRANSFER_SPLITPACKET_DATA);
			return 0;
		}
		else {
			if (m_SplitPacket.nPacketCount < pHeader->nCount) {
#ifdef VOICE_TRANSFER_VERBOSE_DEBUG
				Debug::QuickWarning("GetSplitPacketMessage nPacketCount less than nCount! Clearing All!");
#endif	
				ClearSplitPacket();
				return 0;
			}

			if (m_SplitPacket.pRecievedList[pHeader->nCount]) {
#ifdef VOICE_TRANSFER_VERBOSE_DEBUG
				Debug::QuickWarning("GetSplitPacketMessage Recieved Duplicate Nr#! Clearing All! Possible Packet Loss!");
#endif
				ClearSplitPacket();
				return 0;
			}
			m_SplitPacket.nRecievedPackets++;
			memcpy(m_SplitPacket.pData, pHeader->pDataPacket + (pHeader->nCount * MAX_VOICETRANSFER_SPLITPACKET_DATA), MAX_VOICETRANSFER_SPLITPACKET_DATA);
			if (m_SplitPacket.nPacketCount == m_SplitPacket.nRecievedPackets) {
				pData = malloc(m_SplitPacket.nSize);
				memcpy(pData, m_SplitPacket.pData, m_SplitPacket.nSize);
				ClearSplitPacket();
				return true;
			}
			return 0;
		}
	}

	int VoiceTransferNetChannel::ProcessVoiceTransferPacket(VoiceTransferDataTransmit_t* pMsg) {
		if (pMsg->nDataType | VoiceTransferPacketFlags::SplitPacket) {
			void* pData = NULL;
			if (GetSplitPacketMessage(pMsg, pData)) {
				m_pMessageHandler->Handle(pData);
			}
		}
	}
}
#endif