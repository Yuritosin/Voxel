#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aUv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 vUv;
// out float vBright;

void main() {
    gl_Position = proj * view * model * vec4(aPosition, 1.0);
//     vBright = aBright;
    vUv = aUv;
}
