#pragma once
#include <steam/isteamnetworkingsockets.h>


#if 0
class CSteamNetworkingMessage{

	int32_t nUnknown;
	int32_t m_nCbSize;
	char m_pad[180];
	int8_t m_nFlags;
	char m_pad2[11];
	int32_t m_cbSNPSendReliableHeader; /* Size Of Header */
};
#endif

#ifdef STEAMNETWORKINGSOCKETS_ENABLE_MEM_OVERRIDE
#define STEAMNETWORKINGSOCKETS_DECLARE_CLASS_OPERATOR_NEW \
		static void* operator new( size_t s ) noexcept { return malloc( s ); } \
		static void* operator new[]( size_t ) = delete; \
		static void operator delete( void *p ) noexcept { free( p ); } \
		static void operator delete[]( void * ) = delete;
#else
#define STEAMNETWORKINGSOCKETS_DECLARE_CLASS_OPERATOR_NEW
#endif
typedef unsigned char byte;
class SteamNetworkingMessageQueue;
class CSteamNetworkConnectionBase;
class CSteamNetworkingMessage : public SteamNetworkingMessage_t
{
public:
	//STEAMNETWORKINGSOCKETS_DECLARE_CLASS_OPERATOR_NEW
		static CSteamNetworkingMessage* New(CSteamNetworkConnectionBase* pParent, uint32 cbSize, int64 nMsgNum, int nFlags, SteamNetworkingMicroseconds usecNow);
	static CSteamNetworkingMessage* New(uint32 cbSize);
	static void DefaultFreeData(SteamNetworkingMessage_t* pMsg);

	/// OK to delay sending this message until this time.  Set to zero to explicitly force
	/// Nagle timer to expire and send now (but this should behave the same as if the
	/// timer < usecNow).  If the timer is cleared, then all messages with lower message numbers
	/// are also cleared.
	// NOTE: Intentionally reusing the m_usecTimeReceived field, which is not used on outbound messages
	inline SteamNetworkingMicroseconds SNPSend_UsecNagle() const { return m_usecTimeReceived; }
	inline void SNPSend_SetUsecNagle(SteamNetworkingMicroseconds x) { m_usecTimeReceived = x; }

	/// Offset in reliable stream of the header byte.  0 if we're not reliable.
	inline int64 SNPSend_ReliableStreamPos() const { return m_nConnUserData; }
	inline void SNPSend_SetReliableStreamPos(int64 x) { m_nConnUserData = x; }
	inline int SNPSend_ReliableStreamSize() const
	{
		
		return m_cbSize;
	}

	inline bool SNPSend_IsReliable() const
	{
		if (m_nFlags & k_nSteamNetworkingSend_Reliable)
		{
			
			return true;
		}

		return false;
	}

	// Reliable stream header
	int m_cbSNPSendReliableHeader;
	byte* SNPSend_ReliableHeader()
	{
		// !KLUDGE! Reuse the peer identity to hold the reliable header
		return (byte*)&m_identityPeer;
	}

	/// Remove it from queues
	void Unlink();

	struct Links
	{
		SteamNetworkingMessageQueue* m_pQueue;
		CSteamNetworkingMessage* m_pPrev;
		CSteamNetworkingMessage* m_pNext;

		inline void Clear() { m_pQueue = nullptr; m_pPrev = nullptr; m_pNext = nullptr; }
	};

	/// Intrusive links for the "primary" list we are in
	Links m_links;

	/// Intrusive links for any secondary list we may be in.  (Same listen socket or
	/// P2P channel, depending on message type)
	Links m_linksSecondaryQueue;

	void LinkBefore(CSteamNetworkingMessage* pSuccessor, Links CSteamNetworkingMessage::* pMbrLinks, SteamNetworkingMessageQueue* pQueue);
	void LinkToQueueTail(Links CSteamNetworkingMessage::* pMbrLinks, SteamNetworkingMessageQueue* pQueue);
	void UnlinkFromQueue(Links CSteamNetworkingMessage::* pMbrLinks);

private:
	// Use New and Release()!!
	inline CSteamNetworkingMessage() {}
	inline ~CSteamNetworkingMessage() {}
	static void ReleaseFunc(SteamNetworkingMessage_t* pIMsg);
};
