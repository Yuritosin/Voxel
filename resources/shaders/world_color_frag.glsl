#version 330 core

layout (location = 0) out vec4 aColor;

uniform vec3 color;

void main() {
    aColor = vec4(color, 1.0);
}
