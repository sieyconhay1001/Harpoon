#pragma once

#include <type_traits>

#include "UtlVector.h"
#include "VirtualMethod.h"

struct ConVar {
    VIRTUAL_METHOD(float, getFloat, 12, (), (this))
    VIRTUAL_METHOD(int, getInt, 13, (), (this))
    VIRTUAL_METHOD(void, setValue, 14, (const char* value), (this, value))
    VIRTUAL_METHOD(void, setValue, 15, (float value), (this, value))
    VIRTUAL_METHOD(void, setValue, 16, (int value), (this, value))


    std::byte pad[24];
    std::add_pointer_t<void __cdecl()> changeCallback;
    ConVar* parent;
    const char* defaultValue;
    char* string;
    int							m_StringLength;
    // Values
    float						m_fValue;
    int							m_nValue;
    // Min/Max values
    bool						m_bHasMin;
    float						m_fMinVal;
    bool						m_bHasMax;
    float						m_fMaxVal;
    UtlVector<void(__cdecl*)()> onChangeCallbacks;
};
