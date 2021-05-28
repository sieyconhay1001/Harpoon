   
//===== Copyright  1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: An application framework 
//
// $Revision: $
// $NoKeywords: $
//===========================================================================//
#pragma once
#ifndef IAPPSYSTEM_H
#define IAPPSYSTEM_H

#ifdef COMPILER_MSVC
#pragma once
#endif
typedef void* (*CreateInterfaceFn)(const char* pName, int* pReturnCode);
class KeyValues;
//#include "../../Interfaces.h"
//#include "../OsirisSDK/KeyValues.h"
typedef uint64_t XUID;

//-----------------------------------------------------------------------------
// Specifies a module + interface name for initialization
//-----------------------------------------------------------------------------
struct AppSystemInfo_t
{
	const char* m_pModuleName;
	const char* m_pInterfaceName;
};


//-----------------------------------------------------------------------------
// Client systems are singleton objects in the client codebase responsible for
// various tasks
// The order in which the client systems appear in this list are the
// order in which they are initialized and updated. They are shut down in
// reverse order from which they are initialized.
//-----------------------------------------------------------------------------
enum InitReturnVal_t
{
	INIT_FAILED = 0,
	INIT_OK,
	INIT_LAST_VAL,
};

enum AppSystemTier_t
{
	APP_SYSTEM_TIER0 = 0,
	APP_SYSTEM_TIER1,
	APP_SYSTEM_TIER2,
	APP_SYSTEM_TIER3,

	APP_SYSTEM_TIER_OTHER,
};


class IAppSystem
{
public:
	// Here's where the app systems get to learn about each other 
	virtual bool Connect(CreateInterfaceFn factory) = 0;
	virtual void Disconnect() = 0;

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void* QueryInterface(const char* pInterfaceName) = 0;

	// Init, shutdown
	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;

	// Returns all dependent libraries
	virtual const AppSystemInfo_t* GetDependencies() { return NULL; }

	// Returns the tier
	virtual AppSystemTier_t GetTier() { return APP_SYSTEM_TIER_OTHER; }

	// Reconnect to a particular interface
	virtual void Reconnect(CreateInterfaceFn factory, const char* pInterfaceName) {}
	// Is this appsystem a singleton? (returns false if there can be multiple instances of this interface)
	virtual bool IsSingleton() { return true; }
};


//-----------------------------------------------------------------------------
// Helper empty implementation of an IAppSystem
//-----------------------------------------------------------------------------
template< class IInterface >
class CBaseAppSystem : public IInterface
{
public:
	// Here's where the app systems get to learn about each other 
	virtual bool Connect(CreateInterfaceFn factory) { return true; }
	virtual void Disconnect() {}

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void* QueryInterface(const char* pInterfaceName) { return NULL; }

	// Init, shutdown
	virtual InitReturnVal_t Init() { return INIT_OK; }
	virtual void Shutdown() {}

	virtual const AppSystemInfo_t* GetDependencies() { return NULL; }
	virtual AppSystemTier_t GetTier() { return APP_SYSTEM_TIER_OTHER; }

	virtual void Reconnect(CreateInterfaceFn factory, const char* pInterfaceName)
	{
		//ReconnectInterface(factory, pInterfaceName);
	}
};


//-----------------------------------------------------------------------------
// Helper implementation of an IAppSystem for tier0
//-----------------------------------------------------------------------------
template< class IInterface >
class CTier0AppSystem : public CBaseAppSystem< IInterface >
{
};


#endif // IAPPSYSTEM_H




class IMatchExtensions
{
public:
	// Registers an extension interface
	virtual void RegisterExtensionInterface(char const* szInterfaceString, void* pvInterface) = 0;

	// Unregisters an extension interface
	virtual void UnregisterExtensionInterface(char const* szInterfaceString, void* pvInterface) = 0;

	// Gets a pointer to a registered extension interface
	virtual void* GetRegisteredExtensionInterface(char const* szInterfaceString) = 0;
};

typedef void (*RankedMatchStartCallback)(KeyValues* pSettings, unsigned long volatile* pResult);

class IMatchSession
{
public:
	// Get an internal pointer to session system-specific data
	virtual KeyValues * GetSessionSystemData() = 0;

	// Get an internal pointer to session settings
	virtual KeyValues* GetSessionSettings() = 0;

