#version 330 core

out vec4 o_frag_color;

struct vx_output_t
{
    vec3 color;
    vec3 position;
};

in vx_output_t v_out;

uniform float zoom;
uniform vec2 center;
uniform float R_value;
uniform int n_value;
uniform sampler1D tex;

void main()
{
    int k = 1;
    float n = n_value;
    float R = R_value;

    float x = center.x + v_out.position.x * zoom;
    float y = center.y + v_out.position.y * zoom;
    float new_x = 0.0;
    float new_y = 0.0;
    float cx = x;
    float cy = y;

    while (k <= n) {
        if (x * x + y * y > R) break;

        new_x = x * x - y * y + cx;
        new_y = 2 * x * y + cy;
        x = new_x;
        y = new_y;
        k++;
    }

    o_frag_color = texture(tex, 1.0 * k / n);
}
