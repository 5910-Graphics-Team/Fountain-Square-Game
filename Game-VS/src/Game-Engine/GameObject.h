#pragma once
#include "Model.h"

/**
 * Basic Container for a regular in-game object. 
 * Can also be implemented to provide access to the classes' functionality 
 */
class GameObject {

protected:
    Model model;    
    glm::vec3 trans, scale, rotAngs;
    const char* filepath;
    bool destroyed = false;

public:
    /**
     * Default Constructor which sets the position, scale and rotation fields to default values.
     */
    GameObject(const char* filepath) : GameObject(filepath,glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)) {}

    /**
	 * Creates a game object using the OBJ file at the specified relative path, with provided translation, size scale, and rotation values. 
     * The object will try to load any textures inside the provided directory and map them onto the object.
     */
    GameObject(const char* filepath, glm::vec3 defTrans, glm::vec3 defScale, glm::vec3 defRot) : filepath(filepath), model(filepath), trans(defTrans), scale(defScale), rotAngs(defRot) {}

    void draw(Shader* shader) {
        if (!destroyed) {
            model.Draw(*shader);
        }
    }

    void setTranslation(glm::vec3 trans) {
        this->trans = trans;
    }

    void setRotation(glm::vec3 rot) {
        this->rotAngs = rot;
    }

    glm::vec3 getTranslation() {
        return trans;
    }

    glm::vec3 getScale() {
        return scale;
    }

    glm::vec3 getRotationAngles() {
        return rotAngs;
    }
    /**
     * Method which gets the model matrix for this game object.
     */
    glm::mat4 getModel() {
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, trans);
        m = glm::rotate(m, glm::radians(rotAngs.x), glm::vec3(1.0f, 0.0f, 0.0f)); //rotation x
        m = glm::rotate(m, glm::radians(rotAngs.y), glm::vec3(0.0f, 1.0f, 0.0f)); //rotation y
        m = glm::rotate(m, glm::radians(rotAngs.z), glm::vec3(0.0f, 0.0f, 1.0f)); //rotation z 
        m = glm::scale(m, scale);	  // scale object
        return m;
    }

    void setScale(glm::vec3 scale) {
        this->scale = scale;
    }

    const char* getObjFilePath() {
        return filepath;
    }

    /**
     * Method which can be used to prevent the GameObject from displaying, without removing it from OpenGL. 
     */
	bool isDestroyed() {
		return destroyed;
	}

	void setDestroyed(bool destroyed) {
		this->destroyed = destroyed;
	}
};