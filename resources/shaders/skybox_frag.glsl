#version 330 core

layout (location = 0) out vec4 aColor;

in vec2 vUv;

uniform sampler2D uTexture;

void main() {
    aColor = texture(uTexture, vUv);
}
