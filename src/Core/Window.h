#pragma once
#include <SDL2/SDL.h>
#include <string>

class Window {
public:
    static bool create(int width, int height, const std::string& title) {
        SDL_Init(SDL_INIT_VIDEO);
        
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        
        sdlWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
        if (!sdlWindow) return false;
        
        glContext = SDL_GL_CreateContext(sdlWindow);
        if (!glContext) return false;
        
        windowWidth = width;
        windowHeight = height;
        return true;
    }
    
    static void destroy() {
        if (glContext) SDL_GL_DeleteContext(glContext);
        if (sdlWindow) SDL_DestroyWindow(sdlWindow);
        SDL_Quit();
    }
    
    static void swapBuffers() { SDL_GL_SwapWindow(sdlWindow); }
    static SDL_Window* getHandle() { return sdlWindow; }
    static SDL_GLContext getContext() { return glContext; }
    static int getWidth() { return windowWidth; }
    static int getHeight() { return windowHeight; }
    static float getAspect() { return (float)windowWidth / (float)windowHeight; }
    
private:
    static SDL_Window* sdlWindow;
    static SDL_GLContext glContext;
    static int windowWidth, windowHeight;
};

SDL_Window* Window::sdlWindow = nullptr;
SDL_GLContext Window::glContext = nullptr;
int Window::windowWidth = 0;
int Window::windowHeight = 0;
