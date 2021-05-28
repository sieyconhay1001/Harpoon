#pragma once
#include <steam\isteamnetworkingsockets.h>
#include "../../Hacks/SteamWorks/SteamWorks.h"
#include "../../Hacks/OTHER/Debug.h"








class IHarpoonNetworkingSockets : ISteamNetworkingSockets
{

	HSteamListenSocket CreateListenSocketIP(const SteamNetworkingIPAddr& localAddress, int nOptions, const SteamNetworkingConfigValue_t* pOptions) {
		CON("IHarpoonNetworkingSockets::CreateListenSocketIP");
		return g_pSteamNetworkingSockets->CreateListenSocketIP(std::ref(localAddress), nOptions, pOptions);
	};

	HSteamNetConnection ConnectByIPAddress(const SteamNetworkingIPAddr& address, int nOptions, const SteamNetworkingConfigValue_t* pOptions) {
		CON("IHarpoonNetworkingSockets::ConnectByIPAddress");
		return g_pSteamNetworkingSockets->ConnectByIPAddress(std::ref(address), nOptions, pOptions);
	}

	HSteamListenSocket CreateListenSocketP2P(int nLocalVirtualPort, int nOptions, const SteamNetworkingConfigValue_t* pOptions) {
		CON("IHarpoonNetworkingSockets::CreateListenSocketP2P");
		return g_pSteamNetworkingSockets->CreateListenSocketP2P(nLocalVirtualPort, nOptions, pOptions);
	}

	HSteamNetConnection ConnectP2P(const SteamNetworkingIdentity& identityRemote, int nRemoteVirtualPort, int nOptions, const SteamNetworkingConfigValue_t* pOptions) {
		CON("IHarpoonNetworkingSockets::CreateListenSocketP2P");
		return g_pSteamNetworkingSockets->ConnectP2P(std::ref(identityRemote), nRemoteVirtualPort, nOptions, pOptions);
	}

	EResult AcceptConnection(HSteamNetConnection hConn) {
		CON("IHarpoonNetworkingSockets::AcceptConnection");
		return g_pSteamNetworkingSockets->AcceptConnection(hConn);
	}

	bool CloseConnection(HSteamNetConnection hPeer, int nReason, const char* pszDebug, bool bEnableLinger) {
		CON("IHarpoonNetworkingSockets::AcceptConnection");
		return g_pSteamNetworkingSockets->CloseConnection(hPeer, nReason, pszDebug, bEnableLinger);
	}

	bool CloseListenSocket(HSteamListenSocket hSocket) {
		CON("IHarpoonNetworkingSockets::CloseListenSocket");
		return g_pSteamNetworkingSockets->CloseListenSocket(hSocket);
	}

	bool SetConnectionUserData(HSteamNetConnection hPeer, int64 nUserData) {
		CON("IHarpoonNetworkingSockets::SetConnectionUserData");
		return g_pSteamNetworkingSockets->SetConnectionUserData(hPeer, nUserData);
	}

	int64 GetConnectionUserData(HSteamNetConnection hPeer) {
		CON("IHarpoonNetworkingSockets::SetConnectionUserData");
		return g_pSteamNetworkingSockets->GetConnectionUserData(hPeer);
	}

	void SetConnectionName(HSteamNetConnection hPeer, const char* pszName) {
		CON("IHarpoonNetworkingSockets::SetConnectionName");
		return g_pSteamNetworkingSockets->SetConnectionName(hPeer, pszName);
	}

	bool GetConnectionName(HSteamNetConnection hPeer, char* pszName, int nMaxLen) {
		CON("IHarpoonNetworkingSockets::GetConnectionName")
		return g_pSteamNetworkingSockets->GetConnectionName(hPeer, pszName, nMaxLen);
	}

	EResult SendMessageToConnection(HSteamNetConnection hConn, const void* pData, uint32 cbData, int nSendFlags, int64* pOutMessageNumber)
	{
		CON("IHarpoonNetworkingSockets::SendMessageToConnection");
		return g_pSteamNetworkingSockets->SendMessageToConnection(hConn, pData, cbData, nSendFlags, pOutMessageNumber);
	}

	void SendMessages(int nMessages, SteamNetworkingMessage_t* const* pMessages, int64* pOutMessageNumberOrResult) {
		CON("IHarpoonNetworkingSockets::SendMessages");
		return g_pSteamNetworkingSockets->SendMessages(nMessages, pMessages, pOutMessageNumberOrResult);
	}

	EResult FlushMessagesOnConnection(HSteamNetConnection hConn)
	{
		CON("IHarpoonNetworkingSockets::FlushMessagesOnConnection");
		return g_pSteamNetworkingSockets->FlushMessagesOnConnection(hConn);
	}

