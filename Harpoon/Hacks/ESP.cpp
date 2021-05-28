

#include "ESP.h"
#include "../Pendelum/D3D9Render.h"
#include "../SDK/SDKAddition/EntityListCull.hpp"
#include "../SDK/SDKAddition/Utils/ScreenMath.h"
#include "../SDK/OsirisSDK/Entity.h"
#include "../SDK/OsirisSDK/Vector.h"
#include "../SDK/OsirisSDK/matrix3x4.h"
#include "../Hooks.h"
//#include "../SDK/OsirisSDK/"


#include <limits>
#include <tuple>
#include "../Timing.h"



namespace ESPHelpers {


	bool GetBBoxMinMax2D(Vector& mins, Vector& maxs, RendererStructs::coordinate& minO, RendererStructs::coordinate& maxO,  const matrix3x4* matrix = nullptr) {

		std::array<float, 3> scale = { 1.f,1.f,1.f };
		minO.y = minO.x = (std::numeric_limits<float>::max)();
		maxO.y = maxO.x = -((std::numeric_limits<float>::max)());

		//const auto scaledMins = mins + (maxs - mins) * 2 * (0.25f - 1.f);
		//const auto scaledMaxs = maxs - (maxs - mins) * 2 * (0.25f - 1.f);
		const auto scaledMins = mins;
		const auto scaledMaxs = maxs;

		std::array<RendererStructs::coordinate, 8> vertices;

		for (int i = 0; i < 8; ++i) {
			const Vector point{ i & 1 ? scaledMaxs.x : scaledMins.x,
								i & 2 ? scaledMaxs.y : scaledMins.y,
								i & 4 ? scaledMaxs.z : scaledMins.z };

			if (!ScreenMath::worldToScreen(matrix ? point.transform(*matrix) : point , vertices[i].x, vertices[i].y)) {
				return true;
			}

			minO.x = (std::min)(minO.x, vertices[i].x);
			minO.y = (std::min)(minO.y, vertices[i].y);
			maxO.x = (std::max)(maxO.x, vertices[i].x);
			maxO.y = (std::max)(maxO.y, vertices[i].y);
		}


		return false;




	}










}







#include "../GameData.h"
#include "../Interfaces.h"
#include "../SDK/OsirisSDK/Localize.h"



