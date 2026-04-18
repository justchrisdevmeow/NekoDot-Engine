#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "../src/Core/Window.h"
#include "../src/Core/Input.h"
#include "../src/Graphics/Renderer.h"
#include "../src/Scripting/CSharpBridge.h"

// Editor state
bool showHierarchy = true;
bool showInspector = true;
bool showConsole = true;
bool showAssetBrowser = true;
bool isPlaying = false;
bool isPaused = false;

std::vector<std::string> sceneObjects = {"Player", "Enemy", "Ground"};
int selectedObject = -1;

void drawMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene")) {}
            if (ImGui::MenuItem("Save Scene")) {}
            if (ImGui::MenuItem("Build")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Game")) {
            if (ImGui::MenuItem("Play", nullptr, &isPlaying)) {
                if (isPlaying) {
                    CSharpBridge::callMethod("onStart");
                }
            }
            if (ImGui::MenuItem("Pause", nullptr, &isPaused)) {}
            if (ImGui::MenuItem("Stop")) {
                isPlaying = false;
                isPaused = false;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Hierarchy", nullptr, &showHierarchy);
            ImGui::MenuItem("Inspector", nullptr, &showInspector);
            ImGui::MenuItem("Console", nullptr, &showConsole);
            ImGui::MenuItem("Asset Browser", nullptr, &showAssetBrowser);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void drawHierarchy() {
    if (!showHierarchy) return;
    
    ImGui::Begin("Hierarchy");
    for (int i = 0; i < sceneObjects.size(); i++) {
        if (ImGui::Selectable(sceneObjects[i].c_str(), selectedObject == i)) {
            selectedObject = i;
        }
    }
    ImGui::End();
}

void drawInspector() {
    if (!showInspector) return;
    
    ImGui::Begin("Inspector");
    if (selectedObject >= 0 && selectedObject < sceneObjects.size()) {
        ImGui::Text("Name: %s", sceneObjects[selectedObject].c_str());
        ImGui::Separator();
        ImGui::Text("Transform");
        float pos[3] = {0, 0, 0};
        ImGui::DragFloat3("Position", pos, 0.1f);
        float rot[3] = {0, 0, 0};
        ImGui::DragFloat3("Rotation", rot, 1.0f);
        float scale[3] = {1, 1, 1};
        ImGui::DragFloat3("Scale", scale, 0.1f);
    } else {
        ImGui::Text("No object selected");
    }
    ImGui::End();
}

void drawConsole() {
    if (!showConsole) return;
    
    ImGui::Begin("Console");
    ImGui::TextWrapped("Welcome to MeowDot Editor");
    ImGui::TextWrapped("[Info] Engine initialized");
    ImGui::End();
}

void drawAssetBrowser() {
    if (!showAssetBrowser) return;
    
    ImGui::Begin("Asset Browser");
    ImGui::Text("Textures:");
    if (ImGui::Button("Import Texture")) {}
    ImGui::Text("cat.png");
    ImGui::Text("bg.png");
    ImGui::Separator();
    ImGui::Text("Scripts:");
    if (ImGui::Button("New C# Script")) {}
    ImGui::Text("Player.cs");
    ImGui::Text("Enemy.cs");
    ImGui::End();
}

void drawSceneView() {
    ImGui::Begin("Scene");
    
    // Scene viewport (render texture would go here)
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    ImGui::Image((void*)(intptr_t)0, viewportSize);
    
    if (ImGui::IsItemHovered()) {
        // Handle scene interaction
    }
    
    ImGui::End();
}

int main(int argc, char* argv[]) {
    // Initialize SDL and OpenGL
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    
    SDL_Window* window = SDL_CreateWindow("MeowDot Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    glewInit();
    
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init();
    
    // Set ImGui style
    ImGui::StyleColorsDark();
    
    // Initialize engine core
    CSharpBridge::init();
    
    // Main loop
    bool running = true;
    SDL_Event event;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) running = false;
        }
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        drawMenuBar();
        drawHierarchy();
        drawInspector();
        drawConsole();
        drawAssetBrowser();
        drawSceneView();
        
        ImGui::Render();
        
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        SDL_GL_SwapWindow(window);
        SDL_Delay(16);
    }
    
    // Cleanup
    CSharpBridge::shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
