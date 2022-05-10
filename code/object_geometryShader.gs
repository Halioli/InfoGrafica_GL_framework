#version 330 
layout (points) in; // This for billboards
//layout (triangles) in; // This for the cube
layout (triangle_strip, max_vertices = 6) out;
in vec4 vert_Normal[];
in vec3 FragPos[];
in vec2 fragUVs[];
out vec4 fragmentNorm;
out vec3 fragmentPos;
out vec2 fragmentUV;
out vec3 billboardVect;
uniform mat4 viewProjection;
uniform vec3 cameraPos;
uniform mat4 mvpMat;
void main() {
	vec3 Pos = FragPos[0].xyz;

	vec3 toCamera = normalize(cameraPos - Pos);
	vec3 upVec = vec3(0, 1, 0);
	vec3 rightVec = cross(toCamera, upVec);

	Pos -= (rightVec * 0.5);
	gl_Position = viewProjection * vec4(Pos, 1);
	EmitVertex();

	Pos.y += 1;
	gl_Position = viewProjection * vec4(Pos, 1);
	EmitVertex();

	Pos.y -= 1;
	Pos += rightVec;
	gl_Position = viewProjection * vec4(Pos, 1);
	EmitVertex();

	Pos.y += 1;
	gl_Position = viewProjection * vec4(Pos, 1);
	EmitVertex();

	// OLD
	/*gl_Position = mvpMat * gl_in[0].gl_Position + vec4(0, 0, 0, 1);
	fragmentNorm = vert_Normal[0];
	fragmentPos = FragPos[0];
	fragmentUV = fragUVs[0];
	EmitVertex();
	
	gl_Position = mvpMat * gl_in[1].gl_Position + vec4(0, 0, 0, 1);
	fragmentNorm = vert_Normal[1];
	fragmentPos = FragPos[1];
	fragmentUV = fragUVs[1];
	EmitVertex();
	
	gl_Position = mvpMat * gl_in[2].gl_Position + vec4(0, 0, 0, 1);
	fragmentNorm = vert_Normal[2];
	fragmentPos = FragPos[2];
	fragmentUV = fragUVs[2];
	EmitVertex();*/
	
	fragmentNorm = vert_Normal[0];
	fragmentPos = FragPos[0];
	fragmentUV = fragUVs[0];

	EndPrimitive();
}