#pragma once
#include "CSteamNetworkingMessage.h"
#include "Internal.h"
#include <atomic>
#include <cstdint>
#include <functional>
#include <mutex>
#include <thread>
#include <steam/steamnetworkingtypes.h>
#include "SNP.h"
#include <steamnetworkingsockets_messages.pb.h>
#include <steamnetworkingsockets_messages_certs.pb.h>
#include <steamdatagram_messages_sdr.pb.h>


#include "common/crypto.h"

#include "StatsUtils.h"

#define PRINTF_FORMAT_STRING _Printf_format_string_
#define SCANF_FORMAT_STRING _Scanf_format_string_impl_

// Tag the end of functions using printf-style format strings with this (consumed by GCC).
// Note that for methods the 'this' pointer counts as the first argument in argument numbering.
#ifdef __MINGW32__
#define FMTFUNCTION( fmtargnumber, firstvarargnumber ) __attribute__ (( format( __MINGW_PRINTF_FORMAT, fmtargnumber, firstvarargnumber )))
#elif defined(COMPILER_GCC)
#define FMTFUNCTION( fmtargnumber, firstvarargnumber ) __attribute__ (( format( __printf__, fmtargnumber, firstvarargnumber )))
#else
#define FMTFUNCTION( fmtargnumber, firstvarargnumber )
#endif

// Forward Declares For Shit I Don't need rn
class CSteamNetworkingSockets;
class CConnectionTransport;
class CSteamNetworkPollGroupl;
class CSteamNetworkPollGroup;
class CSteamNetworkingSockets;
class CSteamNetworkingMessages;
class CSteamNetworkConnectionBase;
class CSteamNetworkConnectionP2P;
class CSharedSocket;
class CConnectionTransport;
struct SNPAckSerializerHelper;
struct CertAuthScope;
class CSteamNetworkListenSocketBase;
struct SteamNetConnectionStatusChangedCallback_t;
class ISteamNetworkingSocketsSerialized;
const SteamNetworkingMicroseconds k_usecConnectRetryInterval = k_nMillion / 2;
const SteamNetworkingMicroseconds k_usecFinWaitTimeout = 5 * k_nMillion;
typedef char ConnectionEndDebugMsg[k_cchSteamNetworkingMaxConnectionCloseReason];
typedef char ConnectionTypeDescription_t[64];



enum EUnsignedCert
{
	k_EUnsignedCert_Disallow,
	k_EUnsignedCert_AllowWarn,
	k_EUnsignedCert_Allow,
};




/// In various places, we need a key in a map of remote connections.
struct RemoteConnectionKey_t
{
	SteamNetworkingIdentity m_identity;
	uint32 m_unConnectionID;

	// NOTE: If we assume that peers are well behaved, then we
	// could just use the connection ID, which is a random number.
	// but let's not assume that.  In fact, if we really need to
	// protect against malicious clients we might have to include
	// some random private data so that they don't know how our hash
	// function works.  We'll assume for now that this isn't a problem
	//struct Hash { uint32 operator()(const RemoteConnectionKey_t& x) const { return SteamNetworkingIdentityHash{}(x.m_identity) ^ x.m_unConnectionID; } };
	inline bool operator ==(const RemoteConnectionKey_t& x) const
	{
		return m_unConnectionID == x.m_unConnectionID && m_identity == x.m_identity;
	}
};

/// Base class for connection-type-specific context structure 
struct SendPacketContext_t
{
	inline SendPacketContext_t(SteamNetworkingMicroseconds usecNow, const char* pszReason) : m_usecNow(usecNow), m_pszReason(pszReason) {}
	const SteamNetworkingMicroseconds m_usecNow;
	int m_cbMaxEncryptedPayload;
	const char* m_pszReason; // Why are we sending this packet?
};

/// Context used when receiving a data packet
struct RecvPacketContext_t
{

	//
	// Must be filled in by transport
	//

		/// Current time
	SteamNetworkingMicroseconds m_usecNow;

	/// What transport is receiving this packet?
	CConnectionTransport* m_pTransport;

	/// Jitter measurement, if present
	//int m_usecTimeSinceLast;

//
// Output of DecryptDataChunk
//

	/// Expanded packet number
	int64 m_nPktNum;

	/// Pointer to decrypted data.  Will either point to to the caller's original packet,
	/// if the packet was not encrypted, or m_decrypted, if it was encrypted and we
	/// decrypted it
	const void* m_pPlainText;

	/// Size of plaintext
	int m_cbPlainText;

	// Temporary buffer to hold decrypted data, if we were actually encrypted
	uint8 m_decrypted[k_cbSteamNetworkingSocketsMaxPlaintextPayloadRecv];
};

