#pragma once

//#include "../../Resource/Protobuf/Headers/netmessages.pb.h"
#include "VirtualMethod.h"
#include "../../Memory.h"
#include "ClientClass.h"
#include "../SDK/bitbuf.h"
#include "../SDK/Tier0/utlvector.h"

// https://github.com/pmrowla/demoinfo-csgo/blob/master/netmessages.proto


//#include <google/protobuf/message.h>



class SendTable;
class KeyValue;
class INetMessageHandler;
class IServerMessageHandler;
class IClientMessageHandler;
class NetworkChannel;


class NET_Tick;
class NET_StringCmd;
class NET_SetConVar;
class NET_SignonState;

class CLC_ClientInfo;
class CLC_Move;
class CLC_VoiceData;
class CLC_BaselineAck;
class CLC_ListenEvents;
class CLC_RespondCvarValue;
class CLC_FileCRCCheck;
class CLC_FileMD5Check;
class CLC_SaveReplay;
class CLC_CmdKeyValues;

// each channel packet has 1 byte of FLAG bits
#define PACKET_FLAG_RELIABLE			(1<<0)	// packet contains subchannel stream data
#define PACKET_FLAG_COMPRESSED			(1<<1)	// packet is compressed
#define PACKET_FLAG_ENCRYPTED			(1<<2)  // packet is encrypted
#define PACKET_FLAG_SPLIT				(1<<3)  // packet is split
#define PACKET_FLAG_CHOKED				(1<<4)  // packet was choked by sender
// How fast to converge flow estimates
#define FLOW_AVG ( 3.0 / 4.0 )
 // Don't compute more often than this
#define FLOW_INTERVAL 0.25


#define NET_FRAMES_BACKUP	64		// must be power of 2
#define NET_FRAMES_MASK		(NET_FRAMES_BACKUP-1)
#define MAX_SUBCHANNELS		8		// we have 8 alternative send&wait bits

#define SUBCHANNEL_FREE		0	// subchannel is free to use
#define SUBCHANNEL_TOSEND	1	// subchannel has data, but not send yet
#define SUBCHANNEL_WAITING	2   // sbuchannel sent data, waiting for ACK
#define SUBCHANNEL_DIRTY	3	// subchannel is marked as dirty during changelevel
#define MAX_RATE		(1024*1024)				
#define MIN_RATE		1000
#define DEFAULT_RATE	10000

#define SIGNON_TIME_OUT				300.0f  // signon disconnect timeout

#define FRAGMENT_BITS		8
#define FRAGMENT_SIZE		(1<<FRAGMENT_BITS)
#define MAX_FILE_SIZE_BITS	26
#define MAX_FILE_SIZE		((1<<MAX_FILE_SIZE_BITS)-1)	// maximum transferable size is	64MB

// 0 == regular, 1 == file stream
#define MAX_STREAMS			2    

#define	FRAG_NORMAL_STREAM	0
#define FRAG_FILE_STREAM	1

#define TCP_CONNECT_TIMEOUT		4.0f
#define	PORT_ANY				-1
#define PORT_TRY_MAX			10
#define TCP_MAX_ACCEPTS			8

#define LOOPBACK_SOCKETS	2

#define STREAM_CMD_NONE		0	// waiting for next blob
#define STREAM_CMD_AUTH		1	// first command, send back challengenr
#define STREAM_CMD_DATA		2	// receiving a data blob
#define STREAM_CMD_FILE		3	// receiving a file blob
#define STREAM_CMD_ACKN		4	// acknowledged a recveived blob

#define BYTES2FRAGMENTS(i) ((i+FRAGMENT_SIZE-1)/FRAGMENT_SIZE)
// NETWORKING INFO

// This is the packet payload without any header bytes (which are attached for actual sending)
#define	NET_MAX_PAYLOAD			96000	// largest message we can send in bytes
#define NET_MAX_PALYLOAD_BITS	17		// 2^NET_MAX_PALYLOAD_BITS > NET_MAX_PAYLOAD
// This is just the client_t->netchan.datagram buffer size (shouldn't ever need to be huge)
#define NET_MAX_DATAGRAM_PAYLOAD 4000	// = maximum unreliable playload size

// UDP has 28 byte headers
#define UDP_HEADER_SIZE				(20+8)	// IP = 20, UDP = 8


#define MAX_ROUTABLE_PAYLOAD		1260	// Matches x360 size

#if (MAX_ROUTABLE_PAYLOAD & 3) != 0
#error Bit buffers must be a multiple of 4 bytes
#endif

#define MIN_ROUTABLE_PAYLOAD		16		// minimum playload size

#define NETMSG_TYPE_BITS	5	// must be 2^NETMSG_TYPE_BITS > SVC_LASTMSG

// This is the payload plus any header info (excluding UDP header)

#define HEADER_BYTES	9	// 2*4 bytes seqnr, 1 byte flags

// Pad this to next higher 16 byte boundary
// This is the largest packet that can come in/out over the wire, before processing the header
//  bytes will be stripped by the networking channel layer
#define	NET_MAX_MESSAGE	PAD_NUMBER( ( NET_MAX_PAYLOAD + HEADER_BYTES ), 16 )


#define NET_COMPRESSION_STACKBUF_SIZE 4096


#define FLOW_OUTGOING 0
#define FLOW_INCOMING 1

#define MAX_ROUTABLE_PAYLOAD		1260
#define MIN_USER_MAXROUTABLE_SIZE	576  // ( X.25 Networks )
#define MAX_USER_MAXROUTABLE_SIZE	MAX_ROUTABLE_PAYLOAD
#define NET_MAX_MESSAGE 523956


#define MAX_SPLIT_SIZE	(MAX_USER_MAXROUTABLE_SIZE - sizeof( SPLITPACKET ))
#define MIN_SPLIT_SIZE	(MIN_USER_MAXROUTABLE_SIZE - sizeof( SPLITPACKET ))

#define MAX_SPLITPACKET_SPLITS ( NET_MAX_MESSAGE / MIN_SPLIT_SIZE )





#define NET_HEADER_FLAG_SPLITPACKET				-2
#define NET_HEADER_FLAG_COMPRESSEDPACKET		-3
#define CONNECTIONLESS_HEADER			0xFFFFFFFF	// all OOB packet start with this sequence

#define PORT_REPLAY			27040	// Default replay port

#if !defined( PROTO_OOB_H )
#define PROTO_OOB_H
#ifdef _WIN32
#pragma once
#endif

// This is used, unless overridden in the registry
#define VALVE_MASTER_ADDRESS "207.173.177.10:27011"

#define PORT_RCON			27015	// Default RCON port, TCP
#define	PORT_MASTER			27011	// Default master port, UDP
#define PORT_CLIENT			27005	// Default client port, UDP/TCP
#define PORT_SERVER			27015	// Default server port, UDP/TCP
#define PORT_HLTV			27020	// Default hltv port
#define PORT_HLTV1			27021	// Default hltv[instance 1] port

#define PORT_X360_RESERVED_FIRST	27026	// X360 reserved port first
#define PORT_X360_RESERVED_LAST		27034	// X360 reserved port last
#ifdef ENABLE_RPT
#define PORT_RPT			27035	// default RPT (remote perf testing) port, TCP
#define PORT_RPT_LISTEN		27036	// RPT connection listener (remote perf testing) port, TCP
#endif // ENABLE_RPT
#define PORT_REPLAY			27040	// Default replay port

// out of band message id bytes

// M = master, S = server, C = client, A = any
// the second character will always be \n if the message isn't a single
// byte long (?? not true anymore?)

#define ONNETLANCONNECTIONLESS_HEADER 0
// Requesting for full server list from Server Master
#define	A2M_GET_SERVERS			'c'	// no params

// Master response with full server list
#define	M2A_SERVERS				'd'	// + 6 byte IP/Port list.

// Request for full server list from Server Master done in batches
#define A2M_GET_SERVERS_BATCH	'e' // + in532 uniqueID ( -1 for first batch )

// Master response with server list for channel
#define M2A_SERVER_BATCH		'f' // + int32 next uniqueID( -1 for last batch ) + 6 byte IP/Port list.

// Request for MOTD from Server Master  (Message of the Day)
#define	A2M_GET_MOTD			'g'	// no params

// MOTD response Server Master
#define	M2A_MOTD				'h'	// + string 

// Generic Ping Request
#define	A2A_PING				'i'	// respond with an A2A_ACK

// Generic Ack
#define	A2A_ACK					'j'	// general acknowledgement without info

#define C2S_CONNECT				'k'	// client requests to connect

// Print to client console.
#define	A2A_PRINT				'l'	// print a message on client

