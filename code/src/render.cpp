#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>
#include <cassert>
#include <vector>

#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>

#include "GL_framework.h"
#include "SDL_timer.h"
#include "LoadOBJ.h"
#include "Shaders.h"
#include "Models.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GLuint compileShader(const char* shaderStr, GLenum shaderType, const char* name = "");
void linkProgram(GLuint program);

float camWidth, camHeight;

///////// fw decl
namespace ImGui 
{
	void Render();
}

namespace Axis 
{
	void setupAxis();
	void cleanupAxis();
	void drawAxis();
}

namespace RenderVars
{
	const float FOV = glm::radians(65.f);
	const float zNear = 1.f;
	const float zFar = 50.f;

	glm::mat4 _projection;
	glm::mat4 _modelView;
	glm::mat4 _MVP;
	glm::mat4 _inv_modelview;
	glm::vec4 _cameraPoint;

	struct prevMouse
	{
		float lastx, lasty;
		MouseEvent::Button button = MouseEvent::Button::None;
		bool waspressed = false;
	} prevMouse;

	float panv[3] = { 0.f, -5.f, -15.f };
	float rota[2] = { 0.f, 0.f };
}
namespace RV = RenderVars;

void GLResize(int width, int height) 
{
	camWidth = width;
	camHeight = height;

	glViewport(0, 0, width, height);
	if (height != 0) RV::_projection = glm::perspective(RV::FOV, (float)width / (float)height, RV::zNear, RV::zFar);
	else RV::_projection = glm::perspective(RV::FOV, 0.f, RV::zNear, RV::zFar);
}

void GLmousecb(MouseEvent ev) 
{
	if (RV::prevMouse.waspressed && RV::prevMouse.button == ev.button) 
	{
		float diffx = ev.posx - RV::prevMouse.lastx;
		float diffy = ev.posy - RV::prevMouse.lasty;
		switch (ev.button) 
		{
		case MouseEvent::Button::Left: // ROTATE
			RV::rota[0] += diffx * 0.005f;
			RV::rota[1] += diffy * 0.005f;
			break;
		case MouseEvent::Button::Right: // MOVE XY
			RV::panv[0] += diffx * 0.03f;
			RV::panv[1] -= diffy * 0.03f;
			break;
		case MouseEvent::Button::Middle: // MOVE Z
			RV::panv[2] += diffy * 0.05f;
			break;
		default: break;
		}
	}
	else 
	{
		RV::prevMouse.button = ev.button;
		RV::prevMouse.waspressed = true;
	}
	RV::prevMouse.lastx = ev.posx;
	RV::prevMouse.lasty = ev.posy;
}

//////////////////////////////////////////////////
GLuint compileShader(const char* shaderStr, GLenum shaderType, const char* name)
{
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderStr, NULL);
	glCompileShader(shader);
	GLint res;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE) 
	{
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &res);
		char* buff = new char[res];
		glGetShaderInfoLog(shader, res, &res, buff);
		fprintf(stderr, "Error Shader %s: %s", name, buff);
		delete[] buff;
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

void linkProgram(GLuint program) 
{
	glLinkProgram(program);
	GLint res;
	glGetProgramiv(program, GL_LINK_STATUS, &res);
	if (res == GL_FALSE) 
	{
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &res);
		char* buff = new char[res];
		glGetProgramInfoLog(program, res, &res, buff);
		fprintf(stderr, "Error Link: %s", buff);
		delete[] buff;
	}
}

////////////////////////////////////////////////// OBJECT
namespace Object
{
	Shader billboardShader("object_vertexShader.vs", "object_fragmentShader.fs", "object_geometryShader.gs", "OscarCaco_Head.png", true);
	Shader cubeShader("cube_vertexShader.vs", "cube_fragmentShader.fs", "cube_geometryShader.gs", "red.png", false);
	Shader explodingShader("exploding_vertexShader.vs", "exploding_fragmentShader.fs", "exploding_geometryShader.gs", "tnt.png", true);

