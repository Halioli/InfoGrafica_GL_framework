#version 330 
layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;
in vec4 vert_Normal[];
in vec3 FragPos[];
in vec2 fragUVs[];
out vec4 fragmentNorm;
out vec3 fragmentPos;
out vec2 fragmentUV;
uniform mat4 mvpMat;
uniform float time;

vec3 GetNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));
}

vec4 Explode(vec4 position, vec3 normal)
{
	float magnitude = 2.0;
	vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
	return position + vec4(direction, 0.0);
}

void main() {
	vec3 normal = GetNormal();

	gl_Position = Explode(gl_in[0].gl_Position, normal);
	fragmentNorm = vert_Normal[0];
	fragmentPos = FragPos[0];
	fragmentUV = fragUVs[0];
	EmitVertex();

	gl_Position = Explode(gl_in[1].gl_Position, normal);
	fragmentNorm = vert_Normal[1];
	fragmentPos = FragPos[1];
	fragmentUV = fragUVs[1];
	EmitVertex();

	gl_Position = Explode(gl_in[2].gl_Position, normal);
	fragmentNorm = vert_Normal[2];
	fragmentPos = FragPos[2];
	fragmentUV = fragUVs[2];
	EmitVertex();

	EndPrimitive();
}