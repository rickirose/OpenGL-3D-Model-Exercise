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
in vec3 camPosition;
in vec3 lPos;
flat in int s;

uniform float time;
uniform sampler2D hair;
uniform sampler2D hairD;
uniform sampler2D skin;
uniform sampler2D eye;
out vec4 fragmentColor;

void main()
{
    vec4 color;

    vec3 l = normalize(lPos - worldSpacePosition); //light vector
    vec3 c = normalize(camPosition - worldSpacePosition); //camera vector
    vec3 n = normalize(worldSpaceNormal); //normalize again
    vec3 r = reflect(-l, n); //reflection vector

    vec3 CL = vec3(1.0f,1.0f,1.0f);
    vec3 CD = max(dot(n, l), 0.0f)*CL;
    float a = 0.1;
    vec3 CA = a * CL;
    vec3 CS = pow((max(dot(r,c),0.0f)),s) * CL;


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

    fragmentColor = vec4(vec3(CD + CA + CS) * vec3(color.rgb), color.a);
}