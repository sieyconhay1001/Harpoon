#include "SteamNetHooks.h"
#include "Hacks/SteamWorks/SteamWorks.h"
#pragma once
#include "COMPILERDEFINITIONS.h"

//#define STEAMNETWORKINGSOCKETS_STATIC_LINK
//#define STEAMNETWORKINGSOCKETS_FLAT
//#pragma comment(lib,"../../Resource/SteamWorks/redistributable_bin/steam_api.dll")
#include "steam/steam_api.h"
#include "steam/steam_api_common.h"
#include "steam/isteamgamecoordinator.h"
#include "steam/isteamnetworking.h"
#include "steam/isteamnetworkingutils.h"
#include "steam/isteamnetworkingsockets.h"
#include "steam/isteamnetworkingmessages.h"
#include "steam/steamnetworkingtypes.h"
#include "steam/isteamfriends.h"
#include "steam/isteamuser.h"
#include "steam/steamclientpublic.h"
#include "steam/steamclientpublic.h"
#include "Hooks.h"

#include "Other/CrossCheatTalk.h"
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
#include "Hacks/OTHER/Debug.h"
#include <Windows.h>

#include <intrin.h>
#include <map>

namespace SteamNetHelpers {
	void PrintCSteamNetworkingMessageReliableHeader(CSteamNetworkingMessage* pSendMessage){
		byte* pHeader = pSendMessage->SNPSend_ReliableHeader();


	}
}



#include "Resource/tier1/utlvector.h"
namespace vstd
{
	template< typename T, int N >
	class small_vector
	{
	public:
		small_vector() {}
		small_vector(const small_vector<T, N>& x);
		small_vector<T, N>& operator=(const small_vector<T, N>& x);
		small_vector(small_vector<T, N>&& x);
		small_vector<T, N>& operator=(small_vector<T, N>&& x);
		~small_vector() { clear(); }

		size_t size() const { return size_; }
		size_t capacity() const { return capacity_; }
		bool empty() const { return size_ == 0; }

		T* begin() { return dynamic_ ? dynamic_ : (T*)fixed_; };
		const T* begin() const { return dynamic_ ? dynamic_ : (T*)fixed_; };

		T* end() { return begin() + size_; }
		const T* end() const { return begin() + size_; }

		T& operator[](size_t index) { assert(index < size_); return begin()[index]; }
		const T& operator[](size_t index) const { assert(index < size_); return begin()[index]; }

		void push_back(const T& value);
		void pop_back();
		void erase(T* it);

		void resize(size_t n);
		void reserve(size_t n);
		void clear();
		void assign(const T* srcBegin, const T* srcEnd);

	private:
		size_t size_ = 0, capacity_ = N;
		T* dynamic_ = nullptr;
		char fixed_[N][sizeof(T)];
	};

}

struct SNPRange_t
{
	/// Byte or sequence number range
	int64 m_nBegin;
	int64 m_nEnd; // STL-style.  It's one past the end


	struct NonOverlappingLess
	{
		inline bool operator ()(const SNPRange_t& l, const SNPRange_t& r) const
		{
			if (l.m_nBegin < r.m_nBegin) return true;
			return false;
		}
	};
};

class CConnectionTransport;
struct SNPInFlightPacket_t
{
	//
	// FIXME - Could definitely pack this structure better.  And maybe
	//         worth it to optimize cache
	//

	/// Local timestamp when we sent it
	SteamNetworkingMicroseconds m_usecWhenSent;

	/// Did we get an ack block from peer that explicitly marked this
	/// packet as being skipped?  Note that we might subsequently get an
	/// an ack for this same packet, that's OK!
	bool m_bNack;

	/// Transport used to send
	CConnectionTransport* m_pTransport;

	/// List of reliable segments.  Ignoring retransmission,
	/// there really is no reason why we we would need to have
	/// more than 1 in a packet, even if there are multiple
	/// reliable messages.  If we need to retry, we might
	/// be fragmented.  But usually it will only be a few.
	vstd::small_vector<SNPRange_t, 1> m_vecReliableSegments;
};


