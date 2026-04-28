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
in vec3 worldSpacePosition;
in vec3 worldSpaceNormal;

uniform float time;
uniform sampler2D hair;
uniform sampler2D hairD;
uniform sampler2D skin;
uniform sampler2D eye;
out vec4 fragmentColor;

void main()
{
    vec4 color;
    vec3 lightPosition = vec3(2.0f, 2.0f, 0.0f);

    vec3 l = normalize(lightPosition - worldSpacePosition); //light vector
    vec3 n = normalize(worldSpaceNormal); //normalize again

    float CD = max(dot(n, l), 0.0f);
    float CA = 0.3f;


    if (partIDv == 0.0f)
    {
        color = texture(hair, shaderTexCoord);
        vec2 displacement = vec2(shaderTexCoord.x+ color.r * sin(time * 0.3), shaderTexCoord.y + color.r *  sin(time * 0.3));
        color = texture(hair, displacement);
    }
    else if (partIDv == 1.0f)
    {
        color = texture(skin, shaderTexCoord);
    }
    else if (partIDv == 2.0f)
    {
        color = texture(eye, shaderTexCoord);
    }

    fragmentColor = vec4((CD + CA) * color.rgb, color.a);
}