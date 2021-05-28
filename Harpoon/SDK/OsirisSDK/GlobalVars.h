#pragma once

#include <cstddef>

struct UserCmd;

struct GlobalVars {
    const float realtime;
    int framecount;
    const float absoluteFrameTime;
    const std::byte pad[4];
    float currenttime;
    float frametime;
    const int maxClients;
    int tickCount;
    float intervalPerTick;

    float GetIPT() noexcept;
    float serverTime(UserCmd* = nullptr) noexcept;
};
