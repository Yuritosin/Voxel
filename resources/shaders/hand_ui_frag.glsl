#version 330 core

layout (location = 0) out vec4 aColor;

flat in int vFaceIndex;

void main() {
    vec3 color = vec3(0.0, 1.0, 0.0);
    if (vFaceIndex == 0) {
        color = vec3(0.0, 0.0, 0.0);
    } else if (vFaceIndex == 1) {
        color = vec3(1.0, 0.0, 0.0);
    } else if (vFaceIndex == 2) {
        color = vec3(0.85, 0.74, 0.53);
//         color = vec3(0.89, 0.82, 0.68);
//         color = vec3(0.57, 0.42, 0.33);
//         color = vec3(0.58, 0.36, 0.25);
    } else if (vFaceIndex == 3) {
        color = vec3(0.0, 0.0, 1.0);
    } else if (vFaceIndex == 4) {
        color = vec3(0.89, 0.78, 0.56);
//         color = vec3(0.91, 0.84, 0.7);
//         color = vec3(0.59, 0.44, 0.35);
//         color = vec3(0.6, 0.44, 0.36);
    } else if (vFaceIndex == 5) {
        color = vec3(1.0, 0.0, 1.0);
    }
    aColor = vec4(color, 1.0);
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
