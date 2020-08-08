#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D color_tex;
uniform sampler2D blur_tex;

void main()
{             
    vec4 col = texture(color_tex, TexCoords);
    vec4 brightness = texture(blur_tex, TexCoords);

    vec3 col_out = col.rgb;
    col_out += brightness.rgb;
    col_out /= 2.0;

    FragColor = vec4(col_out, 1.0);
}
