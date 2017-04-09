#version 330 core

in vec3 gCol;

out vec4 outDiffuse;

void main()
{
   outDiffuse = vec4(gCol,1);
}
