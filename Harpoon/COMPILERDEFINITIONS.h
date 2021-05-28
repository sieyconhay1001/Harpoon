#pragma once



#ifndef COMPILERDEFINITIONS_H
	#define COMPILERDEFINITIONS_H
	//#define _DEBUG
	#define _INTERNAL_COPY


	#ifdef DEBUG
		#ifndef _DEBUG
			#define _DEBUG
		#endif
	#endif

	#ifdef _DEBUG
		#ifndef _INTERNAL_COPY
		#define _INTERNAL_COPY
		#endif
	#endif



	#ifdef _INTERNAL_COPY

		#define _EXTRA_DEBUG

		#ifndef _MENU_DEBUG
			#define _MENU_DEBUG
		#endif

		#define _GRAPHS

	#endif


	#define _ALLOW_ALL_REGIONS

	// SoundPlayer Wont Play
	//#define SILENCE


	// Walkbot Settings


	// Disables Walkbot Network Thread
	#define SAFE_BUILD
	//#define FORCE_RENDER
	//#define WALKBOT_MASTER
	#define MASTER_CONTROLLER
	#define HOST_RUN_FRAME_INPUT_DT

	//#define SLAVE
	#ifdef SLAVE
		#define WALKBOT_BUILD
		#define TROLL_BUILD
		#define NO_MENU
		#undef MASTER_CONTROLLER
	#endif


	//#define NET_SENDTOIMPHOOK
	#define FORCE_NET_SENDLONG_COMPRESS
	
	//#define LIGHT_HOOKS

	#define DEV_SECRET_BUILD
	//#define DYLANS_WORRIED

	#define EXPLOITS
	#ifdef EXPLOITS
		#define LAGGER
        #define DUMBEST_LAGGER
		#define LOCAL_TIMING
		#define CRASHER
		//#define ANTI_SHARKLASER_PROTECTION
		//#define ALLOW_PLAYER_UPDATES
		//#define THREADED_LAGGER
	#endif

	#ifdef STABLE
		#undef ALLOW_PLAYER_UPDATES
		#undef THREADED_LAGGER
		#undef NET_SENDTOIMPHOOK
		#ifndef SAFE_BUILD
			#define SAFE_BUILD
		#endif
	#endif




/* THANKS STACK OVERFLOW!*/
#define CRYPT8(str) { CRYPT8_(str "\0\0\0\0\0\0\0\0") }
#define CRYPT8_(str) (str)[0] + 1, (str)[1] + 2, (str)[2] + 3, (str)[3] + 4, (str)[4] + 5, (str)[5] + 6, (str)[6] + 7, (str)[7] + 8, '\0'


#define DEBUGGER_OUT(VAR) OutPutDebugStringA(VAR);
#define CON_OUT(VAR) Debug::QuickPrint(VAR);
#define CON_OUT_CPPSTR(VAR) Debug::QuickPrint(VAR.c_str());
#endif