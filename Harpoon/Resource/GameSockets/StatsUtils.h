#pragma once
#include "CSteamNetworkingMessage.h"
#include "Internal.h"
#include <atomic>
#include <cstdint>
#include <functional>
#include <mutex>
#include <thread>
#include <steam/steamnetworkingtypes.h>
#include "percentile_generator.h"

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




class CMsgSteamDatagramConnectionQuality;

// Internal stuff goes in a private namespace


	/// Default interval for link stats rate measurement
	const SteamNetworkingMicroseconds k_usecSteamDatagramLinkStatsDefaultInterval = 5 * k_nMillion;

	/// Default interval for speed stats rate measurement
	const SteamNetworkingMicroseconds k_usecSteamDatagramSpeedStatsDefaultInterval = 1 * k_nMillion;

	/// We should send tracer ping requests in our packets on approximately
	/// this interval.  (Tracer pings and their replies are relatively cheap.)
	/// These serve both as latency measurements, and also as keepalives, if only
	/// one side or the other is doing most of the talking, to make sure the other side
	/// always does a minimum amount of acking.
	const SteamNetworkingMicroseconds k_usecLinkStatsMinPingRequestInterval = 5 * k_nMillion;
	const SteamNetworkingMicroseconds k_usecLinkStatsMaxPingRequestInterval = 7 * k_nMillion;

	/// Client should send instantaneous connection quality stats
	/// at approximately this interval
	const SteamNetworkingMicroseconds k_usecLinkStatsInstantaneousReportInterval = 20 * k_nMillion;
	const SteamNetworkingMicroseconds k_usecLinkStatsInstantaneousReportMaxInterval = 30 * k_nMillion;

	/// Client will report lifetime connection stats at approximately this interval
	const SteamNetworkingMicroseconds k_usecLinkStatsLifetimeReportInterval = 120 * k_nMillion;
	const SteamNetworkingMicroseconds k_usecLinkStatsLifetimeReportMaxInterval = 140 * k_nMillion;

	/// If we are timing out, ping the peer on this interval
	const SteamNetworkingMicroseconds k_usecAggressivePingInterval = 200 * 1000;

	/// If we haven't heard from the peer in a while, send a keepalive
	const SteamNetworkingMicroseconds k_usecKeepAliveInterval = 10 * k_nMillion;

	/// Track the rate that something is happening
	struct Rate_t
	{
		void Reset() { memset(this, 0, sizeof(*this)); }

		int64	m_nCurrentInterval;
		int64	m_nAccumulator; // does not include the currentinterval
		float	m_flRate;

		int64 Total() const { return m_nAccumulator + m_nCurrentInterval; }

		inline void Process(int64 nIncrement)
		{
			m_nCurrentInterval += nIncrement;
		}

		inline void UpdateInterval(float flIntervalDuration)
		{
			m_flRate = float(m_nCurrentInterval) / flIntervalDuration;
			m_nAccumulator += m_nCurrentInterval;
			m_nCurrentInterval = 0;
		}

		inline void operator+=(const Rate_t& x)
		{
			m_nCurrentInterval += x.m_nCurrentInterval;
			m_nAccumulator += x.m_nAccumulator;
			m_flRate += x.m_flRate;
		}
	};

	/// Track flow rate (number and bytes)
	struct PacketRate_t
	{
		void Reset() { memset(this, 0, sizeof(*this)); }

		Rate_t m_packets;
		Rate_t m_bytes;

		inline void ProcessPacket(int sz)
		{
			m_packets.Process(1);
			m_bytes.Process(sz);
		}

		void UpdateInterval(float flIntervalDuration)
		{
			m_packets.UpdateInterval(flIntervalDuration);
			m_bytes.UpdateInterval(flIntervalDuration);
		}

		inline void operator+=(const PacketRate_t& x)
		{
			m_packets += x.m_packets;
			m_bytes += x.m_bytes;
		}
	};

	/// Class used to track ping values
	struct PingTracker
	{

		struct Ping
		{
			int m_nPingMS;
			SteamNetworkingMicroseconds m_usecTimeRecv;
		};

		/// Recent ping measurements.  The most recent one is at entry 0.
		Ping m_arPing[3];

		/// Number of valid entries in m_arPing.
		int m_nValidPings;
#if 0
		/// Time when the most recent ping was received
		SteamNetworkingMicroseconds TimeRecvMostRecentPing() const { return m_arPing[0].m_usecTimeRecv; }

		/// Return the worst of the pings in the small sample of recent pings
		//int WorstPingInRecentSample() const;

		/// Estimate a conservative (i.e. err on the large side) timeout for the connection
		SteamNetworkingMicroseconds CalcConservativeTimeout() const
		{
			constexpr SteamNetworkingMicroseconds k_usecMaxTimeout = 1250000;
			if (m_nSmoothedPing < 0)
				return k_usecMaxTimeout;
			return std::min(SteamNetworkingMicroseconds{ WorstPingInRecentSample() * 2000 + 250000 }, k_usecMaxTimeout);
		}

#endif
		/// Smoothed ping value
		int m_nSmoothedPing;

		/// Time when we last sent a message, for which we expect a reply (possibly delayed)
		/// that we could use to measure latency.  (Possibly because the reply contains
		/// a simple timestamp, or possibly because it will contain a sequence number, and
		/// we will be able to look up that sequence number and remember when we sent it.)
		SteamNetworkingMicroseconds m_usecTimeLastSentPingRequest;
	protected:
		//virtual void Reset() = 0;

		/// Called when we receive a ping measurement
		//virtual void ReceivedPing(int nPingMS, SteamNetworkingMicroseconds usecNow) = 0;
	};















struct PingHistogram
{
		int m_n25, m_n50, m_n75, m_n100, m_n125, m_n150, m_n200, m_n300, m_nMax;
};

struct QualityHistogram
{
	int m_n100, m_n99, m_n97, m_n95, m_n90, m_n75, m_n50, m_n1, m_nDead;
};
struct JitterHistogram
{
	void Reset() { memset(this, 0, sizeof(*this)); }

