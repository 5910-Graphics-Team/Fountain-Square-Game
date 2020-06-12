#pragma once

/**
 * A collision detection class which can be used alone or implemented by other game object classes.
 * Use method collidesWithSphere() to check if this SphereCollider object collides with another SphereCollider
 * Sphere-To-Sphere collision algorithm source: http://www.miguelcasillas.com/?p=9 
 */
class SphereCollider {
public:
	/**
	 * Constructs a collision detection sphere with provided translation and radius
	 */
	SphereCollider(const glm::vec3 translation, float radius) : collisionSphereCenter(translation), collisionSphereRadius(radius) {}

	/**
	 * Checks if this SphereCollider collides with another.
	 * @param other The SphereCollider to check if this SphereCollider collides with
	 */
	bool collidesWithSphere(const SphereCollider& other) {
		// First, calculate the squared distance between the centers of both spheres
		glm::vec3 vecDist(other.collisionSphereCenter - this->collisionSphereCenter);
		float fDistSq(glm::dot(vecDist, vecDist));

		// Now calculate the squared sum of both radii
		float fRadiiSumSquared(this->collisionSphereRadius + other.collisionSphereRadius);
		fRadiiSumSquared *= fRadiiSumSquared;

		// Check for collision
		// If the distance squared is less than or equal to the square sum
		// of the radii, then we have a collision
		if (fDistSq <= fRadiiSumSquared)
			return true;
		// Otherwise, return false
		return false;
	}
	
protected:
	glm::vec3 collisionSphereCenter;
	float collisionSphereRadius;

	/**
	 * Method that should be called when the posision of the collision sphere must change
	 * @param newPos new translation position of the sphere collider
	 */
	void updateSphereColliderPosition(glm::vec3 newPos) {
		collisionSphereCenter = newPos;
	}
};