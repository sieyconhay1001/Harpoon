
#pragma once


#include "../SDK/OsirisSDK/Vector.h"
#include "../SDK/OsirisSDK/matrix3x4.h"
#include <vector>
#include <mutex>
namespace ESP {
	void run();
	inline std::vector<Vector> VectorOrigins;
	inline std::vector<Vector> g_OldVectorOrigins;
	inline std::mutex VectorOriginsLock;


	void __fastcall BeginFrame(void*, void*);


	struct SharedESPPlayer {
		bool m_nLastESPTick{ 0 };
		bool m_flLastUpdateTime{ 0 };
		Vector m_vOrigin;
		int m_nHealth{ 0 };
		int m_nArmour{ 0 };
		std::string m_strName;
	};

	inline SharedESPPlayer g_arrSharedESP[65];


	struct FreezeTimePlayer {
		int nLastMatrixUpdate{ 0 };
		int nLastOriginUpdate{ 0 };
		int nNumberOfHitboxes{ 0 };
		bool bHasDrawn{ 0 };
		struct hitbox {
			Vector vMin;
			Vector vMax;
			float flRadius;
			int nBone;
		};
		matrix3x4 pMatrix[256];
		hitbox Boxes[20];
		Vector m_vOrigin;
		Vector m_vEyePosition;
		Vector m_vEyeAngles;
		uint32_t m_nSteamID;
	};

	inline FreezeTimePlayer g_Denzil; // TODO: Make this support multi users


	void BuildSharedESPPackets();
	void TransmitFreezeTimeData();
}