#version 330 core

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

out vec4 FragColor;
in vec2 TexCoords;

struct Gbuffer{
   sampler2D gPosition;
   sampler2D gNormal;
   sampler2D gAlbedoSpec;
};
struct MaterialConfig{
   float shininess;
};
uniform MaterialConfig materailConfig;
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform Gbuffer gbuffer;
uniform float exposure;
uniform float gamma;
// uniform mat4 lightSpaceMatrix;
// uniform sampler2D shadowMap;
// float directeLightShadowCalculation(vec4 fragPosLightSpace)
// {
//     // perform perspective divide
//     vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//     // transform to [0,1] range
//     projCoords = projCoords * 0.5 + 0.5;
//     // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
//     float closestDepth = texture(shadowMap, projCoords.xy).r; 
//     // get depth of current fragment from light's perspective
//     float currentDepth = projCoords.z;
//     // check whether current frag pos is in shadow
//     float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

//     return shadow;
// }
// vec3 fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
// float shadow = directeLightShadowCalculation(fragPosLightSpace);  
// vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color; 

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 material_diffuse, float material_specular, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos,vec3 material_diffuse, float material_specular, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos,vec3 material_diffuse, float material_specular, vec3 viewDir);

void main()
{             
    // retrieve data from gbuffer
    vec3 fragPos = texture(gbuffer.gPosition, TexCoords).rgb;
    vec3 normal = texture(gbuffer.gNormal, TexCoords).rgb;
    vec3 diffuse = texture(gbuffer.gAlbedoSpec, TexCoords).rgb;
    float specular = texture(gbuffer.gAlbedoSpec, TexCoords).a;
    vec3 viewDir  = normalize(viewPos - fragPos);
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    vec3 lighting = CalcDirLight(dirLight,normal,diffuse,specular,viewDir);
    // phase 2: point lights
    lighting += CalcPointLight(pointLight, normal, fragPos, diffuse, specular, viewDir);
    // phase 3: spot light
    lighting += CalcSpotLight(spotLight, normal, fragPos, diffuse, specular, viewDir);
    // hdr 
    // reinhard
    // vec3 result = hdrColor / (hdrColor + vec3(1.0));
    // exposure
    vec3 result = vec3(1.0) - exp(-lighting * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 material_diffuse, float material_specular, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materailConfig.shininess);
    // combine results
    vec3 ambient = light.ambient * material_diffuse;
    vec3 diffuse = light.diffuse * diff * material_diffuse;
    vec3 specular = light.specular * spec * material_specular;
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos,vec3 material_diffuse, float material_specular, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materailConfig.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * material_diffuse;
    vec3 diffuse = light.diffuse * diff * material_diffuse;
    vec3 specular = light.specular * spec * material_specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos,vec3 material_diffuse, float material_specular, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materailConfig.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * material_diffuse;
    vec3 diffuse = light.diffuse * diff * material_diffuse;
    vec3 specular = light.specular * spec * material_specular;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}