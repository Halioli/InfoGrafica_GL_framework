#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <GL\glew.h>
#include <cstring>
#include <sstream>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

class Shader
{
public:
	Shader(std::string vertexShaderPath, std::string fragmentShaderPath);
	Shader(std::string vertexShaderPath, std::string fragmentShaderPath, std::string geometryShaderPath);
	~Shader();

	std::string GetShaderFromPath(std::string fragmentPath);

	void CreateAllShaders();
	void UseProgram();
	void DeleteProgram();
	GLuint GetProgram();

	GLuint GetUniformLocation(char* uniformName);
	void SetUniformInt(char* uniformName, int value);
	void SetUniformFloat(char* uniformName, int value);
	void SetUniformVector3(char* uniformName, glm::vec3 value);
	void SetUniformVector4(char* uniformName, glm::vec3 value);
	void SetUniformMatrix4(char* uniformName, glm::mat4 value);

private:
	std::string vertexShaderSource;
	std::string geometryShaderSource;
	std::string fragmentShaderSource;

	GLuint program;
};