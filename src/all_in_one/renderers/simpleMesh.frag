#version 330
in vec2 fragTexCoord;
out vec4 outColor;
uniform vec3 baseColor;
void main()
{
    outColor = vec4(baseColor,1.0);
}