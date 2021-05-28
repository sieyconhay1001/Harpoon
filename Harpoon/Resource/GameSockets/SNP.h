#pragma once
#include "CSteamNetworkingMessage.h"
#include "CSteamNetworkConnectionBase.h"
#include "StatsUtils.h"
#include <vector>
#include <map>
namespace vstd
{

	template <typename T>
	void copy_construct_elements(T* dest, const T* src, size_t n)
	{
		if (std::is_trivial<T>::value)
		{
			memcpy(dest, src, n * sizeof(T));
		}
		else
		{
			T* dest_end = dest + n;
			while (dest < dest_end)
				Construct<T>(dest++, *(src++));
		}
	}

	template <typename T>
	void move_construct_elements(T* dest, T* src, size_t n)
	{
		if (std::is_trivial<T>::value)
		{
			memcpy(dest, src, n * sizeof(T));
		}
		else
		{
			T* dest_end = dest + n;
			while (dest < dest_end)
				Construct(dest++, std::move(*(src++)));
		}
	}

	// Almost the exact same interface as std::vector, only it has a small initial capacity of
	// size N in a statically-allocated block of memory.
	//
	// The only difference between this and std::vector (aside from any missing functions that just
	// need to be written) is the guarantee about not constructing elements on swapping.
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

	template<typename T, int N>
	small_vector<T, N>::small_vector(const small_vector<T, N>& x)
	{
		reserve(x.size_);
		size_ = x.size_;
		vstd::copy_construct_elements<T>(begin(), x.begin(), x.size_);
	}

	template<typename T, int N>
	small_vector<T, N>::small_vector(small_vector<T, N>&& x)
	{
		size_ = x.size_;
		if (x.dynamic_)
		{
			capacity_ = x.capacity_;
			dynamic_ = x.dynamic_;
			x.dynamic_ = nullptr;
			x.size_ = 0;
			x.capacity_ = N;
		}
		else
		{
			vstd::move_construct_elements<T>((T*)fixed_, (T*)x.fixed_, size_);
		}
	}

	template<typename T, int N>
	small_vector<T, N>& small_vector<T, N>::operator=(const small_vector<T, N>& x)
	{
		if (this != &x)
			assign(x.begin(), x.end());
		return *this;
	}

	template<typename T, int N>
	small_vector<T, N>& small_vector<T, N>::operator=(small_vector<T, N>&& x)
	{
		clear();
		size_ = x.size_;
		if (x.dynamic_)
		{
			capacity_ = x.capacity_;
			dynamic_ = x.dynamic_;
			x.dynamic_ = nullptr;
			x.size_ = 0;
			x.capacity_ = N;
		}
		else
		{
			vstd::move_construct_elements<T>((T*)fixed_, (T*)x.fixed_, size_);
		}
		return *this;
	}

	template< typename T, int N >
	void small_vector<T, N>::push_back(const T& value)
	{
		if (size_ >= capacity_)
			reserve(size_ * 2 + (63 + sizeof(T)) / sizeof(T));
		Construct<T>(begin() + size_, value);
		++size_;
	}

	template< typename T, int N >
	void small_vector<T, N>::pop_back()
	{
		assert(size_ > 0);
		--size_;
		(begin() + size_)->~T();
	}

	template< typename T, int N >
	void small_vector<T, N>::erase(T* it)
	{
		T* e = end();
		assert(begin() <= it);
		assert(it < e);

		if (std::is_trivial<T>::value)
		{
			memmove(it, it + 1, (char*)e - (char*)(it + 1));
		}
		else
		{
			--e;
			while (it < e)
			{
				it[0] = std::move(it[1]);
				++it;
			}
			e->~T();
		}
		--size_;
	}

	template< typename T, int N >
	void small_vector<T, N>::reserve(size_t n)
	{
		if (n <= capacity_)
			return;
		assert(capacity_ >= size_);
		if (std::is_trivial<T>::value && dynamic_)
		{
			dynamic_ = (T*)realloc(dynamic_, n * sizeof(T));
		}
		else
		{
			T* new_dynamic = (T*)malloc(n * sizeof(T));
			T* s = begin();
			T* e = s + size_;
			T* d = new_dynamic;
			while (s < e)
			{
				Construct<T>(d, std::move(*s));
				s->~T();
				++s;
				++d;
			}
			if (dynamic_)
				::free(dynamic_);
			dynamic_ = new_dynamic;
		}
		capacity_ = n;
	}

	template< typename T, int N >
	void small_vector<T, N>::resize(size_t n)
	{
		if (n > size_)
		{
			reserve(n);
			T* b = begin();
			while (size_ < n)
			{
				Construct<T>(b); // NOTE: Does not use value initializer, so PODs are *not* initialized
				++b;
				++size_;
			}
		}
		else
		{
			T* e = end();
			while (size_ > n)
			{
				--size_;
				--e;
				e->~T();
			}
		}
	}

