#pragma once

#include <cstdint>

#include "../../ModuleBase/Module.h"

class TestModule : public Module {
private:
	enum class TestState {
		Waiting,
		Passed,
		Failed
	};

	TestState state = TestState::Waiting;
	uint32_t tickCounter = 0;
	bool completed = false;
	bool emptyReported = false;

	void runTest();
	void fail(const char* reason);

public:
	TestModule();

	std::string getModeText() override;
	void onEnable() override;
	void onTick(Level* level) override;
};
