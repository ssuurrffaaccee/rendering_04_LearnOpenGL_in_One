#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
// This is incorrect, in order to keep the basis vectors 
// perpendicular to each other, it is necessary to re orthogonalize
// Linear interpolation cannot guarantee the length of the vectors, 
// and interpolation needs to be performed on the sphere
in mat3 TBN;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;

uniform vec2 uv_scale;

void main()
{    
    vec2 scaledTexCoorrds = TexCoords*uv_scale;
    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    //gNormal = normalize(texture(texture_normal, scaledTexCoorrds).rgb * 2.0 - 1.0);
    gNormal = normalize(TBN * (texture(texture_normal, scaledTexCoorrds).rgb * 2.0 - 1.0));
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(texture_diffuse, scaledTexCoorrds).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(texture_specular, scaledTexCoorrds).r;
}