#ifndef PARTICLEOBJECT_H
#define PARTICLEOBJECT_H

#include <glad.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "sprite_renderer.h"


// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class ParticleObject
{
public:
    // object state
    glm::vec2   Position, Size, Velocity;
    glm::vec3   Color;
    float       Rotation;
    bool        IsSolid;
    bool        Destroyed;
    // render state
    Texture2D   Sprite;
    // constructor(s)
    ParticleObject();
    ParticleObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    // draw sprite
    virtual void Draw(SpriteRenderer& renderer);
};

#endif