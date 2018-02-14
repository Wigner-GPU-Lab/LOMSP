#version 400

in vec4 normal_pos;
in vec4 pos;
out vec4 frag_colour;

void main()
{
	vec4 light_pos = vec4(5.0, 5.0, 5.0, 1.0);
	float d = dot( normal_pos - pos, normalize(light_pos - pos));
	vec3 col = vec3(0.1, 0.1, 0.2) + 0.8* d * vec3(1.0f, 0.9f, 0.7f);
	frag_colour = vec4(col, 1.0);
}
