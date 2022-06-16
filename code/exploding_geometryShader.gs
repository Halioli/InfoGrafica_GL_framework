#version 330 
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
in vec3 vert_Normal[];
in vec3 FragPos[];
in vec2 fragUVs[];
out vec4 fragmentNorm;
out vec2 fragmentUV;
uniform float time;
uniform mat4 objMat;
uniform mat4 mv_Mat;
uniform mat4 mvpMat;

vec3 GetNormal()
{
	vec3 a = vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[0].gl_Position);
	return -normalize(cross(a, b));
}

vec4 Explode(vec4 position, vec3 normal)
{
	return position;

	float magnitude = 2.0;
	vec3 direction = normal * clamp(sin(time), 0.0, 1.0) * magnitude;
	return position + vec4(direction, 0.0);
}

void main() {
	float magnitude = clamp(sin(time), 0.0, 1.0) * 20.0;
	vec3 explosionOffset = vert_Normal[0] * magnitude;

	gl_Position = mvpMat * objMat * vec4(FragPos[0] + explosionOffset, 1.0) + vec4(0, 0, 0, 1);
	fragmentNorm = vec4(vert_Normal[0], 0.0);
	fragmentUV = fragUVs[0];
	EmitVertex();
	
	explosionOffset = vert_Normal[1] * magnitude;
	gl_Position = mvpMat * objMat * vec4(FragPos[1] + explosionOffset, 1.0) + vec4(0, 0, 0, 1);
	fragmentNorm = vec4(vert_Normal[1], 0.0);
	fragmentUV = fragUVs[1];
	EmitVertex();
	
	explosionOffset = vert_Normal[2] * magnitude;
	gl_Position = mvpMat * objMat * vec4(FragPos[2] + explosionOffset, 1.0) + vec4(0, 0, 0, 1);
	fragmentNorm = vec4(vert_Normal[2], 0.0);
	fragmentUV = fragUVs[2];
	EmitVertex();

	EndPrimitive();
}