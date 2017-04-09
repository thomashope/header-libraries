#version 330 core

layout(points) in;
layout(line_strip, max_vertices = 11) out;

in vec3 vCol[];
out vec3 gCol;

const float PI = 3.1415926;

void main()
{

    gCol = vCol[0];

    for (int i = 0; i <= 10; i++) {
        float ang = PI * 2.0 / 10.0 * i;

        vec4 offset = vec4(cos(ang) * 0.3, -sin(ang) * 0.4, 0.0, 0.0);
        gl_Position = gl_in[0].gl_Position + offset;

        EmitVertex();
    }

    EndPrimitive();
}
