#pragma once
#include "Animation.h"
#include "GameObject.h"

class Harp : public GameObject, public Animation {
public:
	Harp(const char* objFile, glm::vec3 defTrans, glm::vec3 defScale, glm::vec3 defRot) : GameObject(objFile, defTrans, defScale, defRot) {

	}

	void update(float time) override {
		glm::vec3 newRot = getRotationAngles();
		newRot.y += 1.0f;
		if (newRot.y == 360.f)
			newRot.y = 0.0f;
		setRotation(newRot);
	}


};