/******************************************************************************
 * This is a really simple vertex shader that simply sets the output vertex's
 * position to be the same as the input.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexColor;
layout (location = 2) in vec2 vertexTexCoord;
layout (location = 3) in float partID;
uniform sampler2D hair;
uniform sampler2D hairD;
uniform sampler2D skin;
uniform sampler2D eye;
uniform mat4 matrix;
out vec3 shaderColor;
out vec2 shaderTexCoord;
out float partIDv;

void main()
{
    gl_Position = matrix * vec4(vertexPosition, 1.0f);
    shaderColor = vertexColor;
    shaderTexCoord = vertexTexCoord;
    partIDv = partID;
}