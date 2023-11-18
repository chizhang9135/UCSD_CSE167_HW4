#version 330 core
out vec4 FragColor;
in vec3 ColorsVector;
in vec2 TexCoord;  // Texture coordinate input

uniform sampler2D ourTexture; // Texture sampler

void main()
{
    vec4 texColor = texture(ourTexture, TexCoord);
    FragColor = vec4(ColorsVector, 1.0f) * texColor;
}
