/******************************************************************************
 * This is a really simple vertex shader that simply sets the output vertex's
 * position to be the same as the input.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTexCoord;

uniform mat4 projectionViewMatrix;
uniform mat4 modelMatrix;

out vec2 shaderTexCoord;
out vec3 localPosition;


void main()
{
    vec4 worldSpacePosition = modelMatrix * vec4(vertexPosition, 1.0f);
    gl_Position = projectionViewMatrix * worldSpacePosition;

   shaderTexCoord = vertexTexCoord;
   localPosition = vertexPosition;
}