	// Update session settings, only changing keys and values need
	// to be passed and they will be updated
	virtual void UpdateSessionSettings(KeyValues* pSettings) = 0;

	// Issue a session command
	virtual void Command(KeyValues* pCommand) = 0;

	// Get the lobby or XSession ID 
	virtual uint64_t GetSessionID() = 0;

	// Callback when team changes
	virtual void UpdateTeamProperties(KeyValues* pTeamProperties) = 0;
};

#if 0
class IMatchExtensions
{
public:
	// Registers an extension interface
	virtual void RegisterExtensionInterface(char const* szInterfaceString, void* pvInterface) = 0;

	// Unregisters an extension interface
	virtual void UnregisterExtensionInterface(char const* szInterfaceString, void* pvInterface) = 0;

	// Gets a pointer to a registered extension interface
	virtual void* GetRegisteredExtensionInterface(char const* szInterfaceString) = 0;
};
#endif


class IMatchEventsSink
{
public:
	virtual void OnEvent(KeyValues * pEvent) {}

	//
	// List of events
	//

/*
	"OnSysStorageDevicesChanged"
		Signalled when system storage device change is detected.
	params:
		void

	"OnSysSigninChange"
		Signalled when one or more users sign out.
	params:
		string "action"		- signin change event: "signin", "signout"
		int "numUsers"		- how many users signed in/out (defines valid user0 - userN-1 fields)
		int "mask"			- bitmask of controllers affected
		int	"user%d"		- controller index affected

	"OnSysXUIEvent"
		Signalled when an XUI event occurs.
	params:
		string "action"		- XUI action type: "opening", "closed"

	"OnSysMuteListChanged"
		Signalled when system mute list change occurs.
	params:
		void

	"OnSysInputDevicesChanged"
		Signalled when input device disconnection is detected.
	params:
		int "mask"			- bitmask of which slot's controller was disconnected [0-1]



	"OnEngineLevelLoadingStarted"
		Signalled when a level starts loading.
	params:
		string "name"		- level name

	"OnEngineListenServerStarted"
		Signalled when a listen server level loads enough to accept client connections.
	params:
		void

	"OnEngineLevelLoadingTick"
		Signalled periodically while a level is loading,
		after loading started and before loading finished.
	params:
		void

	"OnEngineLevelLoadingFinished"
		Signalled when a level is finished loading.
	params:
		int "error"			- whether an extended error occurred
		string "reason"		- reason description

	"OnEngineClientSignonStateChange"
		Signalled when client's signon state is changing.
	params:
		int "slot"			- client ss slot
		int "old"			- old state
		int "new"			- new state
		int "count"			- count

	"OnEngineDisconnectReason"
		Signalled before a disconnect is going to occur and a reason
		for disconnect is available.
	params:
		string "reason"		- reason description

	"OnEngineEndGame"
		Signalled before a disconnect is going to occur and notifies the members
		of the game that the game has reached a conclusion or a vote to end the
		game has passed and the game should terminate and return to lobby if possible.
	params:
		string "reason"		- reason description



	"OnMatchPlayerMgrUpdate"
		Signalled when a player manager update occurs.
	params:
		string "update" =	- update type
							"searchstarted"		- search started
							"searchfinished"	- search finished
							"friend"			- friend details updated
		uint64 "xuid"		- xuid of a player if applicable

	"OnMatchPlayerMgrReset"
		Signalled when the game needs to go into attract mode.
	params:
		string "reason"		- one of the following reasons:
						"GuestSignedIn"			- guest user signed in
						"GameUserSignedOut"		- user involved in game has signed out

	"OnMatchServerMgrUpdate"
		Signalled when a server manager update occurs.
	params:
		string "update" =	- update type
							"searchstarted"		- search started
							"searchfinished"	- search finished
							"server"			- server details updated
		uint64 "xuid"		- xuid of a server if applicable

	"OnMatchSessionUpdate"
		Signalled when a session changes.
	params:
		strings "state" =	- new state of the session
							"ready"			- session is completely initialized and ready
							"updated"		- session settings have been updated



	"OnNetLanConnectionlessPacket"
		Signalled when a lan network packet is received.
	params:
		string "from"		- netadr of sender as recorded by network layer
		subkey				- packet message



	"OnProfilesChanged"
		Signalled when new number of game users are set for the game.
	params:
		int "numProfiles"	- number of game users set for the game

	"OnProfileDataLoaded"
		Signalled when a user profile title data is loaded.
	params:
		int "iController"	- index of controller whose title data is now loaded

	"OnProfileStorageAvailable"
		Signalled when a user profile storage device is selected.
	params:
		int "iController"	- index of controller whose storage device is now selected

	"OnProfileUnavailable"
		Signalled when a user profile is detected as unavailable.
	params:
		int "iController"	- index of controller whose profile was detected as unavailable



	"OnPlayerUpdated"
		Signalled when information about a player changes.
	params:
		uint64 "xuid"		- XUID of the player updated

	"OnPlayerRemoved"
		Signalled when a player is removed from the game.
	params:
		uint64 "xuid"		- XUID of the player removed

	"OnPlayerMachinesConnected"
		Signalled when new machines become part of the session, they will be last
		in the list of connected machines.
	params:
		int "numMachines"	- number of new machines connected

	"OnPlayerActivity"
		Signalled when a player activity is detected.
	params:
		uint64 "xuid"		- XUID of the player
		string "act"		- type of activity:
							"voice"		- player is voice chatting



	"OnMuteChanged"
		Signalled when a mute list is updated.
	params:
		void

	"OnInvite"
		Signalled when game invite event occurs.
	params:
		int "user"			- controller index accepting the invite or causing invite error
		string "sessioninfo" - session info of the invite host
		string "action" =	- invite action
							"accepted" - when an invite is accepted by user
							"storage" - when a storage device needs to be validated
							"error" - when an error occurs that prevents invite from being accepted
							"join" - when destructive actions or storage devices are confirmed by user
							"deny" - when invite is rejected by user
		string "error"		- error description: "NotOnline", "NoMultiplayer", etc.
		ptr int "confirmed"	- handler should set pointed int to 0 if confirmation is pending
								and send a "join" action OnInvite event after destructive
								actions are confirmed by user, storage devices are mounted, etc.
*/

};

