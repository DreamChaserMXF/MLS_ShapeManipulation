#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex_coord;

out vec2 vertex_tex_coord;


uniform vec2 dataScalingFactor;
uniform vec2 windowScalingFactor;
uniform vec2 zoomScalingFactor;

void main()
{
	gl_Position = vec4(position * dataScalingFactor * windowScalingFactor * zoomScalingFactor, 0.0, 1.0);
	vertex_tex_coord = tex_coord;
}