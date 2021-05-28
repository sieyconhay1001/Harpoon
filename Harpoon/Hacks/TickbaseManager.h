#include "../SDK/OsirisSDK/UserCmd.h"
#include <memory>

class NetworkChannel;

#pragma once

namespace Tickbase {
	struct TickBaseInfo_s {
		int nMaxUserProcessCmds{ 16 };
		int nTicksAllowedForProcessing{ 16 };
		int nTicksToShift{ 0 };
		int nTicksShifted{ 0 };
		int nTicksPredicted{ 0 };
		int nLastTicksShifted{ 0 };
		int nSavedTickbase{ 0 };
		bool bDidShift{ 0 };
		bool bFinishedPred{ 0 };
		bool bInShift{ 0 };
		bool bShouldShift{ 0 };
		bool bShouldRecharge{ 0 };
		bool bDidRecharge{ false };
	};

	inline TickBaseInfo_s TickBaseInfo;

	void OnCreateMove(UserCmd* pCmd, bool& bSendPacket);
	bool OnCLMove(float, bool);
	bool OnHostRunFrameInput(float, bool);
	int GetMaxProcessCMD();
};