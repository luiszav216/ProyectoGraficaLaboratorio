#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in vec4 weights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 100;
uniform mat4 bones[MAX_BONES];

out vec2 TexCoords;

void main()
{
    // Skinning: combinar huesos
    mat4 boneTransform = mat4(0.0);
    boneTransform += bones[boneIDs[0]] * weights[0];
    boneTransform += bones[boneIDs[1]] * weights[1];
    boneTransform += bones[boneIDs[2]] * weights[2];
    boneTransform += bones[boneIDs[3]] * weights[3];

    vec4 pos = boneTransform * vec4(aPos, 1.0);

    gl_Position = projection * view * model * pos;

    TexCoords = aTexCoords;
}