class IMatchEventsSubscription
{
public:
	virtual void Subscribe(IMatchEventsSink * pSink) = 0;
	virtual void Unsubscribe(IMatchEventsSink* pSink) = 0;

	virtual void BroadcastEvent(KeyValues* pEvent) = 0;

	virtual void RegisterEventData(KeyValues* pEventData) = 0;
	virtual KeyValues* GetEventData(char const* szEventDataKey) = 0;
};


struct TitleDataFieldsDescription_t
{
	enum DataType_t
	{
		DT_0 = 0,
		DT_uint8 = 8,
		DT_BITFIELD = 9,
		DT_uint16 = 16,
		DT_uint32 = 32,
		DT_float = 33,
		DT_uint64 = 64
	};

	enum DataBlock_t
	{
		DB_TD1 = 0,
		DB_TD2 = 1,
		DB_TD3 = 2,

		DB_TD_COUNT = 3
	};

	char const* m_szFieldName;
	DataBlock_t m_iTitleDataBlock;
	DataType_t m_eDataType;
	union
	{
		int m_numBytesOffset;
		int m_nUserDataValue0;
	};
};

struct TitleAchievementsDescription_t
{
	char const* m_szAchievementName;			// Name by which achievement can be awarded and queried
	int m_idAchievement;						// Achievement ID on the platform
	int m_numComponents;						// Number of achievement component title data bits
};

struct TitleAvatarAwardsDescription_t
{
	char const* m_szAvatarAwardName;			// Name by which avatar award can be awarded and queried
	int m_idAvatarAward;						// Avatar award ID on the platform
	char const* m_szTitleDataBitfieldStatName;	// Name of a bitfield in title data storage representing whether avatar award has been earned
};

struct TitleDlcDescription_t
{
	uint64_t m_uiLicenseMaskId;
	int m_idDlcAppId;
	int m_idDlcPackageId;
	char const* m_szTitleDataBitfieldStatName;	// Name of a bitfield in title data storage representing whether dlc has been discovered installed
};

