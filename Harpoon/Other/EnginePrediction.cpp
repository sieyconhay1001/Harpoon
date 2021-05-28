#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/OsirisSDK/Engine.h"
#include "../SDK/OsirisSDK/Entity.h"
#include "../SDK/OsirisSDK/EntityList.h"
#include "../SDK/OsirisSDK/GameMovement.h"
#include "../SDK/OsirisSDK/GlobalVars.h"
#include "../SDK/OsirisSDK/MoveHelper.h"
#include "../SDK/OsirisSDK/Prediction.h"
#include "../Config.h"
#include "EnginePrediction.h"
#include "../SDK/OsirisSDK/NetworkChannel.h"
#include "../Hacks/OTHER/Debug.h"
#include "../Hooks.h"
#include "../Timing.h"


static int localPlayerFlags;
void EnginePrediction::run(UserCmd* cmd) noexcept
{
    if (!localPlayer)
        return;
    
    if (!config->debug.engineprediction)
        return;

    localPlayerFlags = localPlayer->flags();

    *memory->predictionRandomSeed = 0;

    const auto oldCurrenttime = memory->globalVars->currenttime;
    const auto oldFrametime = memory->globalVars->frametime;

    memory->globalVars->currenttime = memory->globalVars->serverTime();
    memory->globalVars->frametime = memory->globalVars->intervalPerTick;

    //if (GetAsyncKeyState(config->TestShit.fuckKey9)) {
    //    memory->globalVars->frametime = (1 / 128.f);
    //}


    memory->moveHelper->setHost(localPlayer.get());
    if (true) {
        //for (int i = 0; i < (GetAsyncKeyState(config->TestShit.fuckKey9) ? 2 : 1); i++) {
            
            interfaces->prediction->setupMove(localPlayer.get(), cmd, memory->moveHelper, memory->moveData);
            interfaces->gameMovement->processMovement(localPlayer.get(), memory->moveData);
            interfaces->prediction->finishMove(localPlayer.get(), cmd, memory->moveData);




            



        //}
    }
    /* We Allow Ourselves To Predict Greater than MULTIPLAYER_BACKUP Number of commands. This can cause many prediction errors! However, without this
       sharklaser's lagger would decimate us, and I ain't playin with that. This is basically assurance that we ain't gonna get decked by the freeze */
    if ((config->debug.InfinitePred || Timing::TimingSet_s.m_bInPredictionMode || Timing::ExploitTiming.m_bForceDoubleSpeed) && (memory->clientState->lastCommandAck < (memory->clientState->lastOutgoingCommand - 148)) && !config->lagger.FULLONBOGANFUCKERY && !config->debug.Lagger) {
            int commandsAck = (memory->clientState->lastOutgoingCommand - 1) - memory->clientState->lastCommandAck;
            memory->RestoreData(localPlayer.get(), "PostEntityPacketReceived", SLOT_ORIGINALDATA, PC_NON_NETWORKED_ONLY);
            memory->RestoreData(localPlayer.get(), "PreEntityPacketReceived", commandsAck - 1, PC_NON_NETWORKED_ONLY);
            for (int i = 0; i < MULTIPLAYER_BACKUP; i++)
            {
                memory->SaveData(localPlayer.get(), "InitPredictable", i, PC_EVERYTHING);
            }
            memory->clientState->lastCommandAck = (memory->clientState->lastOutgoingCommand - 1);
            //interfaces->prediction->PostNetworkDataReceived(commandsAck);
            hooks->Predicition.callOriginal<void, 6, int>(commandsAck);
    }
    

    memory->moveHelper->setHost(nullptr);

    *memory->predictionRandomSeed = -1;

    memory->globalVars->currenttime = oldCurrenttime;
    memory->globalVars->frametime = oldFrametime;
}

int EnginePrediction::getFlags() noexcept
{
    return localPlayerFlags;
}

#include "../SDK/OsirisSDK/GlobalVars.h"
typedef void(__thiscall* RunSimOrg)(void*, int, UserCmd*, Entity*);
void __fastcall EnginePrediction::CPred_RunSimulation(void* pThis, void* edx, int current_command, UserCmd* cmd, Entity* pLocalPlayer) {
  
#if 0
    if (config->mmlagger.turnoffDesync)
    {
        static int nHighestPredCommand{ 0 };

        int cmd_index = current_command % config->mmlagger.desyncTimer;
        int server_drop_count = config->mmlagger.desyncTimer - 8;

        //don't predict any commands the server will drop


        if (current_command > nHighestPredCommand) {
            if (cmd_index > server_drop_count)
            {
                for (int i = 0; i < server_drop_count; i++)
                {
                    nHighestPredCommand = current_command + i;
                    float curtime = memory->globalVars->currenttime;
                    __asm movss xmm2, curtime
                    ((RunSimOrg)memory->oCPrediction_RunSimulation)(pThis, nHighestPredCommand, cmd, pLocalPlayer);
                    cmd->hasbeenpredicted = false;
                }
                return;
            }
        }
           
    }
#endif


    if (Tickbase::TickBaseInfo.bShouldRecharge) {
        int nTicksAllowed = Tickbase::TickBaseInfo.nTicksAllowedForProcessing;
        if (nTicksAllowed < Tickbase::TickBaseInfo.nMaxUserProcessCmds) {
            nTicksAllowed++;
            float curtime = memory->globalVars->currenttime;
            __asm movss xmm2, curtime
            ((RunSimOrg)memory->oCPrediction_RunSimulation)(pThis, current_command, cmd, pLocalPlayer);
            localPlayer->tickBase()++;
        }
    }

    ((RunSimOrg)memory->oCPrediction_RunSimulation)(pThis, current_command, cmd, pLocalPlayer);

#if 0
    if (Timing::ExploitTiming.m_bDidBlip)
    {
        for (int i = 0; (i < (config->mmlagger.blippackets - 1)); i++) {
            float curtime = memory->globalVars->currenttime;
            __asm movss xmm2, curtime
            ((RunSimOrg)memory->oCPrediction_RunSimulation)(pThis, current_command, cmd, pLocalPlayer);
        }
        Timing::ExploitTiming.m_bDidBlip = false;
    }
#endif

    if ((config->mmlagger.bNewPrediction) || Timing::ExploitTiming.m_bNetworkedFreeze) {
        for (int i = 0; (i < (Timing::ExploitTiming.nExtraTicksToSimulate - 1)) && Timing::TimingSet_s.m_bInPredictionMode; i++) {
            float curtime = memory->globalVars->currenttime;
            __asm movss xmm2, curtime
            ((RunSimOrg)memory->oCPrediction_RunSimulation)(pThis, current_command, cmd, pLocalPlayer);
        }
    }
}

