#pragma once
#include "InstancedObject.h"
#include <tgmath.h>
/**
 * Custom instanced object displaying an asteroid belt
 * TODO Origin is set inside the game file, could move to constructor
 */
class AsteroidRing : public InstancedObject, public Animation {
public:
	//const int NUM_ASTEROIDS = 500;

	AsteroidRing(const char* filepath, Shader* shader) : InstancedObject(filepath, shader, 1000), Animation(), displacements(), originAngles(), origin(0.4f, 50.0f, 0.8f) {
		initModelTransformations();
		configureInstancedArray();
		rotSpeed = 4.0;
	}


	// asteroid data
	glm::vec3 origin;
	float radius = 50.0f;
	float offset = 2.5f;

	std::vector<float> originAngles;

	// Array of x,y,z displacements of each instance. size = numInstances
	//glm::vec3** displacements;
	std::vector<glm::vec3> displacements;

	// animation data
	float rotSpeed; // = 1.0f;
	float lastFrame = 0.0f;

	
	// TODO implement rotation around origin
	void update(float time) override {
		// TODO update the orbit of each instance
		//for (unsigned int i = 0; i < numInstances; i++) {
		//	float timeElapsed = time - lastFrame;
		//	glm::mat4 modelMat = modelMatrices[numInstances];
		//	
		//	// Translation: displace along circle with 'radius' in range [-offset, offset]
		//	glm::vec3 displacement = displacements[i];
		//	
		//	float angle = originAngles[i] + timeElapsed * rotSpeed;
		//	if (angle >= 360.0f) angle = fmod(angle, 360.0f);

		//	float x = sin(angle) * radius + displacement.x;
		//	float y = displacement.y * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		//	float z = cos(angle) * radius + displacement.z;

		//	modelMat = glm::translate(modelMat, glm::vec3(x, y, z));
		//	std::cout << "ModelMat[" << i << "] x = " << x << " y = " << y << " z = " << z << "\n";
		//	// scale between 0.05 and 0.25f
		//	float scale = (rand() % 20) / 100.0f + 0.05;
		//	modelMat = glm::scale(modelMat, glm::vec3(scale));

		//	modelMatrices[i] = glm::rotate(modelMat, rotAngs[i], origin);
		//	//std::cout << "Updating Asteroid Ring\n";
		//}
		//configureInstancedArray();
		lastFrame = time;
	}
	
	//void calculateRotation()

	void initModelTransformations() override {
		// generating list of semi-random model transformation matrices
		//glm::mat4* modelMatrices;
		
		srand(glfwGetTime()); // initialize random seed	

		for (unsigned int i = 0; i < numInstances; i++) {
			glm::mat4 modelMat = glm::mat4(1.0f);
			// translation: displace along circle with 'radius' in range [-offset, offset]
			float angle = (float)i / (float)numInstances * 360.0f;
			float displacementX = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float x = sin(angle) * radius + displacementX;
			float displacementY = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float y = displacementY * 0.8f + 10.0f; // keep height of asteroid field smaller compared to width of x and z
			float displacementZ = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float z = cos(angle) * radius + displacementZ;
			modelMat = glm::translate(modelMat, glm::vec3(x, y, z));
			//std::cout << "ModelMat[" << i << "] x = " << x << " y = " << y << " z = " << z << "\n";
			// Scale between 0.05 and 0.25f
			float scale = (rand() % 20) / 100.0f + 0.05;
			modelMat = glm::scale(modelMat, glm::vec3(scale));

			// rotation: add random rotation around a (semi)randomly picked rotation axis vector
			float rotAngle = (rand() % 360);
			modelMat = glm::rotate(modelMat, rotAngle, origin);
				
			// now add to list of matrices
			modelMatrices[i] = modelMat;
			
			// track other data for later animation
			originAngles.push_back(angle);
			rotAngs[i] = rotAngle; // track rotation angle 
			displacements.push_back(glm::vec3(displacementX, displacementY, displacementZ));
			//displacements[i] = new glm::vec3(displacementX, displacementY, displacementZ);
		}
	}

	




};