	int m_nNegligible; // <1ms
	int m_n1; // 1--2ms
	int m_n2; // 2--5ms
	int m_n5; // 5--10ms
	int m_n10; // 10--20ms
	int m_n20; // 20ms or more
};

/// Ping tracker that tracks detailed lifetime stats

class PercentileGenUint16 {
		int m_nSamples;
		int m_nSamplesTotal;
		mutable bool m_bNeedSort;

		/// Raw sample data
	
		uint16 m_arSamples[1000];
};

class PercentileGenUint8 {
	int m_nSamples;
	int m_nSamplesTotal;
	mutable bool m_bNeedSort;

	/// Raw sample data

	uint8 m_arSamples[1000];
};

class PercentileGenInt {
	int m_nSamples;
	int m_nSamplesTotal;
	mutable bool m_bNeedSort;

	/// Raw sample data

	uint8 m_arSamples[1000];
};

struct PingTrackerDetailed : PingTracker
{


	/// Track sample of pings received so we can generate percentiles.
	/// Also tracks how many pings we have received total
	//PercentileGenerator<uint16> m_sample;

	PercentileGenUint16 m_sample;

	/// Counts by bucket
	PingHistogram m_histogram;
#if 0
	/// Populate structure
	void GetLifetimeStats(SteamDatagramLinkLifetimeStats& s) const
	{
		s.m_pingHistogram = m_histogram;

		s.m_nPingNtile5th = m_sample.NumSamples() < 20 ? -1 : m_sample.GetPercentile(.05f);
		s.m_nPingNtile50th = m_sample.NumSamples() < 2 ? -1 : m_sample.GetPercentile(.50f);
		s.m_nPingNtile75th = m_sample.NumSamples() < 4 ? -1 : m_sample.GetPercentile(.75f);
		s.m_nPingNtile95th = m_sample.NumSamples() < 20 ? -1 : m_sample.GetPercentile(.95f);
		s.m_nPingNtile98th = m_sample.NumSamples() < 50 ? -1 : m_sample.GetPercentile(.98f);
	}
#endif
};




struct SteamDatagramLinkLifetimeStats
{
	/// Reset all values to zero / unknown status


	int m_nConnectedSeconds; // -1 if we don't track it

	//
	// Lifetime counters.
	// NOTE: Average packet loss, etc can be deduced from this.
	//
	int64 m_nPacketsSent;
	int64 m_nBytesSent;
	int64 m_nPacketsRecv; // total number of packets received, some of which might not have had a sequence number.  Don't use this number to try to estimate lifetime packet loss, use m_nPacketsRecvSequenced
	int64 m_nBytesRecv;
	int64 m_nPktsRecvSequenced; // packets that we received that had a sequence number.
	int64 m_nPktsRecvDropped;
	int64 m_nPktsRecvOutOfOrder;
	int64 m_nPktsRecvDuplicate;
	int64 m_nPktsRecvSequenceNumberLurch;

	// SNP message counters
	int64 m_nMessagesSentReliable;
	int64 m_nMessagesSentUnreliable;
	int64 m_nMessagesRecvReliable;
	int64 m_nMessagesRecvUnreliable;

	// Ping distribution
	PingHistogram m_pingHistogram;

	// Distribution.
	// NOTE: Some of these might be -1 if we didn't have enough data to make a meaningful estimate!
	// It takes fewer samples to make an estimate of the median than the 98th percentile!
	short m_nPingNtile5th; // 5% of ping samples were <= Nms
	short m_nPingNtile50th; // 50% of ping samples were <= Nms
	short m_nPingNtile75th; // 70% of ping samples were <= Nms
	short m_nPingNtile95th; // 95% of ping samples were <= Nms
	short m_nPingNtile98th; // 98% of ping samples were <= Nms
	short m__pad1;


	//
	// Connection quality distribution
	//
	QualityHistogram m_qualityHistogram;

	// Distribution.  Some might be -1, see above for why.
	short m_nQualityNtile2nd; // 2% of measurement intervals had quality <= N%
	short m_nQualityNtile5th; // 5% of measurement intervals had quality <= N%
	short m_nQualityNtile25th; // 25% of measurement intervals had quality <= N%
	short m_nQualityNtile50th; // 50% of measurement intervals had quality <= N%

	// Jitter histogram
	JitterHistogram m_jitterHistogram;

	//
	// Connection transmit speed histogram
	//
	int m_nTXSpeedMax; // Max speed we hit

	int m_nTXSpeedHistogram16; // Speed at kb/s
	int m_nTXSpeedHistogram32;
	int m_nTXSpeedHistogram64;
	int m_nTXSpeedHistogram128;
	int m_nTXSpeedHistogram256;
	int m_nTXSpeedHistogram512;
	int m_nTXSpeedHistogram1024;
	int m_nTXSpeedHistogramMax;


	// Distribution.  Some might be -1, see above for why.
	int m_nTXSpeedNtile5th; // 5% of transmit samples were <= N kb/s
	int m_nTXSpeedNtile50th; // 50% of transmit samples were <= N kb/s 
	int m_nTXSpeedNtile75th; // 75% of transmit samples were <= N kb/s 
	int m_nTXSpeedNtile95th; // 95% of transmit samples were <= N kb/s 
	int m_nTXSpeedNtile98th; // 98% of transmit samples were <= N kb/s 

	//
	// Connection receive speed histogram
	//
	int m_nRXSpeedMax; // Max speed we hit that formed the histogram

	int m_nRXSpeedHistogram16; // Speed at kb/s
	int m_nRXSpeedHistogram32;
	int m_nRXSpeedHistogram64;
	int m_nRXSpeedHistogram128;
	int m_nRXSpeedHistogram256;
	int m_nRXSpeedHistogram512;
	int m_nRXSpeedHistogram1024;
	int m_nRXSpeedHistogramMax;


