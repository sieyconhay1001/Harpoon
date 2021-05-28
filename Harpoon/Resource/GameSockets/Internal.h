#pragma once
#include <steam/isteamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#include <tier0/t0constants.h>
#include <cstdint>
#include <stdio.h>
#include <climits>

struct SteamDatagramLinkStats;
struct SteamDatagramLinkLifetimeStats;
struct SteamDatagramLinkInstantaneousStats;
struct SteamNetworkingDetailedConnectionStatus;

// An identity operator that always returns its operand.
// NOTE: std::hash is an identity operator on many compilers
//       for the basic primitives.  If you really need actual
//       hashing, don't use std::hash!
template <typename T >
struct Identity
{
	 const T &operator()( const T &x ) const { return x; }
};

/// Max size of UDP payload.  Includes API payload and
/// any headers, but does not include IP/UDP headers
/// (IP addresses, ports, checksum, etc.
const int k_cbSteamNetworkingSocketsMaxUDPMsgLen = 1300;

/// Do not allow MTU to be set less than this
const int k_cbSteamNetworkingSocketsMinMTUPacketSize = 200;

/// Overhead that we will reserve for stats, etc when calculating the max
/// message that we won't fragment
const int k_cbSteamNetworkingSocketsNoFragmentHeaderReserve = 100;

/// Size of security tag for AES-GCM.
/// It would be nice to use a smaller tag, but BCrypt requires a 16-byte tag,
/// which is what OpenSSL uses by default for TLS.
const int k_cbSteamNetwokingSocketsEncrytionTagSize = 16;

/// Max length of plaintext and encrypted payload we will send.  AES-GCM does
/// not use padding (but it does have the security tag).  So this can be
/// arbitrary, it does not need to account for the block size.
const int k_cbSteamNetworkingSocketsMaxEncryptedPayloadSend = 1248;
const int k_cbSteamNetworkingSocketsMaxPlaintextPayloadSend = k_cbSteamNetworkingSocketsMaxEncryptedPayloadSend-k_cbSteamNetwokingSocketsEncrytionTagSize;

/// Use larger limits for what we are willing to receive.
const int k_cbSteamNetworkingSocketsMaxEncryptedPayloadRecv = k_cbSteamNetworkingSocketsMaxUDPMsgLen;
const int k_cbSteamNetworkingSocketsMaxPlaintextPayloadRecv = k_cbSteamNetworkingSocketsMaxUDPMsgLen;

/// If we have a cert that is going to expire in <N secondws, try to renew it
const int k_nSecCertExpirySeekRenew = 3600*2;

/// Make sure we have enough room for our headers and occasional inline pings and stats and such
/// FIXME - For relayed connections, we send some of the stats outside the encrypted block, so that
/// they can be observed by the relay.  For direct connections, we put it in the encrypted block.
/// So we might need to adjust this to be per connection type instead off constant.
//COMPILE_TIME_ASSERT( k_cbSteamNetworkingSocketsMaxEncryptedPayloadSend + 50 < k_cbSteamNetworkingSocketsMaxUDPMsgLen );

/// Min size of raw UDP message.
const int k_nMinSteamDatagramUDPMsgLen = 5;

/// When sending a stats message, what sort of reply is requested by the calling code?
enum EStatsReplyRequest
{
	k_EStatsReplyRequest_NothingToSend, // We don't have anything to send at all
	k_EStatsReplyRequest_NoReply, // We have something to send, but it does not require a reply
	k_EStatsReplyRequest_DelayedOK, // We have something to send, but a delayed reply is OK
	k_EStatsReplyRequest_Immediate, // Immediate reply is requested
};

/// Max time that we we should "Nagle" an ack, hoping to combine them together or
/// piggy back on another outgoing message, before sending a standalone message.
const SteamNetworkingMicroseconds k_usecMaxAckStatsDelay = 250*1000;

/// Max duration that a receiver could pend a data ack, in the hopes of trying
/// to piggyback the ack on another outbound packet.
/// !KLUDGE! This really ought to be application- (or connection-) specific.
const SteamNetworkingMicroseconds k_usecMaxDataAckDelay = 50*1000;

