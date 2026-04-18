#pragma once
#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>

class Shader {
public:
    static GLuint load(const char* vertexPath, const char* fragmentPath) {
        std::string vertexCode = readFile(vertexPath);
        std::string fragmentCode = readFile(fragmentPath);
        
        GLuint vertex = compileShader(vertexCode.c_str(), GL_VERTEX_SHADER);
        GLuint fragment = compileShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);
        
        GLuint program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        
        return program;
    }
    
    static void use(GLuint program) { glUseProgram(program); }
    
private:
    static std::string readFile(const char* path) {
        std::ifstream file(path);
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }
    
    static GLuint compileShader(const char* source, GLenum type) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        return shader;
    }
};
