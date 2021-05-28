#pragma once
#include "../SDK/SDK/IMatchFramework.h"
#include "../SDK/OsirisSDK/KeyValues.h"
#include "../Hacks/OTHER/Debug.h"
#include "../Memory.h"
class OurMMEventListener : IMatchEventsSink
{
public:
	void OnEvent(KeyValues* event) {
		//SDKKeyValues* SDKKeyValue = (SDKKeyValues*)event;
	}
};