struct SteamNetworkingMessageQueue
{
	CSteamNetworkingMessage* m_pFirst = nullptr;
	CSteamNetworkingMessage* m_pLast = nullptr;
	void* m_pRequiredLock = nullptr; // Is there a lock that is required to be held while we access this queue?
};

struct SSNPSendMessageList : public SteamNetworkingMessageQueue
{
	CSteamNetworkingMessage* pop_front()
	{
		CSteamNetworkingMessage* pResult = m_pFirst;
		if (pResult)
		{
			//Assert(m_pLast);
			//Assert(pResult->m_links.m_pQueue == this);
			//Assert(pResult->m_links.m_pPrev == nullptr);
			m_pFirst = pResult->m_links.m_pNext;
			if (m_pFirst)
			{
				//Assert(m_pFirst->m_links.m_pPrev == pResult);
				//Assert(m_pFirst->m_nMessageNumber > pResult->m_nMessageNumber);
				m_pFirst->m_links.m_pPrev = nullptr;
			}
			else
			{
				//Assert(m_pLast == pResult);
				m_pLast = nullptr;
			}
			pResult->m_links.m_pQueue = nullptr;
			pResult->m_links.m_pNext = nullptr;
		}
		return pResult;
	}

	/// Optimized insertion when we know it goes at the end
	void push_back(CSteamNetworkingMessage* pMsg)
	{
		if (m_pFirst == nullptr)
		{
			//Assert(m_pLast == nullptr);
			m_pFirst = pMsg;
		}
		else
		{
			// Messages are always kept in message number order
			//Assert(pMsg->m_nMessageNumber > m_pLast->m_nMessageNumber);
			//Assert(m_pLast->m_links.m_pNext == nullptr);
			m_pLast->m_links.m_pNext = pMsg;
		}
		pMsg->m_links.m_pQueue = this;
		pMsg->m_links.m_pNext = nullptr;
		pMsg->m_links.m_pPrev = m_pLast;
		m_pLast = pMsg;
	}


};
struct SSNPSenderState
{
	SSNPSenderState();
	~SSNPSenderState() {
		Shutdown();
	}
	void Shutdown();

	/// Nagle timer on all pending messages
	void ClearNagleTimers()
	{
		CSteamNetworkingMessage* pMsg = m_messagesQueued.m_pLast;
		while (pMsg && pMsg->SNPSend_UsecNagle())
		{
			pMsg->SNPSend_SetUsecNagle(0);
			pMsg = pMsg->m_links.m_pPrev;
		}
	}

	// Current message number, we ++ when adding a message
	int64 m_nReliableStreamPos = 1;
	int64 m_nLastSentMsgNum = 0; // Will increment to 1 with first message
	int64 m_nLastSendMsgNumReliable = 0;
	SSNPSendMessageList m_messagesQueued;
	int m_cbCurrentSendMessageSent = 0;
	SSNPSendMessageList m_unackedReliableMessages;

	// Buffered data counters.  See SteamNetworkingQuickConnectionStatus for more info
	int m_cbPendingUnreliable = 0;
	int m_cbPendingReliable = 0;
	int m_cbSentUnackedReliable = 0;
	inline int PendingBytesTotal() const { return m_cbPendingUnreliable + m_cbPendingReliable; }

	// Stats.  FIXME - move to LinkStatsEndToEnd and track rate counters
	int64 m_nMessagesSentReliable = 0;
	int64 m_nMessagesSentUnreliable = 0;
#if 1
	std::map<int64, SNPInFlightPacket_t> m_mapInFlightPacketsByPktNum;
	std::map<int64, SNPInFlightPacket_t>::iterator m_itNextInFlightPacketToTimeout;
	std::map<SNPRange_t, CSteamNetworkingMessage*, SNPRange_t::NonOverlappingLess> m_listInFlightReliableRange;
	std::map<SNPRange_t, CSteamNetworkingMessage*, SNPRange_t::NonOverlappingLess> m_listReadyRetryReliableRange;
	int64 m_nMinPktWaitingOnAck = 0;
#endif
	/// Check invariants in debug.
#if STEAMNETWORKINGSOCKETS_SNP_PARANOIA == 0 
	//inline void DebugCheckInFlightPacketMap() const {}
#else
	void DebugCheckInFlightPacketMap() const;
#endif
#if STEAMNETWORKINGSOCKETS_SNP_PARANOIA > 1
	inline void MaybeCheckInFlightPacketMap() const { DebugCheckInFlightPacketMap(); }
#else
	//inline void MaybeCheckInFlightPacketMap() const {}
#endif

};

