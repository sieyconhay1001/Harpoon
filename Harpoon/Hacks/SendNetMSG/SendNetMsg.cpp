#include "../../COMPILERDEFINITIONS.h"
#ifndef _DEBUG
#pragma comment(lib,"C:\\vcpkg\\installed\\x86-windows-static-md\\lib\\libprotobuf.lib")
#endif
#ifdef _DEBUG
#pragma comment(lib,"C:\\vcpkg\\installed\\x86-windows-static-md\\debug\\lib\\libprotobufd.lib")
#endif
//#pragma comment(lib,"../../Resource/Protobuf/libprotobuf.lib")


#include "../../Resource/Protobuf/Headers/cstrike15_gcmessages.pb.h"
#include "../../Resource/Protobuf/Headers/netmessages.pb.h"
#include "../OTHER/Debug.h"
#include <Windows.h>
#include "SendNetMsg.h"
#include "../../SDK/OsirisSDK/NetworkChannel.h"

#include "../../Hooks.h"
bool OurSendNetMsg::OurSendNet(NetworkChannel* netchann, NetworkMessage& msg, bool bForceReliable, bool bVoice) {

	auto original = hooks->networkChannel.getOriginal<bool, NetworkMessage&, bool, bool>(40, msg, bForceReliable, bVoice);

	
	if (msg.getType() == NET_Messages::net_StringCmd) {
		if (config->debug.spectateForce) {
			CNETMsg_StringCmd* stringmsg = (CNETMsg_StringCmd*)&msg;
			if (strstr("joingame", stringmsg->command().c_str())) {
				return true;
			}
		}
	}
	
	
	if (original) {
		return original(netchann, msg, bForceReliable, bVoice);
	}
	else {
		return false;
	}

}


