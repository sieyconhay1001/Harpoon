#include "GlobalVars.h"
#include "UserCmd.h"
#include "../../Interfaces.h"
#include "Engine.h"
#include "Entity.h"
#include "EntityList.h"

float GlobalVars::serverTime(UserCmd* cmd) noexcept
{

    if (HITPAUSE) {
        //return memory->globalVars->tickCount;
    }

    static int tick;
    static UserCmd* lastCmd;

    if (cmd) {
        if (localPlayer && (!lastCmd || lastCmd->hasbeenpredicted))
            tick = localPlayer->tickBase();
        else
            tick++;
        lastCmd = cmd;
    }
    return tick * intervalPerTick;
}

float GlobalVars::GetIPT() noexcept
{
    return intervalPerTick;
}
