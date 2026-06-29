#version 330 core

layout (location = 0) out vec4 color;

in vec2 vUv;
// in float vBright;

uniform sampler2D tex;

void main() {
    color = texture(tex, vUv);
    if (color.a <= 0.01) discard;
// 	if (!debug) {
//         color = texture(tex, vUv) * vBright;
//         color = vec4(
//             vec3(
//                 (chunkColor >> 16) / 255.0,
//                 (chunkColor >> 8 & 0xff) / 255.0,
//                 (chunkColor & 0xff) / 255.0
//             ) * vBright,
//             1.0
//         );
//         vec2 uv = gl_FragCoord.xy / iResolution;

//         color = vec4(0.5 + 0.5 * cos((iTime) + uv.xyx + vec3(0, 2, 4)), 1.0);
//         color = vec4((0.5 + 0.5 * cos((iTime) + vec3(0, 2, 4))) * vBright, 1.0);
//     } else {
//         color = vec4((0.5 + 0.5 * cos((iTime) + vec3(0, 2, 4))) * vBright, 1.0);
//         color = vec4(1.0, 0.0, 0.0, 1.0);
//     }
}