	// Distribution.  Some might be -1, see above for why.
	int m_nRXSpeedNtile5th; // 5% of transmit samples were <= N kb/s
	int m_nRXSpeedNtile50th; // 50% of transmit samples were <= N kb/s 
	int m_nRXSpeedNtile75th; // 75% of transmit samples were <= N kb/s 
	int m_nRXSpeedNtile95th; // 95% of transmit samples were <= N kb/s 
	int m_nRXSpeedNtile98th; // 98% of transmit samples were <= N kb/s 

};


struct SteamDatagramLinkInstantaneousStats
{

	/// Data rates
	float m_flOutPacketsPerSec;
	float m_flOutBytesPerSec;
	float m_flInPacketsPerSec;
	float m_flInBytesPerSec;

	/// Smoothed ping.  This will be -1 if we don't have any idea!
	int m_nPingMS;

	/// 0...1, estimated number of packets that were sent to us, but we failed to receive.
	/// <0 if we haven't received any sequenced packets and so we don't have any way to estimate this.
	float m_flPacketsDroppedPct;

	/// Packets received with a sequence number abnormality, other than basic packet loss.  (Duplicated, out of order, lurch.)
	/// <0 if we haven't received any sequenced packets and so we don't have any way to estimate this.
	float m_flPacketsWeirdSequenceNumberPct;

	/// Peak jitter
	int m_usecMaxJitter;

	/// Current sending rate, this can be low at connection start until the slow start
	/// ramps it up.  It's adjusted as packets are lost and congestion is encountered during
	/// the connection
	int m_nSendRate;

	/// How many pending bytes are waiting to be sent.  This is data that is currently waiting 
	/// to be sent and in outgoing buffers.  If this is zero, then the connection is idle
	/// and all pending data has been sent.  Note that in case of packet loss any pending
	/// reliable data might be re-sent.  This does not include data that has been sent and is
	/// waiting for acknowledgment.
	int m_nPendingBytes;

};

/// Link stats.  Pretty much everything you might possibly want to know about the connection
struct SteamDatagramLinkStats
{

	/// Latest instantaneous stats, calculated locally
	SteamDatagramLinkInstantaneousStats m_latest;

	/// Peak values for each instantaneous stat
	//SteamDatagramLinkInstantaneousStats m_peak;

	/// Lifetime stats, calculated locally
	SteamDatagramLinkLifetimeStats m_lifetime;

	/// Latest instantaneous stats received from remote host.
	/// (E.g. "sent" means they are reporting what they sent.)
	SteamDatagramLinkInstantaneousStats m_latestRemote;

	/// How many seconds ago did we receive m_latestRemote?
	/// This will be <0 if the data is not valid!
	float m_flAgeLatestRemote;

	/// Latest lifetime stats received from remote host.
	SteamDatagramLinkLifetimeStats m_lifetimeRemote;

	/// How many seconds ago did we receive the lifetime stats?
	/// This will be <0 if the data is not valid!
	float m_flAgeLifetimeRemote;

	/// Reset everything to unknown/initial state.

};


struct SequencedPacketCounters
{
	int m_nRecv; // packets successfully received containing a sequence number
	int m_nDropped; // packets assumed to be dropped in the current interval
	int m_nOutOfOrder; // any sequence number deviation other than a simple dropped packet.  (Most recent interval.)
	int m_nLurch; // any sequence number deviation other than a simple dropped packet.  (Most recent interval.)
	int m_nDuplicate; // any sequence number deviation other than a simple dropped packet.  (Most recent interval.)
	int m_usecMaxJitter;

};


struct LinkStatsTrackerBase
{
	/// What version is the peer running?  It's 0 if we don't know yet.
	uint32 m_nPeerProtocolVersion;

	/// Ping
	PingTrackerDetailed m_ping;

	//
	// Outgoing stats
	//
	int64 m_nNextSendSequenceNumber;
	PacketRate_t m_sent;
	SteamNetworkingMicroseconds m_usecTimeLastSentSeq;

	/// Called when we sent a packet, with or without a sequence number
	inline void TrackSentPacket(int cbPktSize)
	{
		m_sent.ProcessPacket(cbPktSize);
	}

	/// Consume the next sequence number, and record the time at which
	/// we sent a sequenced packet.  (Don't call this unless you are sending
	/// a sequenced packet.)
	inline uint16 ConsumeSendPacketNumberAndGetWireFmt(SteamNetworkingMicroseconds usecNow)
	{
		m_usecTimeLastSentSeq = usecNow;
		return uint16(m_nNextSendSequenceNumber++);
	}

	//
	// Incoming
	//

	/// Highest (valid!) packet number we have ever processed
	int64 m_nMaxRecvPktNum;

	/// Packet and data rate trackers for inbound flow
	PacketRate_t m_recv;

	// Some additional debugging for sequence number accounting
	int64 m_nDebugLastInitMaxRecvPktNum;
	int64 m_nDebugPktsRecvInOrder;
	int64 m_arDebugHistoryRecvSeqNum[8];

	/// Setup state to expect the next packet to be nPktNum+1,
	/// and discard all packets <= nPktNum
	void InitMaxRecvPktNum(int64 nPktNum);
	void ResetMaxRecvPktNumForIncomingWirePktNum(uint16 nPktNum)
	{
		InitMaxRecvPktNum((int64)(uint16)(nPktNum - 1));
	}

	/// Bitmask of recently received packets, used to reject duplicate packets.
	/// (Important for preventing replay attacks.)
	///
	/// Let B be m_nMaxRecvPktNum & ~63.  (The largest multiple of 64
	/// that is <= m_nMaxRecvPktNum.)   Then m_recvPktNumberMask[1] bit n
	/// corresponds to B + n.  (Some of these bits may represent packet numbers
	/// higher than m_nMaxRecvPktNum.)  m_recvPktNumberMask[0] bit n
	/// corresponds to B - 64 + n.
	uint64 m_recvPktNumberMask[2];

	/// Get string describing state of recent packets received.
	std::string RecvPktNumStateDebugString() const;

	/// Packets that we receive that exceed the rate limit.
	/// (We might drop these, or we might just want to be interested in how often it happens.)
	PacketRate_t m_recvExceedRateLimit;