template<typename TStatsMsg>
struct SendPacketContext : SendPacketContext_t
{
	inline SendPacketContext(SteamNetworkingMicroseconds usecNow, const char* pszReason) : SendPacketContext_t(usecNow, pszReason) {}

	uint32 m_nFlags; // Message flags that we need to set.
	TStatsMsg msg; // Type-specific stats message
	int m_cbMsgSize; // Size of message
	int m_cbTotalSize; // Size needed in the header, including the serialized size field

	void SlamFlagsAndCalcSize()
	{
		SetStatsMsgFlagsIfNotImplied(msg, m_nFlags);
		m_cbTotalSize = m_cbMsgSize = ProtoMsgByteSize(msg);
		if (m_cbMsgSize > 0)
			m_cbTotalSize += VarIntSerializedSize((uint32)m_cbMsgSize);
	}

	bool Serialize(byte*& p)
	{
		if (m_cbTotalSize <= 0)
			return false;

		// Serialize the stats size, var-int encoded
		byte* pOut = SerializeVarInt(p, uint32(m_cbMsgSize));

		// Serialize the actual message
		pOut = msg.SerializeWithCachedSizesToArray(pOut);

		// Make sure we wrote the number of bytes we expected
		if (pOut != p + m_cbTotalSize)
		{
			// ABORT!
			AssertMsg(false, "Size mismatch after serializing inline stats blob");
			return false;
		}

		// Advance pointer
		p = pOut;
		return true;
	}

	void CalcMaxEncryptedPayloadSize(size_t cbHdrReserve, CSteamNetworkConnectionBase* pConnection);
};

const SteamNetworkingMicroseconds k_nThinkTime_Never = INT64_MAX;
const SteamNetworkingMicroseconds k_nThinkTime_ASAP = 1; // by convention, we do not allow setting a think time to 0, since 0 is often an uninitialized variable.
class ThinkerSetIndex;

class IThinker
{
public:
	STEAMNETWORKINGSOCKETS_DECLARE_CLASS_OPERATOR_NEW
		virtual ~IThinker();

	/// Called to set when you next want to get your Think() callback.
	/// You should assume that, due to scheduler inaccuracy, you could
	/// get your callback 1 or 2 ms late.
	void SetNextThinkTime(SteamNetworkingMicroseconds usecTargetThinkTime);

	/// Adjust schedule time to the earlier of the current schedule time,
	/// or the given time.
	inline void EnsureMinThinkTime(SteamNetworkingMicroseconds usecTargetThinkTime)
	{
		if (usecTargetThinkTime < m_usecNextThinkTime)
			InternalEnsureMinThinkTime(usecTargetThinkTime);
	}

	/// Clear the next think time.  You won't get a callback.
	void ClearNextThinkTime() { SetNextThinkTime(k_nThinkTime_Never); }

	/// Request an immediate wakeup.
	void SetNextThinkTimeASAP() { EnsureMinThinkTime(k_nThinkTime_ASAP); }

	/// Fetch time when the next Think() call is currently scheduled to
	/// happen.
	inline SteamNetworkingMicroseconds GetNextThinkTime() const { return m_usecNextThinkTime; }

	/// Return true if we are scheduled to get our callback
	inline bool IsScheduled() const { return m_usecNextThinkTime != k_nThinkTime_Never; }

	/// Try to acquire our lock.  Returns false if we fail.
	virtual bool TryLock() const;

	static void Thinker_ProcessThinkers();
	static SteamNetworkingMicroseconds Thinker_GetNextScheduledThinkTime();
protected:
	IThinker();

	/// Callback to do whatever periodic processing you need.  If you don't
	/// explicitly call SetNextThinkTime inside this function, then thinking
	/// will be disabled.
	///
	/// Think callbacks will always happen from the service thread,
	/// with the lock held.
	///
	/// Note that we assume a limited precision of the thread scheduler,
	/// and you won't get your callback exactly when you request.
	virtual void Think(SteamNetworkingMicroseconds usecNow) = 0;

private:
	SteamNetworkingMicroseconds m_usecNextThinkTime;
	int m_queueIndex;
	friend class ThinkerSetIndex;

	void InternalSetNextThinkTime(SteamNetworkingMicroseconds usecTargetThinkTime);
	void InternalEnsureMinThinkTime(SteamNetworkingMicroseconds usecTargetThinkTime);
};
template <typename L>
class ILockableThinker : public IThinker
{
public:
	virtual bool TryLock() const final { return m_pLock->try_lock(); }
	inline void Unlock() { m_pLock->unlock(); }

	L* m_pLock;
protected:
	ILockableThinker(L& lock) : IThinker(), m_pLock(&lock) {}
};

