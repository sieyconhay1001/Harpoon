// dllmain.cpp : Defines the entry point for the DLL application.
//#include "pch.h"
#include "Hooks.h"
#include "Hooks/Antaeus/Hooks/UserMode/MinHook.h"
#include <Windows.h>

extern "C" BOOL WINAPI _CRT_INIT(HMODULE module, DWORD reason, LPVOID reserved);




/*

This honestly was the project that taught me C++. I knew barely anything when I started. I was just a python scripter.

This code is horrendous and honeslty embarrasing now. (Especially the code that's not mine). But this wasn't meant to be
a good cheat. This was meant to be a fun project and a vessel for exploiting server networking code. I did that and now im
bored of it all. 

*/



// 5 - 19 - 2021 IF ANYONE KNOWS HOW TO PROGRAM LET ME KNOW. ID LIKE TO LEARN SOME DAY







BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    //if (!_CRT_INIT(hModule, ul_reason_for_call, lpReserved))
    //    return FALSE;

    if ((ul_reason_for_call == DLL_PROCESS_ATTACH))
        hooks = std::make_unique<Hooks>(hModule);


    return TRUE;
}