	/// Time when we last received anything
	SteamNetworkingMicroseconds m_usecTimeLastRecv;

	/// Time when we last received a sequenced packet
	SteamNetworkingMicroseconds m_usecTimeLastRecvSeq;

	/// Called when we receive any packet, with or without a sequence number.
	/// Does not perform any rate limiting checks
	inline void TrackRecvPacket(int cbPktSize, SteamNetworkingMicroseconds usecNow)
	{
		m_recv.ProcessPacket(cbPktSize);
		m_usecTimeLastRecv = usecNow;
		m_usecInFlightReplyTimeout = 0;
		m_nReplyTimeoutsSinceLastRecv = 0;
		m_usecWhenTimeoutStarted = 0;
	}

	//
	// Quality metrics stats
	//

	// Track instantaneous rate of number of sequence number anomalies
	SequencedPacketCounters m_seqPktCounters;

	// Instantaneous rates, calculated from most recent completed interval
	float m_flInPacketsDroppedPct;
	float m_flInPacketsWeirdSequencePct;
	int m_usecMaxJitterPreviousInterval;

	// Lifetime counters.  The "accumulator" values do not include the current interval -- use the accessors to get those
	int64 m_nPktsRecvSequenced;
	int64 m_nPktsRecvDroppedAccumulator;
	int64 m_nPktsRecvOutOfOrderAccumulator;
	int64 m_nPktsRecvDuplicateAccumulator;
	int64 m_nPktsRecvLurchAccumulator;
	inline int64 PktsRecvDropped() const { return m_nPktsRecvDroppedAccumulator + m_seqPktCounters.m_nDropped; }
	inline int64 PktsRecvOutOfOrder() const { return m_nPktsRecvOutOfOrderAccumulator + m_seqPktCounters.m_nOutOfOrder; }
	inline int64 PktsRecvDuplicate() const { return m_nPktsRecvDuplicateAccumulator + m_seqPktCounters.m_nDuplicate; }
	inline int64 PktsRecvLurch() const { return m_nPktsRecvLurchAccumulator + m_seqPktCounters.m_nLurch; }

	/// Lifetime quality statistics
	PercentileGenUint16 m_qualitySample;

	/// Histogram of quality intervals
	QualityHistogram m_qualityHistogram;

	// Histogram of incoming latency variance
	JitterHistogram m_jitterHistogram;

	//
	// Misc stats bookkeeping
	//

	/// Check if it's been long enough since the last time we sent a ping,
	/// and we'd like to try to sneak one in if possible.
	///
	/// Note that in general, tracer pings are the only kind of pings that the relay
	/// ever sends.  It assumes that the endpoints will take care of any keepalives,
	/// etc that need to happen, and the relay can merely observe this process and take
	/// note of the outcome.
	///
	/// Returns:
	/// 0 - Not needed right now
	/// 1 - Opportunistic, but don't send by itself
	/// 2 - Yes, send one if possible
#if 0
	inline int ReadyToSendTracerPing(SteamNetworkingMicroseconds usecNow) const
	{
		if (m_bPassive)
			return 0;
		SteamNetworkingMicroseconds usecTimeSince = usecNow - std::max(m_ping.m_usecTimeLastSentPingRequest, m_ping.TimeRecvMostRecentPing());
		if (usecTimeSince > k_usecLinkStatsMaxPingRequestInterval)
			return 2;
		if (usecTimeSince > k_usecLinkStatsMinPingRequestInterval)
			return 1;
		return 0;
	}

	/// Check if we appear to be timing out and need to send an "aggressive" ping, meaning send it right
	/// now, request that the reply not be delayed, and also request that the relay (if any) confirm its
	/// connectivity as well.
	inline bool BNeedToSendPingImmediate(SteamNetworkingMicroseconds usecNow) const
	{
		return
			!m_bPassive
			&& m_nReplyTimeoutsSinceLastRecv > 0 // We're timing out
			&& m_usecLastSendPacketExpectingImmediateReply + k_usecAggressivePingInterval <= usecNow; // we haven't just recently sent an aggressive ping.
	}

	/// Check if we should send a keepalive ping.  In this case we haven't heard from the peer in a while,
	/// but we don't have any reason to think there are any problems.
	inline bool BNeedToSendKeepalive(SteamNetworkingMicroseconds usecNow) const
	{
		return
			!m_bPassive
			&& m_usecInFlightReplyTimeout == 0 // not already tracking some other message for which we expect a reply (and which would confirm that the connection is alive)
			&& m_usecTimeLastRecv + k_usecKeepAliveInterval <= usecNow; // haven't heard from the peer recently
	}

	/// Fill out message with everything we'd like to send.  We don't assume that we will
	/// actually send it.  (We might be looking for a good opportunity, and the data we want
	/// to send doesn't fit.)
	void PopulateMessage(int nNeedFlags, CMsgSteamDatagramConnectionQuality& msg, SteamNetworkingMicroseconds usecNow);
	void PopulateLifetimeMessage(CMsgSteamDatagramLinkLifetimeStats& msg);
	/// Called when we send any message for which we expect some sort of reply.  (But maybe not an ack.)
	void TrackSentMessageExpectingReply(SteamNetworkingMicroseconds usecNow, bool bAllowDelayedReply);

	/// Called when we receive stats from remote host
	void ProcessMessage(const CMsgSteamDatagramConnectionQuality& msg, SteamNetworkingMicroseconds usecNow);
#endif

	/// Received from remote host
	SteamDatagramLinkInstantaneousStats m_latestRemote;
	SteamNetworkingMicroseconds m_usecTimeRecvLatestRemote;
	SteamDatagramLinkLifetimeStats m_lifetimeRemote;
	SteamNetworkingMicroseconds m_usecTimeRecvLifetimeRemote;

	int64 m_pktNumInFlight;
	bool m_bInFlightInstantaneous;
	bool m_bInFlightLifetime;

	/// Time when the current interval started
	SteamNetworkingMicroseconds m_usecIntervalStart;

