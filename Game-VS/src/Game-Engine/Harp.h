#pragma once
#include "Animation.h"
#include "GameObject.h"
/**
 * Custom animated game object that displays a rotating harp.
 */
class Harp : public GameObject, public Animation {
public:
	Harp(const char* objFile, glm::vec3 defTrans, glm::vec3 defScale, glm::vec3 defRot) : GameObject(objFile, defTrans, defScale, defRot) {
	}
	/**
	 * Updates the harp's rotation around the Y axis based on the frame timestamp.
	 */
	void update(float time) override {
		glm::vec3 newRot = getRotationAngles();
		newRot.y += 1.0f;
		if (newRot.y == 360.f)
			newRot.y = 0.0f;
		setRotation(newRot);
	}


};