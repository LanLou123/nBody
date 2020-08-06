#version 430 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 vel;

out vec3 FragPos;
out vec3 vel_;
out float m;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    vel_ = vel;
    m = aPos.w;
    gl_Position = projection * view * model * vec4(aPos.xyz, 1.0);
}