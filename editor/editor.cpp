#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include "../src/Core/Window.h"
#include "../src/Core/Input.h"
#include "../src/Graphics/Renderer.h"
#include "../src/Graphics/Texture.h"
#include "../src/Graphics/Mesh.h"
#include "../src/Scene/GameObject.h"
#include "../src/Scene/Component.h"
#include "../src/Scripting/CSharpBridge.h"


using json = nlohmann::json;

bool showHierarchy = true;
bool showInspector = true;
bool showConsole = true;
bool showAssetBrowser = true;
bool isPlaying = false;
bool isPaused = false;
bool showProjectSelector = true;
std::string currentProjectPath = "";
std::string consoleBuffer = "";

std::vector<GameObject*> sceneObjects;
int selectedObject = -1;

std::vector<std::string> textures;
std::vector<std::string> meshes;
std::vector<std::string> sounds;
std::vector<std::string> scripts;

void logToConsole(const std::string& msg) {
    consoleBuffer += msg + "\n";
}

void refreshAssets() {
    textures.clear();
    meshes.clear();
    sounds.clear();
    scripts.clear();
    
    std::string assetPath = currentProjectPath + "/assets/";
    std::string scriptPath = currentProjectPath + "/scripts/";
    
    if (std::filesystem::exists(assetPath + "textures")) {
        for (auto& entry : std::filesystem::directory_iterator(assetPath + "textures")) {
            if (entry.path().extension() == ".png" || entry.path().extension() == ".jpg") {
                textures.push_back(entry.path().filename().string());
            }
        }
    }
    
    if (std::filesystem::exists(assetPath + "models")) {
        for (auto& entry : std::filesystem::directory_iterator(assetPath + "models")) {
            if (entry.path().extension() == ".obj") {
                meshes.push_back(entry.path().filename().string());
            }
        }
    }
    
    if (std::filesystem::exists(assetPath + "sounds")) {
        for (auto& entry : std::filesystem::directory_iterator(assetPath + "sounds")) {
            if (entry.path().extension() == ".wav" || entry.path().extension() == ".mp3") {
                sounds.push_back(entry.path().filename().string());
            }
        }
    }
    
    if (std::filesystem::exists(scriptPath)) {
        for (auto& entry : std::filesystem::directory_iterator(scriptPath)) {
            if (entry.path().extension() == ".cs") {
                scripts.push_back(entry.path().filename().string());
            }
        }
    }
}

void loadScene(const std::string& path) {
    if (!std::filesystem::exists(path)) return;
    
    for (auto* obj : sceneObjects) delete obj;
    sceneObjects.clear();
    
    std::ifstream file(path);
    json data;
    file >> data;
    
    for (auto& objData : data["objects"]) {
        GameObject* obj = new GameObject(objData["name"]);
        obj->transform.position.x = objData["x"];
        obj->transform.position.y = objData["y"];
        obj->transform.position.z = objData["z"];
        sceneObjects.push_back(obj);
    }
    
    logToConsole("Loaded scene: " + path);
}

void saveScene(const std::string& path) {
    json data;
    json objects = json::array();
    
    for (auto* obj : sceneObjects) {
        json objData;
        objData["name"] = obj->getName();
        objData["x"] = obj->transform.position.x;
        objData["y"] = obj->transform.position.y;
        objData["z"] = obj->transform.position.z;
        objects.push_back(objData);
    }
    
    data["objects"] = objects;
    
    std::ofstream file(path);
    file << data.dump(4);
    logToConsole("Saved scene: " + path);
}

