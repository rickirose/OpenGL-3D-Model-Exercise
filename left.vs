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
layout (location = 3) in float vertexPartID;
layout (location = 4) in vec3 vertexNormal;

uniform sampler2D hair;
uniform sampler2D hairD;
uniform sampler2D skin;
uniform sampler2D eye;

uniform mat4 projectionViewMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;

uniform vec3 eyePosition;
uniform vec3 lightPosition;
uniform int shine;

out vec3 camPosition;
out vec3 lPos;
flat out int s;
out vec3 worldSpacePosition;
out vec3 worldSpaceNormal;

out vec3 shaderColor;
out vec2 shaderTexCoord;
out float partIDv;



void main()
{
    gl_Position = projectionViewMatrix * modelMatrix * vec4(vertexPosition, 1.0f);

    worldSpacePosition = normalize((modelMatrix * vec4(vertexPosition, 1.0f))).xyz;
    worldSpaceNormal = normalize((normalMatrix * vec4(vertexNormal, 0.0f))).xyz;
    shaderColor = vertexColor;
    shaderTexCoord = vertexTexCoord;
    partIDv = vertexPartID;
    camPosition = eyePosition;
    lPos = lightPosition;
    s = shine;
}