	Model billboardModel("planeTest.obj");
	Model cubeModel("newCube.obj");
	Model explodingModel("newCube.obj");

	// this should be at the fragment shader
	struct Material {
		glm::vec3 ambient = glm::vec3(1.f, 0.5f, 0.31f);
		glm::vec3 diffuse = glm::vec3(1.f, 0.5f, 0.31f);
		glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f);
		float shininess = 32.f;
	};
	Material material;

	struct Light
	{
		glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
		glm::vec3 direction = glm::vec3(0.f, 0.f, 0.f);
		glm::vec3 ambient = glm::vec3(0.6f, 0.6f, 0.6f);
		glm::vec3 diffuse;
		glm::vec3 specular;

		float constant = 1.f;
		float linear = 0.09f;
		float quadratic = 0.032f;

		float cutOff = glm::cos(glm::radians(12.5f));
	};
	Light light;

	void setup()
	{
		//Inicialitzar el Shader 
		billboardShader.CreateAllShaders();
		cubeShader.CreateAllShaders();
		explodingShader.CreateAllShaders();

		//Create the vertex array object
		billboardModel.CreateVertexArrayObject();
		cubeModel.CreateVertexArrayObject();
		explodingModel.CreateVertexArrayObject();

		// Texture
		billboardShader.GenerateTexture();
		cubeShader.GenerateTexture();
		explodingShader.GenerateTexture();

		// Clean
		glBindVertexArray(0);
	}

	void cleanup()
	{
		billboardShader.DeleteProgram();
		cubeShader.DeleteProgram();
		explodingShader.DeleteProgram();

		billboardModel.Cleanup();
		cubeModel.Cleanup();
		explodingModel.Cleanup();
	}

	void render()
	{
		glm::vec4 fragColor = glm::vec4(5.f, 5.f, 5.f, 1.0f);
		glm::vec4 camPos = glm::inverse(RV::_modelView) * glm::vec4(0.f, 0.f, 0.f, 1.f);
		glm::vec3 correctCamPos = glm::vec3(camPos.x, camPos.y, camPos.z);
		float time = ImGui::GetTime();

		// == BILLBOARD ==
		billboardShader.UseProgram();
		billboardModel.BindVertex();
		
		// Texture
		billboardShader.ActivateTexture();

		billboardModel.SetUniforms(billboardShader, RenderVars::_modelView, RenderVars::_MVP, correctCamPos, fragColor);
		
		billboardModel.DrawArraysPoints();
		// == ==

		// == CUBE ==
		cubeShader.UseProgram();
		cubeModel.BindVertex();

		// Texture
		cubeShader.ActivateTexture();

		cubeModel.SetLocation(glm::vec3(0.f, -4.f, 0.f));
		cubeModel.SetScale(glm::vec3(0.3f));
		cubeModel.SetUniforms(cubeShader, RenderVars::_modelView, RenderVars::_MVP, fragColor);
		
		cubeModel.DrawArraysTriangles();
		// == ==

		// == EXPLODING ==
		explodingShader.UseProgram();
		explodingModel.BindVertex();

		// Texture
		explodingShader.ActivateTexture();
		
		explodingModel.SetLocation(glm::vec3(0.f, 0.f, -30.f));
		explodingModel.SetScale(glm::vec3(0.8f));
		explodingModel.SetUniforms(explodingShader, RenderVars::_modelView, RenderVars::_MVP, time, fragColor);
		
		explodingModel.DrawArraysTriangles();
		// == ==

		glBindVertexArray(0);
	}
}

////////////////////////////////////////////////// AXIS
namespace Axis 
{
	GLuint AxisVao;
	GLuint AxisVbo[3];
	GLuint AxisShader[2];
	GLuint AxisProgram;