enum TitleSettingsFlags_t
{
	MATCHTITLE_SETTING_MULTIPLAYER = (1 << 0),	// Title wants network sockets initialization
	MATCHTITLE_SETTING_NODEDICATED = (1 << 1),	// Title doesn't support dedicated servers
	MATCHTITLE_PLAYERMGR_DISABLED = (1 << 2),	// Title doesn't need friends presence poll
	MATCHTITLE_SERVERMGR_DISABLED = (1 << 3),	// Title doesn't need group servers poll
	MATCHTITLE_INVITE_ONLY_SINGLE_USER = (1 << 4),	// Accepted game invite forcefully disables splitscreen (e.g.: 1-on-1 games)
	MATCHTITLE_VOICE_INGAME = (1 << 5),	// When in active gameplay lobby system doesn't transmit voice
	MATCHTITLE_XLSPPATCH_SUPPORTED = (1 << 6),	// Title supports xlsp patch containers
	MATCHTITLE_PLAYERMGR_ALLFRIENDS = (1 << 7),	// Player manager by default fetches only friends for same game, this will force all friends to be fetched
	MATCHTITLE_PLAYERMGR_FRIENDREQS = (1 << 8),	// Player manager by default fetches only real friends, this will force friend requests to also be fetched
};

class IMatchTitle
{
public:
	// Title ID
	virtual uint64_t GetTitleID() = 0;

	// Service ID for XLSP
	virtual uint64_t GetTitleServiceID() = 0;

	// Describe title settings using a bitwise combination of flags
	virtual uint64_t GetTitleSettingsFlags() = 0;

	// Prepare network startup params for the title
	virtual void PrepareNetStartupParams(void* pNetStartupParams) = 0;

	// Get total number of players supported by the title
	virtual int GetTotalNumPlayersSupported() = 0;

	// Get a guest player name
	virtual char const* GetGuestPlayerName(int iUserIndex) = 0;

	// Decipher title data fields
	virtual TitleDataFieldsDescription_t const* DescribeTitleDataStorage() = 0;

	// Title achievements
	virtual TitleAchievementsDescription_t const* DescribeTitleAchievements() = 0;

	// Title avatar awards
	virtual TitleAvatarAwardsDescription_t const* DescribeTitleAvatarAwards() = 0;

	// Title leaderboards
	virtual KeyValues* DescribeTitleLeaderboard(char const* szLeaderboardView) = 0;

	// Sets up all necessary client-side convars and user info before
	// connecting to server
	virtual void PrepareClientForConnect(KeyValues* pSettings) = 0;

	// Start up a listen server with the given settings
	virtual bool StartServerMap(KeyValues* pSettings) = 0;

	// Title DLC description
	virtual TitleDlcDescription_t const* DescribeTitleDlcs() = 0;

	// Run every frame
	virtual void RunFrame() = 0;
};

//
// Matchmaking title settings extension interface
//

class IMatchTitleGameSettingsMgr
{
public:
	// Extends server game details
	virtual void ExtendServerDetails(KeyValues * pDetails, KeyValues * pRequest) = 0;

	// Adds the essential part of game details to be broadcast
	virtual void ExtendLobbyDetailsTemplate(KeyValues* pDetails, char const* szReason, KeyValues* pFullSettings) = 0;

	// Extends game settings update packet for lobby transition,
	// either due to a migration or due to an endgame condition
	virtual void ExtendGameSettingsForLobbyTransition(KeyValues* pSettings, KeyValues* pSettingsUpdate, bool bEndGame) = 0;

	// Adds data for datacenter reporting
	virtual void ExtendDatacenterReport(KeyValues* pReportMsg, char const* szReason) = 0;


	// Rolls up game details for matches grouping
	//	valid pDetails, null pRollup
	//		returns a rollup representation of pDetails to be used as an indexing key
	//	valid pDetails, valid pRollup (usually called second time)
	//		rolls the details into the rollup, aggregates some values, when
	//		the aggregate values are missing in pRollup, then this is the first
	//		details entry being aggregated and would establish the first rollup
	//		returns pRollup
	//	null pDetails, valid pRollup
	//		tries to determine if the rollup should remain even though no details
	//		matched it, adjusts pRollup to represent no aggregated data
	//		returns null to drop pRollup, returns pRollup to keep rollup
	virtual KeyValues* RollupGameDetails(KeyValues* pDetails, KeyValues* pRollup, KeyValues* pQuery) = 0;


	// Defines session search keys for matchmaking
	virtual KeyValues* DefineSessionSearchKeys(KeyValues* pSettings) = 0;

	// Defines dedicated server search key
	virtual KeyValues* DefineDedicatedSearchKeys(KeyValues* pSettings, bool bNeedOfficialServer, int nSearchPass) = 0;


