#version 130

in vec2 point;							// 2D point from GPU memory					  

void main() {
	// REQUIREMENT 1A) transform vertex:											  
	gl_Position = vec4(point, 0, 1);	// 'built-in' variable						  
};