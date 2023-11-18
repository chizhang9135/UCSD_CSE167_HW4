#version 330 core
out vec4 FragColor;
in vec3 ColorsVector;

void main()
{
    FragColor = vec4(ColorsVector, 1.0f);
}
