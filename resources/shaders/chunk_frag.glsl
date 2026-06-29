#version 330 core

layout (location = 0) out vec4 color;

// uniform float iTime;
// uniform vec2 iResolution;

uniform bool debug;
// uniform bool isAlpha;
uniform float uOpacity;
// uniform int chunkColor;

// in vec3 outColor;


// in float vBright;

in vec2 vUv;
in float vFaceBright;
in float vLightLevel;

uniform sampler2D tex;

void main() {
    if (!debug) {
//         color = texture(tex, vUv) * vBright;
//         if (!isAlpha) {
//             color.a = opacity;
//         } else {
//             color.a *= opacity;
        //         }
    	color = texture(tex, vUv) * max(vFaceBright * vLightLevel, 0.2);
        //color = vec4(vec3(0.9) * vFaceBright, 1.0);
//        color = vec4(vec3(1.0, 0.0, 0.0) * vFaceBright, 1.0);
//        color = vec4(vec3(0.8, 0.8, 0.8) * vFaceBright, 1.0);
        color.a *= uOpacity;
        if (color.a <= 0.01) discard;
//         color.a = opacity;
//         color.rgb *= opacity;
//         color.a = opacity;
//         if (isFade) color.a = opacity;
//         else if (color.a <= 0.01) discard;
//         if (!isAlpha) color.a = opacity;

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
    } else {
//         color = vec4((0.5 + 0.5 * cos((iTime) + vec3(0, 2, 4))) * vBright, 1.0);
//         color = vec4(1.0, 0.0, 0.0, 1.0);
        color = vec4(0.0, 1.0, 0.0, 1.0);
    }
}
