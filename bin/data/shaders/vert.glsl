#version 150

in vec4 position;
in vec2 texcoord;
in vec4 color;

out vec2 TexCoord;
out vec4 Color;

void main()
{
	Color = color;
	TexCoord = texcoord;
	gl_Position = position;
}
