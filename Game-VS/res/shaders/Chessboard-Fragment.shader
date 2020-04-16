#version 130

out vec4 pColor;

bool isEven(float num) {
	return mod(num, 2.0) == 0.0;
}

void main() {
	// REQUIREMENT 1B) shade pixel:													  
	vec2 point = vec2(floor(gl_FragCoord.x / 37.5), floor(gl_FragCoord.y / 37.5));
	if ((isEven(point.x) && isEven(point.y)) || (!isEven(point.y) && !isEven(point.x)))
		pColor = vec4(0, 0, 0, 1); // black											  
	else
		pColor = vec4(1, 1, 1, 1); // white											  
};