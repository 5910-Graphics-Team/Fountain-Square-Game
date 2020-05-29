#pragma once
#include "InstancedObject.h"

class Coins : public InstancedObject, public Animation {
public:
	//const int NUM_ASTEROIDS = 500;

	Coins(const char* filepath, Shader* shader) : InstancedObject(filepath, shader, 500), Animation() {
		initModelTransformations();
		configureInstancedArray();
		speed = 4.0;
		
	}
	// asteroid data
	float  radius = 15.0f;
	float  offset = 2.5f;
	

	// animation data
	float speed;// = 1.0f;
	float lastFrame = 0.0f;

	void update(float time) override {
		//std::cout << "Updating Asteroid Ring\n";
		// update the orbit of each instance
		for (unsigned int i = 0; i < numInstances; i++) {
			//glm::mat4 mat = modelMatrices[i];
			float timeElapsed = time - lastFrame;
			//std::cout << "timeElapsed = " << timeElapsed << " speed = " << speed <<"\n";
			//modelMatrices[i] = glm::translate(modelMatrices[i], glm::vec3(0.0f, timeElapsed * speed, 0.0f));
			//modelMatrices[i] = glm::translate(modelMatrices[i], glm::vec3(0.0f, timeElapsed * speed, 0.0f));
			//std::cout << "Updating Asteroid Ring\n";
		}
		configureInstancedArray();
		lastFrame = time;
	}

	// todo rename to 'initModelTransforms'
	void initModelTransformations() override {
		// generating list of semi-random model transformation matrices
		//glm::mat4* modelMatrices;
		modelMatrices = new glm::mat4[numInstances];
		srand(glfwGetTime()); // initialize random seed	
		
		for (unsigned int i = 0; i < numInstances; i++) {
			glm::mat4 modelMat = glm::mat4(1.0f);
			// 1. translation: displace along circle with 'radius' in range [-offset, offset]
			float angle = (float)i / (float)numInstances * 360.0f;
			float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float x = sin(angle) * radius + displacement;
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float z = cos(angle) * radius + displacement;
			modelMat = glm::translate(modelMat, glm::vec3(x, y, z));
			std::cout << "ModelMat[" << i << "] x = " << x << " y = " << y << " z = " << z << "\n";
			// 2. scale: Scale between 0.05 and 0.25f
			float scale = (rand() % 20) / 100.0f + 0.05;
			modelMat = glm::scale(modelMat, glm::vec3(scale));

			// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
			float rotAngle = (rand() % 360);
			modelMat = glm::rotate(modelMat, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

			// 4. now add to list of matrices
			modelMatrices[i] = modelMat;
		}
	}

	
	void drawInstances(glm::mat4 projection, glm::mat4 view) override {

		//std::cout << "In AsteroidRing::drawInstances(), numInstances = " << numInstances << "\n";
		shader->use();

		shader->setMat4("projection", projection);
		shader->setMat4("view", view);

		//std::cout << "After shader->use():\n";

		 //texture code (causes errors)
		shader->setInt("texture_diffuse1", 0);
		//std::cout << "After shader->setInt():\n";
		glActiveTexture(GL_TEXTURE0);
		//std::cout << "After glActiveTexture, model.textures_loaded[0].id = " << model.textures_loaded[0].id << "\n";
		glBindTexture(GL_TEXTURE_2D, model.textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.

		//std::cout << "After glBindTexture\n";

		for (unsigned int i = 0; i < model.meshes.size(); i++) {
			//std::cout << "In ForLoop i = " << i << "\n";
			glBindVertexArray(model.meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, model.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, numInstances);
			glBindVertexArray(0);
		}
	}



};