// info request
#define S2A_INFO_DETAILED		'm'	// New Query protocol, returns dedicated or not, + other performance info.

#define A2S_RESERVE				'n' // reserves this server for specific players for a short period of time.  Fails if not empty.

#define S2A_RESERVE_RESPONSE	'p' // server response to reservation request

// Another user is requesting a challenge value from this machine
// NOTE: this is currently duplicated in SteamClient.dll but for a different purpose,
// so these can safely diverge anytime. SteamClient will be using a different protocol
// to update the master servers anyway.
#define A2S_GETCHALLENGE		'q'	// Request challenge # from another machine

#define A2S_RCON				'r'	// client rcon command

#define A2A_CUSTOM				't'	// a custom command, follow by a string for 3rd party tools


// A user is requesting the list of master servers, auth servers, and titan dir servers from the Client Master server
#define A2M_GETMASTERSERVERS	'v' // + byte (type of request, TYPE_CLIENT_MASTER or TYPE_SERVER_MASTER)

// Master server list response
#define M2A_MASTERSERVERS		'w'	// + byte type + 6 byte IP/Port List

#define A2M_GETACTIVEMODS		'x' // + string Request to master to provide mod statistics ( current usage ).  "1" for first mod.

#define M2A_ACTIVEMODS			'y' // response:  modname\r\nusers\r\nservers

#define M2M_MSG					'z' // Master peering message

// SERVER TO CLIENT/ANY

// Client connection is initiated by requesting a challenge value
//  the server sends this value back
#define S2C_CHALLENGE			'A' // + challenge value

// Server notification to client to commence signon process using challenge value.
#define	S2C_CONNECTION			'B' // no params

// Response to server info requests

// Request for detailed server/rule information.
#define S2A_INFO_GOLDSRC		'm' // Reserved for use by goldsrc servers

#define S2M_GETFILE				'J'	// request module from master
#define M2S_SENDFILE			'K'	// send module to server

#define S2C_REDIRECT			'L'	// + IP x.x.x.x:port, redirect client to other server/proxy 

#define	C2M_CHECKMD5			'M'	// player client asks secure master if Module MD5 is valid
#define M2C_ISVALIDMD5			'N'	// secure servers answer to C2M_CHECKMD5

// MASTER TO SERVER
#define M2A_ACTIVEMODS3			'P' // response:  keyvalues struct of mods
#define A2M_GETACTIVEMODS3		'Q' // get a list of mods and the stats about them

#define S2A_LOGSTRING			'R'	// send a log string
#define S2A_LOGKEY				'S'	// send a log event as key value
#define S2A_LOGSTRING2			'S'	// send a log string including a secret value << this clashes with S2A_LOGKEY that nothing seems to use in CS:GO and is followed by secret value so should be compatible for server ops with their existing tools

#define A2S_SERVERQUERY_GETCHALLENGE		'W'	// Request challenge # from another machine

#define A2S_KEY_STRING		"Source Engine Query" // required postfix to a A2S_INFO query

#define A2M_GET_SERVERS_BATCH2	'1' // New style server query

#define A2M_GETACTIVEMODS2		'2' // New style mod info query

#define C2S_AUTHREQUEST1        '3' // 
#define S2C_AUTHCHALLENGE1      '4' //
#define C2S_AUTHCHALLENGE2      '5' //
#define S2C_AUTHCOMPLETE        '6'
#define C2S_AUTHCONNECT         '7'  // Unused, signals that the client has
									 // authenticated the server

#define C2S_VALIDATE_SESSION    '8'
// #define UNUSED_A2S_LANSEARCH			'C'	 // LAN game details searches
// #define UNUSED_S2A_LANSEARCHREPLY		'F'	 // LAN game details reply

#define S2C_CONNREJECT			'9'  // Special protocol for rejected connections.

#define MAX_OOB_KEYVALUES		600		// max size in bytes for keyvalues included in an OOB msg
#define MAKE_4BYTES( a, b, c, d ) ( ( ((unsigned char)(d)) << 24 ) | ( ((unsigned char)(c)) << 16 ) | ( ((unsigned char)(b)) << 8 ) | ( ((unsigned char)(a)) << 0 ) )

#define A2A_KV_CMD				'?'	// generic KeyValues command [1 byte: version] [version dependent data...]
#define A2A_KV_VERSION			1	// version of generic KeyValues command
									// [4 bytes: header] [4 bytes: replyid] [4 bytes: challenge] [4 bytes: extra] [4 bytes: numbytes] [numbytes: serialized KV]

// These can be owned by Steam after we get rid of this legacy code.
#define S2A_INFO_SRC			'I'	// + Address, hostname, map, gamedir, gamedescription, active players, maxplayers, protocol
#define	S2M_HEARTBEAT			'a'	// + challeange + sequence + active + #channels + channels
#define S2M_HEARTBEAT2			'0' // New style heartbeat
#define	S2M_SHUTDOWN			'b' // no params
#define M2A_CHALLENGE			's'	// + challenge value 
#define M2S_REQUESTRESTART		'O' // HLMaster rejected a server's connection because the server needs to be updated
#define A2S_RULES				'V'	// request rules list from server
#define S2A_RULES				'E' // + number of rules + string key and string value pairs
#define A2S_INFO				'T' // server info request - this must match the Goldsrc engine
#define S2A_PLAYER				'D' // + Playernum, name, frags, /*deaths*/, time on server
#define A2S_PLAYER				'U'	// request player list

#define A2S_PING2				'Y' // new-style minimalist ping request
#define S2A_PING2REPLY			'Z' // new-style minimalist ping reply


// temp hack until we kill the legacy interface
// The new S2A_INFO_SRC packet has a byte at the end that has these bits in it, telling 
// which data follows.
#define S2A_EXTRA_DATA_HAS_GAME_PORT				0x80		// Next 2 bytes include the game port.
#define S2A_EXTRA_DATA_HAS_SPECTATOR_DATA			0x40		// Next 2 bytes include the spectator port, then the spectator server name.
#define S2A_EXTRA_DATA_HAS_GAMETAG_DATA				0x20		// Next bytes are the game tag string
#define S2A_EXTRA_DATA_HAS_STEAMID					0x10		// Next 8 bytes are the steamID
#define S2A_EXTRA_DATA_GAMEID						0x01		// Next 8 bytes are the gameID of the server

#define A2S_RESERVE_CHECK			'!' // check if server reservation cookie is same as the one we are holding
#define S2A_RESERVE_CHECK_RESPONSE	'%' // server response to reservation request

#define A2S_PING					'$'
#define S2A_PING_RESPONSE			'^'

#endif



