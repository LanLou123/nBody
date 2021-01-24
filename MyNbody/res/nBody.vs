#version 430 core

layout (location = 0) in vec3 qPos;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec4 aPos;
layout (location = 3) in vec4 vel;


out vec3 FragPos;
out vec3 vel_;
out float m;
out float m_pow;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 camera_axes;


void main()
{
    vel_ = vel.xyz;
    TexCoords = texCoords;
    m = aPos.w;
    m = min(1.0, m);
    m_pow = vel.w;
    float size = 1.0;
    vec3 quad_pos = size * pow(m, 3.0f) * qPos.x * camera_axes[0] + size * pow(m, 3.0f) * qPos.y * camera_axes[1];
    gl_Position = projection * view * model * vec4(quad_pos + aPos.xyz, 1.0);
}