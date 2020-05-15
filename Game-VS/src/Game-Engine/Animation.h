#pragma once
#include <glm/glm.hpp>

class Animation {

public:
	virtual glm::vec3 updateLocation(float time) = 0;
};