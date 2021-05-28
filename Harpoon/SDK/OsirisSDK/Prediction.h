#pragma once

#include "VirtualMethod.h"

class Entity;
class MoveData;
class MoveHelper;
struct UserCmd;

class Prediction {
public:
    VIRTUAL_METHOD(void, Update, 3, (int startframe, bool validframe, int incoming_acknowledged, int outgoing_command), (this, startframe, validframe, incoming_acknowledged, outgoing_command))
        VIRTUAL_METHOD(void, setupMove, 20, (Entity* localPlayer, UserCmd* cmd, MoveHelper* moveHelper, MoveData* moveData), (this, localPlayer, cmd, moveHelper, moveData))
        VIRTUAL_METHOD(void, finishMove, 21, (Entity* localPlayer, UserCmd* cmd, MoveData* moveData), (this, localPlayer, cmd, moveData))
        VIRTUAL_METHOD(void, SetViewOrigin, 9, (Vector& vec), (this, vec))
        VIRTUAL_METHOD(void, SetViewAngles, 11, (Vector& vec), (this, vec))
        VIRTUAL_METHOD(void, RunCommand, 19, (Entity* localPlayer, UserCmd* cmd, MoveData* moveData), (this, localPlayer, cmd, moveData))
        VIRTUAL_METHOD(void, PreEntityPacketRecieved, 4, (int commAck, int currWorldUpdate, int serverTicks), (this, commAck, currWorldUpdate, serverTicks))
        VIRTUAL_METHOD(void, PostEntityPacketRecieved, 5, (void), (this))
        VIRTUAL_METHOD(void, PostNetworkDataReceived, 6, (int commands), (this, commands))
        VIRTUAL_METHOD(void, _Update, 24, (int nSlot, int startframe, bool validframe, int incoming_acknowledged, int outgoing_command), (this, nSlot, startframe, validframe, incoming_acknowledged, outgoing_command))

        VIRTUAL_METHOD(bool, PerformPrediction, 25, (int nSlot, Entity* localPlayer, bool received_new_world_update, int incoming_acknowledged, int outgoing_command), (this, nSlot, localPlayer, received_new_world_update, incoming_acknowledged, outgoing_command))
        VIRTUAL_METHOD(void, RunSimulation, 35, (int current_command, /*float curtime, xmm0*/ UserCmd* cmd, Entity* localPlayer), (this, current_command,  cmd, localPlayer));
        // PerformPrediction(int nSlot, C_BasePlayer* localPlayer, bool received_new_world_update, int incoming_acknowledged, int outgoing_command);

        // void			RunSimulation( int current_command, float curtime, CUserCmd *cmd, C_BasePlayer *localPlayer );
};
enum
{
    PC_NON_NETWORKED_ONLY = 0,
    PC_NETWORKED_ONLY,

    PC_COPYTYPE_COUNT,
    PC_EVERYTHING = PC_COPYTYPE_COUNT,
};

enum
{
    TD_OFFSET_NORMAL = 0,
    TD_OFFSET_PACKED = 1,

    // Must be last
    TD_OFFSET_COUNT,
};
enum
{
    SLOT_ORIGINALDATA = -1,
};

#define MULTIPLAYER_BACKUP 150