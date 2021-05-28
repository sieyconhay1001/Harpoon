#include <vector>
#include <string>
#include <deque>
class ChatConsole {

public:
	bool OnNewMessage(const char* pCommand);
private:
	//std::deque<std::string> m_dqMessages;
};


class Console {
public:
	bool OnDispatchUserMessage();
	bool OnSendNetMessage(void* pMessage);
	bool OnClientConsole(const char* pCommand);
private:

};




void __fastcall ClientCmdUnRestricted(void* ecx, void* edx, const char* pCommand, bool);
void __fastcall ClientCmdUnRestricted2(void* ecx, void* edx, const char* pCommand, bool bFromConsoleOrKeybind, int nUserSlot, bool bCheckValidSlot = true);
void __fastcall ClientCmd(void*, void*, const char* szCmdString);
void __fastcall ServerCmd(void*, void*, const char* szCmdString);


void __fastcall ExecuteClientCmd(void*, void*, const char* szCmdString);

void __fastcall ClientModeClientCmd(void* ecx, void* edx, const char* pCommand);

inline ChatConsole* g_pChatConsole;
inline Console* g_pConsole;