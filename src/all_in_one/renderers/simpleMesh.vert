#version 330
uniform mat4 WorldTransform;
uniform mat4 ViewTransform;
uniform mat4 ProjectionTransform;
layout(location = 0) in vec3 inPosition;
out vec2 fragTexCoord;
void main()
{
	vec4 pos = vec4(inPosition, 1.0);
	gl_Position = ProjectionTransform * ViewTransform * WorldTransform * pos;
}