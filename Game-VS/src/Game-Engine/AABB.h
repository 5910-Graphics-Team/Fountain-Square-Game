#pragma once
#include <iostream>

/**
 * A simple AABB (Axis-Aligned Bounding-Box) collision detection class
 * Can be implemented by child class to add AABB collision detection functionality
 * to a game object, or can be used on its own.
 */
class AABB {

private:
	
	glm::vec3* vecMax, * vecMin;
	float length, width, height;

protected:
	
	// convenience method to create AABB around a translation
	void generateAABBoxAroundPoint(glm::vec3 trans) {
		vecMax = new glm::vec3(trans.x + this->length / 2.0f, trans.y + this->length / 2.0f, trans.z + this->height / 2.0f);
		vecMin = new glm::vec3(trans.x - this->length / 2.0f, trans.y - this->length / 2.0f, trans.z - this->height / 2.0f);
		//printAABB();
	}

	void printAABB() {
		std::cout << "AABB Box Created - vecMax(" << vecMax->x << ", " << vecMax->y << ", " << vecMax->z << ") VecMin(" << vecMin->x << ", " << vecMin->y << ", " << vecMin->z << ")\n";
	}


public:

	AABB(const glm::vec3 translation, const glm::vec3 lenWidthHeight) : AABB(translation, lenWidthHeight.x, lenWidthHeight.y, lenWidthHeight.z) {
		
	}

	AABB(const glm::vec3 translation, float length, float width, float height) {
		//std::cout << "In AABB constructor\n";
		this->length = length;
		this->width = width;
		this->height = height;
		generateAABBoxAroundPoint(translation);
	}

	// collision algorithm source: http://www.miguelcasillas.com/?p=
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