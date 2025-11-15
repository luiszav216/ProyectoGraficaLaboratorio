#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

// IMPORTANTE: este nombre debe coincidir con lo que usa MeshAnim
uniform sampler2D texture_diffuse1;

void main()
{
    vec4 texColor = texture(texture_diffuse1, TexCoords);

    // Si hay alpha y quieres recortar (por el pelo, etc.), puedes descomentar:
    // if (texColor.a < 0.1)
    //     discard;

    // Sin iluminación: pinta directamente la textura
    FragColor = texColor;
}
