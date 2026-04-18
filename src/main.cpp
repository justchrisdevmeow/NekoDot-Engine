#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include "Core/Window.h"
#include "Core/Input.h"
#include "Core/Time.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Physics/Collision.h"
#include "Audio/Audio.h"
#include "Scene/GameObject.h"
#include "Scene/Component.h"
#include "Scripting/CSharpBridge.h"

int main(int argc, char* argv[]) {
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    
    // Create window
    Window::create(1280, 720, "MeowDot");
    
    // Initialize OpenGL
    glewInit();
    
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(Window::getHandle(), Window::getContext());
    ImGui_ImplOpenGL3_Init();
    
    // Initialize Mono (C# scripting)
    mono_jit_init("MeowDot");
    CSharpBridge::init();
    
    // Load user scripts
    CSharpBridge::loadScript("game/scripts/main.cs");
    
    // Main loop
    bool running = true;
    SDL_Event event;
    
    while (running) {
        Time::update();
        
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) running = false;
            Input::handleEvent(event);
        }
        
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        // Call user C# update
        CSharpBridge::callMethod("onUpdate", Time::deltaTime());
        
        // Render
        Renderer::clear();
        
        // Call user C# draw
        CSharpBridge::callMethod("onDraw");
        
        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        Window::swapBuffers();
    }
    
    // Cleanup
    CSharpBridge::shutdown();
    mono_jit_cleanup();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    Window::destroy();
    SDL_Quit();
    
    return 0;
}
