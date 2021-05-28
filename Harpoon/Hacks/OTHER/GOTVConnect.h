struct ns_address;
#include <cstdint>
namespace GOTVConnect {
	int __fastcall CBaseClientState_SendConnectPacket(void* CBaseClientState, void* edx, const ns_address* netAdrRemote, int challengeNr, int authProtocol, uint64_t unGSSteamID, bool bGSSecure);
	int __fastcall CSteam3Client_OnGameServerChangeRequested(void* CSteam3Client, void* edx, void* GameServerChangeRequested);
	void ConnectToServer(void* matchInfo, void* packet = NULL);
	void __fastcall UnknownFunction(void* ecx, void* edx, void* watchUserInfo);
	void* __fastcall UnknownFunction2(void* ecx, void* edx, void* a2, int a3, int a4, char a5, int a6);
}