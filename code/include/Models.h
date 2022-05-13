#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <vector>
#include "LoadOBJ.h"
#include "Shaders.h"

class Model
{
public:
	Model(char* objPath);
	~Model();

	void CreateVertexArrayObject();
	void Cleanup();
	void BindVertex();
	void SetUniforms(Shader shader, glm::mat4 modelView, glm::mat4 MVP, glm::vec3 fragColor);
	void SetUniforms(Shader shader, glm::mat4 modelView, glm::mat4 MVP, glm::vec4 cameraPoint, glm::vec3 fragColor);
	void DrawArrays();

private:
	GLuint VAO;
	GLuint VBO[3];

	std::vector<glm::vec3> objVertices;
	std::vector<glm::vec2> objUVs;
	std::vector<glm::vec3> objNormals;

	glm::mat4 objMat;
};