#pragma once

#include <cstddef>
#include <memory>

class MinHook {
public:
    void init(void* base) noexcept;
    void restore() noexcept {}
    void hookAt(std::size_t index, void* fun) noexcept;
    void hook(void* fun) noexcept;

    template<typename T, std::size_t Idx, typename ...Args>
    constexpr auto getOriginal(Args... args) const noexcept
    {
        return reinterpret_cast<T(__thiscall*)(void*, Args...)>(originals[Idx]);
    }

    auto getOriginalPtr(int idx) noexcept
    {
        return originals[idx];
    }

    template<typename T, std::size_t Idx, typename ...Args>
    constexpr auto callOriginal(Args... args) const noexcept
    {
        return getOriginal<T, Idx>(args...)(base, args...);
    }


    constexpr void* getThis() const noexcept
    {
        return base;
    }


private:
    void* base;
    std::unique_ptr<uintptr_t[]> originals;
};
