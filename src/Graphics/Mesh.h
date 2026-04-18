#pragma once
#include <GL/glew.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

struct Vertex {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

class Mesh {
public:
    static Mesh* load(const std::string& path) {
        Mesh* mesh = new Mesh();
        
        std::ifstream file(path);
        std::string line;
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> texCoords;
        std::vector<int> indices;
        
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string type;
            ss >> type;
            
            if (type == "v") {
                float x, y, z;
                ss >> x >> y >> z;
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
            else if (type == "vn") {
                float nx, ny, nz;
                ss >> nx >> ny >> nz;
                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);
            }
            else if (type == "vt") {
                float u, v;
                ss >> u >> v;
                texCoords.push_back(u);
                texCoords.push_back(v);
            }
            else if (type == "f") {
                for (int i = 0; i < 3; i++) {
                    std::string vertexData;
                    ss >> vertexData;
                    std::replace(vertexData.begin(), vertexData.end(), '/', ' ');
                    std::stringstream vss(vertexData);
                    int vi, ti, ni;
                    vss >> vi >> ti >> ni;
                    indices.push_back(vi - 1);
                }
            }
        }
        
        mesh->create(vertices, normals, texCoords, indices);
        return mesh;
    }
    
    void create(const std::vector<float>& vertices, const std::vector<float>& normals, const std::vector<float>& texCoords, const std::vector<int>& indices) {
        vertexCount = indices.size();
        
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        // Position
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);
        
        // Normals
        glGenBuffers(1, &nbo);
        glBindBuffer(GL_ARRAY_BUFFER, nbo);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(1);
        
        // UVs
        glGenBuffers(1, &uvbo);
        glBindBuffer(GL_ARRAY_BUFFER, uvbo);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), texCoords.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(2);
        
        // Indices
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
    }
    
    void draw() {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
    }
    
private:
    GLuint vao, vbo, nbo, uvbo, ibo;
    int vertexCount;
};