using ShortDurationMutexImpl = std::mutex; // No recursion, no timeout, should only be held for a short time, so expect low contention.  Good candidate for spinlock.
using RecursiveMutexImpl = std::recursive_mutex; // Need to able to lock recursively, but don't need to be able to wait with timeout.
using RecursiveTimedMutexImpl = std::recursive_timed_mutex; // Recursion, and need to be able to wait with timeout.  (Does this ability actually add any extra work on any OS we care about?)

/// Debug record for a lock.
struct LockDebugInfo
{
	static constexpr int k_nFlag_ShortDuration = (1 << 0);
	static constexpr int k_nFlag_Connection = (1 << 1);
	static constexpr int k_nFlag_PollGroup = (1 << 2);
	static constexpr int k_nFlag_Table = (1 << 4);

	const char* const m_pszName;
	const int m_nFlags;

#if STEAMNETWORKINGSOCKETS_LOCK_DEBUG_LEVEL > 0
	void _AssertHeldByCurrentThread(const char* pszFile, int line, const char* pszTag = nullptr) const;
#else
	inline void _AssertHeldByCurrentThread(const char* pszFile, int line, const char* pszTag = nullptr) const {}
#endif

protected:
	LockDebugInfo(const char* pszName, int nFlags) : m_pszName(pszName), m_nFlags(nFlags) {}

#if STEAMNETWORKINGSOCKETS_LOCK_DEBUG_LEVEL > 0
	void AboutToLock(bool bTry);
	void OnLocked(const char* pszTag);
	void AboutToUnlock();
	~LockDebugInfo();
#else
	void AboutToLock(bool bTry) {}
	void OnLocked(const char* pszTag) {}
	void AboutToUnlock() {}
#endif
};

/// Wrapper for locks to make them somewhat debuggable.
template<typename TMutexImpl >
struct Lock : LockDebugInfo
{
	inline Lock(const char* pszName, int nFlags) : LockDebugInfo(pszName, nFlags) {}
	inline void lock(const char* pszTag = nullptr)
	{
		LockDebugInfo::AboutToLock(false);
		m_impl.lock();
		LockDebugInfo::OnLocked(pszTag);
	}
	inline void unlock()
	{
		LockDebugInfo::AboutToUnlock();
		m_impl.unlock();
	}
	inline bool try_lock(const char* pszTag = nullptr) {
		LockDebugInfo::AboutToLock(true);
		if (!m_impl.try_lock())
			return false;
		LockDebugInfo::OnLocked(pszTag);
		return true;
	}
	inline bool try_lock_for(int msTimeout, const char* pszTag = nullptr)
	{
		LockDebugInfo::AboutToLock(true);
		if (!m_impl.try_lock_for(std::chrono::milliseconds(msTimeout)))
			return false;
		LockDebugInfo::OnLocked(pszTag);
		return true;
	}

private:
	TMutexImpl m_impl;
};

/// Object that automatically unlocks a lock when it goes out of scope using RIAA
template<typename TLock>
struct ScopeLock
{
	ScopeLock() : m_pLock(nullptr) {}
	explicit ScopeLock(TLock& lock, const char* pszTag = nullptr) : m_pLock(&lock) { lock.lock(pszTag); }
	~ScopeLock() { if (m_pLock) m_pLock->unlock(); }
	bool IsLocked() const { return m_pLock != nullptr; }
	void Lock(TLock& lock, const char* pszTag = nullptr)
	{
		if (m_pLock)
		{
			AssertMsg(false, "Scopelock already holding %s, while locking %s!  tag=%s",
				m_pLock->m_pszName, lock.m_pszName, pszTag ? pszTag : "???");
			m_pLock->unlock();
		}
		m_pLock = &lock;
		lock.lock(pszTag);
	}
	bool TryLock(TLock& lock, int msTimeout, const char* pszTag)
	{
		if (m_pLock)
		{
			AssertMsg(false, "Scopelock already holding %s, while trylock %s!  tag=%s",
				m_pLock->m_pszName, lock.m_pszName, pszTag ? pszTag : "???");
			m_pLock->unlock();
			m_pLock = nullptr;
		}
		if (!lock.try_lock_for(msTimeout, pszTag))
			return false;
		m_pLock = &lock;
		return true;
	}
	void Unlock() { if (!m_pLock) return; m_pLock->unlock(); m_pLock = nullptr; }

	// If we have a lock, forget about it
	void Abandon() { m_pLock = nullptr; }
private:
	TLock* m_pLock;
};


