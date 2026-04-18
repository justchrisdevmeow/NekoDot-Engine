#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D mainTexture;
uniform vec3 materialColor;
uniform float materialShininess;

struct Light {
    int type;
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float range;
    float spotAngle;
};

uniform Light lights[8];
uniform int lightCount;

out vec4 FragColor;

void main() {
    vec4 texColor = texture(mainTexture, TexCoord);
    vec3 diffuseColor = texColor.rgb * materialColor;
    vec3 ambient = diffuseColor * 0.2;
    vec3 result = ambient;
    
    for (int i = 0; i < lightCount; i++) {
        Light light = lights[i];
        vec3 lightDir;
        
        if (light.type == 0) { // Directional
            lightDir = normalize(-light.direction);
        } else { // Point or Spot
            lightDir = normalize(light.position - FragPos);
        }
        
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = diff * diffuseColor * light.color * light.intensity;
        
        // Attenuation
        float distance = length(light.position - FragPos);
        float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.01 * distance * distance);
        if (light.type == 0) attenuation = 1.0;
        
        result += diffuse * attenuation;
    }
    
    FragColor = vec4(result, texColor.a);
}
