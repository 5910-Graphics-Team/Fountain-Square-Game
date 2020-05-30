#pragma once
#include <glm/common.hpp>


class AABB {

private:
	
	glm::vec3* vecMax, * vecMin;
	float length, width, height;


protected:
	
	// convenience method to create AABB around a translation
	void generateAABBoxAroundPoint(glm::vec3 trans) {
		vecMax = new glm::vec3(trans.x + this->length / 2.0f, trans.y + this->length / 2.0f, trans.z + this->height / 2.0f);
		vecMin = new glm::vec3(trans.x - this->length / 2.0f, trans.y - this->length / 2.0f, trans.z - this->height / 2.0f);
	}

	// convenience method to create AABB around a translation, with specified length, width, and height dimensions
	void generateAABBoxAroundPoint(glm::vec3 trans, float length, float width, float height) {
		vecMax = new glm::vec3(trans.x + length / 2.0f, trans.y + length / 2.0f, trans.z + height / 2.0f);
		vecMin = new glm::vec3(trans.x - length / 2.0f, trans.y - length / 2.0f, trans.z - height / 2.0f);
		printf("AABB(%f", vecMax->x, vecMax->y, vecMax->z, vecMin->x, vecMin->y, vecMin->z);
	}

	//void initAABB(glm::vec3 vecMax, glm::vec3 vecMin) {
	//	this->vecMax = new glm::vec3(vecMax);
	//	this->vecMin = new glm::vec3(vecMin);
	//}

public:
	
	AABB(const glm::vec3 translation, float length, float width, float height) {
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