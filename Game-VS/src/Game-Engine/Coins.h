#pragma once
#include "InstancedObject.h"

class Coins : public InstancedObject {
public:
	

	Coins(const char* filepath, Shader* shader) : InstancedObject(filepath, shader, 100) {
		//setModelTransformations();
		//configureInstancedArray();

		// TEST:Setup in constructor
		// generate a large list of semi-random model transformation matrices
		// ------------------------------------------------------------------
		unsigned int amount = 100;
		glm::mat4* modelMatrices;
		modelMatrices = new glm::mat4[amount];
		srand(glfwGetTime()); // initialize random seed	
		float radius = 15.0f;
		float offset = 2.5f;
		for (unsigned int i = 0; i < amount; i++)
		{
			glm::mat4 modelMat = glm::mat4(1.0f);
			// 1. translation: displace along circle with 'radius' in range [-offset, offset]
			float angle = (float)i / (float)amount * 360.0f;
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

		// configure instanced array
		// -------------------------
		unsigned int buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

		// set transformation matrices as an instance vertex attribute (with divisor 1)
        // note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
        // normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
        // -----------------------------------------------------------------------------------------------------------------------------------
		for (unsigned int i = 0; i < model.meshes.size(); i++)
		{
			unsigned int VAO = model.meshes[i].VAO;
			glBindVertexArray(VAO);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}


	}

	void setModelTransformations()  {
		// generate a large list of semi-random model transformation matrices
		// ------------------------------------------------------------------
		unsigned int amount = 100000;
		glm::mat4* modelMatrices;
		modelMatrices = new glm::mat4[amount];
		srand(glfwGetTime()); // initialize random seed	
		float radius = 150.0;
		float offset = 25.0f;
		for (unsigned int i = 0; i < amount; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			// 1. translation: displace along circle with 'radius' in range [-offset, offset]
			float angle = (float)i / (float)amount * 360.0f;
			float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float x = sin(angle) * radius + displacement;
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float z = cos(angle) * radius + displacement;
			model = glm::translate(model, glm::vec3(x, y, z));

			// 2. scale: Scale between 0.05 and 0.25f
			float scale = (rand() % 20) / 100.0f + 0.05;
			model = glm::scale(model, glm::vec3(scale));

			// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
			float rotAngle = (rand() % 360);
			model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

			// 4. now add to list of matrices
			modelMatrices[i] = model;
		}
	}


	//void setModelTransformations() {
	//	std::cout << " In Coins::setModelTransformations()\n";
	//	modelMatrices = new glm::mat4[numInstances];
	//	//srand(glfwGetTime()); // initialize random seed	
	//	//float radius = 150.0;
	//	float offset = 5.0f;
	//	for (unsigned int i = 0; i < numInstances; i++) {
	//		glm::mat4 model = glm::mat4(1.0f);
	//		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
	//		//float angle = (float)i / (float)numInstances * 360.0f;
	//		//float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
	//		//float x = 0.5f;
	//		//float y = 0.0f;
	//		//float z = i * offset;
	//		model = glm::translate(model, glm::vec3(tranRock.x, tranRock.y, tranRock.z + offset));

	//		// 2. scale: Scale between 0.05 and 0.25f
	//		//float scale = (rand() % 20) / 100.0f + 0.05;
	//		model = glm::scale(model, scaleRock);

	//		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
	//		//float rotAngle = (rand() % 360);
	//		//model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

	//		// 4. now add to list of matrices
	//		modelMatrices[i] = model;
	//	}
	//}
	void configureInstancedArray() {
		std::cout << " In InstancedObject::configureInstancedArray()\n";
		unsigned int buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

		// set transformation matrices as an instance vertex attribute (with divisor 1)
		// note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
		// normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
		// -----------------------------------------------------------------------------------------------------------------------------------
		for (unsigned int i = 0; i < model.meshes.size(); i++)
		{
			unsigned int VAO = model.meshes[i].VAO;
			glBindVertexArray(VAO);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}
	}
	
	void drawInstances(glm::mat4 projection, glm::mat4 view) override {

		std::cout << "In InstancedObject::drawInstances(), numInstances = " << numInstances << "\n";
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
		//shader->use();
		//for (unsigned int i = 0; i < model.meshes.size(); i++) {
		//	glBindVertexArray(model.meshes[i].VAO);
		//	glDrawElementsInstanced(
		//		GL_TRIANGLES, model.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, numInstances
		//	);
		//}
	}



};