#version 330 core

// layout (location = 0) in vec3 aPosition;
// layout (location = 1) in float aBright;
// layout (location = 2) in vec2 aUv;

layout (location = 0) in uint aPackedData;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform float uChunkWidth;
uniform float uChunkHeight;

// out vec2 vUv;
out float vFaceBright;
// out float vBright;

void main() {
    float x = float((aPackedData >> (6u + 2u + 3u + 6u)) & (64u - 1u)) / 32.0 * uChunkWidth;
    float y = float((aPackedData >> (6u + 2u + 3u + 6u + 6u)) & (512u - 1u)) / 256.0 * uChunkHeight;
    float z = float((aPackedData >> (6u + 2u + 3u)) & (64u - 1u)) / 32.0 * uChunkWidth;
    uint faceIdx = (aPackedData >> (6u + 2u)) & (8u - 1u);
    uint modelType = (aPackedData >> 6u) & (4u - 1u);

    vec3 position = vec3(x, y, z);
    float faceBright;
    if (faceIdx == 0u) { // NORTH
        faceBright = .6;
    } else if (faceIdx == 1u) { // SOUTH
        faceBright = .6;
    } else if (faceIdx == 2u) { // EAST
        faceBright = .65;
    } else if (faceIdx == 3u) { // WEST
        faceBright = .65;
    } else if (faceIdx == 4u) { // TOP
        faceBright = 1.0;
    } else { // BOTTOM
        faceBright = .55;
    }

    gl_Position = proj * view * model * vec4(position, 1.0);
    vFaceBright = faceBright;
    //     vBright = aBright;
    //     vUv = aUv;
}
