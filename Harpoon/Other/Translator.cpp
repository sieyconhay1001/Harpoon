#include "Translator.h"
#include "cstrike15_usermessages.pb.h"
#include "../SDK/OsirisSDK/Client.h"
#include "../SDK/SDK/ClientModeShared.h"
#include "../Hacks/OTHER/Debug.h"
#include "Memory.h"

#include "../SDK/OsirisSDK/LocalPlayer.h"
#include "../SDK/OsirisSDK/Entity.h"
#include "../SDK/OsirisSDK/EntityList.h"

#include "../nlohmann/json.hpp"


#ifndef _DEBUG
#pragma comment(lib,"C:\\Users\\user\\source\\repos\\Harpoon\\Harpoon\\Resource\\Protobuf\\libprotobuf.lib")
#endif
#ifdef _DEBUG
#pragma comment(lib,"C:\\vcpkg\\installed\\x86-windows-static-md\\debug\\lib\\libprotobufd.lib")
#endif

#include <wininet.h>
#pragma comment(lib, "wininet.lib")

#if 0
/* YANDEX DOESNT SUPPORT FREE API KEYS ANYMORE*/

// Si Senor, it is paste https://pastebin.com/sUcpup5B
std::string Translate(const std::string& text, const std::string& lang){
    // https://github.com/nlohmann/json
    using json = nlohmann::json;
    const std::string url = "";
    const std::string key = "";

    auto _url = url + std::string("?key=") + key;
    _url += std::string("&text=") + text;
    //_url += std::string("&lang=") + lang;

    if (auto connect = InternetOpenA("GoogleChrome", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0); connect) {
        if (auto address = InternetOpenUrlA(connect, _url.c_str(), NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0); address) {
            char data_received[1024];
            DWORD number_of_bytes_read = 0;

            if (InternetReadFile(address, data_received, 1024, &number_of_bytes_read) && number_of_bytes_read) {
                std::string output(data_received, number_of_bytes_read);
                InternetCloseHandle(address);
                InternetCloseHandle(connect);
                auto json = json::parse(output.c_str());
                //std::cout << json.dump() << std::endl;
                auto code = json["code"].get<int>();
                if (code == 200)
                    return json["text"][0].get<std::string>();
            }
        }
        else {
            InternetCloseHandle(connect);
        }
    }
    return std::string();
}
#endif



void Translator::OnDispatchUserEvent(unsigned int nSize, const void* pData) {

	CCSUsrMsg_SayText2 TextMsg;
	TextMsg.ParseFromArray(pData, nSize);

	if (TextMsg.ent_idx() == localPlayer->index())
		return;




















}