#include "../Hooks.h"
typedef void(__thiscall* RunCommandFunc)(void* _this, Entity*, UserCmd*, MoveHelper*);
void __fastcall EnginePrediction::CPred_RunCommand(void* pThis, void* edx, Entity* pPlayer, UserCmd* pCmd, MoveHelper* pMoveHelper) {

   //CON("[   Prediction   ] CPrediction::RunCommandCall\n");
    RunCommandFunc pOriginal = reinterpret_cast<RunCommandFunc>(hooks->Predicition.getOriginal<void, 19>());
    if (Tickbase::TickBaseInfo.bInShift || Tickbase::TickBaseInfo.bDidShift || !Tickbase::TickBaseInfo.bFinishedPred) {

        if (Tickbase::TickBaseInfo.nTicksPredicted == -1) {
            Tickbase::TickBaseInfo.bFinishedPred = false;
            Tickbase::TickBaseInfo.bDidShift = false;

            Tickbase::TickBaseInfo.nTicksPredicted = (Tickbase::TickBaseInfo.nLastTicksShifted - 1);
        }
        int nTickbaseSave = localPlayer->tickBase();
        int nTickbase = Tickbase::TickBaseInfo.nSavedTickbase - Tickbase::TickBaseInfo.nTicksPredicted;
        localPlayer->tickBase() = nTickbase;
        VCON(" [   Prediction   ] [ DT ] Setting Tickbase to %d, (%d -> %d). Ticks Predicted %d. Last Ticks Shifted %d\n", nTickbase, nTickbaseSave, localPlayer->tickBase(), Tickbase::TickBaseInfo.nTicksPredicted, Tickbase::TickBaseInfo.nLastTicksShifted);
        Tickbase::TickBaseInfo.nTicksPredicted--;

        if (Tickbase::TickBaseInfo.nTicksPredicted == -1) {
            Tickbase::TickBaseInfo.bFinishedPred = true;
        }
    }
    else if (Tickbase::TickBaseInfo.nTicksPredicted > -1) {
        Tickbase::TickBaseInfo.nTicksPredicted = -1;
    }

    if (Tickbase::TickBaseInfo.bDidRecharge) {
        Tickbase::TickBaseInfo.bDidRecharge = false;
        localPlayer->tickBase() += (Tickbase::TickBaseInfo.nLastTicksShifted - 1);
    }


    pOriginal(pThis, pPlayer, pCmd, pMoveHelper);
}















/*
#include "../SDK/GlobalVars.h"
#include "../SDK/UserCmd.h"
#include "../Hacks/EnginePrediction.h"
#include "../SDK/Prediction.h"
#include "../Interfaces.h"
#include "../SDK/PseudoMd5.h"

static float m_flOldCurtime;
static float m_flOldFrametime;
MoveData m_MoveData;

void PredictionSys::RunEnginePred(UserCmd* cmd) noexcept
{
    const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

    *memory.predictionRandomSeed = MD5_PseudoRandom(cmd->commandNumber) & 0x7FFFFFFF;

    m_flOldCurtime = memory.globalVars->currenttime;
    m_flOldFrametime = memory.globalVars->frametime;

    memory.globalVars->currenttime = memory.globalVars->serverTime(cmd);
    memory.globalVars->frametime = memory.globalVars->intervalPerTick;

    interfaces.gameMovement->StartTrackPredictionErrors(localPlayer);

    memset(&m_MoveData, 0, sizeof(m_MoveData));
    memory.moveHelper->SetHost(localPlayer);
    interfaces.prediction->SetupMove(localPlayer, cmd, memory.moveHelper, &m_MoveData);
    interfaces.gameMovement->ProcessMovement(localPlayer, &m_MoveData);
    interfaces.prediction->FinishMove(localPlayer, cmd, &m_MoveData);
}

void PredictionSys::EndEnginePred() noexcept
{
    auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

    interfaces.gameMovement->FinishTrackPredictionErrors(localPlayer);
    memory.moveHelper->SetHost(nullptr);

    *memory.predictionRandomSeed = -1;

    memory.globalVars->currenttime = m_flOldCurtime;
    memory.globalVars->frametime = m_flOldFrametime;
}


*/