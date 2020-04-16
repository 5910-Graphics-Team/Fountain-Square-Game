#ifndef SHADER_FILE_READER_H
#define SHADER_FILE_READER_H
#include <vector> 
#include <string.h>

enum class ShaderType {
	NONE = -1, VERTEX = 0, FRAGMENT = 1
};

struct ShaderSourceCode {
	const char* vertexSource;
	const char* fragmentSource;
};
class ShaderFileReader {
private:
	
	static ShaderSourceCode shaderSourceCode;
	
	ShaderSourceCode parseShader(const std::string& filepath);

public:
	ShaderFileReader(const std::string& filepath);
	const char* getVertexShader() {
		return shaderSourceCode.vertexSource;
	}
	const char* getFragmentShader() {
		return shaderSourceCode.fragmentSource;
	}
};
#endif