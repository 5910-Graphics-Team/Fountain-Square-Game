#pragma once
#include "Animation.h"
#include "GameObject.h"
#include "AABB.h"
#include "GameData.h"

class Coin : public GameObject, public Animation, public AABB {

public:
	Coin(const char* objFile, glm::vec3 defTrans, glm::vec3 defScale, glm::vec3 defRot) : GameObject(objFile, defTrans, defScale, defRot), AABB(defTrans,AABB_DIMS_COIN) {
	}
		
	
	void update(float time) override {
		glm::vec3 newRot = getRotationAngles();
		newRot.y += 1.0f;
		if (newRot.y == 360.f)
			newRot.y = 0.0f;
		setRotation(newRot);
	}

	
private:

	
};