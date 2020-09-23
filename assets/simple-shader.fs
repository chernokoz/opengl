#version 330 core

out vec4 o_frag_color;

struct vx_output_t
{
    vec3 color;
    vec3 position;
};

in vx_output_t v_out;

uniform vec3 u_color;
uniform float u_time;
uniform float zoom;
uniform vec2 delta;
uniform float R_value;
uniform int n_value;
uniform vec2 pos;
uniform vec2 size;

void main()
{
    float a_x = zoom;
    float a_y = zoom;
    float x_center = delta.x + pos.x - pos.x / zoom;
    float y_center = delta.y + pos.y - pos.y / zoom;
    int k = 1;
    float n = n_value;
    float x = x_center + v_out.position.x * a_x;
    float y = y_center + v_out.position.y * a_y;
    float R = R_value;
    float cx = x;
    float cy = y;
    float new_x = 0.0;
    float new_y = 0.0;

    while ( k <= n) {
        if (x * x + y * y > R) break;

        new_x = x * x - y * y + cx;
        new_y = 2 * x * y + cy;
        x = new_x;
        y = new_y;
        k++;
    }

    vec3 color;
    if (k <= n / 4) {
    color = vec3(1.0, 0, 0);
    } else if (k <= n / 4 * 2){
     color = vec3(0, 1.0, 0);
    } else if (k <= n / 4 * 3){
    color = vec3(0, 0, 1.0);
    } else {
    color = vec3(1, 1, 1);
    }

    o_frag_color = vec4(color, 1.0);
}