void DrawESPText(RendererStructs::coord& Coords, std::string str, RendererStructs::rCOLOR Color, float Dist) {

	int font = 0;
	if (Dist < 300.f) {
		//renderer->drawShadowedText(Coords, 1, str.c_str(), Color);
		renderer->drawText({ Coords.x + 2, Coords.y + 2 }, 0, str.c_str(), { 0,0,0,Color.a });
		renderer->drawText({ Coords.x + 1, Coords.y + 1 }, 0, str.c_str(), { Color.a / 2, Color.g / 2, Color.b / 2, Color.a});
		renderer->drawText(Coords, 0, str.c_str(), Color);

		font = 0;
	}
	else if (Dist < 800.f) {
		//renderer->drawShadowedText(Coords, 1, str.c_str(), Color);
		renderer->drawText({ Coords.x + 2, Coords.y + 2 }, 1, str.c_str(), { 0,0,0,Color.a });
		renderer->drawText({ Coords.x + 1, Coords.y + 1 }, 1, str.c_str(), { Color.a / 2, Color.g / 2, Color.b / 2, Color.a });
		renderer->drawText(Coords, 1, str.c_str(), Color);

		font = 1;
	}
	else if (Dist < 2000.f) {
		//renderer->drawShadowedText(Coords, 2, str.c_str(), Color);
		renderer->drawText({ Coords.x + 2, Coords.y + 2 }, 1, str.c_str(), { 0,0,0,Color.a });
		renderer->drawText({ Coords.x + 1, Coords.y + 1 }, 1, str.c_str(), { Color.a / 2, Color.g / 2, Color.b / 2, Color.a });
		renderer->drawText(Coords, 1, str.c_str(), Color);
		font = 1;
	}
	else {
		renderer->drawText({ Coords.x + 2, Coords.y + 2 }, 2, str.c_str(), { 0,0,0,Color.a });
		renderer->drawText({ Coords.x + 1, Coords.y + 1 }, 2, str.c_str(), { Color.a / 2, Color.g / 2, Color.b / 2, Color.a });
		renderer->drawText(Coords, 2, str.c_str(), Color);
		font = 2;
	}

	auto [w, h] = renderer->getTextSize(font, str.c_str());	
	Coords.y -= h - 1.f;
}
#include "../SDK/SDK/IVDebugOverlay.h"
void ESPBoxes() {

	for (EntityQuick entq : entitylistculled->getEntities()) {

		if (entq.m_bisLocalPlayer)
			continue;

		if (!entq.m_bisAlive)
			continue;

		if (entq.m_bisDormant) {
			ESP::SharedESPPlayer Shared = ESP::g_arrSharedESP[entq.index];
			if ((Shared.m_flLastUpdateTime + 20.f) < memory->globalVars->realtime)
				continue;
			interfaces->debugOverlay->AddTextOverlay(Shared.m_vOrigin, 4.f, "%s", entq.entity->getPlayerName(false));
			continue;
		}

		Entity* pEnt = interfaces->entityList->getEntity(entq.index);

		if (pEnt)
			entq.entity = pEnt;



		RendererStructs::rCOLOR Color = { config->esp.Box.color[0] * 255, config->esp.Box.color[1] * 255,config->esp.Box.color[2] * 255,config->esp.Box.color[3] * 255 };
		
		std::vector<RendererStructs::coord> Coords;
		Collideable* Collide = entq.entity->getCollideable();
		Vector Min;
		Vector Max;
		Vector Diff;
		Vector Org = entq.entity->getAbsOrigin();
		Min = Collide->obbMins();
		Max = Collide->obbMaxs();

		//Diff.x = Vector{ Max.y, Max.x, Max.z }.distTo(Vector{ Min.y, Max.x, Max.z }) / 2.f;
		//Diff.y = Vector{ Max.y, Max.x, Max.z }.distTo(Vector{ Max.y, Min.x, Max.z }) / 2.f;
		//VCON("Diff : (%f , %f) -- Origin (%f, %f, %f) -- Min (%f, %f, %f)\n", fabs(Diff.x), fabs(Diff.y), Org.x, Org.y, Org.z, Min.x, Min.y, Min.z);


		RendererStructs::coord Min2D, Max2D;
		//matrix3x4 mat[256];
		//pEnt->setupBones(mat, 256, 256, memory->globalVars->currenttime);
		matrix3x4 mat = entq.entity->toWorldTransform();
		float Distance = localPlayer->origin().distTo(entq.entity->getAbsOrigin());
		if (ESPHelpers::GetBBoxMinMax2D(Min, Max, Min2D, Max2D, &mat)) {
			continue;
		}

		if (config->esp.Box.enabled) {
			Min2D.x -= 1;
			Min2D.y -= 1;
			Max2D.x += 1;
			Max2D.y += 1;
			Coords.push_back(Min2D);
			Coords.push_back({ Min2D.x, Max2D.y });
			Coords.push_back(Max2D);
			Coords.push_back({ Max2D.x, Min2D.y });
			Coords.push_back(Min2D);
			renderer->drawMultiLine(Coords, { 0,0,0,255 });
			Coords.clear();


			Min2D.x += 2;
			Min2D.y += 2;
			Max2D.x -= 2;
			Max2D.y -= 2;
			Coords.push_back(Min2D);
			Coords.push_back({ Min2D.x, Max2D.y });
			Coords.push_back(Max2D);
			Coords.push_back({ Max2D.x, Min2D.y });
			Coords.push_back(Min2D);
			renderer->drawMultiLine(Coords, { 0,0,0,255 });
			Coords.clear();

			Min2D.x -= 1;
			Min2D.y -= 1;
			Max2D.x += 1;
			Max2D.y += 1;
			Coords.push_back(Min2D);
			Coords.push_back({ Min2D.x, Max2D.y });
			Coords.push_back(Max2D);
			Coords.push_back({ Max2D.x, Min2D.y });
			Coords.push_back(Min2D);
			renderer->drawMultiLine(Coords, Color);
		}
		if (config->esp.Text.enabled) {
			RendererStructs::rCOLOR Color = { config->esp.Text.color[0] * 255, config->esp.Text.color[1] * 255,config->esp.Text.color[2] * 255,config->esp.Text.color[3] * 255 };
			auto PlayerName = entq.entity->getPlayerName(false);
			Vector Point;
			Point = entq.entity->getAbsOrigin();
			Point.z += entq.entity->getCollideable()->obbMaxs().z;

			RendererStructs::coord Coords = { 0,0 };
			//if (ESPHelpers::GetBBoxMinMax2D(Min, Max, Min2D, Max2D, &mat)) {
			//	continue;
			//}


			Coords = { Max2D.x , Min2D.y };
			Coords.x += 5;
			int font = 0;
			DrawESPText(Coords, PlayerName, Color, Distance);



			/*
			if (const auto weapon = entq.entity->getActiveWeapon()) {
				if (const auto weaponInfo = weapon->getWeaponData()) { // crash
					std::string activeWeapon = interfaces->localize->findAsUTF8(weaponInfo->name);
					DrawESPText(Coords, activeWeapon, Color, Distance);
				}
			}
			*/
			std::string healthStr = std::string{ "H: " + std::to_string(entq.entity->health()) };
			DrawESPText(Coords, healthStr, Color, Distance);
		}
		if (config->esp.HealthBar) {
			RendererStructs::coord BarMin, BarMax;

			int size = 2;
			if (Distance < 200) {
				size = 5;
			}

			BarMax.x = Min2D.x - 6.f;
			BarMax.y = Max2D.y;
			BarMin.x = BarMax.x - size;
			BarMin.y = Min2D.y;

			RendererStructs::coordQuad BarCoords;
			BarCoords.lowerLeft = { BarMin.x, BarMin.y };
			BarCoords.lowerRight = { BarMax.x, BarMin.y };
			BarCoords.upperLeft = { BarMin.x, BarMax.y };
			BarCoords.upperRight = { BarMax.x, BarMax.y };



			renderer->drawFilledRectangle2D(BarCoords, { 230,0,0,255 }, false);

			float y = BarMax.y - ((entq.entity->health() / 100.f) * (BarMax.y - BarMin.y)) ;

			BarCoords.lowerLeft = { BarMin.x, y };
			BarCoords.lowerRight = { BarMax.x, y };

			renderer->drawFilledRectangle2D(BarCoords, { 0,230,0,255 }, false);

			BarMax.x += 1;
			BarMax.y += 1;
			BarMin.x -= 1;
			BarMin.y -= 1;

			Coords.clear();
			Coords.push_back(BarMax);
			Coords.push_back({ BarMax.x, BarMin.y });
			Coords.push_back(BarMin);
			Coords.push_back({ BarMin.x, BarMax.y });
			Coords.push_back(BarMax);
			renderer->drawMultiLine(Coords, { 0,0,0,255 });
		}
	}
}