	// Initializes full game settings from potentially abbreviated game settings
	virtual void InitializeGameSettings(KeyValues* pSettings, char const* szReason) = 0;

	// Sets the bspname key given a mapgroup
	virtual void SetBspnameFromMapgroup(KeyValues* pSettings) = 0;

	// Extends game settings update packet before it gets merged with
	// session settings and networked to remote clients
	virtual void ExtendGameSettingsUpdateKeys(KeyValues* pSettings, KeyValues* pUpdateDeleteKeys) = 0;

	virtual KeyValues* ExtendTeamLobbyToGame(KeyValues* pSettings) = 0;

	// Prepares system for session creation
	virtual KeyValues* PrepareForSessionCreate(KeyValues* pSettings) = 0;


	// Executes the command on the session settings, this function on host
	// is allowed to modify Members/Game subkeys and has to fill in modified players KeyValues
	// When running on a remote client "ppPlayersUpdated" is NULL and players cannot
	// be modified
	virtual void ExecuteCommand(KeyValues* pCommand, KeyValues* pSessionSystemData, KeyValues* pSettings, KeyValues** ppPlayersUpdated) = 0;

	// Prepares the host lobby for game or adjust settings of new players who
	// join a game in progress, this function is allowed to modify
	// Members/Game subkeys and has to fill in modified players KeyValues
	virtual void PrepareLobbyForGame(KeyValues* pSettings, KeyValues** ppPlayersUpdated) = 0;

	// Prepares the host team lobby for game adjusting the game settings
	// this function is allowed to prepare modification package to update
	// Game subkeys.
	// Returns the update/delete package to be applied to session settings
	// and pushed to dependent two sesssion of the two teams.
	virtual KeyValues* PrepareTeamLinkForGame(KeyValues* pSettingsLocal, KeyValues* pSettingsRemote) = 0;


	// Prepares the client lobby for migration
	// this function is called when the client session is still in the state
	// of "client" while handling the original host disconnection and decision
	// has been made that local machine will be elected as new "host"
	// Returns NULL if migration should proceed normally
	// Returns [ kvroot { "error" "n/a" } ] if migration should be aborted.
	virtual KeyValues* PrepareClientLobbyForMigration(KeyValues* pSettingsLocal, KeyValues* pMigrationInfo) = 0;

	// Prepares the session for server disconnect
	// this function is called when the session is still in the active gameplay
	// state and while localhost is handling the disconnection from game server.
	// Returns NULL to allow default flow
	// Returns [ kvroot { "disconnecthdlr" "<opt>" } ] where <opt> can be:
	//		"destroy" : to trigger a disconnection error and destroy the session
	//		"lobby" : to initiate a "salvaging" lobby transition
	virtual KeyValues* PrepareClientLobbyForGameDisconnect(KeyValues* pSettingsLocal, KeyValues* pDisconnectInfo) = 0;

	// Validates if client profile can set a stat or get awarded an achievement
	virtual bool AllowClientProfileUpdate(KeyValues* kvUpdate) = 0;

	// Retrieves the indexed formula from the match system settings file. (MatchSystem.360.res)
	virtual char const* GetFormulaAverage(int index) = 0;

	// Called by the client to notify matchmaking that it should update matchmaking properties based
	// on player distribution among the teams.
	virtual void UpdateTeamProperties(KeyValues* pCurrentSettings, KeyValues* pTeamProperties) = 0;
};

class IPlayerManager;
class IGameManager;
class IServerManager;
class ISearchManager;
class IMatchVoice;
class IDatacenter;
class IDlcManager;

class IMatchSystem
{
public:
	virtual IPlayerManager* GetPlayerManager() = 0;

	virtual IMatchVoice* GetMatchVoice() = 0;

	virtual IServerManager* GetUserGroupsServerManager() = 0;

	virtual ISearchManager* CreateGameSearchManager(KeyValues* pParams) = 0;

	virtual IDatacenter* GetDatacenter() = 0;

	virtual IDlcManager* GetDlcManager() = 0;
};

struct MM_QOS_t
{
	int nPingMsMin;		// Minimum round-trip time in ms
	int nPingMsMed;		// Median round-trip time in ms
	float flBwUpKbs;	// Bandwidth upstream in kilobytes/s
	float flBwDnKbs;	// Bandwidth downstream in kilobytes/s
	float flLoss;		// Average packet loss in percents
};

