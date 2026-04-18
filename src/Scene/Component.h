#pragma once

class GameObject;

class Component {
public:
    Component(GameObject* owner) : owner(owner) {}
    virtual ~Component() {}
    virtual void update(float dt) {}
    virtual void start() {}
    
protected:
    GameObject* owner;
};

struct Transform {
    float x = 0, y = 0, z = 0;
    float scaleX = 1, scaleY = 1, scaleZ = 1;
    float rotation = 0;
};
