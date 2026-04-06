/******************************************************************************
 * This is a really simple fragment shader that simply sets the output fragment
 * color to yellow.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

in float partIDv;
in vec2 shaderTexCoord;
in vec3 shaderColor;
uniform float time;
uniform sampler2D hair;
uniform sampler2D hairD;
uniform sampler2D skin;
uniform sampler2D eye;
out vec4 fragmentColor;

void main()
{
    vec4 color;

    if (partIDv == 0.0f)
    {
        color = texture(hair, shaderTexCoord);
        vec2 displacement = vec2(shaderTexCoord.x+ color.r * sin(time * 0.3), shaderTexCoord.y + color.r *  sin(time * 0.3));
        color = texture(hair, displacement);
    }
    else if (partIDv == 1.0f)
    {
        color = vec4(shaderColor, 1.0) * texture(skin, shaderTexCoord);
    }
    else if (partIDv == 2.0f)
    {
        color = texture(eye, shaderTexCoord);
    }

    fragmentColor = color;
}