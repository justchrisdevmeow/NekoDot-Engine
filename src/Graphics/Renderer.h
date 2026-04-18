#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "Mesh.h"
#include "Light.h"
#include "Material.h"
#include <vector>

class Renderer {
public:
    static void init() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Default shader paths (will be set by user)
        currentShader = 0;
    }
    
    static void clear() {
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    static void setClearColor(float r, float g, float b, float a) {
        clearColor = {r, g, b, a};
    }
    
    static void setShader(GLuint shader) {
        currentShader = shader;
        glUseProgram(shader);
        
        // Set camera uniforms
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &Camera::getViewMatrix()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &Camera::getProjectionMatrix()[0][0]);
        
        // Set lights
        LightManager::setLightsUniform(shader);
    }
    
    static GLuint getCurrentShader() { return currentShader; }
    
    // 2D SPRITE RENDERING
    static void drawSprite(GLuint texture, float x, float y, float width, float height, float rotation = 0) {
        if (!currentShader) return;
        
        // Orthographic projection for 2D
        glm::mat4 ortho = glm::ortho(0.0f, (float)Window::getWidth(), (float)Window::getHeight(), 0.0f, -1.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(currentShader, "projection"), 1, GL_FALSE, &ortho[0][0]);
        
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x + width/2, y + height/2, 0));
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 0, 1));
        model = glm::scale(model, glm::vec3(width, height, 1));
        glUniformMatrix4fv(glGetUniformLocation(currentShader, "model"), 1, GL_FALSE, &model[0][0]);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(currentShader, "mainTexture"), 0);
        
        // Draw quad
        static GLuint quadVAO = 0, quadVBO;
        if (!quadVAO) {
            float vertices[] = {
                -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
                -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
                 0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
                 0.5f,  0.5f, 0.0f, 1.0f, 0.0f
            };
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }
        
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // Restore perspective projection for 3D
        glUniformMatrix4fv(glGetUniformLocation(currentShader, "projection"), 1, GL_FALSE, &Camera::getProjectionMatrix()[0][0]);
    }
    
    // 3D MESH RENDERING
    static void drawMesh(Mesh* mesh, const glm::vec3& position, const glm::vec3& scale = glm::vec3(1), const glm::vec3& rotation = glm::vec3(0)) {
        if (!currentShader || !mesh) return;
        
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        model = glm::scale(model, scale);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        
        glUniformMatrix4fv(glGetUniformLocation(currentShader, "model"), 1, GL_FALSE, &model[0][0]);
        
        mesh->draw();
    }
    
    static void setViewport(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
    }
    
private:
    static GLuint currentShader;
    static struct { float r, g, b, a; } clearColor;
};

GLuint Renderer::currentShader = 0;
decltype(Renderer::clearColor) Renderer::clearColor = {0.2f, 0.2f, 0.3f, 1.0f};
