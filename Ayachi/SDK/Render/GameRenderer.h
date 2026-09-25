#pragma once
#include "Matrix.h"

class GameRenderer {
private:
	char pad[0x380];
public:
	Matrix lastViewMatrix;
private:
	char pad2[0x40];
public:
	Matrix lastProjectionMatrix;
};
