#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraType { Orthographic, Perspective };

class Camera {
public:
    static void setOrthographic(float left, float right, float bottom, float top, float near, float far) {
        type = CameraType::Orthographic;
        projection = glm::ortho(left, right, bottom, top, near, far);
    }
    
    static void setPerspective(float fov, float aspect, float near, float far) {
        type = CameraType::Perspective;
        projection = glm::perspective(glm::radians(fov), aspect, near, far);
        this->fov = fov;
        this->near = near;
        this->far = far;
    }
    
    static void setPosition(float x, float y, float z) {
        position = glm::vec3(x, y, z);
        updateView();
    }
    
    static void setTarget(float x, float y, float z) {
        target = glm::vec3(x, y, z);
        updateView();
    }
    
    static glm::mat4 getViewMatrix() { return view; }
    static glm::mat4 getProjectionMatrix() { return projection; }
    static CameraType getType() { return type; }
    
private:
    static void updateView() {
        view = glm::lookAt(position, target, glm::vec3(0, 1, 0));
    }
    
    static CameraType type;
    static glm::mat4 projection;
    static glm::mat4 view;
    static glm::vec3 position;
    static glm::vec3 target;
    static float fov, near, far;
};

CameraType Camera::type = CameraType::Perspective;
glm::mat4 Camera::projection = glm::mat4(1.0f);
glm::mat4 Camera::view = glm::mat4(1.0f);
glm::vec3 Camera::position = glm::vec3(0, 0, 10);
glm::vec3 Camera::target = glm::vec3(0, 0, 0);
float Camera::fov = 60.0f;
float Camera::near = 0.1f;
float Camera::far = 1000.0f;