class CSteamNetworkConnectionBase {

public:
	char pad[3504];
	SSNPSenderState m_senderState;

	virtual void APICloseConnection(int nReason, const char* pszDebug, bool bEnableLinger) = 0;
	
	virtual EResult APISendMessageToConnection(const void* pData, uint32 cbData, int nSendFlags, int64* pOutMessageNumber) = 0;

	/// Send a message.  Returns the assigned message number, or a negative EResult value
	virtual int64 APISendMessageToConnection(CSteamNetworkingMessage* pMsg, SteamNetworkingMicroseconds usecNow, bool* pbThinkImmediately = nullptr) = 0;

	/// Flush any messages queued for Nagle
	virtual EResult APIFlushMessageOnConnection() = 0;

	/// Receive the next message(s)
	virtual int APIReceiveMessages(SteamNetworkingMessage_t** ppOutMessages, int nMaxMessages) = 0;

	/// Accept a connection.  This will involve sending a message
	/// to the client, and calling ConnectionState_Connected on the connection
	/// to transition it to the connected state.
	virtual EResult APIAcceptConnection() = 0;
	virtual EResult AcceptConnection(SteamNetworkingMicroseconds usecNow) = 0;

	/// Fill in quick connection stats
	virtual void APIGetQuickConnectionStatus(SteamNetworkingQuickConnectionStatus& stats) = 0;

	/// Fill in detailed connection stats
	virtual void APIGetDetailedConnectionStatus(/*SteamNetworkingDetailedConnectionStatus& stats, SteamNetworkingMicroseconds usecNow*/) = 0;

	/// Hook to allow connections to customize message sending.
	/// (E.g. loopback.)
	virtual int64 _APISendMessageToConnection(CSteamNetworkingMessage* pMsg, SteamNetworkingMicroseconds usecNow, bool* pbThinkImmediately) = 0;
	//
	// Accessor
	//

	// Get/set user data
#if 0
	inline int64 GetUserData() const
	{
		// User data is locked when we create a connection!
		//Assert(m_connectionConfig.m_ConnectionUserData.IsSet());
		return m_connectionConfig.m_ConnectionUserData.m_data;
	}
#endif
	virtual void SetUserData(int64 nUserData) = 0;

	// Get/set name
	//inline const char* GetAppName() const { return m_szAppName; }
	virtual void SetAppName(const char* pszName) = 0;

	// Debug description
	//inline const char* GetDescription() const { return m_szDescription; }

	/// When something changes that goes into the description, call this to rebuild the description
	virtual void SetDescription() = 0;

	/// High level state of the connection
	virtual ESteamNetworkingConnectionState GetState() = 0; //const { return m_eConnectionState; }
	virtual ESteamNetworkingConnectionState GetWireState() = 0; //const { return m_eConnectionWireState; }

	/// Check if the connection is 'connected' from the perspective of the wire protocol.
	/// (The wire protocol doesn't care about local states such as linger)
	virtual bool BStateIsConnectedForWirePurposes() = 0;// const { return m_eConnectionWireState == k_ESteamNetworkingConnectionState_Connected; }

	/// Return true if the connection is still "active" in some way.
	virtual bool BStateIsActive() = 0;

	virtual ESteamNetConnectionEnd GetConnectionEndReason() = 0;
	virtual const char* GetConnectionEndDebugString() = 0;

	/// When did we enter the current state?
	//inline SteamNetworkingMicroseconds GetTimeEnteredConnectionState() const { return m_usecWhenEnteredConnectionState; }

	/// Fill in connection details
	virtual void ConnectionPopulateInfo(SteamNetConnectionInfo_t& info) = 0;

	virtual void ConnectionQueueDestroy() = 0;
	virtual void ProcessDeletionList() = 0;

	/// Free up all resources.  Close sockets, etc
	virtual void FreeResources() = 0;

	/// Nuke all transports
	virtual void DestroyTransport() = 0;

