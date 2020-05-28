#pragma once
#include "GameObject.h"

class InstancedObject {
public:
	/*struct InstanceData {
		glm::vec3 trans, scale, rotAngs;
		bool destroyed = false;
	};*/

	InstancedObject(const char* filepath, Shader* shader, int numInstances) : model(filepath), shader(shader), numInstances(numInstances) {
		
	}



	virtual void drawInstances(glm::mat4 projection, glm::mat4 view) = 0;

protected:
	Shader* shader;
	Model model;

	glm::mat4* modelMatrices;
	unsigned int numInstances;
	
	virtual void setModelTransformations() = 0;


	virtual void configureInstancedArray() = 0;

	
private:

};