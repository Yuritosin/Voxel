#version 330 core

layout (location = 0) out vec4 color;

uniform vec3 uColor;
uniform float uOpacity;
uniform float uThinness;

in vec2 vUv;

void main() {
    if (vUv.x > uThinness && vUv.x < 1.0 - uThinness &&
        vUv.y > uThinness && vUv.y < 1.0 - uThinness) {
        discard;
    } else {
        color = vec4(uColor, uOpacity);
    }
}
