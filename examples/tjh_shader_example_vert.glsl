#version 330 core

in vec3 pos;
in vec3 col;

out vec3 vCol;

void main()
{
    gl_Position = vec4(pos, 1.0);
    vCol = col;
}
