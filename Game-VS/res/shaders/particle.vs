#version 330 core
layout (location = 0) in vec3 vertex; // <vec2 position, vec2 texCoords>
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform vec3 offset;
uniform vec4 color;

void main()
{
    float scale = 10.0f;
    TexCoords = aTexCoords;
    ParticleColor = color;
    gl_Position = projection * vec4((vertex.xyz * scale) + offset, 1.0);
}