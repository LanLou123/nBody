#version 430 core
out vec4 FragColor;


in vec3 vel_;
in float m;

uniform vec3 viewPos;

void main()
{             
    
    FragColor = vec4(m * abs(vel_)/100.0, 1.0);
}
