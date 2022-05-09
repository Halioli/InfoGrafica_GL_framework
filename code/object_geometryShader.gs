#version 330 
layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;
in vec4 vert_Normal[];
in vec3 FragPos[];
in vec2 fragUVs[];
in vec3 cameraPos[];
out vec4 fragmentNorm;
out vec3 fragmentPos;
out vec2 fragmentUV;
out vec3 billboardVect;
uniform mat4 mvpMat;
void main() {
	billboardVect = vec3(FragPos[0].x - cameraPos[0].x, FragPos[0].y - cameraPos[0].y, FragPos[0].z - cameraPos[0].z);
	//billboardVect = vec3(cameraPos[0].x - FragPos[0].x, cameraPos[0].y - FragPos[0].y, cameraPos[0].z - FragPos[0].z);
	vec3 dirVec = vec3(0, 1, 0);
	billboardVect = cross(billboardVect, dirVec);

	gl_Position = mvpMat * gl_in[0].gl_Position + vec4(0, 0, 0, 1);
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
	EmitVertex();
	
	EndPrimitive();
}