	//
	// Reply timeout
	//

	/// If we have a message in flight for which we expect a reply (possibly delayed)
	/// and we haven't heard ANYTHING back, then this is the time when we should
	/// declare a timeout (and increment m_nReplyTimeoutsSinceLastRecv)
	SteamNetworkingMicroseconds m_usecInFlightReplyTimeout;

	/// Time when we last sent some sort of packet for which we expect
	/// an immediate reply.  m_stats.m_ping and m_usecInFlightReplyTimeout both
	/// remember when we send requests that expect replies, but both include
	/// ones that we allow the reply to be delayed.  This timestamp only includes
	/// ones that we do not allow to be delayed.
	SteamNetworkingMicroseconds m_usecLastSendPacketExpectingImmediateReply;

	/// Number of consecutive times a reply from this guy has timed out, since
	/// the last time we got valid communication from him.  This is reset basically
	/// any time we get a packet from the peer.
	int m_nReplyTimeoutsSinceLastRecv;

	/// Time when the current timeout (if any) was first detected.  This is not
	/// the same thing as the time we last heard from them.  For a mostly idle
	/// connection, the keepalive interval is relatively sparse, and so we don't
	/// know if we didn't hear from them, was it because there was a problem,
	/// or just they had nothing to say.  This timestamp measures the time when
	/// we expected to heard something but didn't.
	SteamNetworkingMicroseconds m_usecWhenTimeoutStarted;

#if 0
	//
	// Populate public interface structure
	//
	void GetLinkStats(SteamDatagramLinkStats& s, SteamNetworkingMicroseconds usecNow) const;

	/// This is the only function we needed to make virtual.  To factor this one
	/// out is really awkward, and this isn't called very often anyway.
	virtual void GetLifetimeStats(SteamDatagramLinkLifetimeStats& s) const;

	inline void PeerAckedInstantaneous(SteamNetworkingMicroseconds usecNow)
	{
		m_usecPeerAckedInstaneous = usecNow;
		m_nPktsRecvSeqWhenPeerAckInstantaneous = m_nPktsRecvSequenced;
		m_nPktsSentWhenPeerAckInstantaneous = m_sent.m_packets.Total();
	}
	inline void PeerAckedLifetime(SteamNetworkingMicroseconds usecNow)
	{
		m_usecPeerAckedLifetime = usecNow;
		m_nPktsRecvSeqWhenPeerAckLifetime = m_nPktsRecvSequenced;
		m_nPktsSentWhenPeerAckLifetime = m_sent.m_packets.Total();
	}

	void InFlightPktAck(SteamNetworkingMicroseconds usecNow)
	{
		if (m_bInFlightInstantaneous)
			PeerAckedInstantaneous(usecNow);
		if (m_bInFlightLifetime)
			PeerAckedLifetime(usecNow);
		m_pktNumInFlight = 0;
		m_bInFlightInstantaneous = m_bInFlightLifetime = false;
	}

	void InFlightPktTimeout()
	{
		m_pktNumInFlight = 0;
		m_bInFlightInstantaneous = m_bInFlightLifetime = false;
	}

	/// Get urgency level to send instantaneous/lifetime stats.
	int GetStatsSendNeed(SteamNetworkingMicroseconds usecNow);

	/// Describe this stats tracker, for debugging, asserts, etc
	virtual std::string Describe() const = 0;

#endif



protected:
#if 0
	// Make sure it's used as abstract base.  Note that we require you to call Init()
	// with a timestamp value, so the constructor is empty by default.
	inline LinkStatsTrackerBase() {}

	/// Initialize the stats tracking object
	void InitInternal(SteamNetworkingMicroseconds usecNow);

	/// Check if it's time to update, and if so, do it.
	template <typename TLinkStatsTracker>
	inline static void ThinkInternal(TLinkStatsTracker* pThis, SteamNetworkingMicroseconds usecNow)
	{
		// Check for ending the current QoS interval
		if (!pThis->m_bPassive && pThis->m_usecIntervalStart + k_usecSteamDatagramLinkStatsDefaultInterval < usecNow)
		{
			pThis->UpdateInterval(usecNow);
		}

		// Check for reply timeout.
		if (pThis->m_usecInFlightReplyTimeout > 0 && pThis->m_usecInFlightReplyTimeout < usecNow)
		{
			pThis->InFlightReplyTimeout(usecNow);
		}
	}

	/// Called when m_usecInFlightReplyTimeout is reached.  We intentionally only allow
	/// one of this type of timeout to be in flight at a time, so that the max
	/// rate that we accumulate them is based on the ping time, instead of the packet
	/// rate.
	template <typename TLinkStatsTracker>
	inline static void InFlightReplyTimeoutInternal(TLinkStatsTracker* pThis, SteamNetworkingMicroseconds usecNow)
	{
		pThis->m_usecInFlightReplyTimeout = 0;
		if (pThis->m_usecWhenTimeoutStarted == 0)
		{
			Assert(pThis->m_nReplyTimeoutsSinceLastRecv == 0);
			pThis->m_usecWhenTimeoutStarted = usecNow;
		}
		++pThis->m_nReplyTimeoutsSinceLastRecv;
	}

	void GetInstantaneousStats(SteamDatagramLinkInstantaneousStats& s) const;

	/// Called after we send a packet for which we expect an ack.  Note that we must have consumed the outgoing sequence
	/// for that packet (using GetNextSendSequenceNumber), but must *NOT* have consumed any more!
	/// This call implies TrackSentPingRequest, since we will be able to match up the ack'd sequence
	/// number with the time sent to get a latency estimate.
	template <typename TLinkStatsTracker>
	inline static void TrackSentMessageExpectingSeqNumAckInternal(TLinkStatsTracker* pThis, SteamNetworkingMicroseconds usecNow, bool bAllowDelayedReply)
	{
		pThis->TrackSentPingRequest(usecNow, bAllowDelayedReply);
	}
#endif

