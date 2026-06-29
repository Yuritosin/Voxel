#version 330 core

layout (location = 0) in uint aPackedData;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform int uAtlasWidth;
uniform int uAtlasHeight;
uniform int uTextureWidth;
uniform int uTextureHeight;

const vec3[8] vertices = vec3[](
	vec3(1.0, 1.0, 1.0),
	vec3(1.0, 1.0, 0.0),
	vec3(0.0, 1.0, 1.0),
	vec3(0.0, 1.0, 0.0),
	
	vec3(1.0, 0.0, 1.0),
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 0.0, 1.0),
	vec3(0.0, 0.0, 0.0)
);

const int[6 * 4] indices = int[](
	0, 2, 4, 6, // For NORTH
	3, 1, 7, 5, // For SOUTH
	1, 0, 5, 4, // For EAST
	2, 3, 6, 7, // For WEST
	0, 1, 2, 3, // For TOP
	4, 6, 5, 7  // For BOTTOM
);

const float[4 * 2] uvMap = float[](
	1.0, 0.0,
	0.0, 0.0,
	1.0, 1.0,
	0.0, 1.0
);

out vec2 vUv;
out float vFaceBright;
out float vLightLevel;

void main() {
    uint x = (aPackedData >> (0u + 8u + 2u + 3u + 4u + 4u)) & (16u - 1u);
    uint y = (aPackedData >> (0u + 8u + 2u + 3u + 4u + 4u + 4u)) & (128u - 1u);
    uint z = (aPackedData >> (0u + 8u + 2u + 3u + 4u)) & (16u - 1u);
    uint light = (aPackedData >> (0u + 8u + 2u + 3u)) & (16u - 1u);
    uint faceIdx = (aPackedData >> (0u + 8u + 2u)) & (8u - 1u);
    uint modelType = (aPackedData >> (0u + 8u)) & (4u - 1u);
//    uint blockId = (aPackedData >> 3u) & (32u - 1u);
    uint blockId = aPackedData & (256u - 1u);
    
    
    int vertexId = gl_VertexID % 4;
    
    float uGap = 1.0;
	float vGap = 1.0;
    float twStep = float(uTextureWidth) / float(uAtlasWidth);
    float thStep = float(uTextureHeight) / float(uAtlasHeight);
    float uStep = float(uTextureWidth + uGap) / float(uAtlasWidth);
    float vStep = float(uTextureHeight + vGap) / float(uAtlasHeight);
    float u = uvMap[vertexId * 2 + 0] * twStep + uStep * 1.0;
    float v = uvMap[vertexId * 2 + 1] * thStep + vStep * float(blockId - 1u);
    
    vec3 vertexPos = vertices[
	  indices[vertexId + (4 * int(faceIdx))]
    ];
    float faceBright;
    if (faceIdx == 4u) { //TOP
        u = uvMap[vertexId * 2 + 0] * twStep + uStep * 2.0;
        
//        faceBright = .6;
        faceBright = .85;
    } else if (faceIdx == 5u) { //BOTTOM
    	u = uvMap[vertexId * 2 + 0] * twStep + uStep * 0.0;
    	
//        faceBright = .6;
    	faceBright = .5;
    } else if (faceIdx == 0u) { //NORTH
//        faceBright = .65;
    	faceBright = .6;
    } else if (faceIdx == 1u) { //SOUTH
//        faceBright = .65;
    	faceBright = .8;
    } else if (faceIdx == 2u) { //EAST
//        faceBright = 1.0;
        faceBright = .65;
    } else { //WEST
        faceBright = .75;
    }
    vec3 localChunkPos = vec3(float(x), float(y), float(z));
    vertexPos += localChunkPos;
    
    gl_Position = proj * view * model * vec4(vertexPos, 1.0);
    vLightLevel = float(light) / 15.0;
    vFaceBright = faceBright;
    vUv = vec2(u, v);
}
