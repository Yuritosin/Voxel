#version 330 core

layout (location = 0) out vec4 color;

uniform float iTime;
uniform vec2 iResolution;

uniform bool debug;

in vec3 outColor;

void main() {
	if (!debug) {
        vec2 uv = gl_FragCoord.xy / iResolution;

        color = vec4(0.5 + 0.5 * cos((iTime) + uv.xyx + vec3(0, 2, 4)), 1.0);
    } else {
        color = vec4(0.0, 1.0, 0.0, 1.0);
    }
}
