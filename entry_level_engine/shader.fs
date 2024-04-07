#version 330 core

in vec3 our_color;

out vec4 out_color;

void main()
{
    out_color = vec4(
        our_color.x,
        our_color.y,
        our_color.z, 
        1.0); 
}