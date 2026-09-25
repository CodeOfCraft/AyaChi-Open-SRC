#pragma once
#include <cstdint>

class FuncHook {
public:
	const char* name;
	uintptr_t address;
	void* OriginFunc;
	void* func;
	bool enable = false;

	virtual bool enableHook();
	virtual void onHookRequest();
};
