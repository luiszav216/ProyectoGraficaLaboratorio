#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>   // o glad.h si ya cambiaste todo a GLAD
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"    // tu clase Shader

using namespace std;

struct Vertex
{
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
    // TexCoords
    glm::vec2 TexCoords;
    // Tangent
    glm::vec3 Tangent;
    // Bitangent
    glm::vec3 Bitangent;
};

struct Texture
{
    GLuint id;
    string type;
    aiString path;
};

class Mesh
{
public:
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        setupMesh();
    }

    void Draw(Shader shader)
    {
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;

        for (GLuint i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);

            string number;
            string name = textures[i].type;

            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);

            glUniform1i(glGetUniformLocation(shader.Program, (name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 16.0f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        for (GLuint i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

private:
    GLuint VAO, VBO, EBO;

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
            &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
            &indices[0], GL_STATIC_DRAW);

        // Posición
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
        // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (GLvoid*)offsetof(Vertex, Normal));
        // TexCoords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (GLvoid*)offsetof(Vertex, TexCoords));
        // Tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (GLvoid*)offsetof(Vertex, Tangent));
        // Bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (GLvoid*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }
};
