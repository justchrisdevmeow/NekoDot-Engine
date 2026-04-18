#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Component.h"

class GameObject {
public:
    GameObject(const std::string& name = "") : name(name), active(true) {}
    
    template<typename T>
    T* addComponent() {
        T* component = new T(this);
        components.push_back(component);
        componentMap[typeid(T).name()] = component;
        return component;
    }
    
    template<typename T>
    T* getComponent() {
        auto it = componentMap.find(typeid(T).name());
        if (it != componentMap.end()) {
            return static_cast<T*>(it->second);
        }
        return nullptr;
    }
    
    void update(float dt) {
        if (!active) return;
        for (auto comp : components) {
            comp->update(dt);
        }
    }
    
    void setActive(bool active) { this->active = active; }
    bool isActive() const { return active; }
    const std::string& getName() const { return name; }
    
private:
    std::string name;
    bool active;
    std::vector<Component*> components;
    std::unordered_map<std::string, Component*> componentMap;
};
