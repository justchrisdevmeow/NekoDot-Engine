#pragma once
#include <SDL2/SDL.h>

class Time {
public:
    static void update() {
        currentTime = SDL_GetTicks() / 1000.0f;
        delta = currentTime - lastTime;
        if (delta > 0.033f) delta = 0.033f;
        lastTime = currentTime;
    }
    
    static float deltaTime() { return delta; }
    static float time() { return currentTime; }
    
private:
    static float delta;
    static float currentTime;
    static float lastTime;
};

float Time::delta = 0;
float Time::currentTime = 0;
float Time::lastTime = 0;
