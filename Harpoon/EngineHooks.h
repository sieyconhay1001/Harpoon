#pragma once
class bf_write;
struct netpacket_s;
class NetworkChannel;
class INetChannelHandler;


#include <ctype.h>

inline unsigned long long g_OurServerReservation = NULL;
inline bool corrupt_server = false;
inline int packet_index = 0;


namespace EngineHooks {

	int __fastcall NET_SendPacket(void* chan, unsigned char* data, double to, size_t length, bf_write* pVoicePayload, bool bUseCompression);
	signed int __cdecl Calls_SendToImpl(int buffer, int* socket, signed int size, int pVoice);
	void SendLargeSchlong(NetworkChannel* netchan);
	void SendCrasherData();

	typedef void(__cdecl* CL_MoveFunc)(float, bool);
	inline CL_MoveFunc oCL_Move;
	void Run_Original_CL_Move(float accumulated_extra_samples, bool bFinalTick);
	void CL_Move(float accumulated_extra_samples, bool bFinalTick);
	void Hooked_CL_Move();
	void __cdecl CL_SendMove(void* ecx, void* edx);
	int __fastcall DrawWorldLists(void* _this, void* edx, void* pList, int flags, float waterZAdjust) noexcept;
	bool __stdcall NET_BufferToBufferCompress(char* dest, unsigned int* destLen, char* source, unsigned int sourceLen);
	int __fastcall NET_SendLong(void* netchannel, DWORD* socket, int nBytesLeft, int nMaxRoutableSize);
	bool __fastcall EnqueueVeryLargeAsyncTransfer(void* netchannel, void*, NetworkMessage* msg);
	void __fastcall CNET_CompressFragments(void* ecx, void* edx);
	bool __fastcall CNET_CreateFragmentsFromBuffer(void* ecx, void* edx, void* buffer, int stream);
	bool __fastcall CNET_SendSubChannelData(void* ecx, void* edx, void* buf);
	NetworkChannel* __cdecl NET_CreateNetChannel(int socket, const void* adr, const char* name, INetChannelHandler* handler, const std::byte* pbEncryptionKey, bool bForceNewChannel);
	bool __fastcall ProcessConnectionless(void* ecx, void* edx, void* packet);


	bool __stdcall Hooked_NetReceiveRawPacket(int sock, void* buf, int len, void* from);
	inline DWORD NET_ReceiveRawPacketJMPBackAdr;

	unsigned int __stdcall FuckServer(void*);
	unsigned int __stdcall ProcessPacketsTestCrasher(void*);
	unsigned int __stdcall BlipKick(void*);
	unsigned int __stdcall ReduceServerFrameTime(void*);



	inline bool SentBlipLagPacket = false;
	int __fastcall SendDatagram(NetworkChannel* network, void* edx, bf_write* datagram);
	void __fastcall SetReservationCookie(void* clientstate, void* edx, unsigned long long cookie);
	void InitLaggers();
	void __fastcall CGameClient_GetSendFrame(void* ecx, void* edx, double unkn1, double unkn2);

	signed int __cdecl NET_SendToImpl(int a1, SOCKET a2, signed int a3, netpacket_s* a4, int a5);

	bool __stdcall CustomSendSubChannel(void* netchan, void* buffer);

	void __fastcall SendClientMessages(void*,void*, double,double,char);

	inline void ehQ_memcpy(void* dest, void* src, int count)
	{
		int  i;

		if ((((long)dest | (long)src | count) & 3) == 0)
		{
			count >>= 2;
			for (i = 0; i < count; i++)
				((int*)dest)[i] = ((int*)src)[i];
		}
		else
			for (i = 0; i < count; i++)
				((std::byte*)dest)[i] = ((std::byte*)src)[i];
	}


}