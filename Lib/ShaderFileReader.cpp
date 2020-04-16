#include "ShaderFileReader.h"
#include <fstream>
#include <sstream>


ShaderFileReader::ShaderFileReader(const std::string& filepath) {
	parseShader(filepath);
}

ShaderFileReader ShaderFileReader::parseShader(const std::string& filepath) {
	std::ifstream stream(filepath);
	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}
	//ss[0].move;
	// copying temp stringstream
	std::string vert_shader_tmp = std::string(ss[0].str());
	std::string frag_shader_tmp = std::string(ss[1].str());


	//shaderSourceCode = { vert_shader_tmp.c_str(), frag_shader_tmp.c_str() };
	//this->vertexSource = vert_shader_tmp.c_str();
	//this->fragmentSource = frag_shader_tmp.c_str();
	//vertexSource = vert_shader_tmp.c_str();
	//fragmentSource = frag_shader_tmp.c_str();
	//		std::string str;
	//		std::vector<char> writable(str.begin(), str.end());
	//		writable.push_back('\0');
	//get the char* using &writable[0] or &*writable.begin()
	//OutputDebugStringA(vert_shader.c_str());
	//OutputDebugStringA(frag_shader.c_str());

	return { vert_shader_tmp.c_str(), frag_shader_tmp.c_str() };
};