/// Precision of the delay ack delay values we send.  A packed value of 1 represents 2^N microseconds
const unsigned k_usecAckDelayPacketSerializedPrecisionShift = 6;
//COMPILE_TIME_ASSERT( ( (k_usecMaxAckStatsDelay*2) >> k_usecAckDelayPacketSerializedPrecisionShift ) < 0x4000 ); // Make sure we varint encode in 2 bytes, even if we overshoot a factor of 2x

/// After a connection is closed, a session will hang out in a CLOSE_WAIT-like
/// (or perhaps FIN_WAIT?) state to handle last stray packets and help both sides
/// close cleanly.
const SteamNetworkingMicroseconds k_usecSteamDatagramRouterCloseWait = k_nMillion*15;

// Internal reason codes
const int k_ESteamNetConnectionEnd_InternalRelay_SessionIdleTimeout = 9001;
const int k_ESteamNetConnectionEnd_InternalRelay_ClientChangedTarget = 9002;

/// Timeout value for pings.  This basically determines the retry rate for pings.
/// If a ping is longer than this, then really, the server should not probably not be
/// considered available.
const SteamNetworkingMicroseconds k_usecSteamDatagramClientPingTimeout = 750000;

/// Keepalive interval for currently selected router.  We send keepalive pings when
/// we haven't heard anything from the router in a while, to see if we need
/// to re-route.
const SteamNetworkingMicroseconds k_usecSteamDatagramClientPrimaryRouterKeepaliveInterval = 1 * k_nMillion;

/// Keepalive interval for backup routers.  We send keepalive pings to
/// make sure our backup session still exists and we could switch to it
/// if it became necessary
const SteamNetworkingMicroseconds k_usecSteamDatagramClientBackupRouterKeepaliveInterval = 45 * k_nMillion;

/// Keepalive interval for gameserver.  We send keepalive pings when we haven't
/// heard anything from the gameserver in a while, in order to try and deduce
/// where the router or gameserver are available.
const SteamNetworkingMicroseconds k_usecSteamDatagramClientServerKeepaliveInterval = 1 * k_nMillion;

/// Timeout value for session request messages
const SteamNetworkingMicroseconds k_usecSteamDatagramClientSessionRequestTimeout = 750000;

/// Router will continue to pend a client ping request for N microseconds,
/// hoping for an opportunity to send it inline.
const SteamNetworkingMicroseconds k_usecSteamDatagramRouterPendClientPing = 200000;

/// When serializing a "time since I last sent a packet" value into the packet,
/// what precision is used?  (A serialized value of 1 = 2^N microseconds.)
const unsigned k_usecTimeSinceLastPacketSerializedPrecisionShift = 4;

/// "Time since last packet sent" values should be less than this.
/// Any larger value will be discarded, and should not be sent
const SteamNetworkingMicroseconds k_usecTimeSinceLastPacketMaxReasonable = k_nMillion/4;
//COMPILE_TIME_ASSERT( ( k_usecTimeSinceLastPacketMaxReasonable >> k_usecTimeSinceLastPacketSerializedPrecisionShift ) < 0x8000 ); // make sure all "reasonable" values can get serialized into 16-bits

///	Don't send spacing values when packets are sent extremely close together.  The spacing
/// should be a bit higher that our serialization precision.
const SteamNetworkingMicroseconds k_usecTimeSinceLastPacketMinReasonable = 2 << k_usecTimeSinceLastPacketSerializedPrecisionShift;

/// A really terrible ping score, but one that we can do some math with without overflowing
constexpr int k_nRouteScoreHuge = INT_MAX/8;

/// Protocol version of this code.  This is a blunt instrument, which is incremented when we
/// wish to change the wire protocol in a way that doesn't have some other easy
/// mechanism for dealing with compatibility (e.g. using protobuf's robust mechanisms).
const uint32 k_nCurrentProtocolVersion = 10;

/// Minimum required version we will accept from a peer.  We increment this
/// when we introduce wire breaking protocol changes and do not wish to be
/// backward compatible.  This has been fine before the	first major release,
/// but once we make a big public release, we probably won't ever be able to
/// do this again, and we'll need to have more sophisticated mechanisms. 
const uint32 k_nMinRequiredProtocolVersion = 8;

/// SteamNetworkingMessages is built on top of SteamNetworkingSockets.  We use a reserved
/// virtual port for this interface
const int k_nVirtualPort_Messages = 0x7fffffff;