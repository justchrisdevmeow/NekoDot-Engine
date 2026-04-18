#pragma once
#include <glm/glm.hpp>
#include <string>
#include <typeindex>
#include <unordered_map>

class GameObject;

class Component {
public:
    Component(GameObject* owner) : owner(owner) {}
    virtual ~Component() {}
    virtual void start() {}
    virtual void update(float dt) {}
    virtual void draw() {}
    virtual json serialize() { return json::object(); }
    virtual void deserialize(const json& data) {}
    
    GameObject* getOwner() { return owner; }
    void destroy() { isDestroyed = true; }
    bool isDestroyed() const { return isDestroyed; }
    
protected:
    GameObject* owner;
    bool isDestroyed = false;
};

class Transform : public Component {
public:
    Transform(GameObject* owner) : Component(owner) {}
    
    glm::vec3 position = glm::vec3(0);
    glm::vec3 rotation = glm::vec3(0);
    glm::vec3 scale = glm::vec3(1);
    
    glm::mat4 getMatrix() {
        glm::mat4 mat = glm::translate(glm::mat4(1), position);
        mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(1,0,0));
        mat = glm::rotate(mat, glm::radians(rotation.y), glm::vec3(0,1,0));
        mat = glm::rotate(mat, glm::radians(rotation.z), glm::vec3(0,0,1));
        mat = glm::scale(mat, scale);
        return mat;
    }
    
    json serialize() override {
        json data;
        data["position"] = {position.x, position.y, position.z};
        data["rotation"] = {rotation.x, rotation.y, rotation.z};
        data["scale"] = {scale.x, scale.y, scale.z};
        return data;
    }
    
    void deserialize(const json& data) override {
        position = glm::vec3(data["position"][0], data["position"][1], data["position"][2]);
        rotation = glm::vec3(data["rotation"][0], data["rotation"][1], data["rotation"][2]);
        scale = glm::vec3(data["scale"][0], data["scale"][1], data["scale"][2]);
    }
};

class SpriteRenderer : public Component {
public:
    SpriteRenderer(GameObject* owner) : Component(owner) {}
    
    std::string texturePath = "";
    GLuint textureID = 0;
    glm::vec4 color = glm::vec4(1);
    
    void setTexture(const std::string& path) {
        texturePath = path;
        textureID = Texture::load(path.c_str());
    }
    
    void draw() override {
        if (textureID) {
            Renderer::drawSprite(textureID, position.x, position.y, 100, 100, rotation.z);
        }
    }
    
    json serialize() override {
        json data;
        data["texture"] = texturePath;
        data["color"] = {color.r, color.g, color.b, color.a};
        return data;
    }
    
    void deserialize(const json& data) override {
        if (data.contains("texture")) setTexture(data["texture"]);
        color = glm::vec4(data["color"][0], data["color"][1], data["color"][2], data["color"][3]);
    }
    
private:
    glm::vec3 position() { return getOwner()->transform.position; }
    float rotation() { return getOwner()->transform.rotation.z; }
};

class MeshRenderer : public Component {
public:
    MeshRenderer(GameObject* owner) : Component(owner) {}
    
    std::string meshPath = "";
    Mesh* mesh = nullptr;
    glm::vec4 color = glm::vec4(1);
    
    void setMesh(const std::string& path) {
        meshPath = path;
        mesh = Mesh::load(path);
    }
    
    void draw() override {
        if (mesh) {
            Renderer::drawMesh(mesh, getOwner()->transform.position, getOwner()->transform.scale, getOwner()->transform.rotation);
        }
    }
    
    json serialize() override {
        json data;
        data["mesh"] = meshPath;
        data["color"] = {color.r, color.g, color.b, color.a};
        return data;
    }
    
    void deserialize(const json& data) override {
        if (data.contains("mesh")) setMesh(data["mesh"]);
        color = glm::vec4(data["color"][0], data["color"][1], data["color"][2], data["color"][3]);
    }
};

class Rigidbody : public Component {
public:
    Rigidbody(GameObject* owner) : Component(owner) {}
    
    glm::vec3 velocity = glm::vec3(0);
    glm::vec3 acceleration = glm::vec3(0);
    float mass = 1.0f;
    bool useGravity = true;
    bool isKinematic = false;
    
    void addForce(const glm::vec3& force) {
        acceleration += force / mass;
    }
    
    void update(float dt) override {
        if (isKinematic) return;
        
        if (useGravity) {
            acceleration.y -= 9.8f;
        }
        
        velocity += acceleration * dt;
        getOwner()->transform.position += velocity * dt;
        acceleration = glm::vec3(0);
    }
    
    json serialize() override {
        json data;
        data["mass"] = mass;
        data["useGravity"] = useGravity;
        data["isKinematic"] = isKinematic;
        return data;
    }
    
    void deserialize(const json& data) override {
        mass = data["mass"];
        useGravity = data["useGravity"];
        isKinematic = data["isKinematic"];
    }
};

class ScriptComponent : public Component {
public:
    ScriptComponent(GameObject* owner) : Component(owner) {}
    
    std::string scriptPath = "";
    
    void setScript(const std::string& path) {
        scriptPath = path;
        CSharpBridge::loadScript(path);
    }
    
    void start() override {
        CSharpBridge::callMethod("onStart");
    }
    
    void update(float dt) override {
        CSharpBridge::callMethod("onUpdate", dt);
    }
    
    json serialize() override {
        json data;
        data["script"] = scriptPath;
        return data;
    }
    
    void deserialize(const json& data) override {
        if (data.contains("script")) setScript(data["script"]);
    }
};
