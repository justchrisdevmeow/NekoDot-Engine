#pragma once
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string>

class CSharpBridge {
public:
    static void init() {
        domain = mono_jit_init("MeowDot");
        if (!domain) return;
        
        // Load user assembly
        assembly = mono_domain_assembly_open(domain, "scripts/main.dll");
        if (!assembly) return;
        
        image = mono_assembly_get_image(assembly);
    }
    
    static void loadScript(const std::string& path) {
        // Compile C# to DLL at runtime
        // Or load pre-compiled DLL
    }
    
    static void callMethod(const std::string& methodName, float param = 0) {
        if (!image) return;
        
        MonoClass* gameClass = mono_class_from_name(image, "", "Game");
        if (!gameClass) return;
        
        MonoMethod* method = mono_class_get_method_from_name(gameClass, methodName.c_str(), param != 0 ? 1 : 0);
        if (!method) return;
        
        if (param != 0) {
            void* args[1] = { &param };
            mono_runtime_invoke(method, nullptr, args, nullptr);
        } else {
            mono_runtime_invoke(method, nullptr, nullptr, nullptr);
        }
    }
    
    static void shutdown() {
        if (domain) mono_jit_cleanup(domain);
    }
    
private:
    static MonoDomain* domain;
    static MonoAssembly* assembly;
    static MonoImage* image;
};

MonoDomain* CSharpBridge::domain = nullptr;
MonoAssembly* CSharpBridge::assembly = nullptr;
MonoImage* CSharpBridge::image = nullptr;
