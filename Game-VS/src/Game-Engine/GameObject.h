#pragma once
#include "Model.h"
#include "Mesh.h"
#include "Shader.h"
//#include "../Audio-Engine/AudioEngine.h"
//#include "../Audio-Engine/Sound.h"

// Container for a Model and its default translation,scale, and rotation values. 
class GameObject {

protected:
    Model model;
    glm::vec3 trans, scale, rotAngs;
    const char* filepath;
    /*
    bool is3DAudioSource = false;

   AudioEngine* audioEngine = nullptr;

    const char* soundfile;

    */

public:
    //GameObject(const char* filepath, bool is3DAudioSource) : GameObject(filepath, glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(0.0f)) {}
    
    /*GameObject(const char* filepath, glm::vec3 defTrans, glm::vec3 defScale, glm::vec3 defRot, bool is3DAudioSource) 
        : model(filepath), trans(defTrans), scale(defScale), rotAngs(defRot), is3DAudioSource(is3DAudioSource) {

    } */
    GameObject(const char* filepath, glm::vec3 defTrans, glm::vec3 defScale, glm::vec3 defRot) : filepath(filepath), model(filepath), trans(defTrans), scale(defScale), rotAngs(defRot) { 
    }

    //void setAudioEngine(AudioEngine* audioEngine) {
    //    this->audioEngine = audioEngine;
    //}
    //void updateSoundPosition(float x, float y, float z) {
    //}
    //bool is3DAudioSource() {
    //    return is3DAudioSource;
    //}

    void draw(Shader* shader) {
        model.Draw(*shader);
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
    glm::mat4 getModel() {
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, trans);
        m = glm::rotate(m, glm::radians(rotAngs.x), glm::vec3(1.0f, 0.0f, 0.0f)); //rotation x
        m = glm::rotate(m, glm::radians(rotAngs.y), glm::vec3(0.0f, 1.0f, 0.0f)); //rotation y
        m = glm::rotate(m, glm::radians(rotAngs.z), glm::vec3(0.0f, 0.0f, 1.0f)); //rotation z 
        m = glm::scale(m, scale);	  // scale object
        return m;
    }
    const char* getObjFilePath() {
        return filepath;
    }
};