	virtual void ConnectionState_ProblemDetectedLocally(/*ESteamNetConnectionEnd eReason, PRINTF_FORMAT_STRING const char* pszFmt, ...*/) = 0;
	virtual void ConnectionState_ClosedByPeer(int nReason, const char* pszDebug) = 0;
	virtual void ConnectionState_FindingRoute(SteamNetworkingMicroseconds usecNow) = 0;
	virtual bool BConnectionState_Connecting(SteamNetworkingMicroseconds usecNow, SteamNetworkingErrMsg& errMsg) = 0;
	virtual void ConnectionState_Connected(SteamNetworkingMicroseconds usecNow) = 0;
	virtual void ConnectionState_FinWait() = 0;
	virtual void SetPollGroup(void* pPollGroup) = 0;
	virtual void RemoveFromPollGroup() = 0;
	virtual void UpdateMTUFromConfig() = 0;
	virtual void _AssertLocksHeldByCurrentThread(const char* pszFile, int line, const char* pszTag = nullptr) = 0;
	virtual bool DecryptDataChunk(/*uint16 nWireSeqNum, int cbPacketSize, const void* pChunk, int cbChunk, RecvPacketContext_t& ctx*/) = 0;
	virtual bool ProcessPlainTextDataChunk(/*int usecTimeSinceLast, RecvPacketContext_t& ctx*/) = 0;


};


struct SendPacketContext_t
{
	inline SendPacketContext_t(SteamNetworkingMicroseconds usecNow, const char* pszReason) : m_usecNow(usecNow), m_pszReason(pszReason) {}
	const SteamNetworkingMicroseconds m_usecNow;
	int m_cbMaxEncryptedPayload;
	const char* m_pszReason; // Why are we sending this packet?
};

template <typename T>
inline byte* SerializeVarInt(byte* p, T x)
{
	while (x >= (unsigned)0x80) // if you get a warning, it's because you are using a signed type!  Don't use this for signed data!
	{
		// Truncate to 7 bits, and turn on the high bit, and write it.
		*(p++) = byte(x | 0x80);

		// Move on to the next higher order bits.
		x >>= 7U;
	}
	*p = x;
	return p + 1;
}

/// Serialize a bar int, but return null if we want to go past the end
template <typename T>
inline byte* SerializeVarInt(byte* p, T x, const byte* pEnd)
{
	while (x >= (unsigned)0x80) // if you get a warning, it's because you are using a signed type!  Don't use this for signed data!
	{
		if (p >= pEnd)
			return nullptr;

		// Truncate to 7 bits, and turn on the high bit, and write it.
		*(p++) = byte(x | 0x80);

		// Move on to the next higher order bits.
		x >>= 7U;
	}
	if (p >= pEnd)
		return nullptr;
	*p = x;
	return p + 1;
}

#include "Memory.h"


#if 0
const char* SendStateFormatter = {
"pBaseConn->m_senderstate    \n"
"m_nReliableStreamPos       = %d\n"
"m_nLastSentMsgNum          = %d\n"
"m_nLastSendMsgNumReliable  = %d\n"
"m_cbCurrentSendMessageSent = %d\n"
"m_cbPendingUnreliable      = %d\n"
"m_cbPendingReliable        = %d\n"
"m_cbSentUnackedReliable    = %d\n"
"m_nMessagesSentReliable    = %d\n"
"m_nMessagesSentUnreliable  = %d\n"
"m_nMinPktWaitingOnAck      = %d\n"
};

char buffer[4096 * 2];


snprintf(buffer, 4096 * 2, SendStateFormatter,
	m_senderState->m_nReliableStreamPos,
	m_senderState->m_nLastSentMsgNum,
	m_senderState->m_nLastSendMsgNumReliable,
	m_senderState->m_cbCurrentSendMessageSent,
	m_senderState->m_cbPendingUnreliable,
	m_senderState->m_cbPendingReliable,
	m_senderState->m_cbSentUnackedReliable,
	m_senderState->m_nMessagesSentReliable,
	m_senderState->m_nMessagesSentUnreliable,
	m_senderState->m_nMinPktWaitingOnAck);

