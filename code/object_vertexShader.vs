#version 330 
layout (location = 0) in vec3 in_Vertices;
uniform mat4 objMat;
void main() {
	gl_Position = objMat * vec4(in_Vertices, 1.0);
}