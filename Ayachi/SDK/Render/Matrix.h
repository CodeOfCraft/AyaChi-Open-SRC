#pragma once
#include "../../Libs/glm/glm/glm.hpp"

class Matrix {
public:
	glm::mat4x4 _m;
};

static_assert(sizeof(Matrix) == 0x40);
