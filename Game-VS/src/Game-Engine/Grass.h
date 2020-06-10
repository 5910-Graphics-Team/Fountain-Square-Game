#pragma once
#include "InstancedObject.h"
#include <GLFW/glfw3.h>
#include <tgmath.h>
#include <math.h> 

class Grass : public InstancedObject {
public:
	/**
	 * Constructs Grass Instanced object which is an instanced object. 
	 */
	Grass(const char* filepath, Shader* shader) : InstancedObject(filepath, shader, 1024) {
		initModelTransformations();
		configureInstancedArray();	
	}

protected:
	
	void initModelTransformations() override {
		// values used in generating the semi-random model matrices
		// TODO simplify this hard-coded implementation
		float offset = .60f;
		float offset2 = 45.0f;
		float factor = 120.0f;
		// generating list of semi-random model transformation matrices
		srand(glfwGetTime()); // init random seed	

		int squareRoot = sqrt(numInstances);
		for (unsigned int i = 0; i < numInstances; i++) {
			glm::mat4 modelMat = glm::mat4(1.0f);
			float displacementX = (rand() % (int)(factor * offset * 100)) / 100.0f - offset;
			float x = (i % (squareRoot * 2)) * offset - offset2 + displacementX;
			float y = 0.0f; 
			float displacementZ = (rand() % (int)(factor * offset * 100)) / 100.0f - offset;
			float z = (i % (squareRoot * 2)) * offset - offset2 + displacementZ; // + displacementZ;
			modelMat = glm::translate(modelMat, glm::vec3(x, y, z));
			std::cout << "ModelMat[" << i << "] x = " << x << " y = " << y << " z = " << z << "\n";
			float scale = (rand() % 100) / 100 + 0.7f;
			modelMat = glm::scale(modelMat, glm::vec3(scale * scaleGrass * GLOBAL_SCALE));

			// rotation: add random rotation around a (semi)randomly picked rotation axis vector
			float rotY = (rand() % 360);			
			
			modelMat = glm::rotate(modelMat, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f)); //rotation around y

			// now add to list of matrices
			modelMatrices[i] = modelMat;
		}
	}

};