void drawProjectSelector() {
    ImGui::Begin("Select Project", nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Open existing project:");
    
    if (ImGui::Button("Browse...")) {
        logToConsole("Project selection not fully implemented");
    }
    
    ImGui::Separator();
    ImGui::Text("Or create new project:");
    static char projectName[256] = "";
    static char projectPath[512] = "";
    ImGui::InputText("Name", projectName, 256);
    ImGui::InputText("Path", projectPath, 512);
    if (ImGui::Button("Create")) {
        std::string fullPath = std::string(projectPath) + "/" + projectName;
        std::filesystem::create_directories(fullPath + "/assets/textures");
        std::filesystem::create_directories(fullPath + "/assets/models");
        std::filesystem::create_directories(fullPath + "/assets/sounds");
        std::filesystem::create_directories(fullPath + "/scripts");
        currentProjectPath = fullPath;
        showProjectSelector = false;
        refreshAssets();
        
        std::filesystem::copy("shaders", fullPath + "/shaders", std::filesystem::copy_options::recursive);
        
        json defaultScene;
        defaultScene["objects"] = json::array();
        std::ofstream sceneFile(fullPath + "/scene.scene");
        sceneFile << defaultScene.dump(4);
        
        logToConsole("Created project at: " + fullPath);
    }
    
    ImGui::End();
}

void drawMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene")) {
                for (auto* obj : sceneObjects) delete obj;
                sceneObjects.clear();
                selectedObject = -1;
                logToConsole("New scene created");
            }
            if (ImGui::MenuItem("Save Scene")) {
                saveScene(currentProjectPath + "/scene.scene");
            }
            if (ImGui::MenuItem("Load Scene")) {
                loadScene(currentProjectPath + "/scene.scene");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Build Game")) {
                logToConsole("Building game... (not implemented)");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                exit(0);
            }
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
        if (ImGui::Selectable(sceneObjects[i]->getName().c_str(), selectedObject == i)) {
            selectedObject = i;
        }
    }
    if (ImGui::Button("Create Empty")) {
        sceneObjects.push_back(new GameObject("GameObject"));
    }
    if (ImGui::Button("Delete Selected") && selectedObject >= 0) {
        delete sceneObjects[selectedObject];
        sceneObjects.erase(sceneObjects.begin() + selectedObject);
        selectedObject = -1;
    }
    ImGui::End();
}

void drawInspector() {
    if (!showInspector) return;
    
    ImGui::Begin("Inspector");
    if (selectedObject >= 0 && selectedObject < sceneObjects.size()) {
        GameObject* obj = sceneObjects[selectedObject];
        
        char name[256];
        strcpy(name, obj->getName().c_str());
        if (ImGui::InputText("Name", name, 256)) {
            obj->setName(name);
        }
        
        ImGui::Separator();
        ImGui::Text("Transform");
        ImGui::DragFloat3("Position", &obj->transform->position.x, 0.1f);
        ImGui::DragFloat3("Rotation", &obj->transform->rotation.x, 1.0f);
        ImGui::DragFloat3("Scale", &obj->transform->scale.x, 0.1f);
        
        ImGui::Separator();
        ImGui::Text("Components");
        
        SpriteRenderer* sprite = obj->getComponent<SpriteRenderer>();
        if (sprite) {
            ImGui::Text("Sprite Renderer");
            static char texPath[256];
            strcpy(texPath, sprite->texturePath.c_str());
            if (ImGui::InputText("Texture", texPath, 256)) {
                sprite->setTexture(texPath);
            }
            ImGui::ColorEdit4("Color", &sprite->color.r);
        }
        
        MeshRenderer* mesh = obj->getComponent<MeshRenderer>();
        if (mesh) {
            ImGui::Text("Mesh Renderer");
            static char meshPath[256];
            strcpy(meshPath, mesh->meshPath.c_str());
            if (ImGui::InputText("Mesh", meshPath, 256)) {
                mesh->setMesh(meshPath);
            }
            ImGui::ColorEdit4("Color", &mesh->color.r);
        }
        
        Rigidbody* rb = obj->getComponent<Rigidbody>();
        if (rb) {
            ImGui::Text("Rigidbody");
            ImGui::DragFloat("Mass", &rb->mass, 0.1f);
            ImGui::Checkbox("Use Gravity", &rb->useGravity);
            ImGui::Checkbox("Is Kinematic", &rb->isKinematic);
        }
        
        ScriptComponent* script = obj->getComponent<ScriptComponent>();
        if (script) {
            ImGui::Text("Script");
            static char scriptPath[256];
            strcpy(scriptPath, script->scriptPath.c_str());
            if (ImGui::InputText("Script File", scriptPath, 256)) {
                script->setScript(scriptPath);
            }
        }
        
        if (ImGui::Button("Add Component")) {
            ImGui::OpenPopup("AddComponentPopup");
        }
        if (ImGui::BeginPopup("AddComponentPopup")) {
            if (ImGui::MenuItem("Sprite Renderer")) {
                obj->addComponent<SpriteRenderer>();
            }
            if (ImGui::MenuItem("Mesh Renderer")) {
                obj->addComponent<MeshRenderer>();
            }
            if (ImGui::MenuItem("Rigidbody")) {
                obj->addComponent<Rigidbody>();
            }
            if (ImGui::MenuItem("Script")) {
                obj->addComponent<ScriptComponent>();
            }
            ImGui::EndPopup();
        }
    } else {
        ImGui::Text("No object selected");
    }
    ImGui::End();
}

