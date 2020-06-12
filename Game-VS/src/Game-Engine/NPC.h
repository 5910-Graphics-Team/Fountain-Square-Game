#pragma once
#include "GameObject.h"
#include "SphereCollider.h"
/**
 * Custom collidable GameObject which represents the non-player 'Man-In-Armor' character in the game.
 */
class NPC : public GameObject, public SphereCollider {
public:
	NPC(const char* objFile, glm::vec3 defTrans, glm::vec3 defScale, glm::vec3 defRot, float radius) : GameObject(objFile, defTrans, defScale, defRot), SphereCollider(defTrans, radius), saidDialogue(false) {

	}
	/**
	 * Method that checks if the NPC has said his dialogue line.
	 */
	bool hasSaidDialogueLine() {
		return saidDialogue;
	}
	/**
	 * Method to set whether the NPC has said his dialogue line.
	 */
	void setHasSaidDialogueLine(bool val) {
		saidDialogue = val;
	}

private:
	bool saidDialogue;
};