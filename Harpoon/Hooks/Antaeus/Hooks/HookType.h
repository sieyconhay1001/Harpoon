
#include <cstddef>




#ifndef HOOKTYPE_DEF
#define HOOKTYPE_DEF


class HookType {

public:
	virtual void init(void* base) noexcept = 0;
	virtual void restore() noexcept = 0;
	virtual void hookAt(std::size_t index, void* fun) noexcept = 0;
};
#endif