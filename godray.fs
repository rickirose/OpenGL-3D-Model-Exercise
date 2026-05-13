/******************************************************************************
 * This is a really simple fragment shader that simply sets the output fragment
 * color to yellow.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

in vec2 shaderTexCoord;
in vec3 localPosition;

uniform vec3 rayColor;

out vec4 fragmentColor;

void main()
{
    float center = 1.0 - abs(shaderTexCoord.x - 0.5) * 2.0;
    center = pow(center, 2.0);
    float distance = length (localPosition);
    float vertical = smoothstep(4.0, 0.5, abs(localPosition.y));
    float alpha = center * vertical;
    vec3 color = rayColor;
    fragmentColor = vec4(color, alpha * 0.25);
}