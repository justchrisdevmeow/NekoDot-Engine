#pragma once
#include <SDL2/SDL.h>
#include <unordered_map>

class Input {
public:
    static void handleEvent(SDL_Event& event) {
        if (event.type == SDL_KEYDOWN) {
            currentKeys[event.key.keysym.scancode] = true;
        }
        else if (event.type == SDL_KEYUP) {
            currentKeys[event.key.keysym.scancode] = false;
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            currentButtons[event.button.button] = true;
        }
        else if (event.type == SDL_MOUSEBUTTONUP) {
            currentButtons[event.button.button] = false;
        }
        else if (event.type == SDL_MOUSEMOTION) {
            mouseX = event.motion.x;
            mouseY = event.motion.y;
        }
    }
    
    static void update() {
        previousKeys = currentKeys;
        previousButtons = currentButtons;
    }
    
    static bool getKey(int keycode) { return currentKeys[keycode]; }
    static bool getKeyDown(int keycode) { return currentKeys[keycode] && !previousKeys[keycode]; }
    static bool getKeyUp(int keycode) { return !currentKeys[keycode] && previousKeys[keycode]; }
    static bool getMouseButton(int button) { return currentButtons[button]; }
    static bool getMouseButtonDown(int button) { return currentButtons[button] && !previousButtons[button]; }
    static float getMouseX() { return mouseX; }
    static float getMouseY() { return mouseY; }
    
private:
    static std::unordered_map<int, bool> currentKeys;
    static std::unordered_map<int, bool> previousKeys;
    static std::unordered_map<int, bool> currentButtons;
    static std::unordered_map<int, bool> previousButtons;
    static float mouseX, mouseY;
};

std::unordered_map<int, bool> Input::currentKeys;
std::unordered_map<int, bool> Input::previousKeys;
std::unordered_map<int, bool> Input::currentButtons;
std::unordered_map<int, bool> Input::previousButtons;
float Input::mouseX = 0;
float Input::mouseY = 0;
