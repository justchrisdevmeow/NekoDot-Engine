#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Component.h"

class GameObject {
public:
    GameObject(const std::string& name = "") : name(name) {
        transform = addComponent<Transform>();
    }
    
    ~GameObject() {
        for (auto* comp : components) delete comp;
    }
    
    template<typename T>
    T* addComponent() {
        T* comp = new T(this);
        components.push_back(comp);
        componentMap[std::type_index(typeid(T))] = comp;
        return comp;
    }
    
    template<typename T>
    T* getComponent() {
        auto it = componentMap.find(std::type_index(typeid(T)));
        if (it != componentMap.end()) {
            return static_cast<T*>(it->second);
        }
        return nullptr;
    }
    
    void start() {
        for (auto* comp : components) comp->start();
    }
    
    void update(float dt) {
        if (!active) return;
        for (auto* comp : components) {
            if (!comp->isDestroyed()) comp->update(dt);
        }
    }
    
    void draw() {
        if (!active) return;
        for (auto* comp : components) comp->draw();
    }
    
    void setName(const std::string& n) { name = n; }
    std::string getName() const { return name; }
    void setActive(bool a) { active = a; }
    bool isActive() const { return active; }
    
    Transform* transform = nullptr;
    
private:
    std::string name;
    bool active = true;
    std::vector<Component*> components;
    std::unordered_map<std::type_index, Component*> componentMap;
};
