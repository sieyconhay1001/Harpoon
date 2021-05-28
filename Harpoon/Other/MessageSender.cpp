
#pragma once
//#include <ws2tcpip.h>
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
#define MAX_CMD_BUFFER 4000

#define BITS_PER_INT		32
#define TICK_INTERVAL			(memory->globalVars->intervalPerTick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define ENCODE_PAD_BITS( x ) ( ( x << 5 ) & 0xff )
#define DECODE_PAD_BITS( x ) ( ( x >> 5 ) & 0xff )
#define PAD_NUMBER(number, boundary) \
	( ((number) + ((boundary)-1)) / (boundary) ) * (boundary)

#include "MessageSender.h"
inline int GetBitForBitnum(int bitNum)
{
	static int bitsForBitnum[] =
	{
		(1 << 0),
		(1 << 1),
		(1 << 2),
		(1 << 3),
		(1 << 4),
		(1 << 5),
		(1 << 6),
		(1 << 7),
		(1 << 8),
		(1 << 9),
		(1 << 10),
		(1 << 11),
		(1 << 12),
		(1 << 13),
		(1 << 14),
		(1 << 15),
		(1 << 16),
		(1 << 17),
		(1 << 18),
		(1 << 19),
		(1 << 20),
		(1 << 21),
		(1 << 22),
		(1 << 23),
		(1 << 24),
		(1 << 25),
		(1 << 26),
		(1 << 27),
		(1 << 28),
		(1 << 29),
		(1 << 30),
		(1 << 31),
	};

	return bitsForBitnum[(bitNum) & (BITS_PER_INT - 1)];
}


#if 1

subChannel_s* GetFreeSubChannel(NetworkChannel* pNetChannel)
{
	for (int i = 0; i < MAX_SUBCHANNELS; i++)
	{
		//if (pNetChannel->m_SubChannels[i].state == SUBCHANNEL_FREE)
		//	return &m_SubChannels[i];
	}

	return NULL;
}

#if 0
template <class PB_OBJECT_TYPE>
bool MessageSenderHelpers::WriteProtobufToBuffer(PB_OBJECT_TYPE* Proto, bf_write& buffer, uint32_t MessageType){
	if (!Proto->IsInitialized())
	{
		//Msg("WriteToBuffer Message %s is not initialized! Probably missing required fields!\n", PB_OBJECT_TYPE::GetTypeName().c_str());
	}

	int size = Proto->ByteSize();

	// If the write is byte aligned we can go direct
	if ((buffer.GetNumBitsWritten() % 8) == 0)
	{
		int sizeWithHeader = size + 1 + buffer.ByteSizeVarInt32(MessageType) + buffer.ByteSizeVarInt32(size);

		if (buffer.GetNumBytesLeft() >= sizeWithHeader)
		{
			buffer.WriteVarInt32(MessageType);
			buffer.WriteVarInt32(size);

			if (!Proto->SerializeWithCachedSizesToArray((google::protobuf::uint8*)buffer.GetData() + buffer.GetNumBytesWritten()))
			{
				return false;
			}

			// Tell the buffer we just splatted into it
			buffer.SeekToBit(buffer.GetNumBitsWritten() + (size * 8));
			return true;
		}

		// Won't fit
		Debug::QuickPrint("Message Wont Fit In Buffer!");
		return false;
	}

	// otherwise we have to do a temp allocation so we can write it all shifted

	Debug::QuickWarning("unaligned write of protobuf message");


	void* serializeBuffer = alloca(size);

	if (!Proto->SerializeWithCachedSizesToArray((google::protobuf::uint8*)serializeBuffer))
	{
		Debug::QuickPrint("Message Write Failed!");
		return false;
	}

	buffer.WriteVarInt32(MessageType);
	buffer.WriteVarInt32(size);
	return buffer.WriteBytes(serializeBuffer, size);
}
#endif
template <class PB_OBJECT_TYPE>
bool MessageSenderHelpers::WriteProtobufToBuffer(PB_OBJECT_TYPE* Proto, bf_write* buffer, uint32_t MessageType) {
	if (!Proto->IsInitialized())
	{
		Debug::QuickPrint("WriteToBuffer Message %s is not initialized! Probably missing required fields!");
		return false;
	}

	int size = Proto->ByteSize();

	// If the write is byte aligned we can go direct
	if ((buffer->GetNumBitsWritten() % 8) == 0)
	{
		int sizeWithHeader = size + 1 + buffer->ByteSizeVarInt32(MessageType) + buffer->ByteSizeVarInt32(size);

		if (buffer->GetNumBytesLeft() >= sizeWithHeader)
		{
			buffer->WriteVarInt32(MessageType);
			buffer->WriteVarInt32(size);

			if (!Proto->SerializeWithCachedSizesToArray((google::protobuf::uint8*)buffer->GetData() + buffer->GetNumBytesWritten()))
			{
				return false;
			}

			// Tell the buffer we just splatted into it
			buffer->SeekToBit(buffer->GetNumBitsWritten() + (size * 8));
			return true;
		}

		// Won't fit
		std::string Err("Message Wont Fit In Buffer Aval: " + std::to_string(buffer->GetNumBytesLeft()) + " DataSize: " + std::to_string(sizeWithHeader)
		+ " ByteSize: " + std::to_string(size) + " BytesWritten: " + std::to_string(buffer->GetNumBytesWritten()));
		Debug::QuickPrint(Err.c_str());
		return false;
	}

	// otherwise we have to do a temp allocation so we can write it all shifted

	Debug::QuickWarning("unaligned write of protobuf message");


	void* serializeBuffer = alloca(size);

	if (!Proto->SerializeWithCachedSizesToArray((google::protobuf::uint8*)serializeBuffer))
	{
		Debug::QuickPrint("Message Write Failed!");
		return false;
	}

	buffer->WriteVarInt32(MessageType);
	buffer->WriteVarInt32(size);
	return buffer->WriteBytes(serializeBuffer, size);
}

