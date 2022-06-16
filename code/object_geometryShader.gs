#version 330 
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;
out vec2 TexCoord;
uniform vec3 cameraPos;
uniform mat4 mvpMat;

void main() {
	// New Code
	float width = 10;
	float height = 10;
	
	vec3 Pos = gl_in[0].gl_Position.xyz;
	vec3 toCamera = normalize(cameraPos - Pos);
	vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right =  normalize(cross(toCamera, up)) * width;
	up = normalize(cross(right, toCamera)) * height;

	//BottomRight
	vec3 bottomRight = Pos + (right * 0.5);
    gl_Position = mvpMat * vec4(bottomRight, 1.0);
    TexCoord = vec2(1.0, 0.0);
    EmitVertex();

	//BottomLeft
	vec3 bottomLeft = Pos - (right * 0.5);
    gl_Position = mvpMat * vec4(bottomLeft, 1.0);
    TexCoord = vec2(0.0, 0.0);
    EmitVertex();

	//UpRight
	vec3 upRight = Pos + (right * 0.5) + up;
    gl_Position = mvpMat * vec4(upRight, 1.0);
    TexCoord = vec2(1.0, 1.0);
    EmitVertex();

	//UpLeft
	vec3 upLeft = Pos - (right * 0.5) + up;
    gl_Position = mvpMat * vec4(upLeft, 1.0);
    TexCoord = vec2(0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}