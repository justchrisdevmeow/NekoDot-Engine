#pragma once
#include <cmath>

struct Vec2 {
    float x, y;
    Vec2(float x = 0, float y = 0) : x(x), y(y) {}
};

struct Rect {
    float x, y, w, h;
    Rect(float x = 0, float y = 0, float w = 0, float h = 0) : x(x), y(y), w(w), h(h) {}
    
    bool intersects(const Rect& other) const {
        return x < other.x + other.w &&
               x + w > other.x &&
               y < other.y + other.h &&
               y + h > other.y;
    }
};

struct Circle {
    float x, y, r;
    Circle(float x = 0, float y = 0, float r = 0) : x(x), y(y), r(r) {}
    
    bool intersects(const Circle& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        float dist = sqrt(dx*dx + dy*dy);
        return dist < r + other.r;
    }
    
    bool intersects(const Rect& rect) const {
        float closestX = std::max(rect.x, std::min(x, rect.x + rect.w));
        float closestY = std::max(rect.y, std::min(y, rect.y + rect.h));
        float dx = x - closestX;
        float dy = y - closestY;
        return (dx*dx + dy*dy) < r*r;
    }
};

class Collision {
public:
    static bool rectRect(const Rect& a, const Rect& b) { return a.intersects(b); }
    static bool circleCircle(const Circle& a, const Circle& b) { return a.intersects(b); }
    static bool circleRect(const Circle& c, const Rect& r) { return c.intersects(r); }
};
