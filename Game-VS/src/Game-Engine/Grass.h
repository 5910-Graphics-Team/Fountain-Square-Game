#pragma once
#include "InstancedObject.h"
#include <GLFW/glfw3.h>
#include <tgmath.h>
#include <math.h> 

class Grass : public InstancedObject {
public:
	//const int NUM_ASTEROIDS = 500;


	Grass(const char* filepath, Shader* shader) : InstancedObject(filepath, shader, 1024), 
		displacements(), originAngles(), origin(tranGrass * GLOBAL_POSITION_SCALE) {
		initModelTransformations();
		configureInstancedArray();
		
	}


	// asteroid data
	glm::vec3 origin;
	//float radius = 50.0f;
	float offset = .60f;
	float offset2 = 45.0f;
	float factor = 120.0f;
	//float grassDistance = 2.0f

	std::vector<float> originAngles;

	// Array of x,y,z displacements of each instance. size = numInstances

	std::vector<glm::vec3> displacements;

	// animation data
	float rotSpeed; // = 1.0f;
	float lastFrame = 0.0f;

	//void calculateRotation()

	void initModelTransformations() override {
		// generating list of semi-random model transformation ma0trices
		//glm::mat4* modelMatrices;

		srand(glfwGetTime()); // initialize random seed	

		int squareRoot = sqrt(numInstances);//, numZ = numX;
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


		// TODO New Implementation approach
	//	int squareRoot = sqrt(numInstances);//, numZ = numX;
	//	std::cout << "Root = " << squareRoot << '\n';
	//	for (unsigned int i = 1; i <= squareRoot; i++) {
	//		//std::cout << "i = " << i<< '\n';

	//		for (unsigned int j = 1; j <= squareRoot; j++) {
	//			//std::cout << "j = " << j << '\n';

	//			glm::mat4 modelMat = glm::mat4(1.0f);
	//			// translation: displace along circle with 'radius' in range [-offset, offset]

	//			//float displacementX = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
	//			float x = i * offset; //+ displacementX;

	//			float y = 0.0f;

	//			//float displacementZ = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
	//			float z = j * offset; // + displacementZ;

	//			modelMat = glm::translate(modelMat, glm::vec3(x, y, z) * GLOBAL_POSITION_SCALE);
	//			std::cout << "ModelMat[" << i << "]["<< j << "] x = " << x << " y = " << y << " z = " << z << "\n";

	//			modelMat = glm::scale(modelMat, glm::vec3(scaleGrass * GLOBAL_SCALE));
	//			
	//			// rotation: add random rotation around a (semi)randomly picked rotation axis vector
	//			///float rotAngle = (rand() % 360);
	//			//modelMat = glm::rotate(modelMat, rotAngle, origin);

	//			// now add to list of matrices
	//			modelMatrices[i - 1] = modelMat;
	//		}
	//	
	//	}
	}






};