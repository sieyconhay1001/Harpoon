#pragma once

struct UserCmd;
class Entity;
namespace EnginePrediction
{
    void run(UserCmd* cmd) noexcept;
    int getFlags() noexcept;

	void __fastcall CPred_RunSimulation(void*,void*, int current_command, UserCmd* cmd, Entity* localPlayer);


	void __fastcall CPred_RunCommand(void* pThis, void* edx, Entity* pPlayer, UserCmd* pCmd, MoveHelper* pMoveHelper);

}


/*
namespace PredictionSys {
	void RunEnginePred(UserCmd*) noexcept;
	void EndEnginePred() noexcept;
}
*/