memory->conColorMsg({ 255,0,255,255 }, buffer);
#endif


#if 0
if (config->debug.SteamNetworkingDebug) {
	int cbData = pSendMessage->m_nCbSize;
	const char* szFormatter = {
		"CSteamNetworkConnectionBase::SNP_SendMessage  \n"
		"   pSendMessage->m_nCbSize : %d\n"
		"   pSendMessage->m_nFlags : %d\n"
		"   pSendMessage->m_cbSNPSendReliableHeader : %d\n"
	};
	char pBuffer[4096];
	snprintf(pBuffer, 4096, szFormatter, pSendMessage->m_nCbSize, pSendMessage->m_nFlags, pSendMessage->m_cbSNPSendReliableHeader);
	memory->conColorMsg({ 255,0,255,255 }, pBuffer);
}
#endif


#if 0
if (GetAsyncKeyState(VK_NUMLOCK)) {
	pSendMessage->m_nFlags |= k_nSteamNetworkingSend_Reliable;
	pSendMessage->m_nMessageNumber = 0;
	int nMessageNumSaved = 0;

	int* nLastSentMsgNum = (int*)&(((DWORD*)_this)[3508]);

	for (int i = 0; i < 5000; i++) {
		*nLastSentMsgNum = nMessageNumSaved + (std::rand() % (1000000 - 2000));
		if (i % 2)
			*nLastSentMsgNum = 0;
		((SNP_SendMessageFunc)memory->oSNP_SendMessage)(_this, pSendMessage, INT_MAX, pbThinkImmediately);
		// This stopped being able to crash servers on the early april update of steamnetworkingsockets. I don't know why... no code seems to have changed relevant to this?
	}
}
#endif


const SteamNetworkingMicroseconds k_nThinkTime_Never = INT64_MAX;
const SteamNetworkingMicroseconds k_nThinkTime_ASAP = 1;
typedef void(__thiscall* ThinkFunc)(CSteamNetworkConnectionBase*, std::int64_t);
typedef int64_t(__thiscall* SNPNextThink)(CSteamNetworkConnectionBase*, int64_t usecNow);
typedef int64(__thiscall* SNP_SendMessageFunc)(void*, CSteamNetworkingMessage*, std::int64_t, bool*);