void drawConsole() {
    if (!showConsole) return;
    
    ImGui::Begin("Console");
    ImGui::TextUnformatted(consoleBuffer.c_str());
    if (ImGui::Button("Clear")) {
        consoleBuffer.clear();
    }
    ImGui::End();
}

void drawAssetBrowser() {
    if (!showAssetBrowser) return;
    
    ImGui::Begin("Asset Browser");
    if (ImGui::CollapsingHeader("Textures")) {
        for (auto& tex : textures) {
            ImGui::Text("%s", tex.c_str());
        }
    }
    if (ImGui::CollapsingHeader("Models")) {
        for (auto& mesh : meshes) {
            ImGui::Text("%s", mesh.c_str());
        }
    }
    if (ImGui::CollapsingHeader("Sounds")) {
        for (auto& sound : sounds) {
            ImGui::Text("%s", sound.c_str());
        }
    }
    if (ImGui::CollapsingHeader("Scripts")) {
        for (auto& script : scripts) {
            ImGui::Text("%s", script.c_str());
        }
    }
    ImGui::End();
}

void drawSceneView() {
    ImGui::Begin("Scene");
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    ImGui::Image((void*)(intptr_t)0, viewportSize);
    
    if (ImGui::IsItemHovered()) {
        if (ImGui::IsMouseClicked(0)) {
            ImVec2 mousePos = ImGui::GetMousePos();
            ImVec2 viewportPos = ImGui::GetCursorScreenPos();
            float mouseX = mousePos.x - viewportPos.x;
            float mouseY = mousePos.y - viewportPos.y;
            
            float closestDist = 10000;
            int closestIndex = -1;
            for (int i = 0; i < sceneObjects.size(); i++) {
                GameObject* obj = sceneObjects[i];
                float dx = obj->transform.position.x - mouseX;
                float dy = obj->transform.position.y - mouseY;
                float dist = sqrt(dx*dx + dy*dy);
                if (dist < closestDist && dist < 50) {
                    closestDist = dist;
                    closestIndex = i;
                }
            }
            selectedObject = closestIndex;
        }
        
        if (selectedObject >= 0) {
            GameObject* obj = sceneObjects[selectedObject];
            ImVec2 center = ImGui::GetCursorScreenPos();
            center.x += obj->transform.position.x;
            center.y += obj->transform.position.y;
            
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddRect(ImVec2(center.x - 32, center.y - 32), 
                              ImVec2(center.x + 32, center.y + 32), 
                              IM_COL32(255, 255, 0, 255));
            
            if (ImGui::IsMouseDragging(0)) {
                ImVec2 delta = ImGui::GetMouseDragDelta(0);
                obj->transform.position.x += delta.x;
                obj->transform.position.y += delta.y;
                ImGui::ResetMouseDragDelta();
            }
        }
    }
    
    ImGui::End();
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    
    SDL_Window* window = SDL_CreateWindow("MeowDot Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    glewInit();
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
    
    Renderer::init();
    CSharpBridge::init();
    
    logToConsole("MeowDot Editor started");
    
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
        
        if (showProjectSelector) {
            drawProjectSelector();
        } else {
            drawMenuBar();
            drawHierarchy();
            drawInspector();
            drawConsole();
            drawAssetBrowser();
            drawSceneView();
            
            if (isPlaying && !isPaused) {
                CSharpBridge::callMethod("onUpdate", 0.016f);
            }
        }
        
        ImGui::Render();
        
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        SDL_GL_SwapWindow(window);
        SDL_Delay(16);
    }
    
    CSharpBridge::shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