void ESPText() {

}



void MoltovESP() {

}

void DrawList() {

}
#include "../SDK/OsirisSDK/Beams.h"
#include "../SDK/SDKAddition/EntityListCull.hpp"
#include "CrossCheatTalkMessages.pb.h"
#include "../Other/CrossCheatTalk.h"

void ESP::run() {
	if (!interfaces->engine->isInGame() || !localPlayer || !localPlayer.get() || localPlayer->isDormant()) {
		return;
	}

	ESPBoxes();
	ESPText();


	if (VectorOriginsLock.try_lock()) {
		if (VectorOrigins.size() > 0)
			g_OldVectorOrigins.clear();

		for (Vector position : VectorOrigins) {


			if (!localPlayer)
				return;

			g_OldVectorOrigins.push_back(position);

			BeamInfo_t beam_info;
			beam_info.m_nType = TE_BEAMRINGPOINT;
			beam_info.m_pszModelName = "sprites/purplelaser1.vmt";
			beam_info.m_nModelIndex = interfaces->modelInfo->getModelIndex("sprites/purplelaser1.vmt");
			beam_info.m_pszHaloName = "sprites/purplelaser1.vmt";
			beam_info.m_nHaloIndex = interfaces->modelInfo->getModelIndex("sprites/purplelaser1.vmt");
			beam_info.m_flHaloScale = 5;
			beam_info.m_flLife = 2.f;// (memory->globalVars->intervalPerTick * static_cast<float>(Timing::ExploitTiming.nExtraTicksToSimulate));
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
			beam_info.m_vecCenter = position;
			beam_info.m_flStartRadius = 1;
			beam_info.m_flEndRadius = 100;
			beam_info.m_flRed = 255;
			beam_info.m_flGreen = 255;
			beam_info.m_flBlue = 255;

			beam_info.m_vecStart = position;

			auto beam = memory->renderBeams->CreateBeamRingPoint(beam_info);
			if (beam)
				memory->renderBeams->DrawBeam(beam);
		}
		VectorOrigins.clear();
		VectorOriginsLock.unlock();
	}

#if 0
	for (Vector Pos : g_OldVectorOrigins) {
		Vector Pos2 = Pos;
		Pos2.z += 20;
		float x = 0;
		float y = 0;
		float x2 = 0;
		float y2 = 0;
		if (!ScreenMath::worldToScreen(Pos, x,y)) {
			continue;
		}
		if (!ScreenMath::worldToScreen(Pos2, x2, y2)) {
			continue;
		}
		renderer->drawMultiLine({ {x,y}, {x2, y2} }, { 255,0,0 });
	}

#endif



	//ESPBoxes();
}








