#pragma once
#include <vector>
#include "../../Interfaces.h"
#include "../Backtrack.h"
#include "../../SDK/OsirisSDK/NetworkChannel.h"
#include "../../SDK/OsirisSDK/Engine.h"
#include <deque>
#include "../../SDK/OsirisSDK/Surface.h"
enum class FrameStage;
class GameEvent;
struct UserCmd;

#define CONSOLE_OUT_COLOR(OUT_MSG, R,G,B,A) memory->conColorMsg({R,G,B,A}, OUT_MSG);
#define CON_OUT(OUT_MSG) CONSOLE_OUT_COLOR(OUT_MSG,176,117,255,255);
#define CON(OUT_MSG) Debug::ConsolePrint(OUT_MSG);
#define VCON(OUT_MSG, ...) Debug::ConsolePrintV(OUT_MSG, __VA_ARGS__);
#define COLORCON(OUT_MSG,R,G,B,A) Debug::ConsolePrint(OUT_MSG, R,G,B,A);

#define DEBUGSTR(STR) XorStr(STR)
#define DEBUG_OUT_STR(VAR) OutputDebugStringA(VAR);
#define X_DEBUG_OUT_STR(VAR) OutputDebugStringA(XorStr(VAR));

#define DEBUG_OUT_STDSTR(VAR) OutputDebugStringA(VAR.c_str());
#define DEBUG_OUT_STR_VAR(VAR, ...) Debug::Varidic_Debug_Out(XorStr(VAR), __VA_ARGS__);
#define DEBUG_OUT_EXIT_ERROR(VAR) DEBUG_OUT_STR(VAR); return false;
#define FUNCTION_DEBUG(FUNCTION_NAME, FORMATTER, ...) Debug::Function_Out(FUNCTION_NAME, FORMATTER, __VA_ARGS__);  


namespace Debug{





	struct LogItem {
		//int id = 0;
		std::vector<std::wstring> text = {};
		float time_of_creation = 0.0f;
		bool PrintToScreen = true;
		bool PrintToConsole = true;
		std::array<int, 3> color = { 255,255,255 };
	};


	extern std::deque<LogItem> LOG_OUT;
	extern float DamageQuick;




	struct screen {
		int Width = 0;
		int Height = 0;
		int CurrPosW = 0;
		int CurrPosH = 5;
		int CurrColumnMax = 0;

	};

	extern screen Screen;
	
	struct coords {
		int x;
		int y;
	};

	struct fl_coords {
		float x, y;
	};

	struct ColorInfo {
		bool enabled;
		float r;
		float g;
		float b;
		float a;
	};
	extern Vector TargetVec;
	extern Vector ViewAngles;
	// Output
	void QuickPrint(const char*, bool Harp = true);
	void QuickPrint(const std::array<uint8_t, 4>& color, const char* t);


	void QuickWarning(const char* t);
	void QuickWarning(std::string);





	bool Function_Out(const char* szFunctionName, const char* szFormat, ...);

	void Varidic_Debug_Out(const char* szFormatter, ...);


	void ConsolePrint(const char* szIn, uint8_t r = 176, uint8_t g = 117, uint8_t b = 255, uint8_t a = 255);
	//void ConsolePrint(char* szIn, uint8_t r = 176, uint8_t g = 117, uint8_t b = 255, uint8_t a = 255);
	void ConsolePrintV(const char* szIn, ...);



	void PrintLog();
	void DrawSafePoints();
	void DrawHitBoxInterSection();
	void DrawBox(coords start, coords end);
	void CustomHUD();
	void NetworkChannelDebug(NetworkChannel* netchannel = interfaces->engine->getNetworkChannel());
	void ClientstateDebug();
	void GlobalVarDebug();
	void DrawGraphBox(coords start, coords end, float min_val, float max_val, float val, float ratio, std::wstring name);
	bool SetupTextPos(std::vector <std::wstring>& Text, int Font = Surface::font);
	void Draw_Text(std::vector <std::wstring> &Text, int Font = Surface::font);
	void AnimStateMonitor() noexcept;
	std::vector<std::wstring> formatRecord(Backtrack::Record record, Entity* entity, int index);

	void BacktrackMonitor() noexcept;
	void DrawDesyncInfo();
	void run();

	//input 

	void Logger(GameEvent *event);

	void DrawMultiPoints();

	void AnimStateModifier() noexcept;

}
