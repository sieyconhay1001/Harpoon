#include "KeyValues.h"
#include "../../Memory.h"

KeyValues* KeyValues::fromString(const char* name, const char* value) noexcept
{
    const auto keyValuesFromString = memory->keyValuesFromString;
    KeyValues* keyValues;
    __asm {
        push 0
        mov edx, value
        mov ecx, name
        call keyValuesFromString
        add esp, 4
        mov keyValues, eax
    }
    return keyValues;
}

KeyValues* KeyValues::findKey(const char* keyName, bool create) noexcept
{
    return memory->keyValuesFindKey(this, keyName, create);
}

void KeyValues::setString(const char* keyName, const char* value) noexcept
{
    if (const auto key = findKey(keyName, true))
        memory->keyValuesSetString(key, value);
}

const char* KeyValues::GetName() noexcept
{
    return ((const char*(__thiscall*)(void*))memory->KeyValues_GetName)(this);
}

int KeyValues::GetInt(const char* keyName , int defaultValue )
{
    return ((int (__thiscall*)(void*, const char*, int))memory->KeyValues_GetInt)(this, keyName, defaultValue);
}

const char* KeyValues::GetString(const char* keyName , const char* defaultValue )
{
    return ((const char*(__thiscall*)(void*, const char*, const char*))memory->KeyValues_GetString)(this, keyName, defaultValue);
}

float KeyValues::GetFloat(const char* keyName, float defaultValue)
{
    return ((float (__thiscall*)(void*, const char*, float))memory->KeyValues_GetFloat)(this, keyName, defaultValue);
}