struct MM_GameDetails_QOS_t
{
	void* m_pvData;	// Encoded game details
	int m_numDataBytes; // Length of game details

	int m_nPing;	// Average ping in ms
};

class IMatchNetworkMsgController
{
public:
	// To determine host Quality-of-Service
	virtual MM_QOS_t GetQOS() = 0;

	virtual KeyValues* GetActiveServerGameDetails(KeyValues* pRequest) = 0;

	virtual KeyValues* UnpackGameDetailsFromQOS(MM_GameDetails_QOS_t const* pvQosReply) = 0;
	virtual KeyValues* UnpackGameDetailsFromSteamLobby(uint64_t uiLobbyID) = 0;

	virtual void PackageGameDetailsForQOS(KeyValues* pSettings, void* buf) = 0;

	virtual KeyValues* PackageGameDetailsForReservation(KeyValues* pSettings) = 0;
};


class IMatchVoice
{
public:
	// Whether remote player talking can be visualized / audible
	virtual bool CanPlaybackTalker(XUID xuidTalker) = 0;

	// Whether we are explicitly muting a remote player
	virtual bool IsTalkerMuted(XUID xuidTalker) = 0;

	// Whether we are muting any player on the player's machine
	virtual bool IsMachineMuted(XUID xuidPlayer) = 0;

	// Whether voice recording mode is currently active
	virtual bool IsVoiceRecording() = 0;

	// Enable or disable voice recording
	virtual void SetVoiceRecording(bool bRecordingEnabled) = 0;

	// Enable or disable voice mute for a given talker
	virtual void MuteTalker(XUID xuidTalker, bool bMute) = 0;
};
class IMatchSearchResult;
class ISearchManager;
class IMatchServer
{
public:
	//
	// GetOnlineId
	//	returns server online id to store as reference
	//
	virtual XUID GetOnlineId() = 0;

	//
	// GetGameDetails
	//	returns server game details
	//
	virtual KeyValues* GetGameDetails() = 0;

	//
	// IsJoinable and Join
	//	returns whether server is joinable and initiates join to the server
	//
	virtual bool IsJoinable() = 0;
	virtual void Join() = 0;
};

class IServerManager
{
public:
	//
	// EnableServersUpdate
	//	controls whether server data is being updated in the background
	//
	virtual void EnableServersUpdate(bool bEnable) = 0;

	//
	// GetNumServers
	//	returns number of servers discovered and for which data is available
	//
	virtual int GetNumServers() = 0;

	//
	// GetServerByIndex / GetServerByOnlineId
	//	returns server interface to the given server or NULL if server not found or not available
	//
	virtual IMatchServer* GetServerByIndex(int iServerIdx) = 0;
	virtual IMatchServer* GetServerByOnlineId(XUID xuidServerOnline) = 0;
};


class IMatchSearchResult
{
public:
	//
	// GetOnlineId
	//	returns result online id to store as reference
	//
	virtual XUID GetOnlineId() = 0;

	//
	// GetGameDetails
	//	returns result game details
	//
	virtual KeyValues* GetGameDetails() = 0;

	//
	// IsJoinable and Join
	//	returns whether result is joinable and initiates join to the result
	//
	virtual bool IsJoinable() = 0;
	virtual void Join() = 0;
};

class ISearchManager
{
public:
	//
	// EnableResultsUpdate
	//	controls whether server data is being updated in the background
	//
	virtual void EnableResultsUpdate(bool bEnable, KeyValues * pSearchParams = NULL) = 0;

	//
	// GetNumResults
	//	returns number of results discovered and for which data is available
	//
	virtual int GetNumResults() = 0;

	//
	// GetResultByIndex / GetResultByOnlineId
	//	returns result interface to the given result or NULL if result not found or not available
	//
	virtual IMatchSearchResult* GetResultByIndex(int iResultIdx) = 0;
	virtual IMatchSearchResult* GetResultByOnlineId(XUID xuidResultOnline) = 0;

	//
	// Destroy
	//	destroys the search manager and all its results
	//
	virtual void Destroy() = 0;
};




class IMatchFramework: public IAppSystem{
    public:
    	// Run frame of the matchmaking framework
    	virtual void RunFrame() = 0;
     
     
    	// Get matchmaking extensions
    	virtual IMatchExtensions * GetMatchExtensions() = 0;
     
