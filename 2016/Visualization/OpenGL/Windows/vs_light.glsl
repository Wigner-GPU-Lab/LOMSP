#version 400

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;

uniform mat4 w, vp;

out vec4 pos, normal_pos;
void main()
{
	pos = vec4(vertex_position, 1.0) * w;
	gl_Position = pos * vp;
	normal_pos = vec4(vertex_position + vertex_normal, 1.0) * w;
}
