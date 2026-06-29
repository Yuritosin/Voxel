#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in float aFaceIndex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

flat out int vFaceIndex;
// out float vBright;

void main() {
    gl_Position = proj * view * model * vec4(aPosition, 1.0);
//     vBright = aBright;
    vFaceIndex = int(aFaceIndex);
}