int64_t __fastcall SteamNetHooks::CSteamNetworkConnectionBase__SNP_SendMessage(void* _this, void* edx, CSteamNetworkingMessage* pSendMessage, std::int64_t usecNow, bool* pbThinkImmediately) {

	CSteamNetworkConnectionBase* pBaseConn = reinterpret_cast<CSteamNetworkConnectionBase*>(_this);
	SSNPSenderState* m_senderState =  reinterpret_cast<SSNPSenderState*>(pBaseConn + 3504); //&(pBaseConn->m_senderState);//



#if 1



	return ((SNP_SendMessageFunc)memory->oSNP_SendMessage)(_this, pSendMessage, INT_MAX, pbThinkImmediately);
#endif


	// Connection must be locked, but we don't require the global lock here!
	// E8 ? ? ? ? 8B 7B 24
#if 1



	typedef int(__thiscall* AssertHeldByCurrentThreadFunc)(DWORD, int, int, int);
	((AssertHeldByCurrentThreadFunc)memory->AssertHeldByCurrentThread)(((DWORD*)_this)[6],(int)"",224,0);

	//m_pLock->AssertHeldByCurrentThread();

	int cbData = (int)pSendMessage->m_cbSize;

	// Assume we won't want to wake up immediately
	if (pbThinkImmediately)
		*pbThinkImmediately = false;

	
	//*pbThinkImmediately = true;
	// Check if we're full

	int nSendBufferSize = ((DWORD*)_this)[3517] + ((DWORD*)_this)[3518];

	//  m_connectionConfig_m_SendBufferSize + cbData <= *(_DWORD *)PendingBytesTotal(_this + 3174) 
	typedef int(__thiscall* PendingBytesTotalFunc)(void*);
	if ((((PendingBytesTotalFunc)memory->PendingBytesTotal)(pBaseConn + 3174) + cbData) > nSendBufferSize)
	{
		Debug::QuickWarning("Cannot queue any more messages\n");
		pSendMessage->Release();
		return -k_EResultLimitExceeded;
	}

	// Check if they try to send a really large message

	static const int k_cbMaxUnreliableMsgSizeSend{ 15 * 1100 };

	if (cbData > k_cbMaxUnreliableMsgSizeSend && !(pSendMessage->m_nFlags & k_nSteamNetworkingSend_Reliable))
	{
		//SpewWarningRateLimited(usecNow, "Trying to send a very large (%d bytes) unreliable message.  Sending as reliable instead.\n", cbData);
		pSendMessage->m_nFlags |= k_nSteamNetworkingSend_Reliable;
	}

	if (pSendMessage->m_nFlags & k_nSteamNetworkingSend_NoDelay)
	{
		// FIXME - need to check how much data is currently pending, and return
		// k_EResultIgnored if we think it's going to be a while before this
		// packet goes on the wire.
	}

	// First, accumulate tokens, and also limit to reasonable burst
	// if we weren't already waiting to send


	// SNP_ClampSendRate(); /* Dont Need This */

	typedef void(__thiscall* SNP_TokenBucket_Accumulate_Func)(CSteamNetworkConnectionBase*, std::int64_t);

	static uintptr_t pSNP_TokenBucket_Accumulate{ ((uintptr_t)(memory->findPattern_ex(L"steamnetworkingsockets", "\x56\xE8????\x83\x87?????") + 1)) };
	((SNP_TokenBucket_Accumulate_Func)pSNP_TokenBucket_Accumulate)(pBaseConn, usecNow);

	//SNP_GetNextThinkTime





	// Assign a message number
	pSendMessage->m_nMessageNumber = ++(m_senderState->m_nLastSentMsgNum);

	// Reliable, or unreliable?
	if (pSendMessage->m_nFlags & k_nSteamNetworkingSend_Reliable)
	{
		pSendMessage->SNPSend_SetReliableStreamPos(m_senderState->m_nReliableStreamPos);

		// Generate the header
		byte* hdr = pSendMessage->SNPSend_ReliableHeader();
		hdr[0] = 0;
		byte* hdrEnd = hdr + 1;
		int64 nMsgNumGap = pSendMessage->m_nMessageNumber - m_senderState->m_nLastSendMsgNumReliable;
		Assert(nMsgNumGap >= 1);

		if (nMsgNumGap > 1)
		{
			hdrEnd = SerializeVarInt(hdrEnd, (uint64)nMsgNumGap);
			hdr[0] |= 0x40;
		}
		if (cbData < 0x20)
		{
			hdr[0] |= (byte)cbData;
		}
		else
		{
			hdr[0] |= (byte)(0x20 | (cbData & 0x1f));
			hdrEnd = SerializeVarInt(hdrEnd, cbData >> 5U);
		}
		pSendMessage->m_cbSNPSendReliableHeader = hdrEnd - hdr;

		// Grow the total size of the message by the header
		pSendMessage->m_cbSize += pSendMessage->m_cbSNPSendReliableHeader;

		// Advance stream pointer
		m_senderState->m_nReliableStreamPos += pSendMessage->m_cbSize;

		// Update stats
		++m_senderState->m_nMessagesSentReliable;
		m_senderState->m_cbPendingReliable += pSendMessage->m_cbSize;

		// Remember last sent reliable message number, so we can know how to
		// encode the next one
		m_senderState->m_nLastSendMsgNumReliable = pSendMessage->m_nMessageNumber;

		Assert(pSendMessage->SNPSend_IsReliable());
	}
	else
	{
		pSendMessage->SNPSend_SetReliableStreamPos(0);
		pSendMessage->m_cbSNPSendReliableHeader = 0;

		++(m_senderState->m_nMessagesSentUnreliable);
		m_senderState->m_cbPendingUnreliable += pSendMessage->m_cbSize;

		Assert(!pSendMessage->SNPSend_IsReliable());
	}

	// Add to pending list
	(m_senderState->m_messagesQueued.push_back(pSendMessage));
#if 0
	SpewVerboseGroup(m_connectionConfig.m_LogLevel_Message.Get(), "[%s] SendMessage %s: MsgNum=%lld sz=%d\n",
		GetDescription(),
		pSendMessage->SNPSend_IsReliable() ? "RELIABLE" : "UNRELIABLE",
		(long long)pSendMessage->m_nMessageNumber,
		pSendMessage->m_cbSize);
#endif
	// Use Nagle?
	// We always set the Nagle timer, even if we immediately clear it.  This makes our clearing code simpler,
	// since we can always safely assume that once we find a message with the nagle timer cleared, all messages
	// queued earlier than this also have it cleared.
	// FIXME - Don't think this works if the configuration value is changing.  Since changing the
	// config value could violate the assumption that nagle times are increasing.  Probably not worth
	// fixing.
	pSendMessage->SNPSend_SetUsecNagle(usecNow + 0);
	if (pSendMessage->m_nFlags & k_nSteamNetworkingSend_NoNagle)
		pSendMessage->SNPSend_SetUsecNagle(0);
		//pBaseConn->m_senderState.ClearNagleTimers();

	// Save the message number.  The code below might end up deleting the message we just queued
	int64 result = pSendMessage->m_nMessageNumber;

	// Schedule wakeup at the appropriate time.  (E.g. right now, if we're ready to send, 
	// or at the Nagle time, if Nagle is active.)
	//
	// NOTE: Right now we might not actually be capable of sending end to end data.
	// But that case is relatively rare, and nothing will break if we try to right now.
	// On the other hand, just asking the question involved a virtual function call,
	// and it will return success most of the time, so let's not make the check here.

	int nConnectionState = ((DWORD*)_this)[3566];

	if (nConnectionState == k_ESteamNetworkingConnectionState_Connected)
	{
		SteamNetworkingMicroseconds usecNextThink = ((SNPNextThink)memory->oSNP_GetNextThinkTime)(pBaseConn, usecNow);

		// Ready to send now?
		if (usecNextThink > usecNow)
		{

			// Not ready to send yet.  Is it because Nagle, or because we have previous
			// data queued and are rate limited?
			if (usecNextThink > m_senderState->m_messagesQueued.m_pFirst->SNPSend_UsecNagle())
			{
#if 0
				// It's because of the rate limit
				SpewVerbose("[%s] Send RATELIM.  QueueTime is %.1fms, SendRate=%.1fk, BytesQueued=%d, ping=%dms\n",
					GetDescription(),
					m_sendRateData.CalcTimeUntilNextSend() * 1e-3,
					m_sendRateData.m_nCurrentSendRateEstimate * (1.0 / 1024.0),
					m_senderState.PendingBytesTotal(),
					m_statsEndToEnd.m_ping.m_nSmoothedPing
				);
#endif
			}
			else
			{
#if 0
				// Waiting on nagle
				SpewVerbose("[%s] Send Nagle %.1fms.  QueueTime is %.1fms, SendRate=%.1fk, BytesQueued=%d, ping=%dms\n",
					GetDescription(),
					(m_senderState.m_messagesQueued.m_pFirst->SNPSend_UsecNagle() - usecNow) * 1e-3,
					m_sendRateData.CalcTimeUntilNextSend() * 1e-3,
					m_sendRateData.m_nCurrentSendRateEstimate * (1.0 / 1024.0),
					m_senderState.PendingBytesTotal(),
					m_statsEndToEnd.m_ping.m_nSmoothedPing
				);
#endif
			}

			// Set a wakeup call.
			//EnsureMinThinkTime();
			((ThinkFunc)memory->oInternalMinThinkTime)(pBaseConn, usecNextThink);
		}
		else
		{

			// We're ready to send right now.  Check if we should!
			if (pSendMessage->m_nFlags & k_nSteamNetworkingSend_UseCurrentThread)
			{

				// We should send in this thread, before the API entry point
				// that the app used returns.  Is the caller gonna handle this?
				if (pbThinkImmediately)
				{
					// Caller says they will handle it
					*pbThinkImmediately = true;
				}
				else
				{
					// Caller wants us to just do it here, if we can
					// E8 ? ? ? ? 85 F6 74 0E 8B CE 


					((ThinkFunc)memory->CheckConnectionStateOrScheduleWakeUp)(pBaseConn, k_nThinkTime_ASAP);
					//CheckConnectionStateOrScheduleWakeUp(usecNow);
				}
			}
			else
			{
				//k_nThinkTime_ASAP
				// Wake up the service thread ASAP to send this in the background thread


				((ThinkFunc)memory->oInternalMinThinkTime)(pBaseConn, k_nThinkTime_ASAP);


				//SetNextThinkTimeASAP(); // E8 ? ? ? ? 83 FF 03 
			}
		}
	}

	return result;

#endif










}