struct ConnectionLock : Lock<RecursiveTimedMutexImpl> {
	ConnectionLock() : Lock<RecursiveTimedMutexImpl>( "connection", LockDebugInfo::k_nFlag_Connection ) {}
};
struct ConnectionScopeLock : ScopeLock<ConnectionLock>
{
	ConnectionScopeLock() = default;
	ConnectionScopeLock( ConnectionLock &lock, const char *pszTag = nullptr ) : ScopeLock<ConnectionLock>( lock, pszTag ) {}
	ConnectionScopeLock( CSteamNetworkConnectionBase &conn, const char *pszTag = nullptr );
	void Lock( ConnectionLock &lock, const char *pszTag = nullptr ) { ScopeLock<ConnectionLock>::Lock( lock, pszTag ); }
	void Lock( CSteamNetworkConnectionBase &conn, const char *pszTag = nullptr );
};





struct SteamNetworkingMessageQueue
{
	CSteamNetworkingMessage* m_pFirst = nullptr;
	CSteamNetworkingMessage* m_pLast = nullptr;
	LockDebugInfo* m_pRequiredLock = nullptr; // Is there a lock that is required to be held while we access this queue?

	inline bool empty() const
	{
		if (m_pFirst)
		{

			return false;
		}
		return true;
	}
#if 0
	/// Remove the first messages out of the queue (up to nMaxMessages).  Returns the number returned
	int RemoveMessages(SteamNetworkingMessage_t** ppOutMessages, int nMaxMessages);

	/// Delete all queued messages
	void PurgeMessages();

	/// Check the lock is held, if appropriate
	void AssertLockHeld() const;
#endif
};
struct ConfigValueBase
{

	// Config value we should inherit from, if we are not set
	ConfigValueBase* m_pInherit = nullptr;

	enum EState
	{
		kENotSet,
		kESet,
		kELocked,
	};

	// Is the value set?
	EState m_eState = kENotSet;

	inline bool IsLocked() const { return m_eState == kELocked; }
	inline bool IsSet() const { return m_eState > kENotSet; }

	// Unlock, if we are locked
	inline void Unlock()
	{
		if (m_eState == kELocked)
			m_eState = kESet;
	}
};

template<typename T>
struct ConfigValue : public ConfigValueBase
{
	inline ConfigValue() : m_data{} {}
	inline explicit ConfigValue(const T& defaultValue) : m_data(defaultValue) { m_eState = kESet; }

	T m_data;

	/// Fetch the effective value
	inline const T& Get() const
	{
		const ConfigValueBase* p = this;
		while (!p->IsSet())
		{
			Assert(p->m_pInherit);
			p = p->m_pInherit;
		}

		const auto* t = static_cast<const ConfigValue<T> *>(p);
		return t->m_data;
	}

	inline void Set(const T& value)
	{
		Assert(!IsLocked());
		m_data = value;
		m_eState = kESet;
	}

	// Lock in the current value
	inline void Lock()
	{
		if (!IsSet())
			m_data = Get();
		m_eState = kELocked;
	}
};
struct ConnectionConfig
{
	ConfigValue<int32> m_TimeoutInitial;
	ConfigValue<int32> m_TimeoutConnected;
	ConfigValue<int32> m_SendBufferSize;
	ConfigValue<int32> m_SendRateMin;
	ConfigValue<int32> m_SendRateMax;
	ConfigValue<int32> m_MTU_PacketSize;
	ConfigValue<int32> m_NagleTime;
	ConfigValue<int32> m_IP_AllowWithoutAuth;
	ConfigValue<int32> m_Unencrypted;
	ConfigValue<int32> m_SymmetricConnect;
	ConfigValue<int32> m_LocalVirtualPort;
	ConfigValue<int64> m_ConnectionUserData;

	ConfigValue<int32> m_LogLevel_AckRTT;
	ConfigValue<int32> m_LogLevel_PacketDecode;
	ConfigValue<int32> m_LogLevel_Message;
	ConfigValue<int32> m_LogLevel_PacketGaps;
	ConfigValue<int32> m_LogLevel_P2PRendezvous;

	ConfigValue<void*> m_Callback_ConnectionStatusChanged;

#ifdef STEAMNETWORKINGSOCKETS_ENABLE_ICE
	ConfigValue<std::string> m_P2P_STUN_ServerList;
	ConfigValue<int32> m_P2P_Transport_ICE_Enable;
	ConfigValue<int32> m_P2P_Transport_ICE_Penalty;
#endif

#ifdef STEAMNETWORKINGSOCKETS_ENABLE_SDR
	ConfigValue<std::string> m_SDRClient_DebugTicketAddress;
	ConfigValue<int32> m_P2P_Transport_SDR_Penalty;
#endif

	void Init(ConnectionConfig* pInherit);
};


/// Abstract interface for a connection to a remote host over any underlying
/// transport.  Most of the common functionality for implementing reliable
/// connections on top of unreliable datagrams, connection quality measurement,
/// etc is implemented here. 
class CSteamNetworkConnectionBase : public ILockableThinker< ConnectionLock >
{
public:

};