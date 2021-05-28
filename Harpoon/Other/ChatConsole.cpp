#include "ChatConsole.h"
#ifndef _DEBUG
#pragma comment(lib,"C:\\Users\\user\\source\\repos\\Harpoon\\Harpoon\\Resource\\Protobuf\\libprotobuf.lib")
#endif
#ifdef _DEBUG
#pragma comment(lib,"C:\\vcpkg\\installed\\x86-windows-static-md\\debug\\lib\\libprotobufd.lib")
#endif

#include "../Resource/Protobuf/Headers/cstrike15_gcmessages.pb.h"
#include "../Resource/Protobuf/Headers/netmessages.pb.h"
#include "../Hacks/OTHER/Debug.h"
#include "../Hacks/TickbaseManager.h"
#include "MessageSender.h"
#include "../SDK/OsirisSDK/NetworkChannel.h"
#include "../SDK/OsirisSDK/LocalPlayer.h"
#include "../SDK/OsirisSDK/Entity.h"
#include <CrossCheatTalkMessages.pb.h>
#include <cstrike15_usermessages.pb.h>

#include "../Hooks.h"

#include "CrossCheatTalk.h"
#include "../SDK/OsirisSDK/Client.h"
#include "../SDK/SDK/ClientModeShared.h"
#include "../Timing.h"

/* Why? Because mscv decided it wasnted to take "On" : "Off" and make it essentially mean std::string("On") : std::string("Off"); and idk why!*/
const char* g_On = "On";
const char* g_Off = "Off";

#include "../../Hacks/Exploits/MMEvent.h"

