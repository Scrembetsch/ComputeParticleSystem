#version 330 core
out vec4 FragColor;

in float vId;

void main()
{           
    FragColor = vec4(1.0, vId / 1000, 1.0, 1.0);
}