#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_color;

struct vx_output_t
{
    vec3 color;
    vec3 position;
};
out vx_output_t v_out;

uniform vec2 u_translation;
uniform mat4 u_mvp;

void main()
{

    v_out.color = in_color;
    v_out.position = in_position;


    gl_Position = u_mvp * vec4(3 * in_position.x, 2 * in_position.y, in_position.z, 1.0);
}