bool ChatConsole::OnNewMessage(const char* pCommand) {

	if (strstr(pCommand, "say")) {
            static bool bSendOutCommands{ false };
            static bool bOnlyHarpoonChat{ false };

            if (strstr(pCommand, ".hstoggle")) {
                bOnlyHarpoonChat = !bOnlyHarpoonChat;
                ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sHarpoonChatToggle%s] %s%s%s", RedChat, WhiteChat,  (bOnlyHarpoonChat ? GreenChat : RedChat), bOnlyHarpoonChat ? g_On : g_Off, WhiteChat);
                return true;
            }



            if (strstr(pCommand, ".event")) {
                MMEventExploit::SendEvent();
                ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sSending Event%s]", RedChat, WhiteChat);
                return true;
            }

            if (strstr(pCommand, ".cooldowncooker")) // dont send harpoon speak
            {
                config->mmcrasher.cooldownMaker = !config->mmcrasher.cooldownMaker;
                ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sCoolDownCooker%s] %s%s%s", RedChat, WhiteChat, config->mmcrasher.cooldownMaker ? GreenChat : RedChat, config->mmcrasher.cooldownMaker ? g_On : g_Off, WhiteChat);
                
                
                //config->mmcrasher.cooldowncookertime
                if (!config->mmcrasher.cooldownMaker)
                    config->mmcrasher.enabled = false;

                std::string message{ "" };
                std::string s = std::string(pCommand);
                std::string delim = "\"";
                if (!bOnlyHarpoonChat) {
                    delim = ".cooldowncooker ";
                }

                message = s.substr(s.find(delim) + delim.length(), s.size());
                message.erase(std::remove(message.begin(), message.end(), '\n'), message.end());
                if (message.size() > 1)
                    message = message.substr(0, message.size() - 1);

                float flTime = -1.0f;
                if (message.size() >= 1) {
                    flTime = atof(message.c_str());
                    if (flTime > 0.05f) {
                        config->mmcrasher.cooldowncookertime = flTime;
                        ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sCoolDownCooker%s] Time %f", RedChat, WhiteChat, flTime);
                    }
                }
                
                if (!bSendOutCommands)
                    return true;
            }


            if (strstr(pCommand, ".cooker")) // Yeah ik ik
            {
                config->mmcrasher.cooldownMaker = !config->mmcrasher.cooldownMaker;
                ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sCoolDownCooker%s] %s%s%s", RedChat, WhiteChat, config->mmcrasher.cooldownMaker ? GreenChat : RedChat, config->mmcrasher.cooldownMaker ? g_On : g_Off, WhiteChat);

                if (!config->mmcrasher.cooldownMaker)
                    config->mmcrasher.enabled = false;

                //config->mmcrasher.cooldowncookertime

                std::string message{ "" };
                std::string s = std::string(pCommand);
                std::string delim = "\"";
                if (!bOnlyHarpoonChat) {
                    delim = ".cooker ";
                }

                message = s.substr(s.find(delim) + delim.length(), s.size());
                message.erase(std::remove(message.begin(), message.end(), '\n'), message.end());
                if (message.size() > 1)
                    message = message.substr(0, message.size() - 1);

                float flTime = -1.0f;
                if (message.size() >= 1) {
                    flTime = atof(message.c_str());
                    if (flTime > 0.05f) {
                        config->mmcrasher.cooldowncookertime = flTime;
                        ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sCoolDownCooker%s] Time %f", RedChat, WhiteChat, flTime);
                    }
                }

                if (!bSendOutCommands)
                    return true;
            }

            if (strstr(pCommand, ".sendchat")) // dont send harpoon speak
            {
                bSendOutCommands = !bSendOutCommands;
                ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sSendOutText%s] %s%s%s", RedChat, WhiteChat, (bSendOutCommands ? GreenChat : RedChat), bSendOutCommands ? g_On : g_Off, WhiteChat);
                return true;
            }

            if (strstr(pCommand, ".predictionmode")) // dont send harpoon speak
            {
                config->debug.InfinitePred = !config->debug.InfinitePred;
                ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sInf-PredictionMode%s] %s%s%s", RedChat, WhiteChat, (config->debug.InfinitePred ? GreenChat : RedChat), config->debug.InfinitePred ? g_On : g_Off, WhiteChat);
                return true;
            }

            if (strstr(pCommand, ".reset")) // dont send harpoon speak
            {
                Timing::Reset();
                ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sFull Reset%s]", RedChat, WhiteChat);
                return true;
            }


            if (strstr(pCommand, ".noclip")) // dont send harpoon speak
            {
                localPlayer->moveType() = MoveType::NOCLIP;
                ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sNoClip Mode%s]", RedChat, WhiteChat);
                return true;
            }

            if (strstr(pCommand, ".antikick")) // dont send harpoon speak
            {
                config->mmcrasher.antiVoteKick = !config->mmcrasher.antiVoteKick;
                ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sAnti-Kick%s] %s%s%s", RedChat, WhiteChat, (config->mmcrasher.antiVoteKick ? GreenChat : RedChat), config->mmcrasher.antiVoteKick ? g_On : g_Off, WhiteChat);
                return true;
            }

            if (strstr(pCommand, ".antisurrender")) // dont send harpoon speak
            {
                config->mmcrasher.antiSurrender = !config->mmcrasher.antiSurrender;
                ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sAnti-Surrender%s] %s%s%s", RedChat, WhiteChat, (config->mmcrasher.antiSurrender ? GreenChat : RedChat), config->mmcrasher.antiSurrender ? g_On : g_Off, WhiteChat);
                return true;
            }


            if (strstr(pCommand, ".enemysend")) // dont send harpoon speak
            {
                config->debug.EnemySend = !config->debug.EnemySend;
                ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sEnemyTransmit%s] %s%s%s", RedChat, WhiteChat, (config->debug.EnemySend ? GreenChat : RedChat), config->debug.EnemySend ? g_On : g_Off, WhiteChat);
                return true;
            }

            if (strstr(pCommand, ".matrixsend")) // dont send harpoon speak
            {
                config->debug.bSendMatrix = !config->debug.bSendMatrix;
                ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sSend Matrix To Clients%s] %s%s%s", RedChat, WhiteChat, (config->debug.bSendMatrix ? GreenChat : RedChat), config->debug.bSendMatrix ? g_On : g_Off, WhiteChat);
                return true;
            }

            if (strstr(pCommand, ".spam")) // dont send harpoon speak
            {
                config->debug.HSpam = !config->debug.HSpam;
                ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sHS Spam%s] %s%s%s", RedChat, WhiteChat, (config->debug.HSpam ? GreenChat : RedChat), config->debug.HSpam ? g_On : g_Off, WhiteChat);
                return true;
            }

            if (strstr(pCommand, ".freeze")) // dont send harpoon speak
            {
                config->mmcrasher.enabled = !config->mmcrasher.enabled;
                ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sFreeze%s] %s%s%s", RedChat, WhiteChat, (config->mmcrasher.enabled ? GreenChat : RedChat), config->mmcrasher.enabled ? g_On : g_Off, WhiteChat);
                return true;
            }

            if (strstr(pCommand, ".hs") || bOnlyHarpoonChat) // dont send harpoon speak
            {
                std::string message{ "" };
                std::string s = std::string(pCommand);
                std::string delim = "\"";
                if (!bOnlyHarpoonChat) {
                    delim = ".hs ";
                }

                message = s.substr(s.find(delim) + delim.length(), s.size());
                message.erase(std::remove(message.begin(), message.end(), '\n'), message.end());
                if (message.size() > 1)
                    message = message.substr(0, message.size() - 1);


                ((ChatPrintFFn)memory->ChatPrintf)(((ClientModeShared*)memory->clientMode)->m_pChatElement, 0, 0, 0, "[%sHS%s] %s%s%s : %s", PurpleChat, WhiteChat, PurpleChat, localPlayer->getPlayerName(false).c_str(), WhiteChat, message.c_str());
                HarpoonChat ChatMsg;
                ChatMsg.set_name(localPlayer->getPlayerName(false));
                ChatMsg.set_text(message);
                CrossCheatTalk::g_pSteamNetSocketsNetChannel->BroadCastMessage(k_HarpoonChat, &ChatMsg, 58);
                return true;
            }



        
	}
	return false;
}










