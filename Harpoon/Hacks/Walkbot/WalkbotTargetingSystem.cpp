#include "WalkbotTargetingSystem.h"

namespace TargetingSystemTargetHandler {

	bool DefaultReturn(void*) {
		return 0;
	}

	bool VisibleEntity(void* _Essentially_this) {
		TargetingSystemPrivate::VisibleEntity* VisEnt = reinterpret_cast<TargetingSystemPrivate::VisibleEntity*>(_Essentially_this);

		if (!VisEnt->Player || !VisEnt->Player->isAlive() || VisEnt->Player->isDormant())
			return true;

		return false;
	}


}


Walkbot::TargetingSystem::TargetingSystem(Walkbot::WalkbotData* _this) {
	Bot = _this;
}