void ESP::BuildSharedESPPackets() {

	if (!config->esp.shared_esp.bEnabled)
		return;

	if (!localPlayer || !localPlayer.get() || Timing::ExploitTiming.m_bNetworkedFreeze || Timing::TimingSet_s.m_bInPredictionMode)
		return;

	if (memory->globalVars->tickCount % 10)
		return;



	int nEspPlayer = 0;
	SharedESPUpdate ESPUpdateMsg;
	for (EntityQuick& entq : entitylistculled->getEnemies()) {
		if (entq.m_bisDormant || !entq.m_bisAlive)
			continue;
		EntityPacketMsg* EntityMsg = ESPUpdateMsg.add_entinfo();
		Vector vOrigin = entq.entity->origin();
		EntityMsg->set_x(vOrigin.x);
		EntityMsg->set_y(vOrigin.y);
		EntityMsg->set_z(vOrigin.z);
		EntityMsg->set_playerarmour(entq.entity->armor());
		EntityMsg->set_playerhealth(entq.entity->health());
		EntityMsg->set_serverindex(entq.index);
		PlayerInfo PlayerInf;
		interfaces->engine->getPlayerInfo(entq.index, PlayerInf);
		EntityMsg->set_steamid(PlayerInf.friendsId);
		nEspPlayer++;
	}

	if (nEspPlayer > 0) {
			Debug::QuickPrint("Sending SharedESPUpdate");
			CrossCheatTalk::g_pSteamNetSocketsNetChannel->BroadCastMessage(k_SharedESPUpdate, &ESPUpdateMsg, 58);
			//CrossCheatTalk::g_pSteamNetSocketsNetChannel->BroadCastMessageToConnectedClients(k_SharedESPUpdate, &ESPUpdateMsg, 58);
	}

}

#include "../SDK/OsirisSDK/ModelInfo.h"
#include "../SDK/OsirisSDK/ModelRender.h"

#include "../SDK/SDK/vphysics_interface.h"
#include "../SDK/SDK/IVDebugOverlay.h"

void DrawHitboxes(Entity* pEntity, int r, int g, int b, int a, float duration)
{	
	matrix3x4 pMatrix[256];
	if (!pEntity->setupBones(pMatrix, 256, 256, memory->globalVars->currenttime))
		return;

	StudioHdr* hdr = interfaces->modelInfo->getStudioModel(pEntity->getModel());

	if (!hdr)
		return;

	StudioHitboxSet* set = hdr->getHitboxSet(0);

	if (!set)
		return; 

	for (int i = 0; i < set->numHitboxes; i++)
	{
		StudioBbox* hitbox = set->getHitbox(i);

		if (!hitbox)
			continue;

		Vector vMin = hitbox->bbMin.transform(pMatrix[hitbox->bone]);
		Vector vMax = hitbox->bbMax.transform(pMatrix[hitbox->bone]);
		interfaces->debugOverlay->AddCapsuleOverlay2(vMin, vMax, hitbox->capsuleRadius, r, g, b, a, duration);
	}
}