	template< typename T, int N >
	void small_vector<T, N>::clear()
	{
		T* b = begin();
		T* e = b + size_;
		while (e > b)
		{
			--e;
			e->~T();
		}
		if (dynamic_)
		{
			::free(dynamic_);
			dynamic_ = nullptr;
		}
		size_ = 0;
		capacity_ = N;
	}

	template< typename T, int N >
	void small_vector<T, N>::assign(const T* srcBegin, const T* srcEnd)
	{
		if (srcEnd <= srcBegin)
		{
			clear();
			return;
		}
		size_t n = srcEnd - srcBegin;
		if (n > N)
		{
			// We need dynamic memory.  If we're not exactly sized already,
			// just nuke everyhing we have.
			if (n != capacity_)
			{
				clear();
				reserve(n);
			}
			assert(dynamic_);
			if (!std::is_trivial<T>::value)
			{
				while (size_ > n)
					dynamic_[--size_].~T();
			}
		}
		else if (dynamic_)
		{
			// We have dynamic allocation, but don't need it
			clear();
		}
		assert(capacity_ >= n);
		if (std::is_trivial<T>::value)
		{
			// Just blast them over, and don't bother with the leftovers
			memcpy(begin(), srcBegin, n * sizeof(T));
		}
		else
		{
			assert(size_ <= n);

			// Complex type.  Try to avoid excess constructor/destructor calls
			// First use operator= for items already constructed
			const T* s = srcBegin;
			T* d = begin();
			T* e = d + size_;
			while (d < e && s < srcEnd)
				*(d++) = *(s++);

			// Use copy constructor for any remaining items
			while (s < srcEnd)
				Construct<T>(d++, *(s++));
		}
		size_ = n;
	}

	//template <typename T, int N>
	//struct LikeStdVectorTraits< small_vector<T, N> > { enum { yes = 1 }; typedef T ElemType; };

} // namespace vstd


/// Maximum number of packets we will send in one Think() call.
const int k_nMaxPacketsPerThink = 16;

/// Max number of tokens we are allowed to store up in reserve, for a burst.
const float k_flSendRateBurstOverageAllowance = k_cbSteamNetworkingSocketsMaxEncryptedPayloadSend;

struct SNPRange_t
{
	/// Byte or sequence number range
	int64 m_nBegin;
	int64 m_nEnd; // STL-style.  It's one past the end

	inline int64 length() const
	{
		// In general, allow zero-length ranges, but not negative ones
		Assert(m_nEnd >= m_nBegin);
		return m_nEnd - m_nBegin;
	}

	/// Strict comparison function.  This is used in situations where
	/// ranges must not overlap, AND we also never search for
	/// a range that might overlap.
	struct NonOverlappingLess
	{
		inline bool operator ()(const SNPRange_t& l, const SNPRange_t& r) const
		{
			if (l.m_nBegin < r.m_nBegin) return true;
			AssertMsg(l.m_nBegin > r.m_nBegin || l.m_nEnd == r.m_nEnd, "Ranges should not overlap in this map!");
			return false;
		}
	};
};

/// A packet that has been sent but we don't yet know if was received
/// or dropped.  These are kept in an ordered map keyed by packet number.
/// (Hence the packet number not being a member)  When we receive an ACK,
/// we remove packets from this list.
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

struct SSNPSendMessageList : public SteamNetworkingMessageQueue
{

	/// Unlink the message at the head, if any and return it.
	/// Unlike STL pop_front, this will return nullptr if the
	/// list is empty
	CSteamNetworkingMessage* pop_front()
	{
		CSteamNetworkingMessage* pResult = m_pFirst;
		if (pResult)
		{
			Assert(m_pLast);
			Assert(pResult->m_links.m_pQueue == this);
			Assert(pResult->m_links.m_pPrev == nullptr);
			m_pFirst = pResult->m_links.m_pNext;
			if (m_pFirst)
			{
				Assert(m_pFirst->m_links.m_pPrev == pResult);
				Assert(m_pFirst->m_nMessageNumber > pResult->m_nMessageNumber);
				m_pFirst->m_links.m_pPrev = nullptr;
			}
			else
			{
				Assert(m_pLast == pResult);
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
			Assert(m_pLast == nullptr);
			m_pFirst = pMsg;
		}
		else
		{
			// Messages are always kept in message number order
			Assert(pMsg->m_nMessageNumber > m_pLast->m_nMessageNumber);
			Assert(m_pLast->m_links.m_pNext == nullptr);
			m_pLast->m_links.m_pNext = pMsg;
		}
		pMsg->m_links.m_pQueue = this;
		pMsg->m_links.m_pNext = nullptr;
		pMsg->m_links.m_pPrev = m_pLast;
		m_pLast = pMsg;
	}

};

/// Info used by a sender to estimate the available bandwidth
struct SSendRateData
{
	/// Current sending rate in bytes per second, RFC 3448 4.2 states default
	/// is one packet per second, but that is insane and we're not doing that.
	/// In most cases we will set a default based on initial ping, so this is
	/// only rarely used.
	int m_nCurrentSendRateEstimate = 64 * 1024;