bool Console::OnClientConsole(const char* szStringCmd) {
    if (strstr(szStringCmd, "say")) {
        return g_pChatConsole->OnNewMessage(szStringCmd);
    }

    if (strstr(szStringCmd, "-h ")) {





    }
    return false;
}





void __fastcall ClientCmdUnRestricted(void* ecx, void* edx, const char* pCommand, bool bFromConsoleOrKeybind = false) {

	Debug::QuickPrint(pCommand);

	if (g_pConsole->OnClientConsole(pCommand))
		return;

	hooks->engine.callOriginal<void, 114, const char*, bool>(pCommand, bFromConsoleOrKeybind);
}

void __fastcall ClientCmd2(void* ecx, void* edx, const char* pCommand, bool bFromConsoleOrKeybind, int nUserSlot, bool bCheckValidSlot) {

	Debug::QuickPrint(pCommand);

	if (g_pConsole->OnClientConsole(pCommand))
		return;

	hooks->engine.callOriginal<void, 115, const char*, bool>(pCommand, bFromConsoleOrKeybind, nUserSlot, bCheckValidSlot);
}


void __fastcall ClientModeClientCmd(void* ecx, void* edx, const char* pCommand) {

	Debug::QuickPrint(pCommand);

	if (g_pConsole->OnClientConsole(pCommand))
		return;

	hooks->clientMode.callOriginal<void, 7, const char*>(pCommand);
}



void __fastcall ExecuteClientCmd(void*, void*, const char* szCmdString) {

	Debug::QuickPrint(szCmdString);


	hooks->engine.callOriginal<void, 99, const char*>(szCmdString);
}


void __fastcall ClientCmd(void*, void*, const char* szCmdString) {


	if (g_pConsole->OnClientConsole(szCmdString))
		return;

	hooks->engine.callOriginal<void, 7, const char*>(szCmdString);
}


void __fastcall ServerCmd(void*, void*, const char* szCmdString) {
	Debug::QuickPrint(szCmdString);
	hooks->engine.callOriginal<void, 6, const char*>(szCmdString);
}
