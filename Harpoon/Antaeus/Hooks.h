#pragma once

#include <cstddef>
#include <memory>
//#include "Hooks/UserMode/MinHook.h"
#include "Hooks/UserMode/VmtHook.h"
#include "Hooks/UserMode/VmtSwap.h"
//#include "Hooks/UserMode/Detour.h"




/* I tried to get this to work with Class Factories, and Varadic Functions, and Templated functions. But 
I seem to have reached the limit of either the C++ standard or myself.*/

class InvalidHookType {
public:
    void init(void* base) noexcept{}
    void restore() noexcept{}

    template<typename T>
    void hookAt(std::size_t index, T fun) const noexcept{}

    template<typename T, std::size_t Idx, typename ...Args>
    constexpr auto getOriginal(Args... args) const noexcept{}

    template<typename T, std::size_t Idx, typename ...Args>
    constexpr auto callOriginal(Args... args) const noexcept {}

    template<typename T, typename ...Args>
    constexpr auto getOriginal(std::size_t index, Args... args) const noexcept{}
};


class hookFactory { /* This really *isn't* a factory anymore */
public:

    enum Hooks {
        _MIN_HOOK,
        _VMT_HOOK,
        _VMT_SWAP,
        _DETOUR_HOOK,
    };

    //auto* UseMinHook() {
    //    return new MinHook;
    //}
    auto* UseVmtHook() {
        return new VmtHook;
    }
    auto* UseVMTSwap() {
        return new VmtSwap;
    }
    //auto* UseDetourHook() {
    //    return new DetourHook;
    //}

    /*
    
    All types must have Init, Retore, HookAt.
    
    */
};



static void TestFunc() {
    hookFactory fact;
    //auto NewHook = fact.UseMinHook();
    //NewHook->init(nullptr);
    //NewHook->bruh();
    
    
}