	/// Actual send rate we are going to USE.  This depends on the send rate estimate
	/// and the current BBR state
	float m_flCurrentSendRateUsed = 64 * 1024;

	/// If >=0, then we can send a full packet right now.  We allow ourselves to "store up"
	/// about 1 packet worth of "reserve".  In other words, if we have not sent any packets
	/// for a while, basically we allow ourselves to send two packets in rapid succession,
	/// thus "bursting" over the limit by 1 packet.  That long term rate will be clamped by
	/// the send rate.
	///
	/// If <0, then we are currently "over" our rate limit and need to wait before we can
	/// send a packet.
	///
	/// Provision for accumulating "credits" and burst allowance, to account for lossy
	/// kernel scheduler, etc is mentioned in RFC 5348, section 4.6.
	float m_flTokenBucket = 0;

	/// Last time that we added tokens to m_flTokenBucket
	SteamNetworkingMicroseconds m_usecTokenBucketTime = 0;

	/// Calculate time until we could send our next packet, checking our token
	/// bucket and the current send rate
	SteamNetworkingMicroseconds CalcTimeUntilNextSend() const
	{
		// Do we have tokens to burn right now?
		if (m_flTokenBucket >= 0.0f)
			return 0;

		return SteamNetworkingMicroseconds(m_flTokenBucket * -1e6f / m_flCurrentSendRateUsed) + 1; // +1 to make sure that if we don't have any tokens, we never return 0, since zero means "ready right now"
	}
};


struct SSNPSenderState
{


	// Current message number, we ++ when adding a message
	int64 m_nReliableStreamPos = 1;
	int64 m_nLastSentMsgNum = 0; // Will increment to 1 with first message
	int64 m_nLastSendMsgNumReliable = 0;

	/// List of messages that we have not yet finished putting on the wire the first time.
	/// The Nagle timer may be active on one or more, but if so, it is only on messages
	/// at the END of the list.  The first message may be partially sent.
	SSNPSendMessageList m_messagesQueued;

	/// How many bytes into the first message in the queue have we put on the wire?
	int m_cbCurrentSendMessageSent = 0;

	/// List of reliable messages that have been fully placed on the wire at least once,
	/// but we're hanging onto because of the potential need to retry.  (Note that if we get
	/// packet loss, it's possible that we hang onto a message even after it's been fully
	/// acked, because a prior message is still needed.  We always operate on this list
	/// like a queue, rather than seeking into the middle of the list and removing messages
	/// as soon as they are no longer needed.)
	SSNPSendMessageList m_unackedReliableMessages;

	// Buffered data counters.  See SteamNetworkingQuickConnectionStatus for more info
	int m_cbPendingUnreliable = 0;
	int m_cbPendingReliable = 0;
	int m_cbSentUnackedReliable = 0;
	inline int PendingBytesTotal() const { return m_cbPendingUnreliable + m_cbPendingReliable; }

	// Stats.  FIXME - move to LinkStatsEndToEnd and track rate counters
	int64 m_nMessagesSentReliable = 0;
	int64 m_nMessagesSentUnreliable = 0;

	/// List of packets that we have sent but don't know whether they were received or not.
	/// We keep a dummy sentinel at the head of the list, with a negative packet number.
	/// This vastly simplifies the processing.
	std::map<int64, SNPInFlightPacket_t> m_mapInFlightPacketsByPktNum;

	/// The next unacked packet that should be timed out and implicitly NACKed,
	/// if we don't receive an ACK in time.  Will be m_mapInFlightPacketsByPktNum.end()
	/// if we don't have any in flight packets that we are waiting on.
	std::map<int64, SNPInFlightPacket_t>::iterator m_itNextInFlightPacketToTimeout;

	/// Ordered list of reliable ranges that we have recently sent
	/// in a packet.  These should be non-overlapping, and furthermore
	/// should not overlap with with any range in m_listReadyReliableRange
	///
	/// The "value" portion of the map is the message that has the first bit of
	/// reliable data we need for this message
	std::map<SNPRange_t, CSteamNetworkingMessage*, SNPRange_t::NonOverlappingLess> m_listInFlightReliableRange;

	/// Ordered list of ranges that have been put on the wire,
	/// but have been detected as dropped, and now need to be retried.
	std::map<SNPRange_t, CSteamNetworkingMessage*, SNPRange_t::NonOverlappingLess> m_listReadyRetryReliableRange;

	/// Oldest packet sequence number that we are still asking peer
	/// to send acks for.
	int64 m_nMinPktWaitingOnAck = 0;

	// Remove messages from m_unackedReliableMessages that have been fully acked.
	//void RemoveAckedReliableMessageFromUnackedList();

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