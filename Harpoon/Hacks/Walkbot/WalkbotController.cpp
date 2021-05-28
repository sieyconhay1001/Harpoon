#include "../../SDK/OsirisSDK/EngineTrace.h"
#include "../../SDK/OsirisSDK/Entity.h"
#include "../../SDK/OsirisSDK/Vector.h"
#include "../../SDK/OsirisSDK/Beams.h"
#include "../../SDK/SDK/IEffects.h"
#include "../../SDK/OsirisSDK/UserCmd.h"
#include "../../Config.h"
#include "../../SDK/OsirisSDK/GlobalVars.h"
#include "../../SDK/SDKAddition/Utils/VectorMath.h"
#include "../../Memory.h"
#include "WalkbotNetworking.h"

#include "WalkbotController.h"

#include "../../COMPILERDEFINITIONS.h"

#ifdef MASTER_CONTROLLER
namespace WalkbotController {


	struct DrawInfo {
		bool shouldDraw = false;
		Vector Pos;
	};



	void DispatchToPoint(Vector Vec) {
		if (!localPlayer)
			return;

		WalkbotNetworking::NetworkContainer.WalkbotNetworkAccess->SendToPosition(Vec);

		if (!interfaces->engine->isInGame() || !interfaces->engine->isConnected() || memory->renderBeams == nullptr)
			return;


		{
			BeamInfo_t beam_info;
			beam_info.m_nType = TE_BEAMRINGPOINT;
			beam_info.m_pszModelName = "sprites/purplelaser1.vmt";
			beam_info.m_nModelIndex = interfaces->modelInfo->getModelIndex("sprites/purplelaser1.vmt");
			beam_info.m_pszHaloName = "sprites/purplelaser1.vmt";
			beam_info.m_nHaloIndex = interfaces->modelInfo->getModelIndex("sprites/purplelaser1.vmt");
			beam_info.m_flHaloScale = 5;
			beam_info.m_flLife = 3.f;
			beam_info.m_flWidth = 15;
			beam_info.m_flFadeLength = 1.0f;
			beam_info.m_flAmplitude = 0.f;
			beam_info.m_flBrightness = 255;
			beam_info.m_flSpeed = 0;
			beam_info.m_nStartFrame = 0;
			beam_info.m_flFrameRate = 1;
			beam_info.m_nSegments = 1;
			beam_info.m_bRenderable = true;
			beam_info.m_nFlags = 0;
			beam_info.m_vecCenter = Vec;
			beam_info.m_flStartRadius = 1;
			beam_info.m_flEndRadius = 300;
			beam_info.m_flRed = 0;
			beam_info.m_flGreen = 255;
			beam_info.m_flBlue = 0;
			beam_info.m_vecStart = Vec;

			auto beam = memory->renderBeams->CreateBeamRingPoint(beam_info);
			if (beam)
				memory->renderBeams->DrawBeam(beam);

		}

		{
			BeamInfo_t beam_info;
			beam_info.m_nType = TE_BEAMPOINTS;
			beam_info.m_pszModelName = "sprites/purplelaser1.vmt";
			beam_info.m_nModelIndex = interfaces->modelInfo->getModelIndex("sprites/purplelaser1.vmt");
			beam_info.m_flHaloScale = 0.f;
			beam_info.m_flLife = 5.f;
			beam_info.m_flWidth = 50.f;
			beam_info.m_flEndWidth = 1.f;
			beam_info.m_flFadeLength = 0.2f;
			beam_info.m_flAmplitude = 2.f;
			beam_info.m_flBrightness = 255.f;
			beam_info.m_flSpeed = 0.1f;
			beam_info.m_nStartFrame = 0;
			beam_info.m_flFrameRate = 0.f;
			beam_info.m_flRed = 0;
			beam_info.m_flGreen = 255;
			beam_info.m_flBlue = 0;
			beam_info.m_nSegments = 2;
			beam_info.m_bRenderable = true;
			beam_info.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;
			beam_info.m_nType = TE_BEAMPOINTS;
			// create beam backwards because it looks nicer.


			beam_info.m_vecStart = Vec;
			beam_info.m_vecEnd = Vector{ Vec.x, Vec.y, Vec.z + 1000 };
			auto beam = memory->renderBeams->CreateBeamPoints(beam_info);


			if (beam)
				memory->renderBeams->DrawBeam(beam);



		}






		dlight_t* dLight = interfaces->iveffects->CL_AllocDlight(999);


		dLight->die = memory->globalVars->currenttime + 150.f;
		dLight->radius = 400;
		if (true) {
				dLight->color.r = (std::byte)0;
				dLight->color.g = (std::byte)255;
				dLight->color.b = (std::byte)0;
		}
		dLight->color.exponent = 5;
		dLight->flags = 0;
		dLight->key = 999;
		dLight->decay = 20.0f;
		dLight->m_Direction = Vec;
		dLight->origin = Vec;
	}


	static bool wasDown{ false };
	static bool wasDown2{ false };
	
	static bool inMode{ false };
	void LookAtPoint(UserCmd* cmd) {
		
		if ((GetAsyncKeyState(config->walkbotcontroller.SetPosMode) & 0x8000) && !wasDown2) {
			inMode = !inMode;
			wasDown2 = true;
		}	
		else if (!(GetAsyncKeyState(config->walkbotcontroller.SetPosMode) & 0x8000)) {
			wasDown2 = false;
		}

		if (!inMode)
			return;

		if ((GetAsyncKeyState(config->walkbotcontroller.SetPos) & 0x8000) && !wasDown) {


			Vector viewAngles = Vector::fromAngle(cmd->viewangles) * 5000.f;
			Vector startPos = localPlayer->getEyePosition();
			Vector endPos = startPos + viewAngles;

			Trace trace;
			interfaces->engineTrace->traceRay({ startPos, endPos }, MASK_ALL, localPlayer.get(), trace);

			if (trace.fraction > .98f) {
				return;
			}

			DispatchToPoint(((Vector::fromAngle(cmd->viewangles) * (5000.f * trace.fraction))) + startPos);
			wasDown = true;
		}
		else if(!(GetAsyncKeyState(config->walkbotcontroller.SetPos) & 0x8000)){
			wasDown = false;
		}
	}







	void Run(UserCmd* cmd) {
		LookAtPoint(cmd);
	}








}
#endif