	int ReceiveMessagesOnConnection(HSteamNetConnection hConn, SteamNetworkingMessage_t** ppOutMessages, int nMaxMessages) {
		CON("IHarpoonNetworkingSockets::ReceiveMessagesOnConnection");
		return g_pSteamNetworkingSockets->ReceiveMessagesOnConnection(hConn, ppOutMessages, nMaxMessages);
	}

	bool GetConnectionInfo(HSteamNetConnection hConn, SteamNetConnectionInfo_t* pInfo)
	{
		CON("IHarpoonNetworkingSockets::GetConnectionInfo");
		return g_pSteamNetworkingSockets->GetConnectionInfo(hConn, pInfo);
	}

	bool GetQuickConnectionStatus(HSteamNetConnection hConn, SteamNetworkingQuickConnectionStatus* pStats) {
		CON("IHarpoonNetworkingSockets::GetQuickConnectionStatus");
		return g_pSteamNetworkingSockets->GetQuickConnectionStatus(hConn, pStats);
	}

	int GetDetailedConnectionStatus(HSteamNetConnection hConn, char* pszBuf, int cbBuf)
	{
		CON("IHarpoonNetworkingSockets::GetDetailedConnectionStatus");
		return g_pSteamNetworkingSockets->GetDetailedConnectionStatus(hConn, pszBuf, cbBuf);
	}

	bool GetListenSocketAddress(HSteamListenSocket hSocket, SteamNetworkingIPAddr* address) {
		CON("IHarpoonNetworkingSockets::GetListenSocketAddress");
		return g_pSteamNetworkingSockets->GetListenSocketAddress(hSocket, address);
	}

	bool CreateSocketPair(HSteamNetConnection* pOutConnection1, HSteamNetConnection* pOutConnection2, bool bUseNetworkLoopback, const SteamNetworkingIdentity* pIdentity1, const SteamNetworkingIdentity* pIdentity2)
	{
		CON("IHarpoonNetworkingSockets::CreateSocketPair");
		return g_pSteamNetworkingSockets->CreateSocketPair(pOutConnection1, pOutConnection2, bUseNetworkLoopback, pIdentity1, pIdentity2);
	}

	bool GetIdentity(SteamNetworkingIdentity* pIdentity) {
		CON("IHarpoonNetworkingSockets::GetIdentity");
		return g_pSteamNetworkingSockets->GetIdentity(pIdentity);
	}

	ESteamNetworkingAvailability InitAuthentication() {
		CON("IHarpoonNetworkingSockets::InitAuthentication");
		return g_pSteamNetworkingSockets->InitAuthentication();
	}

	ESteamNetworkingAvailability GetAuthenticationStatus(SteamNetAuthenticationStatus_t* pDetails) {
		CON("IHarpoonNetworkingSockets::GetAuthenticationStatus");
		return g_pSteamNetworkingSockets->GetAuthenticationStatus(pDetails);
	}

	HSteamNetPollGroup CreatePollGroup() {
		CON("IHarpoonNetworkingSockets::CreatePollGroup");
		return g_pSteamNetworkingSockets->CreatePollGroup();
	}

	bool DestroyPollGroup(HSteamNetPollGroup hPollGroup)
	{
		CON("IHarpoonNetworkingSockets::DestroyPollGroup");
		return g_pSteamNetworkingSockets->DestroyPollGroup(hPollGroup);
	}

	bool SetConnectionPollGroup(HSteamNetConnection hConn, HSteamNetPollGroup hPollGroup)
	{
		CON("IHarpoonNetworkingSockets::SetConnectionPollGroup");
		return g_pSteamNetworkingSockets->SetConnectionPollGroup(hConn, hPollGroup); 
	}

	int ReceiveMessagesOnPollGroup(HSteamNetPollGroup hPollGroup, SteamNetworkingMessage_t** ppOutMessages, int nMaxMessages) {
		CON("IHarpoonNetworkingSockets::ReceiveMessagesOnPollGroup");
		return g_pSteamNetworkingSockets->ReceiveMessagesOnPollGroup(hPollGroup, ppOutMessages, nMaxMessages);
	}

	bool ReceivedRelayAuthTicket(const void* pvTicket, int cbTicket, SteamDatagramRelayAuthTicket* pOutParsedTicket) {
		CON("IHarpoonNetworkingSockets::ReceivedRelayAuthTicket");
		return g_pSteamNetworkingSockets->ReceivedRelayAuthTicket(pvTicket, cbTicket, pOutParsedTicket);
	}