#define DECLARE_BASE_MESSAGE( msgtype )						\
	public:													\
		bool			ReadFromBuffer( bf_read &buffer );	\
		bool			WriteToBuffer( bf_write &buffer );	\
		const char		*ToString() const;					\
		int				GetType() const { return msgtype; } \
		const char		*GetName() const { return #msgtype;}\

#define DECLARE_NET_MESSAGE( name )			\
	DECLARE_BASE_MESSAGE( NetMessageProto::net_##name );		\
	INetMessageHandler *m_pMessageHandler;	\
	bool Process() { return m_pMessageHandler->Process##name( this ); }\

#define DECLARE_SVC_MESSAGE( name )		\
	DECLARE_BASE_MESSAGE( NetMessageProto::svc_##name );	\
	IServerMessageHandler *m_pMessageHandler;\
	bool Process() { return m_pMessageHandler->Process##name( this ); }\

#define DECLARE_CLC_MESSAGE( name )		\
	DECLARE_BASE_MESSAGE( NetMessageProto::clc_##name );	\
	IClientMessageHandler *m_pMessageHandler;\
	bool Process() { return m_pMessageHandler->Process##name( this ); }\

#define DECLARE_MM_MESSAGE( name )		\
	DECLARE_BASE_MESSAGE( mm_##name );	\
	IMatchmakingMessageHandler *m_pMessageHandler;\
	bool Process() { return m_pMessageHandler->Process##name( this ); }\

#define PROCESS_NET_MESSAGE( name )	\
	virtual bool Process##name( NET_##name *msg )

#define PROCESS_SVC_MESSAGE( name )	\
	virtual bool Process##name( SVC_##name *msg )

#define PROCESS_CLC_MESSAGE( name )	\
	virtual bool Process##name( CLC_##name *msg )

#define PROCESS_MM_MESSAGE( name )	\
	virtual bool Process##name( MM_##name *msg )


class INetChannelHandler
{
public:
	virtual	~INetChannelHandler(void) {};

	virtual void ConnectionStart(NetworkChannel* chan) = 0;	// called first time network channel is established

	virtual void ConnectionClosing(const char* reason) = 0; // network channel is being closed by remote site

	virtual void ConnectionCrashed(const char* reason) = 0; // network error occured

	virtual void PacketStart(int incoming_sequence, int outgoing_acknowledged) = 0;	// called each time a new packet arrived

	virtual void PacketEnd(void) = 0; // all messages has been parsed

	virtual void FileRequested(const char* fileName, unsigned int transferID) = 0; // other side request a file for download

	virtual void FileReceived(const char* fileName, unsigned int transferID) = 0; // we received a file

	virtual void FileDenied(const char* fileName, unsigned int transferID) = 0;	// a file request was denied by other side

	virtual void FileSent(const char* fileName, unsigned int transferID) = 0;	// we sent a file
};



class INetMessageHandler
{
public:
	virtual ~INetMessageHandler(void) {};

	PROCESS_NET_MESSAGE(Tick) = 0;
	PROCESS_NET_MESSAGE(StringCmd) = 0;
	PROCESS_NET_MESSAGE(SetConVar) = 0;
	PROCESS_NET_MESSAGE(SignonState) = 0;
};


class IClientMessageHandler : public INetMessageHandler
{
public:
	virtual ~IClientMessageHandler(void) {};

	PROCESS_CLC_MESSAGE(ClientInfo) = 0;
	PROCESS_CLC_MESSAGE(Move) = 0;
	PROCESS_CLC_MESSAGE(VoiceData) = 0;
	PROCESS_CLC_MESSAGE(BaselineAck) = 0;
	PROCESS_CLC_MESSAGE(ListenEvents) = 0;
	PROCESS_CLC_MESSAGE(RespondCvarValue) = 0;
	PROCESS_CLC_MESSAGE(FileCRCCheck) = 0;
	PROCESS_CLC_MESSAGE(FileMD5Check) = 0;
#if defined( REPLAY_ENABLED )
	PROCESS_CLC_MESSAGE(SaveReplay) = 0;
#endif
	PROCESS_CLC_MESSAGE(CmdKeyValues) = 0;
};

class INetChannelInfo
{
public:

	enum {
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		TEMPENTS,		// temp entities
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
		PAINTMAP,
	};

	virtual const char* GetName(void) const = 0;	// get channel name
	virtual const char* GetAddress(void) const = 0; // get channel IP address as string
	virtual float		GetTime(void) const = 0;	// current net time
	virtual float		GetTimeConnected(void) const = 0;	// get connection time in seconds
	virtual int			GetBufferSize(void) const = 0;	// netchannel packet history size
	virtual int			GetDataRate(void) const = 0; // send data rate in byte/sec

	virtual bool		IsLoopback(void) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut(void) const = 0;	// true if timing out
	virtual bool		IsPlayback(void) const = 0;	// true if demo playback

	virtual float		GetLatency(int flow) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency(int flow) const = 0; // average packet latency in seconds
	virtual float		GetAvgLoss(int flow) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAvgChoke(int flow) const = 0;	 // avg packet choke[0..1]
	virtual float		GetAvgData(int flow) const = 0;	 // data flow in bytes/sec
	virtual float		GetAvgPackets(int flow) const = 0; // avg packets/sec
	virtual int			GetTotalData(int flow) const = 0;	 // total flow in/out in bytes
	virtual int			GetTotalPackets(int flow) const = 0;
	virtual int			GetSequenceNr(int flow) const = 0;	// last send seq number
	virtual bool		IsValidPacket(int flow, int frame_number) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime(int flow, int frame_number) const = 0; // time when packet was send
	virtual int			GetPacketBytes(int flow, int frame_number, int group) const = 0; // group size of this packet
	virtual bool		GetStreamProgress(int flow, int* received, int* total) const = 0;  // TCP progress if transmitting
	virtual float		GetTimeSinceLastReceived(void) const = 0;	// get time since last recieved packet in seconds
	virtual	float		GetCommandInterpolationAmount(int flow, int frame_number) const = 0;
	virtual void		GetPacketResponseLatency(int flow, int frame_number, int* pnLatencyMsecs, int* pnChoke) const = 0;
	virtual void		GetRemoteFramerate(float* pflFrameTime, float* pflFrameTimeStdDeviation) const = 0;

	virtual float		GetTimeoutSeconds() const = 0;
};








namespace NetMessageProto {
	enum NET_Messages
	{
		net_NOP = 0,
		net_Disconnect = 1,				// disconnect, last message in connection
		net_File = 2,					// file transmission message request/deny
		net_LastControlMessage = 2,
		net_SplitScreenUser = 3,		// Changes split screen user, client and server must both provide handler
		net_Tick = 4, 					// s->c world tick, c->s ack world tick
		net_StringCmd = 5,				// a string command
		net_SetConVar = 6,				// sends one/multiple convar/userinfo settings
		net_SignonState = 7,			// signals or acks current signon state
		net_PlayerAvatarData = 100,
	};



	/*
	
		enum
	{
		NETMSG_Tick,
		NETMSG_StringCmd,
		NETMSG_SetConVar,
		NETMSG_SignonState,
		NETMSG_ClientInfo,
		NETMSG_Move,
		NETMSG_VoiceData,
		NETMSG_BaselineAck,
		NETMSG_ListenEvents,
		NETMSG_RespondCvarValue,
		NETMSG_SplitPlayerConnect,
		NETMSG_FileCRCCheck,
		NETMSG_LoadingProgress,
		NETMSG_CmdKeyValues,
		NETMSG_PlayerAvatarData,
		NETMSG_HltvReplay,
		NETMSG_UserMessage,
		NETMSG_Max
	};
	SERVER
	
	
	*/










	enum CLC_Messages
	{
		clc_ClientInfo = 8,				// client info (table CRC etc)    
		clc_Move = 9,				// [CUserCmd]     
		clc_VoiceData = 10,				// Voicestream data from a client    
		clc_BaselineAck = 11,				// client acknowledges a new baseline seqnr   
		clc_ListenEvents = 12,				// client acknowledges a new baseline seqnr   
		clc_RespondCvarValue = 13,			// client is responding to a svc_GetCvarValue message.  
		clc_FileCRCCheck = 14,				// client is sending a file's CRC to the server to be verified. 
		clc_LoadingProgress = 15,				// client loading progress  
		clc_SplitPlayerConnect = 16,
		clc_ClientMessage = 17,
		clc_CmdKeyValues = 18,
	};




	enum SVC_Messages
	{
		svc_ServerInfo = 8,		// first message from server about game; map etc
		svc_SendTable = 9,		// sends a sendtable description for a game class
		svc_ClassInfo = 10,		// Info about classes (first byte is a CLASSINFO_ define).							
		svc_SetPause = 11,		// tells client if server paused or unpaused
		svc_CreateStringTable = 12,		// inits shared string tables
		svc_UpdateStringTable = 13,		// updates a string table
		svc_VoiceInit = 14,		// inits used voice codecs & quality
		svc_VoiceData = 15,		// Voicestream data from the server
		svc_Print = 16,	// print text to console
		svc_Sounds = 17,		// starts playing sound
		svc_SetView = 18,		// sets entity as point of view
		svc_FixAngle = 19,		// sets/corrects players viewangle
		svc_CrosshairAngle = 20,		// adjusts crosshair in auto aim mode to lock on traget
		svc_BSPDecal = 21,		// add a static decal to the world BSP
		svc_SplitScreen = 22,		// split screen style message
		svc_UserMessage = 23,		// a game specific message 
		svc_EntityMessage = 24,		// a message for an entity
		svc_GameEvent = 25,		// global game event fired
		svc_PacketEntities = 26,		// non-delta compressed entities
		svc_TempEntities = 27,		// non-reliable event object
		svc_Prefetch = 28,		// only sound indices for now
		svc_Menu = 29,		// display a menu from a plugin
		svc_GameEventList = 30,		// list of known games events and fields
		svc_GetCvarValue = 31,		// Server wants to know the value of a cvar on the client	
		svc_PaintmapData = 33,
		svc_CmdKeyValues = 34,		// Server submits KeyValues command for the client
	};
}
/*
class IServerMessageHandler : public INetMessageHandler
{
public:
	virtual ~IServerMessageHandler(void) {};

	// Returns dem file protocol version, or, if not playing a demo, just returns PROTOCOL_VERSION
	virtual int GetDemoProtocolVersion() const = 0;

	PROCESS_SVC_MESSAGE(Print) = 0;
	PROCESS_SVC_MESSAGE(ServerInfo) = 0;
	PROCESS_SVC_MESSAGE(SendTable) = 0;
	PROCESS_SVC_MESSAGE(ClassInfo) = 0;
	PROCESS_SVC_MESSAGE(SetPause) = 0;
	PROCESS_SVC_MESSAGE(CreateStringTable) = 0;
	PROCESS_SVC_MESSAGE(UpdateStringTable) = 0;
	PROCESS_SVC_MESSAGE(VoiceInit) = 0;
	PROCESS_SVC_MESSAGE(VoiceData) = 0;
	PROCESS_SVC_MESSAGE(Sounds) = 0;
	PROCESS_SVC_MESSAGE(SetView) = 0;
	PROCESS_SVC_MESSAGE(FixAngle) = 0;
	PROCESS_SVC_MESSAGE(CrosshairAngle) = 0;
	PROCESS_SVC_MESSAGE(BSPDecal) = 0;
	PROCESS_SVC_MESSAGE(GameEvent) = 0;
	PROCESS_SVC_MESSAGE(UserMessage) = 0;
	PROCESS_SVC_MESSAGE(EntityMessage) = 0;
	PROCESS_SVC_MESSAGE(PacketEntities) = 0;
	PROCESS_SVC_MESSAGE(TempEntities) = 0;
	PROCESS_SVC_MESSAGE(Prefetch) = 0;
	PROCESS_SVC_MESSAGE(Menu) = 0;
	PROCESS_SVC_MESSAGE(GameEventList) = 0;
	PROCESS_SVC_MESSAGE(GetCvarValue) = 0;
	PROCESS_SVC_MESSAGE(CmdKeyValues) = 0;
};
*/

// CNetMessagePB<5,CNETMsg_StringCmd,12,1>

struct Net_StringCmdType {
	const char* command;
};




typedef enum
{
	_NA_UNUSED,
	_NA_LOOPBACK,
	_NA_BROADCAST,
	_NA_IP,
	_NA_IPX,
	_NA_BROADCAST_IPX,
} netadrtype_tt;


enum SIGNONSTATE
{
	SIGNONSTATE_NONE = 0,
	SIGNONSTATE_CHALLENGE = 1,
	SIGNONSTATE_CONNECTED = 2,
	SIGNONSTATE_NEW = 3,
	SIGNONSTATE_PRESPAWN = 4,
	SIGNONSTATE_SPAWN = 5,
	SIGNONSTATE_FULL = 6,
	SIGNONSTATE_CHANGELEVEL = 7,
};


typedef struct netadr_ss
{
	netadrtype_tt	type;
	unsigned char	ip[4];
	//unsigned char	ipx[10]; //<--- ? 
	unsigned short	port;
} netadr_tt;



typedef struct netpacket_s
{
	netadr_tt		from;		// sender IP
	int				source;		// received source 
	double			received;	// received time
	unsigned char* data;		// pointer to raw packet data
	bf_read			message;	// easy bitbuf data access
	int				size;		// size in bytes
	int				wiresize;   // size in bytes before decompression
	bool			stream;		// was send as stream
	struct netpacket_s* pNext;	// for internal use, should be NULL in public
} netpacket_t;


//inline int j = sizeof(bf_read);
//inline int k = sizeof(netpacket_s);

typedef struct dataFragments_s
{

	void*	file;			// open file handle
	char			filename[MAX_PATH]; // filename
	//std::byte pad[61];
	char* buffer;			// if NULL it's a file
	unsigned int	bytes;			// size in bytes
	unsigned int	bits;			// size in bits
	unsigned int	transferID;		// only for files
	bool			isCompressed;	// true if data is bzip compressed
	unsigned int	nUncompressedSize; // full size in bytes
	bool			asTCP;			// send as TCP stream
	bool            isReplayDemo;
	int				numFragments;	// number of total fragments
	int				ackedFragments; // number of fragments send & acknowledged
	int				pendingFragments; // number of fragments send, but not acknowledged yet
} dataFragments_t;

struct subChannel_s
{
	int				startFraggment[MAX_STREAMS];
	int				numFragments[MAX_STREAMS];
	int				sendSeqNr;
	int				state; // 0 = free, 1 = scheduled to send, 2 = send & waiting, 3 = dirty
	int				index; // index in m_SubChannels[]

	void Free()
	{
		state = SUBCHANNEL_FREE;
		sendSeqNr = -1;
		for (int i = 0; i < MAX_STREAMS; i++)
		{
			numFragments[i] = 0;
			startFraggment[i] = -1;
		}
	}
};

// Client's now store the command they sent to the server and the entire results set of
//  that command. 
typedef struct netframe_s
{
	// Data received from server
	float			time;			// net_time received/send
	int				size;			// total size in bytes
	float			latency;		// raw ping for this packet, not cleaned. set when acknowledged otherwise -1.
	float			avg_latency;	// averaged ping for this packet
	bool			valid;			// false if dropped, lost, flushed
	int				choked;			// number of previously chocked packets
	int				dropped;
	float			m_flInterpolationAmount;
	unsigned short	msggroups[INetChannelInfo::TOTAL];	// received bytes for each message group
} netframe_t;

typedef struct
{
	float		nextcompute;	// Time when we should recompute k/sec data
	float		avgbytespersec;	// average bytes/sec
	float		avgpacketspersec;// average packets/sec
	float		avgloss;		// average packet loss [0..1]
	float		avgchoke;		// average packet choke [0..1]
	float		avglatency;		// average ping, not cleaned
	float		latency;		// current ping, more accurate also more jittering
	int			totalpackets;	// total processed packets
	int			totalbytes;		// total processed bytes
	int			currentindex;		// current frame index
	netframe_t	frames[NET_FRAMES_BACKUP]; // frame history
	netframe_t* currentframe;	// current frame
} netflow_t;

// Use this to pick apart the network stream, must be packed
#pragma pack(1)
typedef struct
{
	int		netID;
	int		sequenceNumber;
	int		packetID : 16;
	int		nSplitSize : 16;
} SPLITPACKET;
#pragma pack()


class NetworkMessage
{
public:
	    VIRTUAL_METHOD(void, SetNetChannel, 1, (NetworkChannel* netchann), (this, netchann))
		VIRTUAL_METHOD(void, SetReliable, 2, (bool state), (this, state))
		VIRTUAL_METHOD(bool, Process, 3, (), (this))
		VIRTUAL_METHOD(bool, ReadFromBuffer, 4, (bf_read* buffer), (this, buffer)) 
		VIRTUAL_METHOD(bool, WriteToBuffer, 5, (bf_write* buffer), (this, buffer))
		VIRTUAL_METHOD(bool, IsReliable, 6, (), (this))
		VIRTUAL_METHOD(int, getType, 7, (), (this))
		VIRTUAL_METHOD(int, getGroup, 8, (), (this))
		VIRTUAL_METHOD(const char*, getName, 9, (), (this))
		VIRTUAL_METHOD(void*, getNetworkChannel, 10, (), (this))
		VIRTUAL_METHOD(const char*, toString, 11, (), (this))
};



typedef unsigned int CRC32_t;
class EventInfo {
public:
	enum {
		EVENT_INDEX_BITS = 8,
		EVENT_DATA_LEN_BITS = 11,
		MAX_EVENT_DATA = 192,  // ( 1<<8 bits == 256, but only using 192 below )
	};
	short class_id;
	float fire_delay;
	const void* m_send_table;
	const ClientClass* m_client_class;
	int bits;
	uint8_t* data;
	int flags;
	PAD(0x1C);
	EventInfo* m_next;
};


class ClientState
{
public:
	void ForceFullUpdate()
	{
		deltaTick = -1;
	}
	std::byte		pad0[0x9C];
	NetworkChannel* netChannel;
	int				challengeNr;
	std::byte		pad1[0x04];
	double          m_connect_time;
	int             m_retry_number;
	std::byte		padgay[0x54];
	int				signonState;
	std::byte		pad2[0x8];
	float           nextCmdTime;
	int				serverCount;
	int				currentSequence;
	char			pad99[8];
	struct {
		float		m_clock_offsets[16];
		int			m_cur_clock_offset;
		int			m_server_tick;
		int			m_client_tick;
	} m_clock_drift_mgr;
	int				deltaTick;
	bool			paused;
	std::byte		pad4[0x7];
	int				viewEntity;
	int				playerSlot;
	char			levelName[260];
	char			levelNameShort[80];
	char			groupName[80];
	char			szLastLevelNameShort[80]; // 0x032C
	std::byte		pad5[0xC];
	int				maxClients;
	char pad_030C[4083];
	uint32_t string_table_container;
	char pad_1303[14737];
	float			lastServerTickTime;
	bool			InSimulation;
	std::byte		pad7[0x3];
	int				oldTickcount;
	float			tickRemainder;
	float			frameTime;
	int				lastOutgoingCommand;
	int				chokedCommands;
	int				lastCommandAck;
	int				commandAck;
	int				soundSequence;
	//std::byte		pad8[0x50];
	int                m_last_progress_percent;
	bool            m_is_hltv;

	std::byte padfuck[0x4B];
	Vector			angViewPoint;
	std::byte		pad9[0xD0];
	EventInfo* pEvents;
};


class CNetMessage : public NetworkMessage
{
public:
	CNetMessage() {
		m_bReliable = true;
		m_NetChannel = NULL;
	}

	virtual ~CNetMessage() {};

	virtual int		getGroup() const { return NetMessageProto::NET_Messages::net_NOP; }
	NetworkChannel* getNetChannel() const { return m_NetChannel; }

	virtual void	SetReliable(bool state) { m_bReliable = state; };
	virtual bool	IsReliable() const { return m_bReliable; };
	virtual void    SetNetChannel(NetworkChannel* netchan) { m_NetChannel = netchan; }
	virtual bool	Process() { return false; };	// no handler set


	bool				m_bReliable;	// true if message should be send reliable
	NetworkChannel* m_NetChannel;	// netchannel this message is from/for
};
#include "../../Memory.h"



class NET_Tick : public CNetMessage
{
	//DECLARE_NET_MESSAGE(Tick);
public:

	NET_Tick(){}
	NET_Tick(float host_computationtime, float host_computationtime_std_deviation, int m_nDeltaTick, float host_framestarttime_std_deviation) {
		((void(__thiscall*)(void*, float, float, int , float))memory->dylan.CNetMsg_Tick_Constructor)(this, host_computationtime, host_computationtime_std_deviation, m_nDeltaTick, host_framestarttime_std_deviation);

	}

	~NET_Tick() {
		((void(__thiscall*)(void*))memory->dylan.CNetMsg_Tick_Destructor)(this);
	}
public:
	int32_t tick = 0;
	uint32_t host_computationtime = 0;
    uint32_t host_computationtime_std_deviation = 0;
	uint32_t host_framestarttime_std_deviation = 0;

};

inline void __fastcall NET_Tick_Creator(void* inmemory, float host_computationtime /*@<xmm2>*/, float host_computationtime_std_deviation /*@<xmm3>*/, int m_nDeltaTick, float host_framestarttime_std_deviation /*@<xmm0>*/)
{
	inmemory;
	DWORD pmemory; // edi
	unsigned int framestarttime_std_deviation; // esi
	unsigned int computationtime; // ecx
	unsigned int computationtime_std_deviation; // ecx
	double fl_framestarttime_std_deviation; // xmm0_8

	pmemory = (DWORD)inmemory;
	((void(__thiscall*)(void*))memory->dylan.CNetMsg_Tick_t_Setup)(inmemory);
	*(DWORD*)pmemory = memory->dylan.CNetMsg_Tick_t_VFTable1;// &CNETMsg_Tick_t::`vftable';
	*(DWORD*)(pmemory + 4) = memory->dylan.CNetMsg_Tick_t_VFTable2;//&CNETMsg_Tick_t::`vftable';
	*(BYTE*)(pmemory + 40) = 0;
	*(DWORD*)(pmemory + 36) |= 1u;
	*(DWORD*)(pmemory + 12) = m_nDeltaTick;
	framestarttime_std_deviation = 1000000;
	computationtime = 1000000;
	if ((unsigned int)(host_computationtime * 1000000.0) < 1000000)
		computationtime = (unsigned int)(host_computationtime * 1000000.0);
	*(DWORD*)(pmemory + 36) |= 2u;
	*(DWORD*)(pmemory + 16) = computationtime;
	computationtime_std_deviation = 1000000;
	if ((unsigned int)(host_computationtime_std_deviation * 1000000.0) < 1000000)
		computationtime_std_deviation = (unsigned int)(host_computationtime_std_deviation * 1000000.0);
	*(DWORD*)(pmemory + 36) |= 4u;
	*(DWORD*)(pmemory + 20) = computationtime_std_deviation;
	fl_framestarttime_std_deviation = host_framestarttime_std_deviation * 1000000.0;
	if ((unsigned int)fl_framestarttime_std_deviation < 1000000)
		framestarttime_std_deviation = (unsigned int)fl_framestarttime_std_deviation;
	*(DWORD*)(pmemory + 36) |= 8u;
	*(DWORD*)(pmemory + 24) = framestarttime_std_deviation;
}


inline NET_Tick* CreateNewTick(float host_computationtime , float host_computationtime_std_deviation , int m_nDeltaTick, float host_framestarttime_std_deviation) {
	char* mem = (char*)calloc(1, 2048);
	NET_Tick_Creator(mem, host_computationtime, host_computationtime_std_deviation, m_nDeltaTick, host_framestarttime_std_deviation);
	return (NET_Tick*)mem;
}



class NET_SignonState : public CNetMessage
{
	//DECLARE_NET_MESSAGE(Tick);
public:
	uint32_t signon_state;
	uint32_t spawn_count;
	uint32_t num_server_players;
	char players_networkids[1024];
	char map_name[1024];

};

class NET_StringCmd : public CNetMessage
{
	DECLARE_NET_MESSAGE(StringCmd);

	int	getGroup() const { return NetMessageProto::NET_Messages::net_StringCmd; }

	NET_StringCmd() { m_szCommand = NULL; };
	NET_StringCmd(const char* cmd) { m_szCommand = cmd; };

public:
	const char* m_szCommand;	// execute this command

private:
	char		m_szCommandBuffer[1024];	// buffer for received messages

};

class SVC_PacketEntities : public CNetMessage
{
	//DECLARE_SVC_MESSAGE(PacketEntities);

	int	GetGroup() const { return INetChannelInfo::ENTITIES; }

public:

	int			m_nMaxEntries;
	int			m_nUpdatedEntries;
	bool		m_bIsDelta;
	bool		m_bUpdateBaseline;
	int			m_nBaseline;
	int			m_nDeltaFrom;
	int			m_nLength;
	bf_read		m_DataIn;
	bf_write	m_DataOut;
};

class CLC_VoiceData : public CNetMessage
{
	DECLARE_CLC_MESSAGE(VoiceData);

	int	getGroup() const { return NetMessageProto::CLC_Messages::clc_VoiceData; }

	CLC_VoiceData() { m_bReliable = false; };

public:
	int				m_nLength;
	bf_read			m_DataIn;
	bf_write		m_DataOut;
	uint64			m_xuid;
};


class CLC_RespondCvarValue : public CNetMessage
{
public:
	int32_t cookie;
	int32_t StatusCode;
	const char* Name;
	const char* Value;
};

class CLC_BaselineAck : public CNetMessage
{
	DECLARE_CLC_MESSAGE(BaselineAck);

	CLC_BaselineAck() {};
	CLC_BaselineAck(int tick, int baseline)
	{
		m_nBaselineTick = tick;
		m_nBaselineNr = baseline;
	}

	int GetGroup() const
	{
		return 0;//INetChannelInfo::ENTITIES;
	}

public:
	int m_nBaselineTick; // sequence number of baseline
	int m_nBaselineNr;   // 0 or 1
};


class CLC_CmdKeyValues : public CNetMessage
{
	DECLARE_CLC_MESSAGE(CmdKeyValues);

	int GetGroup() const
	{
		return 0;// net_Gen;
	};
	CLC_CmdKeyValues()
	{
		m_bReliable = false;
	};

public:
};


class CLC_ListenEvents : public CNetMessage {
public:
	int32_t event_mask;
};





class NET_SetConVar : public CNetMessage
{
public:
	//DECLARE_NET_MESSAGE(SetConVar);

	int GetGroup() const
	{


		return NetMessageProto::net_StringCmd;
	}

	NET_SetConVar(const char* name, const char* value)
	{
		((void(__thiscall*)(void*))memory->net_setConvarFunc.Constructor)(this);
		((void(__thiscall*)(void*, const char*, const char*))memory->net_setConvarFunc.Constructor)(this, name, value);
	}

	~NET_SetConVar() {
		((void(__thiscall*)(void*))memory->net_setConvarFunc.Destructior)(this);
	}
	//NET_SetConVar(const char* name, const char* value)
	//{
		//cvar_t cvar;
		//strncpy(cvar.name, name, MAX_PATH);
		//strncpy(cvar.value, value, MAX_PATH);
		//convar = cvar;
	//}

public:
	typedef struct cvar_s
	{
		char name[MAX_PATH];
		char value[MAX_PATH];
	} cvar_t;
	cvar_t convar;
	// CUtlVector<cvar_t> m_ConVars;
};



class Net_PlayerAvatarData : public CNetMessage {
public:
	Net_PlayerAvatarData() {
		((void(__thiscall*)(void*))memory->Net_PlayerAvatarData.Constructor)(this);

	}

	~Net_PlayerAvatarData() {
		((void(__thiscall*)(void*))memory->Net_PlayerAvatarData.Destructior)(this);
	}

	uint32_t accountid;
	char rgb[64 * 64 * 3];
};


class CLC_FileCRCCheck : public CNetMessage
{
public:
	DECLARE_CLC_MESSAGE(FileCRCCheck);

	char		m_szPathID[MAX_PATH];
	char		m_szFilename[MAX_PATH];
	CRC32_t		m_CRC;
};


class CLC_Move : public CNetMessage
{
	//DECLARE_CLC_MESSAGE(Move);
public:
	uint32_t num_backup_commands;
	uint32_t num_new_commands;
	//int		m_nLength;
	bf_write data;
};


struct VoiceDataCustom
{
	uint32_t xuid_low{};
	uint32_t xuid_high{};
	int32_t sequence_bytes{};
	uint32_t section_number{};
	uint32_t uncompressed_sample_offset{};

	__forceinline uint8_t* get_raw_data()
	{
		return (uint8_t*)this;
	}
};

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

struct CCLCMsg_VoiceData_Legacy
{
	uint32_t INetMessage_Vtable; //0x0000
	char pad_0004[4]; //0x0004
	uint32_t CCLCMsg_VoiceData_Vtable; //0x0008
	char pad_000C[8]; //0x000C
	void* data; //0x0014
	uint32_t xuid_low{};
	uint32_t xuid_high{};
	int32_t format; //0x0020
	int32_t sequence_bytes; //0x0024
	uint32_t section_number; //0x0028
	uint32_t uncompressed_sample_offset; //0x002C
	int32_t cached_size; //0x0030

	uint32_t flags; //0x0034

	uint8_t no_stack_overflow[0xFF];

	__forceinline void set_data(VoiceDataCustom* cdata)
	{
		xuid_low = cdata->xuid_low;
		xuid_high = cdata->xuid_high;
		sequence_bytes = cdata->sequence_bytes;
		section_number = cdata->section_number;
		uncompressed_sample_offset = cdata->uncompressed_sample_offset;
	}
};



class INetMessageBinder 
{
public:
	virtual	~INetMessageBinder() {};
	virtual int	GetType(void) const = 0; // returns module specific header tag eg svc_serverinfo
	virtual void SetNetChannel(NetworkChannel* netchan) = 0; // netchannel this message is from/for
	virtual NetworkMessage* CreateFromBuffer(bf_read& buffer) = 0;
	virtual bool Process(const NetworkMessage& src) = 0;
};















//https://github.com/VSES/SourceEngine2007/blob/master/src_main/common/netmessages.h

/* TODO: Setup classes https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/tier1/utlmemory.h */

typedef enum
{
	NA_NULL = 0,
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
} netadrtype_t;

typedef struct netadr_s
{
public:	// members are public to avoid to much changes
public:
	netadr_s() { memset(ip, 0, 4); type = NA_BROADCAST, port = 0;/*SetIP(0); SetPort(0); SetType(NA_IP);*/ }
	netadr_s(uint unIP, uint16 usPort) { SetIP(unIP); SetPort(usPort); SetType(NA_IP); }
	netadr_s(const char* pch) { SetFromString(pch); }
	void	Clear();	// invalids Address

	void	SetType(netadrtype_t _type) {
		type = _type;
	};
	void	SetPort(unsigned short _port) { port = _port; };
	bool	SetFromSockadr(const struct sockaddr* s);
	void	SetIP(uint8 b1, uint8 b2, uint8 b3, uint8 b4);
	void	SetIP(uint unIP) {};									// Sets IP.  unIP is in host order (little-endian)
	void    SetIPAndPort(uint unIP, unsigned short usPort) { SetIP(unIP); SetPort(usPort); }
	bool	SetFromString(const char* pch, bool bUseDNS = false); // if bUseDNS is true then do a DNS lookup if needed

	bool	CompareAdr(const netadr_s& a, bool onlyBase = false) const;
	bool	CompareClassBAdr(const netadr_s& a) const;
	bool	CompareClassCAdr(const netadr_s& a) const;

	netadrtype_t	GetType() const;
	unsigned short	GetPort() const;

	// DON'T CALL THIS
	const char* ToString(bool onlyBase = false) const; // returns xxx.xxx.xxx.xxx:ppppp

	void	ToString(char* pchBuffer, uint32 unBufferSize, bool onlyBase = false) const; // returns xxx.xxx.xxx.xxx:ppppp
	template< size_t maxLenInChars >
	void	ToString_safe(char(&pDest)[maxLenInChars], bool onlyBase = false) const
	{
		ToString(&pDest[0], maxLenInChars, onlyBase);
	}

	void			ToSockadr(struct sockaddr* s) const;

	// Returns 0xAABBCCDD for AA.BB.CC.DD on all platforms, which is the same format used by SetIP().
	// (So why isn't it just named GetIP()?  Because previously there was a fucntion named GetIP(), and
	// it did NOT return back what you put into SetIP().  So we nuked that guy.)
	unsigned int	GetIPHostByteOrder() const;

	// Returns a number that depends on the platform.  In most cases, this probably should not be used.
	unsigned int	GetIPNetworkByteOrder() const;

	bool	IsLocalhost() const; // true, if this is the localhost IP 
	bool	IsLoopback() const;	// true if engine loopback buffers are used
	bool	IsReservedAdr() const; // true, if this is a private LAN IP
	bool	IsValid() const;	// ip & port != 0
	bool	IsBaseAdrValid() const;	// ip != 0

	void    SetFromSocket(int hSocket);

	bool	Unserialize(bf_read& readBuf);
	bool	Serialize(bf_write& writeBuf);

	bool operator==(const netadr_s& netadr) const { return (CompareAdr(netadr)); }
	bool operator!=(const netadr_s& netadr) const { return !(CompareAdr(netadr)); }
	bool operator<(const netadr_s& netadr) const;

	netadrtype_t	type;
	unsigned char	ip[4];
	unsigned short	port;




} netadr_t;
#if 0
enum EUniverse
{
	k_EUniverseInvalid = 0,
	k_EUniversePublic = 1,
	k_EUniverseBeta = 2,
	k_EUniverseInternal = 3,
	k_EUniverseDev = 4,
	// k_EUniverseRC = 5,				// no such universe anymore
	k_EUniverseMax
};
#endif
enum PeerToPeerAddressType_t
{
	P2P_STEAMID,
};

// Build int subchannel types
// FIXME - these really are game specific.  I wish we could standardize!
enum SteamPeerToPeerChannel_t
{
	STEAM_P2P_GAME_CLIENT = 0,
	STEAM_P2P_GAME_SERVER = 1,
	STEAM_P2P_LOBBY = 2,
	STEAM_P2P_HLTV = 3,
	STEAM_P2P_HLTV1 = 4,
};
#if 0
class CSteamID
{
public:
	// 64 bits total
	union SteamID_t
	{
		struct SteamIDComponent_t
		{
#ifdef VALVE_BIG_ENDIAN
			EUniverse			m_EUniverse : 8;	// universe this account belongs to
			unsigned int		m_EAccountType : 4;			// type of account - can't show as EAccountType, due to signed / unsigned difference
			unsigned int		m_unAccountInstance : 20;	// dynamic instance ID
			uint32				m_unAccountID : 32;			// unique account identifier
#else
			uint32				m_unAccountID : 32;			// unique account identifier
			unsigned int		m_unAccountInstance : 20;	// dynamic instance ID
			unsigned int		m_EAccountType : 4;			// type of account - can't show as EAccountType, due to signed / unsigned difference
			EUniverse			m_EUniverse : 8;	// universe this account belongs to
#endif
		} m_comp;

		uint64 m_unAll64Bits;
	} m_steamid;
};
#endif

class CPeerToPeerAddress
{
public:
	uint64_t m_steamID;
	int m_steamChannel; // SteamID channel (like a port number to disambiguate multiple connections)

	PeerToPeerAddressType_t m_AddrType;
};


enum NetworkSystemAddressType_t
{
	NSAT_NETADR,
	NSAT_P2P,
	NSAT_PROXIED_GAMESERVER,	// Client proxied through Steam Datagram Transport
	NSAT_PROXIED_CLIENT,		// Client proxied through Steam Datagram Transport
};

struct ns_address
{
	netadr_t m_adr; // ip:port and network type (NULL/IP/BROADCAST/etc).
	CPeerToPeerAddress m_steamID; // SteamID destination
	NetworkSystemAddressType_t m_AddrType;
};
class Netmsgbinder;
#pragma pack(push, 1)
class NetworkChannel {
public:
	VIRTUAL_METHOD(const char*, getName, 0, (void), (this))
		VIRTUAL_METHOD(const char*, getAddress, 1, (void), (this))
		VIRTUAL_METHOD(float, getLatency, 9, (int flow), (this, flow))

		VIRTUAL_METHOD(INetMessageBinder*, FindMessageBinder, 27, (int type, int index), (this, type, index))
		VIRTUAL_METHOD(bool, RegisterMessage, 28, (INetMessageBinder* msg), (this, msg))
		VIRTUAL_METHOD(void, ProcessPacket, 39, (netpacket_s* packet, bool bHasHeader), (this, packet, bHasHeader)) //	ProcessPacket(struct netpacket_s* packet, bool bHasHeader) = 0;
		VIRTUAL_METHOD(bool, SendNetMsg, 40, (NetworkMessage* msg, bool bForceReliable, bool bVoice = false), (this, msg, bForceReliable, bVoice))
		VIRTUAL_METHOD(bool, SendData, 41, (bf_write& msg, bool bReliable = true), (this, msg, bReliable))
		VIRTUAL_METHOD(int, SendDatagram, 46, (/*bf_write**/void* data), (this, data))
		VIRTUAL_METHOD(bool, Transmit, 47, (bool onlyReliable), (this, onlyReliable))
		VIRTUAL_METHOD(SOCKET, GetSocket, 51, (void), (this))
		VIRTUAL_METHOD(void, SetFileTransmissionMode, 59, (bool bBackGroundMode), (this, bBackGroundMode))
		VIRTUAL_METHOD(unsigned int, RequestFile, 61, (const char* filename, bool bIsReplayDemoFile), (this, filename, bIsReplayDemoFile))
		VIRTUAL_METHOD(void, SetMaxRoutablePayloadSize, 68, (int nSize), (this, nSize))
		VIRTUAL_METHOD(const unsigned char*, GetChannelEncryptionKey, 75, (void), (this));

		//virtual unsigned int RequestFile(const char* filename) = 0; <- 62


	/*
		__int32 vtable; //0x0000
		Netmsgbinder* msgbinder1;
		Netmsgbinder* msgbinder2;
		Netmsgbinder* msgbinder3;
		Netmsgbinder* msgbinder4;
		unsigned char m_bProcessingMessages;
		unsigned char m_bShouldDelete;
		char pad_0x0016[0x2]

	*/
	//std::byte pad[24];

	//std::byte pad[20];
	Netmsgbinder* msgbinder1; //0x0004 
	Netmsgbinder* msgbinder2; //0x0008 
	Netmsgbinder* msgbinder3; //0x000C 
	Netmsgbinder* msgbinder4; //0x0010 
	unsigned char m_bProcessingMessages;
	unsigned char m_bShouldDelete;
	char pad_0x0016[0x2];

	__int32 OutSequenceNr;
	__int32 InSequenceNr;
	__int32 OutSequenceNrAck;
	__int32 OutReliableState;
	__int32 InReliableState;
	__int32 chokedPackets; //  E8 ? ? ? ? 8B CB E8 ? ? ? ? 33 D2       0x2C

	bf_write m_StreamReliable; //0x0030 
	/*CUtlMemory*/ char m_ReliableDataBuffer[12]; //0x0048 
	bf_write m_StreamUnreliable; //0x0054 
	/*CUtlMemory*/ char m_UnreliableDataBuffer[12]; //0x006C 
	bf_write m_StreamVoice; //0x0078 
	/*CUtlMemory*/char m_VoiceDataBuffer[12]; //0x0090 
	__int32 m_Socket; //0x009C 
	//__int32 m_StreamSocket; //0x00A0 
	__int32 m_MaxReliablePayloadSize; //0x00A4  //0x00A0 2020
	bool m_bWasLastMessageReliable; //0x00A8
	char pad_0x00A8[0x3]; //0x00A8
	ns_address remote_address; //0x00AC //0xA8 2020
	char m_szRemoteAddressName[64]; //20180 0xB4 padding added by dylan
	char dylanpadding[4];
	float last_received; //2018 0x10C
	//float last_received_pad;
	//char pad_0x00BC[0x4]; //0x00BC
	double /*float*/ connect_time; //0x00C0 //dylan found 0x110
	//char pad_0x00C4[0x4]; //0x00C4
	__int32 m_Rate;
	__int32 m_RatePad;
	/*float*/double m_fClearTime; //0x128 not anymore
	CUtlVector<dataFragments_t*>	m_WaitingList[2]; //0x128 as of 2020
	char pad_blehch[0x260]; //0x150
	subChannel_s					m_SubChannels[MAX_SUBCHANNELS]; //0x3B0 as of 2020
	char pad_blech2[8]; //0x490 as of 2020
	char NEWPAD2018[4];
	netflow_t m_DataFlow[2]; //new 2018 0x49C //0x5C0
	int	m_MsgStats[16];	// total bytes for each message group
#if 0
	//CUtlVector m_WaitingList[0]; //0x00D8 
	//std::byte  m_WaitingList0[12];
	//CUtlVector m_WaitingList[1]; //0x00EC 
	//std::byte m_WaitingList1[12];

	char pad_0x0100[0x4120]; //0x0100
#endif
	__int32 m_PacketDrop; //0x4220 
	char m_Name[32]; //0x4224 
	__int32 m_ChallengeNr; //0x4244 
	float m_Timeout; //0x4248 
	//INetChannelHandler* m_MessageHandler; //0x424C 
	INetChannelHandler* m_MessageHandler; //0x4284   0x4288
	/*CUtlVector*/char m_NetMessages[16]; //dylan found 0x4284
	__int32 dylanUnknown;
	void* m_pDemoRecorder; //0x429C
	float m_flInterpolationAmount; //0x42A0
	double m_flRemoteFrameTime; //0x42A4
	float m_flRemoteFrameTimeStdDeviation; //0x42AC
	__int32 m_nMaxRoutablePayloadSize; //0x42B0
	__int32 m_nSplitPacketSequence; //dylan found 0x42b4
	char pad_0x4280[0x14]; //0x4280
};
#pragma pack(pop)


class CNetChan : public NetworkChannel
{

private: 



public:
	CNetChan();
	~CNetChan();

public:	// INetChannelInfo interface

	const char* GetName(void) const;
	const char* GetAddress(void) const;
	float		GetTime(void) const;
	float		GetTimeConnected(void) const;
	float		GetTimeSinceLastReceived(void) const;
	int			GetDataRate(void) const;
	int			GetBufferSize(void) const;

	bool		IsLoopback(void) const;
	bool		IsNull() const; // .dem file playback channel is of type NA_NULL!!!
	bool		IsTimingOut(void) const;
	bool		IsPlayback(void) const;

	float		GetLatency(int flow) const;
	float		GetAvgLatency(int flow) const;
	float		GetAvgLoss(int flow) const;
	float		GetAvgData(int flow) const;
	float		GetAvgChoke(int flow) const;
	float		GetAvgPackets(int flow) const;
	int			GetTotalData(int flow) const;
	int			GetSequenceNr(int flow) const;
	bool		IsValidPacket(int flow, int frame_number) const;
	float		GetPacketTime(int flow, int frame_number) const;
	int			GetPacketBytes(int flow, int frame_number, int group) const;
	bool		GetStreamProgress(int flow, int* received, int* total) const;
	float		GetCommandInterpolationAmount(int flow, int frame_number) const;
	void		GetPacketResponseLatency(int flow, int frame_number, int* pnLatencyMsecs, int* pnChoke) const;
	void		GetRemoteFramerate(float* pflFrameTime, float* pflFrameTimeStdDeviation) const;
	float		GetTimeoutSeconds() const;

public:	// INetChannel interface

	void		SetDataRate(float rate);
	bool		RegisterMessage(NetworkMessage* msg);
	bool		StartStreaming(unsigned int challengeNr);
	void		ResetStreaming(void);
	void		SetTimeout(float seconds);
	void		SetDemoRecorder(void* recorder);
	void		SetChallengeNr(unsigned int chnr);

	void		Reset(void);
	void		Clear(void);
	void		Shutdown(const char* reason);

	void		ProcessPlayback(void);
	bool		ProcessStream(void);
	void		ProcessPacket(netpacket_t* packet, bool bHasHeader);

	void		SetCompressionMode(bool bUseCompression);
	void		SetFileTransmissionMode(bool bBackgroundMode);
	bool		SendNetMsg(NetworkMessage& msg, bool bForceReliable = false, bool bVoice = false); // send a net message
	bool		SendData(bf_write& msg, bool bReliable = true); // send a chunk of data
	bool		SendFile(const char* filename, unsigned int transferID); // transmit a local file
	void		SetChoked(void); // choke a packet
	int			SendDatagram(bf_write* data); // build and send datagram packet
	unsigned int RequestFile(const char* filename); // request remote file to upload, returns request ID
	void RequestFile_OLD(const char* filename, unsigned int transferID); // request remote file to upload, returns request ID
	void		DenyFile(const char* filename, unsigned int transferID); // deny a file request
	bool		Transmit(bool onlyReliable = false); // send data from buffers

	const netadr_tt& GetRemoteAddress(void) const;
	INetChannelHandler* GetMsgHandler(void) const;
	int				GetDropNumber(void) const;
	int				GetSocket(void) const;
	unsigned int	GetChallengeNr(void) const;
	void			GetSequenceData(int& nOutSequenceNr, int& nInSequenceNr, int& nOutSequenceNrAck);
	void			SetSequenceData(int nOutSequenceNr, int nInSequenceNr, int nOutSequenceNrAck);

	void		UpdateMessageStats(int msggroup, int bits);
	bool		CanPacket(void) const;
	bool		IsOverflowed(void) const;
	bool		IsTimedOut(void) const;
	bool		HasPendingReliableData(void);
	void		SetMaxBufferSize(bool bReliable, int nBytes, bool bVoice = false);
	virtual int		GetNumBitsWritten(bool bReliable);
	virtual void	SetInterpolationAmount(float flInterpolationAmount);
	virtual void	SetRemoteFramerate(float flFrameTime, float flFrameTimeStdDeviation);

	// Max # of payload bytes before we must split/fragment the packet
	virtual void	SetMaxRoutablePayloadSize(int nSplitSize);
	virtual int	GetMaxRoutablePayloadSize();

	int			IncrementSplitPacketSequence();
public:

	void		Setup(int sock, netadr_tt* adr, const char* name, INetChannelHandler* handler);
	// Send queue management
	void		IncrementQueuedPackets();
	void		DecrementQueuedPackets();
	bool		HasQueuedPackets() const;

private:


	void	FlowReset(void);
	void	FlowUpdate(int flow, int addbytes);
	void	FlowNewPacket(int flow, int seqnr, int acknr, int nChoked, int nDropped, int nSize);

	bool	ProcessMessages(bf_read& buf);
	bool	ProcessControlMessage(int cmd, bf_read& buf);
	bool	SendReliableViaStream(dataFragments_t* data);
	bool	SendReliableAcknowledge(int seqnr);
	int		ProcessPacketHeader(netpacket_t* packet);
	void	AcknowledgeSubChannel(int seqnr, int list);

	bool	CreateFragmentsFromBuffer(bf_write* buffer, int stream);
	bool	CreateFragmentsFromFile(const char* filename, int stream, unsigned int transferID);

	void	CompressFragments();
	void	UncompressFragments(dataFragments_t* data);

	bool	SendSubChannelData(bf_write& buf);
	bool	ReadSubChannelData(bf_read& buf, int stream);
	void	AcknowledgeSeqNr(int seqnr);
	void	CheckWaitingList(int nList);
	bool	CheckReceivingList(int nList);
	void	RemoveHeadInWaitingList(int nList);
	bool	IsFileInWaitingList(const char* filename);
	subChannel_s* GetFreeSubChannel(); // NULL == all subchannels in use
	void	UpdateSubChannels(void);
	void	SendTCPData(void);


	NetworkMessage* FindMessage(int type);

public:

	bool		m_bProcessingMessages;
	bool		m_bShouldDelete;

	// last send outgoing sequence number
	int			m_nOutSequenceNr;
	// last received incoming sequnec number
	int			m_nInSequenceNr;
	// last received acknowledge outgoing sequnce number
	int			m_nOutSequenceNrAck;

	// state of outgoing reliable data (0/1) flip flop used for loss detection
	int			m_nOutReliableState;
	// state of incoming reliable data
	int			m_nInReliableState;

	int			m_nChokedPackets;	//number of choked packets


	// Reliable data buffer, send which each packet (or put in waiting list)
	bf_write	m_StreamReliable;
	CUtlMemory<std::byte> m_ReliableDataBuffer;
	//void* m_ReliableDataBuffer;
	// unreliable message buffer, cleared which each packet
	bf_write	m_StreamUnreliable;
	CUtlMemory<std::byte> m_UnreliableDataBuffer;
	//void* m_UnreliableDataBuffer;


	bf_write	m_StreamVoice;
	//CUtlMemory<byte> m_VoiceDataBuffer;
	void* m_VoiceDataBuffer;
	// don't use any vars below this (only in net_ws.cpp)

	int			m_Socket;   // NS_SERVER or NS_CLIENT index, depending on channel.
	int			m_StreamSocket;	// TCP socket handle

	unsigned int m_MaxReliablePayloadSize;	// max size of reliable payload in a single packet	

	// Address this channel is talking to.
	netadr_tt	remote_address;

	// For timeouts.  Time last message was received.
	float		last_received;
	// Time when channel was connected.
	double      connect_time;

	// Bandwidth choke
	// Bytes per second
	int			m_Rate;
	// If realtime > cleartime, free to send next packet
	double		m_fClearTime;

	CUtlVector<dataFragments_t*>	m_WaitingList[MAX_STREAMS];	// waiting list for reliable data and file transfer
	//dataFragments_t* m_WaitingList[MAX_STREAMS];
	dataFragments_t					m_ReceiveList[MAX_STREAMS]; // receive buffers for streams
	subChannel_s					m_SubChannels[MAX_SUBCHANNELS];

	unsigned int	m_FileRequestCounter;	// increasing counter with each file request
	bool			m_bFileBackgroundTranmission; // if true, only send 1 fragment per packet
	bool			m_bUseCompression;	// if true, larger reliable data will be bzip compressed

	// TCP stream state maschine:
	bool		m_StreamActive;		// true if TCP is active
	int			m_SteamType;		// STREAM_CMD_*
	int			m_StreamSeqNr;		// each blob send of TCP as an increasing ID
	int			m_StreamLength;		// total length of current stream blob
	int			m_StreamReceived;	// length of already received bytes
	char		m_SteamFile[MAX_PATH];	// if receiving file, this is it's name
	CUtlMemory<std::byte> m_StreamData;			// Here goes the stream data (if not file). Only allocated if we're going to use it.
	//void* m_StreamData;
	// packet history
	netflow_t		m_DataFlow[2];
	int				m_MsgStats[INetChannelInfo::TOTAL];	// total bytes for each message group


	int				m_PacketDrop;	// packets lost before getting last update (was global net_drop)

	char			m_Name[32];		// channel name

	unsigned int	m_ChallengeNr;	// unique, random challenge number 

	float		m_Timeout;		// in seconds 

	INetChannelHandler* m_MessageHandler;	// who registers and processes messages
	CUtlVector<NetworkMessage*>	m_NetMessages;		// list of registered message
	//NetworkMessage* m_NetMessages;
	void** m_DemoRecorder;			// if != NULL points to a recording/playback demo object
	int							m_nQueuedPackets;

	float						m_flInterpolationAmount;
	float						m_flRemoteFrameTime;
	float						m_flRemoteFrameTimeStdDeviation;
	int							m_nMaxRoutablePayloadSize;

	int							m_nSplitPacketSequence;
};


#if 1
#include "../SDK/checksum_crc.h"
FORCEINLINE unsigned short BufferToShortChecksum(const void* pvData, size_t nLength)
{
	CRC32_t crc = CRC32_ProcessSingleBuffer(pvData, nLength);

	unsigned short lowpart = (crc & 0xffff);
	unsigned short highpart = ((crc >> 16) & 0xffff);

	return (unsigned short)(lowpart ^ highpart);
}

#else
// If the CRC version ever is deemed to expensive, here's a quick xor version.
//  It's probably not super robust.
FORCEINLINE inline unsigned short BufferToShortChecksum(const void* pvData, size_t nSize)
{
	const uint32* pData = (const uint32*)pvData;
	unsigned short us = 0;
	while (nSize >= sizeof(uint32))
	{
		us ^= (*pData & 0xffff);
		us ^= ((*pData >> 16) & 0xffff);
		nSize -= sizeof(uint32);
		pData += sizeof(uint32);
	}
	const byte* pbData = (const byte*)pData;
	while (nSize > 0)
	{
		us ^= *pbData;
		++pbData;
		--nSize;
	}
	return us;
}
#endif

