#pragma once

//#include "../HookType.h"
class HookType;
#include <Windows.h>
#include <vector>
#include "MSDetour/detours.h"


#pragma comment (lib, "Hooks//Antaeus//Hooks//UserMode//MSDetour//detours.lib")


class DetourHook {

public:
	void init(void* pbase) noexcept {
        base = (uintptr_t)pbase;
	}

	void restoreAt(std::size_t index) {
        for (int i = 0; i < funcs.size(); i++) {
            VtableItem function = funcs.at(i);
            if (index != function.idx)
                continue;
            TransactionBegin();
            PVOID ppPointer = (PVOID)(base + funcs.at(index).idx);
            DetourAttach((PVOID*)&base, funcs.at(index).fun);
            TransactionEnd();
            std::swap(funcs.at(i), funcs.back());
            funcs.pop_back();
        }
	}

    void restore() noexcept{
        for (VtableItem function : funcs) {
            restoreAt(function.idx);
        }
        funcs.clear();
    }

	void hookAt(std::size_t index, void* fun) noexcept { /* For VTabes */
        TransactionBegin();
        PVOID ppPointer = (PVOID)(base + index);
        DetourAttach((PVOID*)&base, fun);
        TransactionEnd();
        VtableItem function;
        function.idx = index;
        function.fun = fun;
        funcs.push_back(function);
	}

	void hook(void* fun) { /* For Specific Functions */
        hookAt(0, fun);
	}


    template<typename T, std::size_t Idx, typename ...Args>
    constexpr auto getOriginal(Args... args) const noexcept
    {
        return reinterpret_cast<T(__thiscall*)(void*, Args...)>(base[Idx]);
    }

    template<typename T, std::size_t Idx, typename ...Args>
    constexpr auto callOriginal(Args... args) const noexcept
    {
        return getOriginal<T, Idx>(args...)(base, args...);
    }

private:
    struct VtableItem {
        std::size_t idx;
        void* fun;
    };
    std::vector<VtableItem> funcs;

    static void TransactionBegin() {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
    }

    static void TransactionEnd() {
        DetourTransactionCommit();
    }

    uintptr_t base;
};