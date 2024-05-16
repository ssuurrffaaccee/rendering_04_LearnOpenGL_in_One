#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform float nearPlane;
uniform float farPlane;
uniform int isPerspective;


uniform sampler2D sprite;



// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));	
}

void main()
{             
    float depthValue = texture(sprite, TexCoords).r;
    if(isPerspective==1){
       FragColor = vec4(vec3(LinearizeDepth(depthValue) / farPlane), 1.0); // perspective
    }else{
       FragColor = vec4(vec3(depthValue), 1.0); // orthographic
    }
}