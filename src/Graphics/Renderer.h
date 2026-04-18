#pragma once
#include <GL/glew.h>
#include <vector>

class Renderer {
public:
    static void clear() {
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    static void setClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }
    
    static void enableDepthTest() { glEnable(GL_DEPTH_TEST); }
    static void disableDepthTest() { glDisable(GL_DEPTH_TEST); }
    
    static void setViewport(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
    }
};
