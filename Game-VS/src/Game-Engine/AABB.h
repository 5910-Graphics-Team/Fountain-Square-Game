#pragma once
#include <iostream>

/**
 * A simple AABB (Axis-Aligned Bounding-Box) collision detection class
 * Can be implemented by child class to add AABB collision detection functionality
 * to a game object, or can be used on its own.
 * AABB collision detection algorithm source: http://www.miguelcasillas.com/?p=30
 */
class AABB {

private:
	
	glm::vec3* vecMax, * vecMin;
	float length, width, height;

protected:
	
	/**
	 * Method to create AABB around a translation
	 */
	void generateAABBoxAroundPoint(glm::vec3 trans) {
		vecMax = new glm::vec3(trans.x + this->length / 2.0f, trans.y + this->length / 2.0f, trans.z + this->height / 2.0f);
		vecMin = new glm::vec3(trans.x - this->length / 2.0f, trans.y - this->length / 2.0f, trans.z - this->height / 2.0f);

	}
	/**
	 * Convenience Method that prints the AABB values to the console
	 */
	void printAABB() {
		std::cout << "AABB Box Created - vecMax(" << vecMax->x << ", " << vecMax->y << ", " << vecMax->z << ") VecMin(" << vecMin->x << ", " << vecMin->y << ", " << vecMin->z << ")\n";
	}


public:
	/**
	 * Constructs an AABB around a translation using provided vec3 of length, width and height
	 */
	AABB(const glm::vec3 translation, const glm::vec3 lenWidthHeight) : AABB(translation, lenWidthHeight.x, lenWidthHeight.y, lenWidthHeight.z) {
		
	}
	/**
	 * Constructs an AABB around a translation using provided length, width and height
	 */
	AABB(const glm::vec3 translation, float length, float width, float height) {
		this->length = length;
		this->width = width;
		this->height = height;
		generateAABBoxAroundPoint(translation);
	}

	/**
	 * Checks if this AABB collides with another AABB.
	 */
	bool collides(const AABB other) {
		//Check if Box1's max is greater than Box2's min and Box1's min is less than Box2's max
		return(this->vecMax->x > other.vecMin->x&&
			   this->vecMin->x < other.vecMax->x&&
			   this->vecMax->y > other.vecMin->y&&
			   this->vecMin->y < other.vecMax->y&&
			   this->vecMax->z > other.vecMin->z&&
			   this->vecMin->z < other.vecMax->z);
	}

		
};