	/// Are we in "passive" state?  When we are "active", we expect that our peer is awake
	/// and will reply to our messages, and that we should be actively sending our peer
	/// connection quality statistics and keepalives.  When we are passive, we still measure
	/// statistics and can receive messages from the peer, and send acknowledgments as necessary.
	/// but we will indicate that keepalives or stats need to be sent to the peer.
	bool m_bPassive;

#if 0
	/// Called to switch the passive state.  (Should only be called on an actual state change.)
	void SetPassiveInternal(bool bFlag, SteamNetworkingMicroseconds usecNow);

	/// Check if we really need to flush out stats now.  Derived class should provide the reason strings.
	/// (See the code.)
	const char* InternalGetSendStatsReasonOrUpdateNextThinkTime(SteamNetworkingMicroseconds usecNow, const char* const arpszReasonStrings[4], SteamNetworkingMicroseconds& inOutNextThinkTime);

	/// Called when we send a packet for which we expect a reply and
	/// for which we expect to get latency info.
	/// This implies TrackSentMessageExpectingReply.
	template <typename TLinkStatsTracker>
	inline static void TrackSentPingRequestInternal(TLinkStatsTracker* pThis, SteamNetworkingMicroseconds usecNow, bool bAllowDelayedReply)
	{
		pThis->TrackSentMessageExpectingReply(usecNow, bAllowDelayedReply);
		pThis->m_ping.m_usecTimeLastSentPingRequest = usecNow;
	}

	/// Called when we receive a reply from which we are able to calculate latency information
	template <typename TLinkStatsTracker>
	inline static void ReceivedPingInternal(TLinkStatsTracker* pThis, int nPingMS, SteamNetworkingMicroseconds usecNow)
	{
		pThis->m_ping.ReceivedPing(nPingMS, usecNow);
	}

	inline bool BInternalNeedToSendPingImmediate(SteamNetworkingMicroseconds usecNow, SteamNetworkingMicroseconds& inOutNextThinkTime)
	{
		if (m_nReplyTimeoutsSinceLastRecv == 0)
			return false;
		SteamNetworkingMicroseconds usecUrgentPing = m_usecLastSendPacketExpectingImmediateReply + k_usecAggressivePingInterval;
		if (usecUrgentPing <= usecNow)
			return true;
		if (usecUrgentPing < inOutNextThinkTime)
			inOutNextThinkTime = usecUrgentPing;
		return false;
	}

	inline bool BInternalNeedToSendKeepAlive(SteamNetworkingMicroseconds usecNow, SteamNetworkingMicroseconds& inOutNextThinkTime)
	{
		if (m_usecInFlightReplyTimeout == 0)
		{
			SteamNetworkingMicroseconds usecKeepAlive = m_usecTimeLastRecv + k_usecKeepAliveInterval;
			if (usecKeepAlive <= usecNow)
				return true;
			if (usecKeepAlive < inOutNextThinkTime)
				inOutNextThinkTime = usecKeepAlive;
		}
		else
		{
			if (m_usecInFlightReplyTimeout < inOutNextThinkTime)
				inOutNextThinkTime = m_usecInFlightReplyTimeout;
		}
		return false;
	}

	// Hooks that derived classes may override when we process a packet
	// and it meets certain characteristics
	inline void InternalProcessSequencedPacket_Count()
	{
		m_seqPktCounters.OnRecv();
		++m_nPktsRecvSequenced;
	}
	void InternalProcessSequencedPacket_OutOfOrder(int64 nPktNum);
	inline void InternalProcessSequencedPacket_Duplicate()
	{
		m_seqPktCounters.OnDuplicate();
	}
	inline void InternalProcessSequencedPacket_Lurch()
	{
		m_seqPktCounters.OnLurch();
	}
	inline void InternalProcessSequencedPacket_Dropped(int nDropped)
	{
		m_seqPktCounters.OnDropped(nDropped);
	}
#endif
private:

	// Number of lifetime sequenced packets received, and overall packets sent,
	// the last time the peer acked stats
	int64 m_nPktsRecvSeqWhenPeerAckInstantaneous;
	int64 m_nPktsSentWhenPeerAckInstantaneous;
	int64 m_nPktsRecvSeqWhenPeerAckLifetime;
	int64 m_nPktsSentWhenPeerAckLifetime;

	/// Local time when peer last acknowledged lifetime stats.
	SteamNetworkingMicroseconds m_usecPeerAckedLifetime;

	/// Local time when peer last acknowledged instantaneous stats.
	SteamNetworkingMicroseconds m_usecPeerAckedInstaneous;

	bool BCheckHaveDataToSendInstantaneous(SteamNetworkingMicroseconds usecNow);
	bool BCheckHaveDataToSendLifetime(SteamNetworkingMicroseconds usecNow);

	/// Called to force interval to roll forward now
	void UpdateInterval(SteamNetworkingMicroseconds usecNow);

	void StartNextInterval(SteamNetworkingMicroseconds usecNow);
};

struct LinkStatsTrackerEndToEnd : public LinkStatsTrackerBase
{


	/// Time when the connection entered the connection state
	SteamNetworkingMicroseconds m_usecWhenStartedConnectedState;

	/// Time when the connection ended
	SteamNetworkingMicroseconds m_usecWhenEndedConnectedState;

	/// Time when the current interval started
	SteamNetworkingMicroseconds m_usecSpeedIntervalStart;

	/// TX Speed, should match CMsgSteamDatagramLinkLifetimeStats 
	int m_nTXSpeed;
	int m_nTXSpeedMax;
	PercentileGenInt m_TXSpeedSample;
	int m_nTXSpeedHistogram16; // Speed at kb/s
	int m_nTXSpeedHistogram32;
	int m_nTXSpeedHistogram64;
	int m_nTXSpeedHistogram128;
	int m_nTXSpeedHistogram256;
	int m_nTXSpeedHistogram512;
	int m_nTXSpeedHistogram1024;
	int m_nTXSpeedHistogramMax;

