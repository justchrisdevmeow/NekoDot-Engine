#pragma once
#include <glm/glm.hpp>

enum class LightType { Directional, Point, Spot };

struct Light {
    LightType type = LightType::Directional;
    glm::vec3 position = glm::vec3(0, 5, 5);
    glm::vec3 direction = glm::vec3(0, -1, 0);
    glm::vec3 color = glm::vec3(1, 1, 1);
    float intensity = 1.0f;
    float range = 10.0f;
    float spotAngle = 30.0f;
};

class LightManager {
public:
    static void addLight(const Light& light) {
        lights.push_back(light);
    }
    
    static void setLightsUniform(GLuint shader) {
        int count = lights.size();
        glUniform1i(glGetUniformLocation(shader, "lightCount"), count);
        
        for (int i = 0; i < count; i++) {
            std::string prefix = "lights[" + std::to_string(i) + "].";
            glUniform1i(glGetUniformLocation(shader, (prefix + "type").c_str()), (int)lights[i].type);
            glUniform3fv(glGetUniformLocation(shader, (prefix + "position").c_str()), 1, &lights[i].position[0]);
            glUniform3fv(glGetUniformLocation(shader, (prefix + "direction").c_str()), 1, &lights[i].direction[0]);
            glUniform3fv(glGetUniformLocation(shader, (prefix + "color").c_str()), 1, &lights[i].color[0]);
            glUniform1f(glGetUniformLocation(shader, (prefix + "intensity").c_str()), lights[i].intensity);
            glUniform1f(glGetUniformLocation(shader, (prefix + "range").c_str()), lights[i].range);
            glUniform1f(glGetUniformLocation(shader, (prefix + "spotAngle").c_str()), lights[i].spotAngle);
        }
    }
    
private:
    static std::vector<Light> lights;
};
std::vector<Light> LightManager::lights;