	int FindRelayAuthTicketForServer(const SteamNetworkingIdentity& identityGameServer, int nRemoteVirtualPort, SteamDatagramRelayAuthTicket* pOutParsedTicket) {
		CON("IHarpoonNetworkingSockets::FindRelayAuthTicketForServer");
		return g_pSteamNetworkingSockets->FindRelayAuthTicketForServer(identityGameServer, nRemoteVirtualPort, pOutParsedTicket);
	}

	HSteamNetConnection ConnectToHostedDedicatedServer(const SteamNetworkingIdentity& identityTarget, int nRemoteVirtualPort, int nOptions, const SteamNetworkingConfigValue_t* pOptions) {
		CON("IHarpoonNetworkingSockets::ConnectToHostedDedicatedServer");
		return g_pSteamNetworkingSockets->ConnectToHostedDedicatedServer(identityTarget, nRemoteVirtualPort, nOptions, pOptions);
	}

	uint16 GetHostedDedicatedServerPort() {
		CON("IHarpoonNetworkingSockets::GetHostedDedicatedServerPort");
		return g_pSteamNetworkingSockets->GetHostedDedicatedServerPort();
	}

	SteamNetworkingPOPID GetHostedDedicatedServerPOPID() {
		CON("IHarpoonNetworkingSockets::GetHostedDedicatedServerPOPID");
		return g_pSteamNetworkingSockets->GetHostedDedicatedServerPOPID();
	}


	virtual EResult GetHostedDedicatedServerAddress(SteamDatagramHostedAddress* pRouting) {
		CON("IHarpoonNetworkingSockets::GetHostedDedicatedServerAddress");
		return g_pSteamNetworkingSockets->GetHostedDedicatedServerAddress(pRouting);
	}

	HSteamListenSocket CreateHostedDedicatedServerListenSocket(int nLocalVirtualPort, int nOptions, const SteamNetworkingConfigValue_t* pOptions) {
		CON("IHarpoonNetworkingSockets::CreateHostedDedicatedServerListenSocket");
		return g_pSteamNetworkingSockets->CreateHostedDedicatedServerListenSocket(nLocalVirtualPort, nOptions, pOptions);
	}

	virtual EResult GetGameCoordinatorServerLogin(SteamDatagramGameCoordinatorServerLogin* pLoginInfo, int* pcbSignedBlob, void* pBlob) {
		CON("IHarpoonNetworkingSockets::GetGameCoordinatorServerLogin");
		return g_pSteamNetworkingSockets->GetGameCoordinatorServerLogin(pLoginInfo, pcbSignedBlob, pBlob);
	}

	virtual HSteamNetConnection ConnectP2PCustomSignaling(ISteamNetworkingConnectionSignaling* pSignaling, const SteamNetworkingIdentity* pPeerIdentity, int nRemoteVirtualPort, int nOptions, const SteamNetworkingConfigValue_t* pOptions) {
		CON("IHarpoonNetworkingSockets::ConnectP2PCustomSignaling");
		return g_pSteamNetworkingSockets->ConnectP2PCustomSignaling(pSignaling, pPeerIdentity, nRemoteVirtualPort, nOptions, pOptions);
	}

	bool ReceivedP2PCustomSignal(const void* pMsg, int cbMsg, ISteamNetworkingSignalingRecvContext* pContext) {
		CON("IHarpoonNetworkingSockets::ReceivedP2PCustomSignal");
		return g_pSteamNetworkingSockets->ReceivedP2PCustomSignal(pMsg, cbMsg, pContext);
	}

	bool GetCertificateRequest(int* pcbBlob, void* pBlob, SteamNetworkingErrMsg& errMsg) {
		CON("IHarpoonNetworkingSockets::GetCertificateRequest");
		return g_pSteamNetworkingSockets->GetCertificateRequest(pcbBlob, pBlob, std::ref(errMsg));
	}

	bool SetCertificate(const void* pCertificate, int cbCertificate, SteamNetworkingErrMsg& errMsg) {
		CON("IHarpoonNetworkingSockets::SetCertificate");
		return g_pSteamNetworkingSockets->SetCertificate(pCertificate, cbCertificate, std::ref(errMsg));
	}

#if 0 /* Function Not Supported On Steam!!! */
	void ResetIdentity(const SteamNetworkingIdentity* pIdentity) {
		CON("IHarpoonNetworkingSockets::ResetIdentity");
		return g_pSteamNetworkingSockets->ResetIdentity(pIdentity);
	}
#endif

	virtual void RunCallbacks() {
		CON("IHarpoonNetworkingSockets::ResetIdentity");
		return g_pSteamNetworkingSockets->RunCallbacks();
	}

	~IHarpoonNetworkingSockets(){}


};

inline IHarpoonNetworkingSockets* g_pHarpoonNetworkingSockets;