	/// RX Speed, should match CMsgSteamDatagramLinkLifetimeStats 
	int m_nRXSpeed;
	int m_nRXSpeedMax;
	PercentileGenInt m_RXSpeedSample;
	int m_nRXSpeedHistogram16; // Speed at kb/s
	int m_nRXSpeedHistogram32;
	int m_nRXSpeedHistogram64;
	int m_nRXSpeedHistogram128;
	int m_nRXSpeedHistogram256;
	int m_nRXSpeedHistogram512;
	int m_nRXSpeedHistogram1024;
	int m_nRXSpeedHistogramMax;

	/// Called when we get a speed sample
	void UpdateSpeeds(int nTXSpeed, int nRXSpeed);

};


template <typename TLinkStatsTracker>
struct LinkStatsTracker final : public TLinkStatsTracker
{

	// "Virtual functions" that we are "overriding" at compile time
	// by the template argument
	inline void Init(SteamNetworkingMicroseconds usecNow, bool bStartDisconnected = false)
	{
		TLinkStatsTracker::InitInternal(usecNow);
		TLinkStatsTracker::SetPassiveInternal(bStartDisconnected, usecNow);
	}
	inline void Think(SteamNetworkingMicroseconds usecNow) { TLinkStatsTracker::ThinkInternal(this, usecNow); }
	inline void SetPassive(bool bFlag, SteamNetworkingMicroseconds usecNow) { if (TLinkStatsTracker::m_bPassive != bFlag) TLinkStatsTracker::SetPassiveInternal(bFlag, usecNow); }
	inline bool IsPassive() const { return TLinkStatsTracker::m_bPassive; }
	inline void TrackSentMessageExpectingSeqNumAck(SteamNetworkingMicroseconds usecNow, bool bAllowDelayedReply) { TLinkStatsTracker::TrackSentMessageExpectingSeqNumAckInternal(this, usecNow, bAllowDelayedReply); }
	inline void TrackSentPingRequest(SteamNetworkingMicroseconds usecNow, bool bAllowDelayedReply) { TLinkStatsTracker::TrackSentPingRequestInternal(this, usecNow, bAllowDelayedReply); }
	inline void ReceivedPing(int nPingMS, SteamNetworkingMicroseconds usecNow) { TLinkStatsTracker::ReceivedPingInternal(this, nPingMS, usecNow); }
	inline void InFlightReplyTimeout(SteamNetworkingMicroseconds usecNow) { TLinkStatsTracker::InFlightReplyTimeoutInternal(this, usecNow); }
#if 0
	/// Called after we actually send connection data.  Note that we must have consumed the outgoing sequence
	/// for that packet (using GetNextSendSequenceNumber), but must *NOT* have consumed any more!
	void TrackSentStats(const CMsgSteamDatagramConnectionQuality& msg, SteamNetworkingMicroseconds usecNow, bool bAllowDelayedReply)
	{

		// Check if we expect our peer to know how to acknowledge this
		if (!TLinkStatsTracker::m_bPassive)
		{
			TLinkStatsTracker::m_pktNumInFlight = TLinkStatsTracker::m_nNextSendSequenceNumber - 1;
			TLinkStatsTracker::m_bInFlightInstantaneous = msg.has_instantaneous();
			TLinkStatsTracker::m_bInFlightLifetime = msg.has_lifetime();

			// They should ack.  Make a note of the sequence number that we used,
			// so that we can measure latency when they reply, setup timeout bookkeeping, etc
			TrackSentMessageExpectingSeqNumAck(usecNow, bAllowDelayedReply);
		}
		else
		{
			// Peer can't ack.  Just mark them as acking immediately
			Assert(TLinkStatsTracker::m_pktNumInFlight == 0);
			TLinkStatsTracker::m_pktNumInFlight = 0;
			TLinkStatsTracker::m_bInFlightInstantaneous = false;
			TLinkStatsTracker::m_bInFlightLifetime = false;
			if (msg.has_instantaneous())
				TLinkStatsTracker::PeerAckedInstantaneous(usecNow);
			if (msg.has_lifetime())
				TLinkStatsTracker::PeerAckedLifetime(usecNow);
		}
	}

	inline bool RecvPackedAcks(const google::protobuf::RepeatedField<google::protobuf::uint32>& msgField, SteamNetworkingMicroseconds usecNow)
	{
		bool bResult = true;
		for (uint32 nPackedAck : msgField)
		{
			if (!TLinkStatsTracker::RecvPackedAckInternal(this, nPackedAck, usecNow))
				bResult = false;
		}
		return bResult;
	}

	// Shortcut when we know that we aren't going to send now, but we want to know when to wakeup and do so
	inline SteamNetworkingMicroseconds GetNextThinkTime(SteamNetworkingMicroseconds usecNow)
	{
		SteamNetworkingMicroseconds usecNextThink = k_nThinkTime_Never;
		EStatsReplyRequest eReplyRequested;
		if (TLinkStatsTracker::GetSendReasonOrUpdateNextThinkTime(usecNow, eReplyRequested, usecNextThink))
			return k_nThinkTime_ASAP;
		return usecNextThink;
	}

