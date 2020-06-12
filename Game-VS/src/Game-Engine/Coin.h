#pragma once
#include "Animation.h"
#include "GameObject.h"
#include "AABB.h"
#include "GameData.h"
#include "SphereCollider.h"
/**
 * Custom animated and collidable game object for a coin.
 */
class Coin : public GameObject, public Animation, public SphereCollider {

public:
	/**
	 * Constructs the coin with provided file location, translation, scale and rotation values.
	 */
	Coin(const char* objFile, glm::vec3 defTrans, glm::vec3 defScale, glm::vec3 defRot) 
		: GameObject(objFile, defTrans, defScale, defRot), SphereCollider(defTrans, 3.0f) {
	}
		
	/**
	 * Updates the coin's rotation animation around the Y axis based on the provided timestamp.
	 */
	void update(float time) override {
		glm::vec3 newRot = getRotationAngles();
		newRot.y += 1.0f;
		if (newRot.y == 360.f)
			newRot.y = 0.0f;
		setRotation(newRot);
	}
	
};