#pragma once

#include "UtlVector.h"
#include "VirtualMethod.h"

class GameEvent {
public:
#if 0
	virtual ~GameEvent() {};
	virtual const char* GetName() const = 0;	// get event name

	virtual bool  IsReliable() const = 0; // if event handled reliable
	virtual bool  IsLocal() const = 0; // if event is never networked
	virtual bool  IsEmpty(const char* keyName = 0) const = 0; // check if data field exists

	// Data access
	virtual bool  GetBool(const char* keyName = 0, bool defaultValue = false) const = 0;
	virtual int   GetInt(const char* keyName = 0, int defaultValue = 0) const = 0;
	virtual uint64_t GetUint64(const char* keyName = 0, uint64_t defaultValue = 0) const = 0;
	virtual float GetFloat(const char* keyName = 0, float defaultValue = 0.0f) const = 0;
	virtual const char* GetString(const char* keyName = 0, const char* defaultValue = "") const = 0;
	virtual const wchar_t* GetWString(const char* keyName = 0, const wchar_t* defaultValue = L"") const = 0;
	virtual const void* GetPtr(const char* keyName = 0) const = 0;	// LOCAL only

	virtual void SetBool(const char* keyName, bool value) = 0;
	virtual void SetInt(const char* keyName, int value) = 0;
	virtual void SetUint64(const char* keyName, uint64_t value) = 0;
	virtual void SetFloat(const char* keyName, float value) = 0;
	virtual void SetString(const char* keyName, const char* value) = 0;
	virtual void SetWString(const char* keyName, const wchar_t* value) = 0;
	virtual void SetPtr(const char* keyName, const void* value) = 0;	// LOCAL only
	virtual bool ForEventData(void* event) const = 0;
#endif


    VIRTUAL_METHOD(const char*, getName, 1, (), (this))
    VIRTUAL_METHOD(int, getInt, 6, (const char* keyName, int defaultValue = 0), (this, keyName, defaultValue))
    VIRTUAL_METHOD(float, getFloat, 8, (const char* keyName, float defaultValue = 0.0f), (this, keyName, defaultValue))
    VIRTUAL_METHOD(const char*, getString, 9, (const char* keyName, const char* defaultValue = ""), (this, keyName, defaultValue))
    VIRTUAL_METHOD(void, setString, 16, (const char* keyName, const char* value), (this, keyName, value))
};

class GameEventListener {
public:
    virtual ~GameEventListener() {}
    virtual void fireGameEvent(GameEvent* event) = 0;
    virtual int getEventDebugId() { return 42; }
};

class KeyValues;

class GameEventDescriptor {
public:
    int	eventid;
    int elementIndex;
    KeyValues* keys;
    UtlVector<void*> listeners;
};

class GameEventManager {
public:
    VIRTUAL_METHOD(bool, addListener, 3, (GameEventListener* listener, const char* name), (this, listener, name, false))
    VIRTUAL_METHOD(void, removeListener, 5, (GameEventListener* listener), (this, listener))
};
