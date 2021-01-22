uniform mat4 projection;
uniform mat4 view;
uniform mat4 world;
uniform mat4 worldIT;

in vec4 a_position;
in vec4 a_color;
in vec3 a_normal;

out vec4 v_normal;
out vec4 v_color;
out vec3 v_pos;

void main()
{
    gl_Position = projection * view * world * a_position;
	v_pos = (world * a_position).xyz;
    v_normal = worldIT * vec4(a_normal, 0.0);
    v_color = a_color;
}