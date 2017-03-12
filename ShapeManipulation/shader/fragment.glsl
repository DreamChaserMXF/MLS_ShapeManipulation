#version 330 core
 
uniform bool enableTexture;
uniform vec4 myColor;
uniform sampler2D cur_texture;

in vec2 vertex_tex_coord;

out vec4 color;

void main()
{
	if(enableTexture)
	{
		color = texture(cur_texture, vertex_tex_coord);
	}
	else
	{
		color = myColor;
	}
	//color = texture(cur_texture, vertex_tex_coord);
}