namespace MessageSenderHelpers {

	bool ProtoWriter::WriteNetTick(void* message, bf_write* buffer) {
		CNETMsg_Tick* tick = reinterpret_cast<CNETMsg_Tick*>(message);
		return WriteProtobufToBuffer(tick, buffer, net_Tick);
	}

	bool ProtoWriter::WriteSignonState(void* message, bf_write* buffer) {
		CNETMsg_SignonState* tick = reinterpret_cast<CNETMsg_SignonState*>(message);
		return WriteProtobufToBuffer(tick, buffer, net_SignonState);
	}

	bool ProtoWriter::WriteListenEvents(void* message, bf_write* buffer) {
		CCLCMsg_ListenEvents* tick = reinterpret_cast<CCLCMsg_ListenEvents*>(message);
		return WriteProtobufToBuffer(tick, buffer, clc_ListenEvents);
	}

	bool ProtoWriter::WriteClientInfo(void* message, bf_write* buffer) {
		CCLCMsg_ClientInfo* tick = reinterpret_cast<CCLCMsg_ClientInfo*>(message);
		return WriteProtobufToBuffer(tick, buffer, clc_ClientInfo);
	}

	bool ProtoWriter::WriteLoadingProgress(void* message, bf_write* buffer) {
		CCLCMsg_LoadingProgress* tick = reinterpret_cast<CCLCMsg_LoadingProgress*>(message);
		return WriteProtobufToBuffer(tick, buffer, clc_LoadingProgress);
	}

	bool ProtoWriter::WriteHLTVReplay(void* message, bf_write* buffer) {
		CCLCMsg_HltvReplay* tick = reinterpret_cast<CCLCMsg_HltvReplay*>(message);
		return WriteProtobufToBuffer(tick, buffer, clc_HltvReplay);
	}

	bool ProtoWriter::WriteSplitScreen(void* message, bf_write* buffer) {
		CNETMsg_SplitScreenUser* tick = reinterpret_cast<CNETMsg_SplitScreenUser*>(message);
		return WriteProtobufToBuffer(tick, buffer, net_SplitScreenUser);
	}

	bool ProtoWriter::WriteFile(void* message, bf_write* buffer) {
		CNETMsg_File* tick = reinterpret_cast<CNETMsg_File*>(message);
		return WriteProtobufToBuffer(tick, buffer, net_File);
	}

	bool ProtoWriter::WriteStringCmd(void* message, bf_write* buffer) {
		CNETMsg_StringCmd* tick = reinterpret_cast<CNETMsg_StringCmd*>(message);
		return WriteProtobufToBuffer(tick, buffer, net_StringCmd);
	}

