#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class Material {
public:
    void setShader(GLuint shader) { this->shader = shader; }
    void setTexture(GLuint texture, const std::string& uniformName) {
        textures.push_back({texture, uniformName});
    }
    void setColor(const glm::vec3& color) { this->color = color; }
    void setShininess(float shininess) { this->shininess = shininess; }
    
    void bind() {
        glUseProgram(shader);
        
        for (int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures[i].first);
            glUniform1i(glGetUniformLocation(shader, textures[i].second.c_str()), i);
        }
        
        glUniform3fv(glGetUniformLocation(shader, "material.color"), 1, &color[0]);
        glUniform1f(glGetUniformLocation(shader, "material.shininess"), shininess);
    }
    
private:
    GLuint shader = 0;
    std::vector<std::pair<GLuint, std::string>> textures;
    glm::vec3 color = glm::vec3(1, 1, 1);
    float shininess = 32.0f;
};
