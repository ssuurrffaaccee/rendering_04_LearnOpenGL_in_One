#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
// layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

mat3 cal_TBN(vec3 Normal,vec3 Tangent){
    vec3 N = normalize(Normal);
    vec3 T = normalize(Tangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    return mat3(T, B, N); 
}
void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz; 
    TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 Normal = normalMatrix * aNormal;
    vec3 Tangent = normalMatrix * aTangent;
    TBN = cal_TBN(Normal,Tangent);
    gl_Position = projection * view * worldPos;
}