	bool ProtoWriter::WriteCLCMove(void* message, bf_write* buffer) {
		CCLCMsg_Move* tick = reinterpret_cast<CCLCMsg_Move*>(message);
		return WriteProtobufToBuffer(tick, buffer, clc_Move);
	}

	bool ProtoWriter::WriteCLCKeyValues(void* message, bf_write* buffer) {
		CCLCMsg_CmdKeyValues* tick = reinterpret_cast<CCLCMsg_CmdKeyValues*>(message);
		return WriteProtobufToBuffer(tick, buffer, clc_CmdKeyValues);
	}

}

static int SendPacket(volatile NetworkChannel* network, unsigned char* packet = NULL, int ASMSIZE = 0) {
#ifdef DEV_SECRET_BUILD
	if (!packet) {
		static unsigned char byte[1];
		packet = byte;
	}
	void* callSentToPtr = (void*)memory->Calls_SendToImpl;
	int ret;
	__asm {
		mov     eax, ASMSIZE
		push    0FFFFFFFFh
		push    eax
		lea     edx, packet
		mov		ecx, network
		call callSentToPtr
		add esp, 8
		mov ret, eax
	}
	return ret;
#endif
}
bool SendAsFragmentedData(bf_write Send, void* pData, size_t nDataSize, bool asTcp = false, bool bCompress = false) {

	bf_write buffer;
	bf_write bfwrite;
	dataFragments_t* data = NULL;

	if (!data)
	{
		int totalBytes = Bits2Bytes(nDataSize*8);

		totalBytes = PAD_NUMBER(totalBytes, 4); // align to 4 bytes boundary

		data = new dataFragments_t;
		data->bytes = 0;	// not filled yet
		data->bits = 0;
		data->buffer = new char[totalBytes];
		data->isCompressed = false;
		data->isReplayDemo = false;
		data->nUncompressedSize = 0;
		data->file = NULL; // FILESYSTEM_INVALID_HANDLE
		data->filename[0] = 0;

		bfwrite.StartWriting(data->buffer, totalBytes);

		//m_WaitingList[stream].AddToTail(data);	// that's it for now
	}

	// write new reliable data to buffer
	bfwrite.WriteBits(pData, nDataSize*8);

	// fill last bits in last byte with NOP if necessary
	int nRemainingBits = bfwrite.GetNumBitsWritten() % 8;
	if (nRemainingBits > 0 && nRemainingBits <= (8 - NETMSG_TYPE_BITS))
	{
		CNETMsg_NOP_t nop;
		nop.WriteToBuffer(bfwrite);
	}

	// update bit length
	data->bits += nDataSize*8;
	data->bytes = Bits2Bytes(data->bits);

	// check if send as stream or with snapshot
	data->asTCP = asTcp;

	// calc number of fragments needed
	data->numFragments = BYTES2FRAGMENTS(data->bytes);
	data->ackedFragments = 0;
	data->pendingFragments = 0;
	return true;
}
typedef signed int(__cdecl* Calls_SendToImplFunc2)(int* buffer, unsigned char* socket, signed int size, int pVoice);
static int callAmount = 0;
static bool inLoop = false;
unsigned char* compressPacket = NULL;
static int CUSTOM_NET_SendLong(void* chan, unsigned char* sendbuf, int sendlen, int nMaxRoutableSize, bool Encrypt = false, bool Decrypt = true)
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
#if 0
	if (Encrypt) {
		int savedsendlen = sendlen;
		if (sendlen) {
			if (Decrypt)
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
			add esp, 8
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

int OurSendPacket(NetworkChannel* pNetChannel, unsigned char* pData, int nDataBytes, void* Voice = NULL, bool bCompression = false, bool SplitPacket = false) {
	CUtlMemoryFixedGrowable< byte, NET_COMPRESSION_STACKBUF_SIZE > memCompressed(NET_COMPRESSION_STACKBUF_SIZE);
	CUtlMemoryFixedGrowable< byte, NET_COMPRESSION_STACKBUF_SIZE > memEncryptedAll(NET_COMPRESSION_STACKBUF_SIZE);
	int ret = 0;

	if (bCompression)
	{
		CLZSS lzss;
		unsigned int nCompressedLength = nDataBytes;

		memCompressed.EnsureCapacity(nDataBytes + sizeof(unsigned int));

		*(int*)memCompressed.Base() = LittleLong(NET_HEADER_FLAG_COMPRESSEDPACKET);

		byte* pOutput = lzss.CompressNoAlloc((byte*)pData, nDataBytes, memCompressed.Base() + sizeof(unsigned int), &nCompressedLength);
		if (pOutput)
		{
			pData = memCompressed.Base();
			nDataBytes = nCompressedLength + sizeof(unsigned int);
		}
	}

	if (const unsigned char* pubEncryptionKey = pNetChannel ? pNetChannel->GetChannelEncryptionKey() : NULL)
	{
		IceKey iceKey(2);
		iceKey.set(pubEncryptionKey);

		// Generate some random fudge, ICE operates on 64-bit blocks, so make sure our total size is a multiple of 8 bytes
		int numRandomFudgeBytes = RandomInt(16, 72);
		int numTotalEncryptedBytes = 1 + numRandomFudgeBytes + sizeof(int32) + nDataBytes;
		numRandomFudgeBytes += iceKey.blockSize() - (numTotalEncryptedBytes % iceKey.blockSize());
		numTotalEncryptedBytes = 1 + numRandomFudgeBytes + sizeof(int32) + nDataBytes;

		char* pchRandomFudgeBytes = (char*)malloc(numRandomFudgeBytes);
		for (int k = 0; k < numRandomFudgeBytes; ++k)
			pchRandomFudgeBytes[k] = RandomInt(16, 250);

		// Prepare the encrypted memory
		memEncryptedAll.EnsureCapacity(numTotalEncryptedBytes);
		*memEncryptedAll.Base() = numRandomFudgeBytes;
		std::memcpy(memEncryptedAll.Base() + 1, pchRandomFudgeBytes, numRandomFudgeBytes);

		int32 const numBytesWrittenWire = BigLong(nDataBytes);	// byteswap for the wire
		std::memcpy(memEncryptedAll.Base() + 1 + numRandomFudgeBytes, &numBytesWrittenWire, sizeof(numBytesWrittenWire));
		std::memcpy(memEncryptedAll.Base() + 1 + numRandomFudgeBytes + sizeof(int32), pData, nDataBytes);

		// Encrypt the message
		unsigned char* pchCryptoBuffer = (unsigned char*)malloc(iceKey.blockSize());
		for (int k = 0; k < numTotalEncryptedBytes; k += iceKey.blockSize())
		{
			iceKey.encrypt((const unsigned char*)(memEncryptedAll.Base() + k), pchCryptoBuffer);
			std::memcpy(memEncryptedAll.Base() + k, pchCryptoBuffer, iceKey.blockSize());
		}

		// Set the pointers to network out the encrypted data
		pData = memEncryptedAll.Base();
		nDataBytes = numTotalEncryptedBytes;
	}


	if (!SplitPacket) {
		if (nDataBytes > MAX_ROUTABLE_PAYLOAD) {
			Debug::QuickPrint("Attempting To Send Packet With > MAX_ROUTABLE_PAYLOAD Size and SplitPacket as false!");
			return 0;
		}
		else {
			return SendPacket(pNetChannel, pData, nDataBytes);
		}
	}
	else {
		//return CUSTOM_NET_SendLong(pNetChannel, pData, nDataBytes, 1200);
	}
}


bool SendOurMessage(NetworkChannel* pNetChannel, void* pData, size_t nDataSize, bool bAsFragmentedReliale /*= false*/, bool bCompress /*= false*/, bool bTcp /*= false*/, bool bShouldLock /*= false*/,  bool bSendAsChoked /*= false*/, int nChokedAmount/* = 1*/){
	

	if (bShouldLock) {
		float Time = memory->globalVars->realtime;
		NetworkingMutex.lock();
		if ((Time - memory->globalVars->realtime) > 1.f) {
			Debug::QuickPrint(("SendOurMessage Waited " + std::to_string(Time) + "s For a lock!").c_str());
		}
	}


	// This is dumb don't do this
	//if (pNetChannel != interfaces->engine->getNetworkChannel())
	//	return false;

	byte		send_buf[NET_MAX_MESSAGE];
	bf_write send(send_buf, NET_MAX_MESSAGE);
	send.m_iCurBit = 0;
	unsigned char flags = 0;
	//Debug::QuickPrint("212");


	//pNetChannel->OutSequenceNr++;
	send.WriteLong(pNetChannel->OutSequenceNr);
	send.WriteLong(pNetChannel->InSequenceNr);


	bf_write flagsPos = send; // remember flags byte position

	send.WriteByte(0); // write correct flags value later
	send.WriteShort(0);  // write correct checksum later
	

	int nCheckSumStart = send.GetNumBytesWritten();

	send.WriteByte(pNetChannel->InReliableState);

	if ((pNetChannel->chokedPackets > 0) && bSendAsChoked)
	{
		int nChoked = nChokedAmount ? nChokedAmount : pNetChannel->chokedPackets;
		flags |= PACKET_FLAG_CHOKED;
		send.WriteByte(pNetChannel->chokedPackets & 0xFF);	// send number of choked packets
	}

	if (bAsFragmentedReliale) { /* We Are Sending It As Reliable Data Fragments */
		flags |= PACKET_FLAG_RELIABLE;
		//SendAsFragmentedData(send, pData, nDataSize, bTcp, bCompress);
	}
	else { /* We Are Sending It As Unreliable normal ass data */
		if ((nDataSize*8) < send.GetNumBitsLeft())
		{
			send.WriteBits(pData, (nDataSize*8));
		}
		else
		{
			Debug::QuickPrint("SendOurMessage:  data would overfow, ignoring\n");
		}
	}


	//Debug::QuickPrint("249");
	int nMinRoutablePayload = 576;
	while (send.GetNumBytesWritten() < nMinRoutablePayload)
	{
		//Debug::QuickPrint("253");
		// Go ahead and pad some bits as long as needed
		CNETMsg_NOP nop;
		MessageSenderHelpers::WriteProtobufToBuffer(&nop, &send, net_NOP);

	}


	// Make sure we have enough bits to read a final net_NOP opcode before compressing 
	int nRemainingBits = send.GetNumBitsWritten() % 8;
	if (nRemainingBits > 0 && nRemainingBits <= (8 - NETMSG_TYPE_BITS))
	{
		//Debug::QuickPrint("259");
		CNETMsg_NOP nop;
		MessageSenderHelpers::WriteProtobufToBuffer(&nop, &send, net_NOP);
	}

	//Debug::QuickPrint("269");
	// if ( IsX360() )
	{
		// Now round up to byte boundary
		nRemainingBits = send.GetNumBitsWritten() % 8;
		if (nRemainingBits > 0)
		{
			int nPadBits = 8 - nRemainingBits;

			flags |= ENCODE_PAD_BITS(nPadBits);

			// Pad with ones
			if (nPadBits > 0)
			{
				unsigned int unOnes = GetBitForBitnum(nPadBits) - 1;
				//send.WriteUBitLong(unOnes, nPadBits);
			}
		}
	}
	//Debug::QuickPrint("288");
	// write correct flags value and the checksum
	flagsPos.WriteByte(flags);

	// Compute checksum (must be aligned to a byte boundary!!)
	if (true/*ShouldChecksumPackets()*/)
	{
		const void* pvData = send.GetData() + nCheckSumStart;
		int nCheckSumBytes = send.GetNumBytesWritten() - nCheckSumStart;
		unsigned short usCheckSum = BufferToShortChecksum(pvData, nCheckSumBytes);
		flagsPos.WriteUBitLong(usCheckSum, 16);
	}
	//Debug::QuickPrint("302");
	typedef int((__fastcall* NSPo)(NetworkChannel*, unsigned char*, int, void*, bool));
	static NSPo NET_SendPacket{ NULL };
	if (!NET_SendPacket) { 
		NET_SendPacket = (NSPo)memory->NET_SendPacket;
		if (memory->oNET_SendPacket) { /* Check To See If We Hooked it or not*/
			NET_SendPacket = (NSPo)memory->oNET_SendPacket;
		}
	}
	/* TODO: Rewrite NET_SendPacket So We Handle Everything Ourselves */
	Debug::QuickPrint("Sending Packet Out!");
	pNetChannel->OutSequenceNr++;
	int	bytesSent = NET_SendPacket(pNetChannel, send.GetData(), send.GetNumBytesWritten(), 0, bCompress);
	_asm add esp, 12
	//int bytesSent = OurSendPacket(pNetChannel, send.GetData(), send.GetNumBytesWritten(), 0, 0, 1);
	Debug::QuickPrint(("bytesSent = " + std::to_string(bytesSent)).c_str());


	//pNetChannel->chokedPackets = 0;

LOCK_EXIT:
	if(bShouldLock)
		NetworkingMutex.unlock();
}

#endif






