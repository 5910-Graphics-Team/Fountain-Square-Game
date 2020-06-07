#pragma once
//#include <glm/fwd.hpp>
//#include <glm/detail/func_geometric.inl>

class SphereCollider {
public:
	SphereCollider(const glm::vec3 translation, float radius) : m_vecCenter(translation), m_fRadius(radius) {

	}
	glm::vec3 m_vecCenter;
	float m_fRadius;
	

	bool collidesWithSphere(const SphereCollider& other)
	{

		//Calculate the squared distance between the centers of both spheres
		glm::vec3 vecDist(other.m_vecCenter - this->m_vecCenter);
		float fDistSq(glm::dot(vecDist, vecDist));

		//Calculate the squared sum of both radii
		float fRadiiSumSquared(this->m_fRadius + other.m_fRadius);
		fRadiiSumSquared *= fRadiiSumSquared;

		//Check for collision
		//If the distance squared is less than or equal to the square sum
		//of the radii, then we have a collision
		if (fDistSq <= fRadiiSumSquared)
			return true;

		//If not, then return false
		return false;
	}
	
protected:
	void updateSphereColliderPosition(glm::vec3 newPos) {
		m_vecCenter = newPos;
	}

private:

};