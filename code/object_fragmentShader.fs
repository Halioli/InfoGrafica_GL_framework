#version 330 
in vec2 TexCoord;
out vec4 out_Color;
uniform sampler2D diffuseTexture;

void main() {
	out_Color = texture(diffuseTexture, TexCoord);
	
	// Remove alpha color
	if (out_Color.a < 0.9)
	{
		discard;
	}
}