void __fastcall ESP::BeginFrame(void* ecx, void*) {

#if 0
	if (!interfaces->engine->isInGame() || !interfaces->engine->isConnected() || !localPlayer || !localPlayer.get() || localPlayer->isDormant() || !localPlayer->isAlive())
	{
		hooks->studioRender.callOriginal<void, 8>();
		return;
	}
#endif 

	if (Timing::ExploitTiming.m_bNetworkedFreeze || Timing::TimingSet_s.m_bInPredictionMode || ((Timing::TimingStats.m_fLastNetTickUpdateAlwaysRecord + 3.f) < memory->globalVars->realtime) || ((g_Denzil.nLastMatrixUpdate+5) > memory->globalVars->tickCount)) {
		if ((g_Denzil.nLastMatrixUpdate + 64) > memory->globalVars->tickCount) {
			static int nLastTickDraw{ 0 };
			if ((nLastTickDraw + 2) < memory->globalVars->tickCount) {
				nLastTickDraw = memory->globalVars->tickCount;
				//Debug::QuickWarning("Drawing " + std::to_string(ESP::g_Denzil.nNumberOfHitboxes) + " Boxes!");
				for (int i = 0; i < ESP::g_Denzil.nNumberOfHitboxes; i++) {
					ESP::FreezeTimePlayer::hitbox Box = g_Denzil.Boxes[i];		
					interfaces->debugOverlay->AddCapsuleOverlay2(Box.vMin, Box.vMax, Box.flRadius, 255, 0, 0, 255, (memory->globalVars->intervalPerTick*4.0f));
				}
			}
		}
	}


#if 0
	static int nLastTickDraw{ 0 };
	if ((nLastTickDraw + 2) < memory->globalVars->tickCount) {
		nLastTickDraw = memory->globalVars->tickCount;
		for (EntityQuick entq : entitylistculled->getEnemies()) {
			if (entq.m_bisDormant || !entq.m_bisAlive)
				continue;
			DrawHitboxes(entq.entity, 255, 0, 0, 255, memory->globalVars->intervalPerTick*2);
		}
	}
#endif


	hooks->studioRender.callOriginal<void, 8>();
}

//#include "SteamWorks/SteamWorks.h"
//#include <steam/isteamuser.h>
void ESP::TransmitFreezeTimeData() {
	if (config->debug.bSendMatrix || Timing::ExploitTiming.m_bNetworkedFreeze || Timing::TimingSet_s.m_bInPredictionMode || ((Timing::TimingStats.m_fLastNetTickUpdateAlwaysRecord + 1.5f) < memory->globalVars->realtime)) {
		OriginUpdate Org;
		Org.set_x(localPlayer->origin().x);
		Org.set_y(localPlayer->origin().y);
		Org.set_z(localPlayer->origin().z);

		VectorMsg* EyeAngles = Org.mutable_eyeangles();

		Vector vEyeAngles;
		interfaces->engine->getViewAngles(vEyeAngles);


		EyeAngles->set_x(vEyeAngles.x);
		EyeAngles->set_y(vEyeAngles.y);
		EyeAngles->set_z(vEyeAngles.z);


		VectorMsg* EyePosition = Org.mutable_eyeposition();
		Vector vEyePosition = localPlayer->getEyePosition();
		EyePosition->set_x(vEyePosition.x);
		EyePosition->set_y(vEyePosition.y);
		EyePosition->set_z(vEyePosition.z);


		PlayerInfo LPPlayerInf;
		interfaces->engine->getPlayerInfo(localPlayer->index(), LPPlayerInf);
		Org.set_steamid(LPPlayerInf.friendsId);
		if (memory->globalVars->tickCount % 2) {
			matrix3x4 pMatrix[256];
			if (localPlayer->setupBones(pMatrix, 256, 256, memory->globalVars->currenttime)) {
				//ESP::FreezeTimePlayer::hitbox Boxes[20];

				StudioHdr* hdr = interfaces->modelInfo->getStudioModel(localPlayer->getModel());
				StudioHitboxSet* set = hdr->getHitboxSet(0);
				int nHitboxes = 0;
				for (int i = 0; i < set->numHitboxes; i++)
				{
					StudioBbox* hitbox = set->getHitbox(i);

					if (!hitbox)
						continue;

					nHitboxes++;
					Vector vMin, vMax;
					vMin = hitbox->bbMin.transform(pMatrix[hitbox->bone]);
					vMax = hitbox->bbMax.transform(pMatrix[hitbox->bone]);


					HitBoxMsg* NewHitbox = Org.add_hitboxes();
					VectorMsg* Mins = NewHitbox->mutable_mins();
					Mins->set_x(vMin.x);
					Mins->set_y(vMin.y);
					Mins->set_z(vMin.z);

					VectorMsg* Maxs = NewHitbox->mutable_maxs();
					Maxs->set_x(vMax.x);
					Maxs->set_y(vMax.y);
					Maxs->set_z(vMax.z);
					
					NewHitbox->set_radius(hitbox->capsuleRadius);
				}	

				if(config->debug.TransmitMatrix)
					Org.set_matrix(pMatrix, sizeof(matrix3x4) * 256);



				//if(nHitboxes > 0)
						//Org.set_hitboxes(Boxes, sizeof(ESP::FreezeTimePlayer::hitbox) * 20);
			}
		}
		CrossCheatTalk::g_pSteamNetSocketsNetChannel->BroadCastMessageToConnectedClients(CrossCheatMsgType::k_OriginUpdate, &Org, 58);
	}
}