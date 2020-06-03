#pragma once
#include "InstancedObject.h"
#include <GLFW/glfw3.h>
#include <tgmath.h>

class Grass : public InstancedObject {
public:
	//const int NUM_ASTEROIDS = 500;


	Grass(const char* filepath, Shader* shader) : InstancedObject(filepath, shader, 1000), displacements(), originAngles(), origin(tranGrass) {
		initModelTransformations();
		configureInstancedArray();
		rotSpeed = 4.0;
	}


	// asteroid data
	glm::vec3 origin;
	//float radius = 50.0f;
	float offset = 2.5f;

	//float grassDistance = 2.0f

	std::vector<float> originAngles;

	// Array of x,y,z displacements of each instance. size = numInstances
	//glm::vec3** displacements;
	std::vector<glm::vec3> displacements;

	// animation data
	float rotSpeed; // = 1.0f;
	float lastFrame = 0.0f;

	//void calculateRotation()

	void initModelTransformations() override {
		// generating list of semi-random model transformation matrices
		//glm::mat4* modelMatrices;

		srand(glfwGetTime()); // initialize random seed	

		for (unsigned int i = 0; i < numInstances; i++) {
			glm::mat4 modelMat = glm::mat4(1.0f);
			// translation: displace along circle with 'radius' in range [-offset, offset]
			//float angle = (float)i / (float)numInstances * 360.0f;
			float displacementX = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float x = i * offset; //+ displacementX;
			//float displacementY = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float y = 1.0f; //= displacementY * 0.8f + 10.0f; // keep height of asteroid field smaller compared to width of x and z
			float displacementZ = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float z = i * offset; // + displacementZ;
			modelMat = glm::translate(modelMat, glm::vec3(x, y, z));
			//std::cout << "ModelMat[" << i << "] x = " << x << " y = " << y << " z = " << z << "\n";
			// Scale between 0.05 and 0.25f
			//float scale = (rand() % 20) / 100.0f + 0.05;
			modelMat = glm::scale(modelMat, glm::vec3(scaleGrass * GLOBAL_SCALE));

			// rotation: add random rotation around a (semi)randomly picked rotation axis vector
			float rotAngle = (rand() % 360);
			modelMat = glm::rotate(modelMat, rotAngle, origin);

			// now add to list of matrices
			modelMatrices[i] = modelMat;

		
		}
	}






};