	float AxisVerts[] = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 1.0
	};

	float AxisColors[] = {
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0
	};

	GLubyte AxisIdx[] = {
		0, 1,
		2, 3,
		4, 5
	};

	const char* Axis_vertShader =
		"#version 330\n\
		in vec3 in_Position;\n\
		in vec4 in_Color;\n\
		out vec4 vert_color;\n\
		uniform mat4 mvpMat;\n\
		void main() {\n\
			vert_color = in_Color;\n\
			gl_Position = mvpMat * vec4(in_Position, 1.0);\n\
		}";

	const char* Axis_fragShader =
		"#version 330\n\
		in vec4 vert_color;\n\
		out vec4 out_Color;\n\
		void main() {\n\
			out_Color = vert_color;\n\
		}";

	void setupAxis() 
	{
		glGenVertexArrays(1, &AxisVao);
		glBindVertexArray(AxisVao);
		glGenBuffers(3, AxisVbo);

		glBindBuffer(GL_ARRAY_BUFFER, AxisVbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, AxisVerts, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, AxisVbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, AxisColors, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 4, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, AxisVbo[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * 6, AxisIdx, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		AxisShader[0] = compileShader(Axis_vertShader, GL_VERTEX_SHADER, "AxisVert");
		AxisShader[1] = compileShader(Axis_fragShader, GL_FRAGMENT_SHADER, "AxisFrag");

		AxisProgram = glCreateProgram();
		glAttachShader(AxisProgram, AxisShader[0]);
		glAttachShader(AxisProgram, AxisShader[1]);
		glBindAttribLocation(AxisProgram, 0, "in_Position");
		glBindAttribLocation(AxisProgram, 1, "in_Color");
		linkProgram(AxisProgram);
	}

	void cleanupAxis() 
	{
		glDeleteBuffers(3, AxisVbo);
		glDeleteVertexArrays(1, &AxisVao);

		glDeleteProgram(AxisProgram);
		glDeleteShader(AxisShader[0]);
		glDeleteShader(AxisShader[1]);
	}

	void drawAxis() 
	{
		glBindVertexArray(AxisVao);
		glUseProgram(AxisProgram);
		glUniformMatrix4fv(glGetUniformLocation(AxisProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RV::_MVP));
		glDrawElements(GL_LINES, 6, GL_UNSIGNED_BYTE, 0);

		glUseProgram(0);
		glBindVertexArray(0);
	}
}

////////////////////////////////////////////////// CUBE
namespace Cube 
{
	GLuint cubeVao;
	GLuint cubeVbo[3];
	GLuint cubeShaders[2];
	GLuint cubeProgram;
	glm::mat4 objMat = glm::mat4(1.f);

	extern const float halfW = 0.5f;
	int numVerts = 24 + 6; // 4 vertex/face * 6 faces + 6 PRIMITIVE RESTART

						   //   4---------7
						   //  /|        /|
						   // / |       / |
						   //5---------6  |
						   //|  0------|--3
						   //| /       | /
						   //|/        |/
						   //1---------2
	glm::vec3 verts[] = {
		glm::vec3(-halfW, -halfW, -halfW),
		glm::vec3(-halfW, -halfW,  halfW),
		glm::vec3(halfW, -halfW,  halfW),
		glm::vec3(halfW, -halfW, -halfW),
		glm::vec3(-halfW,  halfW, -halfW),
		glm::vec3(-halfW,  halfW,  halfW),
		glm::vec3(halfW,  halfW,  halfW),
		glm::vec3(halfW,  halfW, -halfW)
	};

	glm::vec3 norms[] = {
		glm::vec3(0.f, -1.f,  0.f),
		glm::vec3(0.f,  1.f,  0.f),
		glm::vec3(-1.f,  0.f,  0.f),
		glm::vec3(1.f,  0.f,  0.f),
		glm::vec3(0.f,  0.f, -1.f),
		glm::vec3(0.f,  0.f,  1.f)
	};

	glm::vec3 cubeVerts[] = {
		verts[1], verts[0], verts[2], verts[3],
		verts[5], verts[6], verts[4], verts[7],
		verts[1], verts[5], verts[0], verts[4],
		verts[2], verts[3], verts[6], verts[7],
		verts[0], verts[4], verts[3], verts[7],
		verts[1], verts[2], verts[5], verts[6]
	};

	glm::vec3 cubeNorms[] = {
		norms[0], norms[0], norms[0], norms[0],
		norms[1], norms[1], norms[1], norms[1],
		norms[2], norms[2], norms[2], norms[2],
		norms[3], norms[3], norms[3], norms[3],
		norms[4], norms[4], norms[4], norms[4],
		norms[5], norms[5], norms[5], norms[5]
	};

	GLubyte cubeIdx[] = {
		0, 1, 2, 3, UCHAR_MAX,
		4, 5, 6, 7, UCHAR_MAX,
		8, 9, 10, 11, UCHAR_MAX,
		12, 13, 14, 15, UCHAR_MAX,
		16, 17, 18, 19, UCHAR_MAX,
		20, 21, 22, 23, UCHAR_MAX
	};

	const char* cube_vertShader =
		"#version 330\n\
		in vec3 in_Position;\n\
		in vec3 in_Normal;\n\
		out vec4 vert_Normal;\n\
		uniform mat4 objMat;\n\
		uniform mat4 mv_Mat;\n\
		uniform mat4 mvpMat;\n\
		void main() {\n\
			gl_Position = mvpMat * objMat * vec4(in_Position, 1.0);\n\
			vert_Normal = mv_Mat * objMat * vec4(in_Normal, 0.0);\n\
		}";

	const char* cube_fragShader =
		"#version 330\n\
		in vec4 vert_Normal;\n\
		out vec4 out_Color;\n\
		uniform mat4 mv_Mat;\n\
		uniform vec4 color;\n\
		void main() {\n\
			out_Color = vec4(color.xyz * dot(vert_Normal, mv_Mat*vec4(0.0, 1.0, 0.0, 0.0)) + color.xyz * 0.3, 1.0 );\n\
		}";

	void setupCube() 
	{
		glGenVertexArrays(1, &cubeVao);
		glBindVertexArray(cubeVao);
		glGenBuffers(3, cubeVbo);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNorms), cubeNorms, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glPrimitiveRestartIndex(UCHAR_MAX);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVbo[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIdx), cubeIdx, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		cubeShaders[0] = compileShader(cube_vertShader, GL_VERTEX_SHADER, "cubeVert");
		cubeShaders[1] = compileShader(cube_fragShader, GL_FRAGMENT_SHADER, "cubeFrag");

		cubeProgram = glCreateProgram();
		glAttachShader(cubeProgram, cubeShaders[0]);
		glAttachShader(cubeProgram, cubeShaders[1]);
		glBindAttribLocation(cubeProgram, 0, "in_Position");
		glBindAttribLocation(cubeProgram, 1, "in_Normal");
		linkProgram(cubeProgram);
	}

	void cleanupCube() 
	{
		glDeleteBuffers(3, cubeVbo);
		glDeleteVertexArrays(1, &cubeVao);

		glDeleteProgram(cubeProgram);
		glDeleteShader(cubeShaders[0]);
		glDeleteShader(cubeShaders[1]);
	}

	void updateCube(const glm::mat4& transform) 
	{
		objMat = transform;
	}

	void drawCube() 
	{
		glEnable(GL_PRIMITIVE_RESTART);
		glBindVertexArray(cubeVao);
		glUseProgram(cubeProgram);
		
		// CUBE 01
		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(objMat));
		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mv_Mat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_modelView));
		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mvpMat"), 1, GL_FALSE, glm::value_ptr(RenderVars::_MVP));
		glUniform4f(glGetUniformLocation(cubeProgram, "color"), 0.1f, 1.f, 1.f, 0.f);
		glDrawElements(GL_TRIANGLE_STRIP, numVerts, GL_UNSIGNED_BYTE, 0);

		// CUBE 02
		float time = ImGui::GetTime();

		// Change position (transalte)
		glm::mat4 cubeTranslateMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, cos(time) * 2.0f + 2.0f, 2.0f));//2.0f, cos(time) * 2.0f + 2.0f, 2.0f));

		// Change size (scale)
		float scaleRes = ((sin(time) * 2.0f + 2.0f) + 1) / 2;
		glm::mat4 cubeScaleMatrix = glm::scale(glm::mat4(), glm::vec3(scaleRes, scaleRes, scaleRes));

		// Change y-rotation (rotate)
		float rotateAngle = time; //1.0f * (float)sin(3.0f * time);
		glm::mat4 cubeRotateMatrix = glm::rotate(glm::mat4(), rotateAngle, glm::vec3(0.0f, 1.0f, 0.0f));

		// Rotate along the 1st cube (rotate)
		glm::mat4 cubeToCubeTranslateMatrix = glm::translate(glm::mat4(), glm::vec3(1.0f, 0.0f, 3.0f));

		// Set random color
		const GLfloat cubeColor[] = { sin(time) * 0.5f + 0.5f, cos(time) * 0.5f + 0.5f, 0.0f, 1.0f };

		// "Create" 2nd cube
		glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "objMat"), 1, GL_FALSE, glm::value_ptr(cubeTranslateMatrix * cubeRotateMatrix * cubeToCubeTranslateMatrix * cubeScaleMatrix));
		glUniform4f(glGetUniformLocation(cubeProgram, "color"), cubeColor[0], cubeColor[1], cubeColor[2], 0.f);
		glDrawElements(GL_TRIANGLE_STRIP, numVerts, GL_UNSIGNED_BYTE, 0);

		glUseProgram(0);
		glBindVertexArray(0);
		glDisable(GL_PRIMITIVE_RESTART);
	}
}