#if 1
bool __fastcall CSteamNetworkConnectionBase__SNP_SendPacket(void* _this, void*, void* pTransport, SendPacketContext_t& ctx) {

}
#endif

#include "../../Resource/Protobuf/Headers/steamnetworkingsockets_messages.pb.h"
typedef bool(__thiscall* InternalReceivedP2PSignalFunc)(void*, const void*, int, ISteamNetworkingSignalingRecvContext*, bool);
bool __fastcall SteamNetHooks::InternalRecievedP2PSignal(void* pCSteamNetworkingSockets, void* edx, const void* pMsg, int cbMsg, ISteamNetworkingSignalingRecvContext* pContext, bool bDefaultSignaling)
{
	if (config->debug.SteamNetworkingDebug)
	{
		CMsgSteamNetworkingP2PRendezvous P2PMessage;
		P2PMessage.ParsePartialFromArray(pMsg, cbMsg);

		std::string UserName = "N/A";
		if (false && P2PMessage.has_from_identity())
		{
			CSteamID RequesterID;
			//RequesterID.SetFromString(P2PMessage.from_identity().c_str(), k_EUniversePublic);
			//if (steamFriends->RequestUserInformation(RequesterID, true))
			//	Sleep(5000);
			//UserName = steamFriends->GetFriendPersonaName(RequesterID);
		}

		VCON("CSteamNetworkingSockets::InternalReceivedP2PSignal Call From Player %s\n %s\n", UserName.c_str(), P2PMessage.DebugString().c_str());
	}

	if (config->debug.allowcalloriginal)
		return false;

	return ((InternalReceivedP2PSignalFunc)memory->InternalRecievedP2PSignal)(pCSteamNetworkingSockets, pMsg, cbMsg, pContext, bDefaultSignaling);
}
// CSteamNetworkingSocketsSteamBase


