#version 430 core
//out vec4 FragColor;
layout (location = 0) out vec4 Color;
layout (location = 1) out vec4 BrightColor;


in vec3 vel_;
in float m;
in vec2 TexCoords;

uniform vec3 viewPos;

void main()
{             
    float star_f = length(vel_);

    float l2c = distance(vec2(0.5, 0.5), TexCoords);
    l2c = 0.7f - l2c;

    vec3 star_col_0 = vec3(1, 0, 0);
    vec3 star_col_1 = vec3(1, 0.2, 0);
    vec3 star_col_2 = vec3(1, 0.4, 0);
    vec3 star_col_3 = vec3(1, 0.6, 0);
    vec3 star_col_4 = vec3(1, 0.8, 0);
    vec3 star_col_5 = vec3(1, 1, 0.4);
    vec3 star_col_6 = vec3(1, 1, 1);
    vec3 star_col_7 = vec3(0.8, 1, 1);
    vec3 star_col_8 = vec3(0.7, 1, 1);
    vec3 star_col_9 = vec3(0.6, 0.8, 1);
    vec3 star_col_10 = vec3(0.5, 0.6, 1);

    float mass = (2.0f * m - 1.0f); //0 - 1
    

    vec3 _star_col = vec3(0);

    if(mass > 0.0f && mass <=0.1f){
        _star_col = star_col_0;
	}else if(mass > 0.1f && mass <=0.2f){
        _star_col = star_col_1;
	}else if(mass > 0.1f && mass <=0.2f){
        _star_col = star_col_2;
	}else if(mass > 0.2f && mass <=0.3f){
        _star_col = star_col_3;
	}else if(mass > 0.3f && mass <=0.4f){
        _star_col = star_col_4;
	}else if(mass > 0.4f && mass <=0.5f){
        _star_col = star_col_5;
	}else if(mass > 0.5f && mass <=0.6f){
        _star_col = star_col_6;
	}else if(mass > 0.6f && mass <=0.7f){
        _star_col = star_col_7;
	}else if(mass > 0.7f && mass <=0.8f){
        _star_col = star_col_8;
	}else if(mass > 0.8f && mass <=0.9f){
        _star_col = star_col_9;
	}else if(mass > 0.9f && mass <=1.0f){
        _star_col = star_col_10;
	}

    Color = vec4(10.0f * l2c *(mass + 1.0f) /2.0  * _star_col * 1.2, l2c);

    float brightness = dot(Color.rgb, vec3(0.2126, 0.7152, 0.0722));

    if(brightness > 1.0)
        BrightColor = vec4(Color.rgb/1.1, 1.0);
    else
        BrightColor = vec4(0.0,0.0,0.0,1.0);
}