    	// Get events container
    	virtual IMatchEventsSubscription * GetEventsSubscription() = 0;
     
    	// Get the matchmaking title interface
    	virtual IMatchTitle * GetMatchTitle() = 0;
     
    	// Get the match session interface of the current match framework type
    	virtual IMatchSession * GetMatchSession() = 0;
     
    	// Get the network msg encode/decode factory
    	virtual IMatchNetworkMsgController * GetMatchNetworkMsgController() = 0;
     
    	// Get the match system
    	virtual IMatchSystem * GetMatchSystem() = 0;
     
     
    	// Entry point to create session
    	virtual void CreateSession(KeyValues *pSettings) = 0;
     
    	// Entry point to match into a session
    	virtual void MatchSession(KeyValues *pSettings) = 0;
     
    	// Accept invite
    	virtual void AcceptInvite(int iController) = 0;
     
    	// Close the session
    	virtual void CloseSession() = 0;

		// Checks to see if the current game is being played online ( as opposed to locally against bots )
		virtual bool IsOnlineGame(void) = 0;

		// Called by the client to notify matchmaking that it should update matchmaking properties based
		// on player distribution among the teams.
		virtual void UpdateTeamProperties(KeyValues* pTeamProperties) = 0;
    };


//#include "Tier0/UtlStringMap.h"
class ILocalize;
class INetSupport;
class IEngineVoice;
class IVEngineClient;
class IVEngineServer;
class IServerGameDLL;
class IGameEventManager2;
class IBaseClientDLL;
#if 0
class CMatchExtensions : public IMatchExtensions
{
	// Methods of IMatchExtensions
public:
	// Registers an extension interface
	virtual void RegisterExtensionInterface(char const* szInterfaceString, void* pvInterface);

	// Unregisters an extension interface
	virtual void UnregisterExtensionInterface(char const* szInterfaceString, void* pvInterface);

	// Gets a pointer to a registered extension interface
	virtual void* GetRegisteredExtensionInterface(char const* szInterfaceString);

public:
	CMatchExtensions();
	~CMatchExtensions();

	struct RegisteredInterface_t
	{
		void* m_pvInterface;
		int m_nRefCount;

		RegisteredInterface_t() : m_nRefCount(0), m_pvInterface(0) {}
	};
	typedef CUtlStringMap< RegisteredInterface_t > InterfaceMap_t;
	InterfaceMap_t m_mapRegisteredInterfaces;
protected:
	void OnExtensionInterfaceUpdated(char const* szInterfaceString, void* pvInterface);

public:
	ILocalize* GetILocalize() { return m_exts.m_pILocalize; }
	INetSupport* GetINetSupport() { return m_exts.m_pINetSupport; }
	IEngineVoice* GetIEngineVoice() { return m_exts.m_pIEngineVoice; }
	IVEngineClient* GetIVEngineClient() { return m_exts.m_pIVEngineClient; }
	IVEngineServer* GetIVEngineServer() { return m_exts.m_pIVEngineServer; }
	IServerGameDLL* GetIServerGameDLL() { return m_exts.m_pIServerGameDLL; }
	IGameEventManager2* GetIGameEventManager2() { return m_exts.m_pIGameEventManager2; }
	IBaseClientDLL* GetIBaseClientDLL() { return m_exts.m_pIBaseClientDLL; }
#ifdef _X360
	IXboxSystem* GetIXboxSystem() { return m_exts.m_pIXboxSystem; }
	IXOnline* GetIXOnline() { return m_exts.m_pIXOnline; }
#endif

protected:
	// Known extension interfaces
	struct Exts_t
	{
		inline Exts_t() { memset(this, 0, sizeof(*this)); }

		ILocalize* m_pILocalize;
		INetSupport* m_pINetSupport;
		IEngineVoice* m_pIEngineVoice;
		IVEngineClient* m_pIVEngineClient;
		IVEngineServer* m_pIVEngineServer;
		IServerGameDLL* m_pIServerGameDLL;
		IGameEventManager2* m_pIGameEventManager2;
		IBaseClientDLL* m_pIBaseClientDLL;
#ifdef _X360
		IXboxSystem* m_pIXboxSystem;
		IXOnline* m_pIXOnline;
#endif
	}
	m_exts;
};
#endif
