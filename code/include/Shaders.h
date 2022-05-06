#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <GL\glew.h>
#include <cstring>
#include <sstream>

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

private:
	std::string vertexShaderSource;
	std::string geometryShaderSource;
	std::string fragmentShaderSource;

	GLuint program;
};