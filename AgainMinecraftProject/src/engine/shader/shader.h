#pragma once

#include <map>

#include <glm/glm.hpp>

namespace Engine
{
	struct Shader
	{
		size_t id;
	};


	void loadShaders(std::map<const char*, Shader>& shaders);
	void initShader(Shader& shader, const char* vPath, const char* fPath);
	void useShader(Shader& shader);
	
	void setUniformBool(Shader& shader, const char* uniform, bool value);
	void setUniformi(Shader& shader, const char* uniform, uint32_t value);
	void setUniform3f(Shader& shader, const char* uniform, glm::vec3 vec);
	void setUniform4m(Shader& shader, const char* uniform, glm::mat4 mat);
}
