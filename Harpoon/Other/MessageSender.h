#pragma once
#include "../SDK/OsirisSDK/NetworkChannel.h"
#include <mutex>
inline std::mutex NetworkingMutex;
class NetworkChannel;





/*

	Basically a lot of assorted CS:GO Networking Helpers, in no particular organization or order
	3/8/2021 --- Enron


	For Protobufs make sure you use Ver. 2.5.0!

*/

class NetworkHelper {
public:



private:
	void* m_pNetworkChannel;

};

bool SendOurMessage(NetworkChannel* pNetChannel, void* pData, size_t nDataSize, bool bAsFragmentedReliale = false, bool bCompress = false, bool bTcp = false, bool bShouldLock = false, bool bSendAsChoked = false, int nChokedAmount = 1);


//#define ProtoWrite()

namespace MessageSenderHelpers {

	//template <class PB_OBJECT_TYPE>
	//bool WriteProtobufToBuffer(PB_OBJECT_TYPE* Proto, bf_write& buffer, uint32_t MessageType);
	template <class PB_OBJECT_TYPE>
	bool WriteProtobufToBuffer(PB_OBJECT_TYPE* Proto, bf_write* buffer, uint32_t MessageType);

	class ProtoWriter {
	public:
		bool WriteNetTick(void*, bf_write* buffer);
		bool WriteSignonState(void*, bf_write* buffer);
		bool WriteListenEvents(void*, bf_write* buffer);
		bool WriteClientInfo(void*, bf_write* buffer);
		bool WriteLoadingProgress(void*, bf_write* buffer);
		bool WriteHLTVReplay(void*, bf_write* buffer);
		bool WriteSplitScreen(void*, bf_write* buffer);
		bool WriteFile(void*, bf_write* buffer);
		bool WriteStringCmd(void*, bf_write* buffer);
		bool WriteCLCMove(void*, bf_write* buffer);
		bool WriteCLCKeyValues(void*, bf_write* buffer);
	};
	//template <class PB_OBJECT_TYPE>
	//inline WriteProto(PB_OBJECT_TYPE Proto, bf_write& buffer, uint32_t MessageType) {
	//
	//}
	//inline  WriteProtobufToBuffer(PB_OBJECT_TYPE Proto, bf_write& buffer, uint32_t MessageType) {
	//
	//}
};

inline MessageSenderHelpers::ProtoWriter g_ProtoWriter;