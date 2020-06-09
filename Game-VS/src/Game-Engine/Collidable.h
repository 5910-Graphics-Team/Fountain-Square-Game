#pragma once
/**
 * TODO Implement.
 * A shared interface for collision detection variables
 * Currently not used
 */
class Collidable {
	// TODO implement so that in game objects can reset their hitbox
	//virtual void updateCollision() = 0;
public:
	Collidable(glm::vec3 translation)  {
		setCollisionTranslation(translation);
	}

	//template<typename T> bool collides(T& other); //TODO 

	void setCollisionTranslation(glm::vec3 trans) {
		
		//regenerateCollidableVars();
	}

	glm::vec3 getCollidableTranslation() {
		return *trans;
	}

protected:
	// method that can be implemented to change other Collision variables
	// when a call is made to this object's setCollisionTranslation()
	virtual void regenerateCollidableVars() = 0;

	glm::vec3* trans;
private:
	
};