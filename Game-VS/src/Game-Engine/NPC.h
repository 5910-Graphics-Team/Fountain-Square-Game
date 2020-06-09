#pragma once
#include "GameObject.h"
#include "SphereCollider.h"

class NPC : public GameObject, public SphereCollider {
public:
	NPC(const char* objFile, glm::vec3 defTrans, glm::vec3 defScale, glm::vec3 defRot, float radius) : GameObject(objFile, defTrans, defScale, defRot), SphereCollider(defTrans, radius), saidDialogue(false) {

	}

	bool hasSaidDialogueLine() {
		return saidDialogue;
	}

	void setHasSaidDialogueLine(bool val) {
		saidDialogue = val;
	}

	//bool collidesWithSphere(const SphereCollider& other) {
	//	
	//}
private:
	bool saidDialogue;
};