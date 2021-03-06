#pragma once
#include "InstancedObject.h"
#include <tgmath.h>
/**
 * Custom instanced object continaer encapsulating an asteroid ring made up of rock object with many generated semi-random modelView matrices
 * TODO Move origin to be set in costructor rather than being set statically
 */
class AsteroidRing : public InstancedObject, public Animation {
public:

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

		lastFrame = time;
	}
	
	/**
	 * Generates an asteroid ring around a set origin.
	 * source: https://learnopengl.com/Advanced-OpenGL/Instancing
	 */
	void initModelTransformations() override {

		
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
			
			// TODO track other data for later animation
			//originAngles.push_back(angle);
			//rotAngs[i] = rotAngle; // track rotation angle 
			//displacements.push_back(glm::vec3(displacementX, displacementY, displacementZ));
			//displacements[i] = new glm::vec3(displacementX, displacementY, displacementZ);
		}
	}

	




};