/////////////////////////////////////////////////


void GLinit(int width, int height) 
{
	glViewport(0, 0, width, height);
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);
	glClearDepth(1.f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	RV::_projection = glm::perspective(RV::FOV, (float)width / (float)height, RV::zNear, RV::zFar);
	//RV::_projection = glm::ortho(RV::FOV, (float)width / (float)height, RV::zNear, RV::zFar);

	// Setup shaders & geometry
	Axis::setupAxis();
	Object::setup();
	//Cube::setupCube();


	/////////////////////////////////////////////////////TODO
	// Do your init code here
	// ...
	//Exercise::init();
	// ...
	/////////////////////////////////////////////////////////
}

void GLcleanup() 
{
	Axis::cleanupAxis();
	Object::cleanup();
	//Cube::cleanupCube();

	/////////////////////////////////////////////////////TODO
	// Do your cleanup code here
	// ...
	//Exercise::cleanup();
	// ...
	/////////////////////////////////////////////////////////
}

void GLrender(float dt) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	time_t currentTime = SDL_GetTicks() / 1000;

	const GLfloat color[] = { 0.5f, 0.5f, 0.5f, 1.0f }; //{ (float)sin(currentTime) * 0.5f + 0.5f, (float)cos(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, color);

	RV::_modelView = glm::mat4(1.f);
	RV::_modelView = glm::translate(RV::_modelView, glm::vec3(RV::panv[0], RV::panv[1], RV::panv[2]));
	RV::_modelView = glm::rotate(RV::_modelView, RV::rota[1], glm::vec3(1.f, 0.f, 0.f));
	RV::_modelView = glm::rotate(RV::_modelView, RV::rota[0], glm::vec3(0.f, 1.f, 0.f));

	RV::_MVP = RV::_projection * RV::_modelView;

	Axis::drawAxis();
	//Cube::drawCube();
	Object::render();

	/////////////////////////////////////////////////////TODO
	// Do your render code here
	//Exercise::render();
	/////////////////////////////////////////////////////////

	ImGui::Render();
}


void GUI() 
{
	bool show = true;
	ImGui::Begin("Physics Parameters", &show, 0);

	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		/////////////////////////////////////////////////////TODO
		// Do your GUI code here....
		// ...
		// ...
		// ...
		/////////////////////////////////////////////////////////
	}
	// .........................

	ImGui::End();

	// Example code -- ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	bool show_test_window = false;
	if (show_test_window) 
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}