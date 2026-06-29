#version 330 core

layout (location = 0) out vec4 color;

uniform vec3 inColor;

void main() {
    color = vec4(inColor, 1.0);
}