typedef bool(__thiscall* ConnectionP2P_ProcessSignalFunc)(void*,const CMsgSteamNetworkingP2PRendezvous& , SteamNetworkingMicroseconds);
bool SteamNetHooks::ConnectionP2P_ProcessSignal(void* _this, void* edx, const CMsgSteamNetworkingP2PRendezvous& msg, SteamNetworkingMicroseconds usecNow)
{
	VCON("CSteamNetworkingConnectionP2P::ProcessSignal Call\n %s\n", msg.DebugString().c_str());

	if (config->debug.allowcalloriginal)
		return false;

	return ((ConnectionP2P_ProcessSignalFunc)memory->ConnectionP2P_ProcessSignal)(_this, msg, usecNow);
}




typedef int(__thiscall* SteamP2PHandlerFunc)(void*, const char*);
int __fastcall SteamNetHooks::SteamP2PHandler(void* pCSteamNetworkingSocketsSteamBase, void* edx, const char* pMsg)
{
	
	typedef const char*(__thiscall* MessageToIDFunc)(const char* pMsg);
	static MessageToIDFunc MessageToID = reinterpret_cast<MessageToIDFunc>(Memory::findPattern_ex(L"steamnetworkingsockets", "\x8B\x15????\x6B\xC2\x25"));
	const char* szIDFrom = MessageToID(pMsg);
	VCON("CSteamNetworkingSocketsSteamBase::SteamP2PHandler Recieved Steam P2P Request From %s\n", szIDFrom);

	if (config->debug.allowcalloriginal)
		return 0;

	//CMsgSteamNetworkingP2PRendezvous P2PMessage;
	//P2PMessage.ParsePartialFromArray(pMsg);

	return ((SteamP2PHandlerFunc)memory->SteamP2PHandler)(pCSteamNetworkingSocketsSteamBase, pMsg);
}


#ifdef RCE_EXPLOIT



#endif