#version 130

in vec3 point;
in vec3 color;
in vec3 normal;

uniform vec3 lightVec = vec3(1, .5, -1);
uniform mat4 modelview;
uniform mat4 persp;

out vec4 vColor;

void main() {
	// set gl_Position to input pt transformed by both matrices:
	gl_Position = persp * modelview * vec4(point, 1);
	// transform normal by modelview, set to unit length;
	vec3 xnormal = normalize((modelview * vec4(normal, 0)).xyz);
	//ensure light vector is unit length:
	vec3 vlight = normalize(lightVec);
	// set ambient, 2-sided difuse, and specular coefficients:
	float a = .1;
	float d = abs(dot(xnormal, vlight));
	float s = pow(d, 50);
	// set intensity and outpuyt color:
	float intensity = clamp(a+d+s, 0, 1);
	vColor = vec4(intensity * color, 0);
}