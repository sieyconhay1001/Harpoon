#pragma once

#include "SDK/OsirisSDK/UserCmd.h"
#include "SDK/OsirisSDK/FrameStage.h"
#include "SDK/SDK/ModelRenderSystem.h"
class CViewSetup;
class NetworkChannel;
class NetworkMessage;
#include <mutex>
namespace ClientModeHooks {

	inline bool newTick = false;
	inline std::mutex newTickMut;

	int __stdcall doPostScreenEffects(int param) noexcept;
	bool __stdcall createMove(float inputSampleTime, UserCmd* cmd) noexcept;
	void __stdcall overrideView(CViewSetup* setup) noexcept;
}

namespace ClientHooks {
	void SendZeroSizedPacket(volatile NetworkChannel* network);
	bool __fastcall SendNetMessage(NetworkChannel* netchann, void* edx, NetworkMessage* msg, bool bForceReliable, bool bVoice); // Technically an engine hook

	void __stdcall frameStageNotify(FrameStage stage) noexcept;
	bool __fastcall WriteUsercmdDeltaToBuffer(void* ecx, void* edx, int slot, void* buffer, int from, int to, bool isnewcommand) noexcept;
	//class CModelRenderSystem;
	void __fastcall RenderModels(void* _this, void* edx, StudioModelArrayInfo2_t* pInfo, int nModelTypeCount, ModelRenderSystem::ModelListByType_t* pModelList, int nTotalModelCount, ModelRenderSystem::ModelRenderMode_t renderMode, int nFlags) noexcept;
	void __fastcall ModelRenderDrawModels(void* _this, void* ecx, ModelRenderSystem::ModelRenderSystemData_t* pEntities, int nCount, ModelRenderSystem::ModelRenderMode_t renderMode, char unknown);
	void __fastcall DrawWorld(void* _this, void* edx, int waterZAdjust, int* pRenderContext) noexcept;
	bool __fastcall DispatchUserMessage(void* _this, void* edx, int nType, unsigned int nPassthroughFlags, unsigned int nSize, const void* pData);
	inline volatile NetworkChannel* g_pNetChannel;
}