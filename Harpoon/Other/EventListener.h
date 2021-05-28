#pragma once

#include <memory>

#include "../SDK/OsirisSDK/GameEvent.h"

class EventListener : public GameEventListener {
public:
    EventListener() noexcept;
    void remove() noexcept;
    void fireGameEvent(GameEvent* event);
    //bool isFreezeTime() {
    //    return freezeTime;
    //}
private:
    //bool freezeTime = false;
};

inline std::unique_ptr<EventListener> eventListener;