	/// Called when we receive a packet with a sequence number.
	/// This expands the wire packet number to its full value,
	/// and checks if it is a duplicate or out of range.
	/// Stats are also updated
	int64 ExpandWirePacketNumberAndCheck(uint16 nWireSeqNum)
	{
		int16 nGap = (int16)(nWireSeqNum - (uint16)TLinkStatsTracker::m_nMaxRecvPktNum);
		int64 nPktNum = TLinkStatsTracker::m_nMaxRecvPktNum + nGap;

		// We've received a packet with a sequence number.
		// Update stats
		constexpr int N = V_ARRAYSIZE(TLinkStatsTracker::m_arDebugHistoryRecvSeqNum);
		COMPILE_TIME_ASSERT((N & (N - 1)) == 0);
		TLinkStatsTracker::m_arDebugHistoryRecvSeqNum[TLinkStatsTracker::m_nPktsRecvSequenced & (N - 1)] = nPktNum;
		TLinkStatsTracker::InternalProcessSequencedPacket_Count();

		// Packet number is increasing?
		// (Maybe by a lot -- we don't handle that here.)
		if (likely(nPktNum > TLinkStatsTracker::m_nMaxRecvPktNum))
			return nPktNum;

		// Which block of 64-bit packets is it in?
		int64 B = TLinkStatsTracker::m_nMaxRecvPktNum & ~int64{ 63 };
		int64 idxRecvBitmask = ((nPktNum - B) >> 6) + 1;
		Assert(idxRecvBitmask < 2);
		if (idxRecvBitmask < 0)
		{
			// Too old (at least 64 packets old, maybe up to 128).
			TLinkStatsTracker::InternalProcessSequencedPacket_Lurch(); // Should we track "very old" under a different stat than "lurch"?
			return 0;
		}
		uint64 bit = uint64{ 1 } << (nPktNum & 63);
		if (TLinkStatsTracker::m_recvPktNumberMask[idxRecvBitmask] & bit)
		{
			// Duplicate
			TLinkStatsTracker::InternalProcessSequencedPacket_Duplicate();
			return 0;
		}

		// We have an out of order packet.  We'll update that
		// stat in TrackProcessSequencedPacket
		Assert(nPktNum > 0 && nPktNum < TLinkStatsTracker::m_nMaxRecvPktNum);
		return nPktNum;
	}

	/// Same as ExpandWirePacketNumberAndCheck, but if this is the first sequenced
	/// packet we have ever received, initialize the packet number
	int64 ExpandWirePacketNumberAndCheckMaybeInitialize(uint16 nWireSeqNum)
	{
		if (unlikely(TLinkStatsTracker::m_nMaxRecvPktNum == 0))
			TLinkStatsTracker::ResetMaxRecvPktNumForIncomingWirePktNum(nWireSeqNum);
		return ExpandWirePacketNumberAndCheck(nWireSeqNum);
	}

	/// Called when we have processed a packet with a sequence number, to update estimated
	/// number of dropped packets, etc.  This MUST only be called after we have
	/// called ExpandWirePacketNumberAndCheck, to ensure that the packet number is not a
	/// duplicate or out of range.
	inline void TrackProcessSequencedPacket(int64 nPktNum, SteamNetworkingMicroseconds usecNow, int usecSenderTimeSincePrev)
	{


		// Update bitfield of received packets
		int64 B = TLinkStatsTracker::m_nMaxRecvPktNum & ~int64{ 63 };
		int64 idxRecvBitmask = ((nPktNum - B) >> 6) + 1;
		Assert(idxRecvBitmask >= 0); // We should have discarded very old packets already
		if (idxRecvBitmask >= 2) // Most common case is 0 or 1
		{
			if (idxRecvBitmask == 2)
			{
				// Crossed to the next 64-packet block.  Shift bitmasks forward by one.
				TLinkStatsTracker::m_recvPktNumberMask[0] = TLinkStatsTracker::m_recvPktNumberMask[1];
			}
			else
			{
				// Large packet number jump, we skipped a whole block
				TLinkStatsTracker::m_recvPktNumberMask[0] = 0;
			}
			TLinkStatsTracker::m_recvPktNumberMask[1] = 0;
			idxRecvBitmask = 1;
		}
		uint64 bit = uint64{ 1 } << (nPktNum & 63);
		Assert(!(TLinkStatsTracker::m_recvPktNumberMask[idxRecvBitmask] & bit)); // Should not have already been marked!  We should have already discarded duplicates
		TLinkStatsTracker::m_recvPktNumberMask[idxRecvBitmask] |= bit;

		// Check for dropped packet.  Since we hope that by far the most common
		// case will be packets delivered in order, we optimize this logic
		// for that case.
		int64 nGap = nPktNum - TLinkStatsTracker::m_nMaxRecvPktNum;
		if (likely(nGap == 1))
		{
			++TLinkStatsTracker::m_nDebugPktsRecvInOrder;

			// We've received two packets, in order.  Did the sender supply the time between packets on his side?
			if (usecSenderTimeSincePrev > 0)
			{
				int usecJitter = (usecNow - TLinkStatsTracker::m_usecTimeLastRecvSeq) - usecSenderTimeSincePrev;
				usecJitter = abs(usecJitter);
				if (usecJitter < k_usecTimeSinceLastPacketMaxReasonable)
				{

					// Update max jitter for current interval
					TLinkStatsTracker::m_seqPktCounters.m_usecMaxJitter = std::max(TLinkStatsTracker::m_seqPktCounters.m_usecMaxJitter, usecJitter);
					TLinkStatsTracker::m_jitterHistogram.AddSample(usecJitter);
				}
				else
				{
					// Something is really, really off.  Discard measurement
				}
			}

		}
		else if (unlikely(nGap <= 0))
		{
			// Packet number moving backward
			// We should have already rejected duplicates
			Assert(nGap != 0);

			// Packet number moving in reverse.
			// It should be a *small* negative step, e.g. packets delivered out of order.
			// If the packet is really old, we should have already discarded it earlier.
			Assert(nGap >= -8 * (int64)sizeof(TLinkStatsTracker::m_recvPktNumberMask));

			// out of order
			TLinkStatsTracker::InternalProcessSequencedPacket_OutOfOrder(nPktNum);
			return;
		}
		else
		{
			// Packet number moving forward, i.e. a dropped packet
			// Large gap?
			if (unlikely(nGap >= 100))
			{
				// Very weird.
				TLinkStatsTracker::InternalProcessSequencedPacket_Lurch();

				// Reset the sequence number for packets going forward.
				TLinkStatsTracker::InitMaxRecvPktNum(nPktNum);
				return;
			}

			// Probably the most common case (after a perfect packet stream), we just dropped a packet or two
			TLinkStatsTracker::InternalProcessSequencedPacket_Dropped(nGap - 1);
		}

		// Save highest known sequence number for next time.
		TLinkStatsTracker::m_nMaxRecvPktNum = nPktNum;
		TLinkStatsTracker::m_usecTimeLastRecvSeq = usecNow;
	}

#endif

};
