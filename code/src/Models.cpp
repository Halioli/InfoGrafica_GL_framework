#include "Models.h"

Model::Model(char* objPath)
{
	objMat = glm::mat4(1.f);

	bool res = loadObject::loadOBJ(objPath, objVertices, objUVs, objNormals);
}

Model::~Model()
{
}

void Model::CreateVertexArrayObject()
{
	//Create the vertex array object
	//This object maintains the state related to the input of the OpenGL
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(3, VBO);

	// Vertex
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, objVertices.size() * sizeof(glm::vec3), &objVertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // , 8 * sizeof(float), (void*)(6 * sizeof(float)))
	glEnableVertexAttribArray(0);

	// Normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, objNormals.size() * sizeof(glm::vec3), &objNormals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// UVs
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, objUVs.size() * sizeof(glm::vec2), &objUVs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);
}

void Model::Cleanup()
{
	glDeleteVertexArrays(1, &VAO);

	glDeleteBuffers(3, VBO);
}

void Model::BindVertex()
{
	glBindVertexArray(VAO);
}

void Model::SetUniforms(Shader shader, glm::mat4 modelView, glm::mat4 MVP, glm::vec3 fragColor)
{
	shader.SetUniformInt("diffuseTexture", 0);
	shader.SetUniformMatrix4("objMat", objMat);
	shader.SetUniformMatrix4("mv_Mat", modelView);
	shader.SetUniformMatrix4("mvpMat", MVP);
	shader.SetUniformVector4("color", fragColor);
}

void Model::SetUniforms(Shader shader, glm::mat4 modelView, glm::mat4 MVP, glm::vec4 cameraPoint, glm::vec3 fragColor)
{
	shader.SetUniformVector3("cameraPos", cameraPoint);

	shader.SetUniformInt("diffuseTexture", 0);
	shader.SetUniformMatrix4("objMat", objMat);
	shader.SetUniformMatrix4("mv_Mat", modelView);
	shader.SetUniformMatrix4("mvpMat", MVP);
	shader.SetUniformVector4("color", fragColor);
}

void Model::SetUniforms(Shader shader, glm::mat4 modelView, glm::mat4 MVP, float time, glm::vec3 fragColor)
{
	shader.SetUniformFloat("time", time);

	shader.SetUniformInt("diffuseTexture", 0);
	shader.SetUniformMatrix4("objMat", objMat);
	shader.SetUniformMatrix4("mv_Mat", modelView);
	shader.SetUniformMatrix4("mvpMat", MVP);
	shader.SetUniformVector4("color", fragColor);
}

void Model::DrawArrays()
{
	glDrawArrays(GL_TRIANGLES, 0, objVertices.size());
}