/******************************************************************************
 * This is a really simple fragment shader that simply sets the output fragment
 * color to yellow.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

flat in int triangleID;
in vec2 shaderTexCoord;
in vec3 shaderColor;
uniform sampler2D hair;
uniform sampler2D hairD;
uniform sampler2D skin;
uniform sampler2D eye;
out vec4 fragmentColor;

void main()
{
    vec4 color;

    if (triangleID < 33)
    {
        color = texture(hair, shaderTexCoord);
    }
    else if (triangleID < 39)
    {
        color = texture(skin, shaderTexCoord);
    }
    else
    {
        color = texture(eye, shaderTexCoord);
    }

    fragmentColor = color;

    // fragmentColor = vec4(shaderColor, 1.0f);
}