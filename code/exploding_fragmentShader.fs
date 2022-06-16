#version 330 
layout(pixel_center_integer) in vec4 gl_FragCoord;
in vec4 fragmentNorm;
in vec2 fragmentUV;
out vec4 out_Color;
uniform vec3 lightPos;
uniform vec4 color;
uniform sampler2D diffuseTexture;

void main() {
	out_Color = texture(diffuseTexture, fragmentUV);
	
	// Remove alpha color
	if (out_Color.a < 0.9)
	{
		discard;
	}
}