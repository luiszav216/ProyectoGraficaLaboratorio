#ifndef MESH_ANIM_H
#define MESH_ANIM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "mesh.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

typedef unsigned int uint;
#define NUM_BONES_PER_VEREX 4

struct BoneMatrix
{
    aiMatrix4x4 offset_matrix;
    aiMatrix4x4 final_world_transform;
};

struct VertexBoneData
{
    uint ids[NUM_BONES_PER_VEREX];
    float weights[NUM_BONES_PER_VEREX];

    VertexBoneData()
    {
        memset(ids, 0, sizeof(ids));
        memset(weights, 0, sizeof(weights));
    }

    void addBoneData(uint bone_id, float weight)
    {
        for (uint i = 0; i < NUM_BONES_PER_VEREX; i++)
        {
            if (weights[i] == 0.0f)
            {
                ids[i] = bone_id;
                weights[i] = weight;
                return;
            }
        }
    }
};

class MeshAnim
{
public:
    vector<Vertex>          vertices;
    vector<unsigned int>    indices;
    vector<Texture>         textures;
    vector<VertexBoneData>  bones_id_weights_for_each_vertex;
    unsigned int            VAO;

    MeshAnim(vector<Vertex> vertices,
        vector<unsigned int> indices,
        vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        setupMesh();
    }

    MeshAnim(vector<Vertex> vertices,
        vector<unsigned int> indices,
        vector<Texture> textures,
        vector<VertexBoneData> bone_id_weights)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        bones_id_weights_for_each_vertex = bone_id_weights;
        setupMesh();
    }

    void Draw(Shader shader)
    {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;

        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            string number;
            string name = textures[i].type;

            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);
            else if (name == "texture_normal")
                number = std::to_string(normalNr++);
            else if (name == "texture_height")
                number = std::to_string(heightNr++);

            glUniform1i(glGetUniformLocation(shader.Program, (name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }

private:
    unsigned int VBO, EBO, VBO_bones;

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &VBO_bones);

        glBindVertexArray(VAO);

        // vertices
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
            &vertices[0], GL_STATIC_DRAW);

        // bones buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO_bones);
        glBufferData(GL_ARRAY_BUFFER,
            bones_id_weights_for_each_vertex.size() * sizeof(VertexBoneData),
            &bones_id_weights_for_each_vertex[0], GL_STATIC_DRAW);

        // indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
            &indices[0], GL_STATIC_DRAW);

        // --- atributos del VBO de vértices ---
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // pos
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (void*)offsetof(Vertex, Normal));
        // texcoords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (void*)offsetof(Vertex, TexCoords));
        // tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (void*)offsetof(Vertex, Tangent));
        // bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (void*)offsetof(Vertex, Bitangent));

        // --- atributos del VBO de huesos ---
        glBindBuffer(GL_ARRAY_BUFFER, VBO_bones);
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(VertexBoneData), (void*)0);
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData),
            (void*)offsetof(VertexBoneData, weights));

        glBindVertexArray(0);
    }
};

#endif
