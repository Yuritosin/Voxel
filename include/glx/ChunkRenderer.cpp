#include "ChunkRenderer.h"

#include <cstdint>
#include <algorithm>
#include <filesystem>

#include "world/World.h"

#include "options/Options.h"

#include <glm/gtc/matrix_transform.hpp>

// #include "debug/GlDebugger.h"

void ChunkRenderer::ToggleRenderDebug() {
    ChunkRenderer::renderDebug = !ChunkRenderer::renderDebug;
    glPolygonMode(GL_FRONT_AND_BACK, (ChunkRenderer::renderDebug ? GL_LINE : GL_FILL));
}

static glm::mat4 projMatrix = glm::mat4(1.0f);

#include <iostream>

enum class RenderType {
    OPAQUE, TRANSPARENT, TRANSLUCENT, CUTOUT
};

BlockModelType ChunkRenderer::getBlockModelType(const Block block) {
    switch(block) {
        case Blocks::STONE: return BlockModelType::CUBE_ALL;
        case Blocks::DIRT: return BlockModelType::CUBE_ALL;
        case Blocks::GRASS: return BlockModelType::CUBE_TOP_BOTTOM;
        case Blocks::PLANKS: return BlockModelType::CUBE_ALL;
        case Blocks::BEDROCK: return BlockModelType::CUBE_ALL;
        case Blocks::GLASS: return BlockModelType::CUBE_ALL;
        case Blocks::RED_STAINED_GLASS: return BlockModelType::CUBE_ALL;
        case Blocks::GREEN_STAINED_GLASS: return BlockModelType::CUBE_ALL;
        case Blocks::BLUE_STAINED_GLASS: return BlockModelType::CUBE_ALL;
        case Blocks::WATER: return BlockModelType::CUBE_ALL;
        case Blocks::GRAVEL: return BlockModelType::CUBE_ALL;
        case Blocks::SAND: return BlockModelType::CUBE_ALL;
        case Blocks::LEAVES: return BlockModelType::CUBE_ALL;
        case Blocks::LOG: return BlockModelType::CUBE_COLUMN;
        default: return BlockModelType::CUBE_ALL;
    }
}


//RenderType GetRenderType(const unsigned int blockId) {
//    if (IsTransparent(blockId)) {
//        return RenderType::TRANSPARENT;
//    } else if (IsTranslucent(blockId)) {
//        return RenderType::TRANSLUCENT;
//    } else if (IsCutout(blockId)) {
//        return RenderType::CUTOUT;
//    } else {
//        return RenderType::OPAQUE;
//    }
//}

void ChunkRenderer::Init(const World& world) {
    projMatrix = Camera::GetProjectionMatrix();
    RebuildMesh(world);
    StartFadeIn();
}

void ChunkRenderer::RebuildMesh(const World& world) {
    if (IS_CHUNK_RENDERER_DEBUG) {
        std::cout << "Started mesh building for chunk ("
            << m_Chunk.m_X << ", "
            << m_Chunk.m_Y << ", "
            << m_Chunk.m_Z << ")\n";
    }

    m_TranslucentIndices.clear();
    m_TranslucentIndices.shrink_to_fit();
    m_TranslucentVertices.clear();
    m_TranslucentVertices.shrink_to_fit();

    for (int yy = 0; yy < CHUNK_HEIGHT; yy ++) {
        for (int zz = 0; zz < CHUNK_DEPTH; zz ++) {
            for (int xx = 0; xx < CHUNK_WIDTH; xx ++) {
                const Block block = m_Chunk.GetBlock(xx, yy, zz);
                if (!IsAir(block)) {
                    // std::vector<float>* verticesDestination = &m_Vertices;
                    std::vector<unsigned int>* verticesDestination = &m_Vertices;
                    std::vector<unsigned short>* indicesDestination = &m_Indices;
                    if (IsTransparent(block)) {
                        verticesDestination = &m_TransparentVertices;
                        indicesDestination = &m_TransparentIndices;
                    } else if (IsTranslucent(block)) {
                        verticesDestination = &m_TranslucentVertices;
                        indicesDestination = &m_TranslucentIndices;
                    }
                    // } else if (IsTranslucent(blockId)) {
                    //     verticesDestination = m_TranslucentVertices;
                    //     indicesDestination = m_TranslucentIndices;
                    // }
                    if (IsFaceNeed(world, block, xx + 1, yy, zz)) PushFace(*verticesDestination, *indicesDestination, xx, yy, zz, Direction::EAST, block);
                    if (IsFaceNeed(world, block, xx - 1, yy, zz)) PushFace(*verticesDestination, *indicesDestination, xx, yy, zz, Direction::WEST, block);
                    if (IsFaceNeed(world, block, xx, yy, zz + 1)) PushFace(*verticesDestination, *indicesDestination, xx, yy, zz, Direction::NORTH, block);
                    if (IsFaceNeed(world, block, xx, yy, zz - 1)) PushFace(*verticesDestination, *indicesDestination, xx, yy, zz, Direction::SOUTH, block);
                    if (IsFaceNeed(world, block, xx, yy + 1, zz)) PushFace(*verticesDestination, *indicesDestination, xx, yy, zz, Direction::TOP, block);
                    if (IsFaceNeed(world, block, xx, yy - 1, zz)) PushFace(*verticesDestination, *indicesDestination, xx, yy, zz, Direction::BOTTOM, block);
                }
            }
        }
    }

    m_HasTranslucent = !m_TranslucentVertices.empty();
    m_HasTransparent = !m_TransparentVertices.empty();

    const unsigned int verticesLength = m_Vertices.size();
    const unsigned int verticesCount = verticesLength / 6;
    const unsigned int verticesSize = verticesLength * sizeof(float);
    const unsigned int indicesCount = m_Indices.size();
    const unsigned int indicesSize = indicesCount * sizeof(unsigned short);

    if (IS_CHUNK_RENDERER_DEBUG) {
        std::cout << "Start GPU allocation" << '\n';
        std::cout << "Floats count: " << verticesLength
            << ", vertex count: " << verticesCount
            << ", indices count: " << indicesCount
            << '\n';
    }

    const unsigned int attrs[2] { 1, 0 };
    m_Mesh.AllocateToGpuUInt(
        m_Vertices.data(),
        m_Vertices.size(),
        m_Indices.data(),
        m_Indices.size(),
        attrs
    );
    m_TransparentMesh.AllocateToGpuUInt(
        m_TransparentVertices.data(),
        m_TransparentVertices.size(),
        m_TransparentIndices.data(),
        m_TransparentIndices.size(),
        attrs
    );
    // const unsigned int attrs[4] { 3, 1, 2, 0 };
    // m_Mesh.AllocateToGpu(
    //     m_Vertices.data(),
    //     m_Vertices.size() / 6,
    //     m_Indices.data(),
    //     m_Indices.size(),
    //     attrs
    // );
    // m_TransparentMesh.AllocateToGpu(
    //     m_TransparentVertices.data(),
    //     m_TransparentVertices.size() / 6,
    //     m_TransparentIndices.data(),
    //     m_TransparentIndices.size(),
    //     attrs
    // );
    // // m_TranslucentMesh.AllocateToGpu(
    // //     m_TranslucentVertices.data(),
    // //     m_TranslucentVertices.size() / 6,
    // //     m_TranslucentIndices.data(),
    // //     m_TranslucentIndices.size(),
    // //     attrs
    // // );
    SortAndAllocateTranslucentFaces(world.GetPlayerConst().camera.GetPos());
    // GlDebugger::glCheckError();

    if (IS_CHUNK_RENDERER_DEBUG) {
        std::cout << "End GPU allocation" << '\n';

        const int size = verticesSize + indicesSize;
        std::cout << "Size of chunk mesh ("
            << m_Chunk.m_X << ", "
            << m_Chunk.m_Y << ", "
            << m_Chunk.m_Z
            << ") is: "<< ChunkRenderer::format_size_as_str(size)
            << "\n\n";
        commonSize += size;
    }

    m_Indices.clear();
    m_Indices.shrink_to_fit();
    m_Vertices.clear();
    m_Vertices.shrink_to_fit();

    m_TransparentIndices.clear();
    m_TransparentIndices.shrink_to_fit();
    m_TransparentVertices.clear();
    m_TransparentVertices.shrink_to_fit();

    // m_TranslucentIndices.clear();
    // m_TranslucentIndices.shrink_to_fit();
    // m_TranslucentVertices.clear();
    // m_TranslucentVertices.shrink_to_fit();
}


bool ShouldRenderFace(const Block currentBlock, const Block neighborBlock) {
	if (IsAir(neighborBlock)) return true;
    if (!IsOpaque(currentBlock)) {
        return IsCutout(currentBlock) || GetRawId(currentBlock) != GetRawId(neighborBlock);
    }
    return !IsOpaque(neighborBlock);
}

bool ChunkRenderer::IsFaceNeed(
    const World& world,
    const Block currentBlock,
    const int x, const int y, const int z
) const {
    const unsigned int neighborBlockId = m_Chunk.GetBlock(x, y, z);
    if (x >= 0 && x < CHUNK_WIDTH && z >= 0 && z < CHUNK_DEPTH && y >= 0 && y < CHUNK_HEIGHT) {
        return ShouldRenderFace(currentBlock, neighborBlockId);
    }

    if (y < 0 || y >= CHUNK_HEIGHT) return true;

    Chunk* eastChunk = world.GetChunkConst(m_Chunk.m_X + 1, m_Chunk.m_Z);
    Chunk* westChunk = world.GetChunkConst(m_Chunk.m_X - 1, m_Chunk.m_Z);
    Chunk* northChunk = world.GetChunkConst(m_Chunk.m_X, m_Chunk.m_Z + 1);
    Chunk* southChunk = world.GetChunkConst(m_Chunk.m_X, m_Chunk.m_Z - 1);
    if (y < 0 || y >= CHUNK_HEIGHT) return true;
    if (x >= CHUNK_WIDTH) return eastChunk ? ShouldRenderFace(currentBlock, eastChunk->GetBlock(x - CHUNK_WIDTH, y, z)) : true;
    if (x < 0) return westChunk ? ShouldRenderFace(currentBlock, westChunk->GetBlock(x + CHUNK_WIDTH, y, z)) : true;
    if (z >= CHUNK_DEPTH) return northChunk ? ShouldRenderFace(currentBlock, northChunk->GetBlock(x, y, z - CHUNK_DEPTH)) : true;
    if (z < 0) return southChunk ? ShouldRenderFace(currentBlock, southChunk->GetBlock(x, y, z + CHUNK_DEPTH)) : true;

    // if (x >= CHUNK_WIDTH) return eastChunk ? eastChunk->GetBlock(x - CHUNK_WIDTH, y, z) == 0 : true;
    // if (x < 0) return westChunk ? westChunk->GetBlock(x + CHUNK_WIDTH, y, z) == 0 : true;
    // if (z >= CHUNK_DEPTH) return northChunk ? northChunk->GetBlock(x, y, z - CHUNK_DEPTH) == 0 : true;
    // if (z < 0) return southChunk ? southChunk->GetBlock(x, y, z + CHUNK_DEPTH) == 0 : true;

    return true;
}

glm::vec3 Vertex::UnpackPos(const uint32_t packedData) {
    return glm::vec3(UnpackX(packedData), UnpackY(packedData), UnpackZ(packedData));
}
float Vertex::UnpackX(const uint32_t packedData) {
    return ((packedData >> (0 + 8 + 2 + 3 + 4 + 4)) & (16 - 1));
}
float Vertex::UnpackY(const uint32_t packedData) {
    return ((packedData >> (0 + 8 + 2 + 3 + 4 + 4 + 4)) & (128 - 1));
}
float Vertex::UnpackZ(const uint32_t packedData) {
    return ((packedData >> (0 + 8 + 2 + 3 + 4)) & (16 - 1));
}
uint8_t Vertex::UnpackLightLevel(const uint32_t packedData) {
	return ((packedData >> (0 + 8 + 2 + 3)) & (16 - 1));
}
Direction Vertex::UnpackFaceIndex(const uint32_t packedData) {
    return static_cast<Direction>((packedData >> (0 + 8 + 2)) & (8 - 1));
}
BlockModelType Vertex::UnpackModelType(const uint32_t packedData) {
    return static_cast<BlockModelType>((packedData >> (0 + 8)) & (4 - 1));
}
Block Vertex::UnpackBlock(const uint32_t packedData) {
//	return (packedData >> (3)) & (32 - 1);
	return (packedData) & (256 - 1);
}

uint32_t Vertex::Pack() const {
//    const unsigned int packedX = (unsigned int)(m_X / CHUNK_WIDTH * 32.0f); // 4 bit
//    const unsigned int packedY = (unsigned int)(m_Y / CHUNK_HEIGHT * 256.0f); // 7 bit
//    const unsigned int packedZ = (unsigned int)(m_Z / CHUNK_WIDTH * 32.0f); // 4 bit
    const uint8_t packedX = (m_X & (16 - 1)); // 4 bit
	const uint8_t packedY = (m_Y & (128 - 1)); // 7 bit
	const uint8_t packedZ = (m_Z & (16 - 1)); // 4 bit
    const uint16_t packedPos = packedY << 8 | packedX << 4 | packedZ; // 15 bit

    const uint8_t packedLightLevel = m_LightLevel & (16 - 1); // 4 bit
    const unsigned int packedFaceIdx = static_cast<unsigned int>(m_Face); // 3 bit
    const unsigned int packedBlockMdlType = static_cast<unsigned int>(m_ModelType); // 2 bit
    const unsigned int packedBlockId = (GetRawId(m_Block) & (256 - 1)); // 8 bit

    // 32-15-4-3-2-8 = 0 bits remaind

    return (
        packedPos << (32 - 15) |
		packedLightLevel << (32 - 15 - 4) |
		packedFaceIdx << (32 - 15 - 4 - 3) |
		packedBlockMdlType << (32 - 15 - 4 - 3 - 2) |
		packedBlockId << (32 - 15 - 4 - 3 - 2 - 8)
    );
}

// void ChunkRenderer::PushVertex(std::vector<float>& destination, const Vertex& vertex) {
void ChunkRenderer::PushVertex(std::vector<uint32_t>& destination, const Vertex& vertex) {
    destination.push_back(vertex.Pack());
    // std::cout << vertex.m_X << ", " << vertex.m_Y << ", " << vertex.m_Z << '\n';

    // destination.push_back(vertex.m_X); destination.push_back(vertex.m_Y); destination.push_back(vertex.m_Z);
    // destination.push_back(vertex.m_Bright);
    // destination.push_back(vertex.m_U); destination.push_back(vertex.m_V);

    // destination.push_back(vertex.m_X); destination.push_back(vertex.m_Y); destination.push_back(vertex.m_Z);
    // destination.push_back(vertex.m_Bright);
    // destination.push_back(vertex.m_U); destination.push_back(vertex.m_V);
}

void ChunkRenderer::PushFace(
    // std::vector<float>& verticesDestination,
    std::vector<unsigned int>& verticesDestination,
    std::vector<unsigned short>& indicesDestination,
    const int x, const int y, const int z,
    const Direction direction, const Block block
) {
    // const float realSize = 1.0f;
    // const float size = realSize / 2;
    const float size = 1.0f;


    // const unsigned int baseIndex = verticesDestination.size() / 6;
    const unsigned int baseIndex = verticesDestination.size();
    const uint8_t lightLevel = m_Chunk.GetLightLevel(x, y, z);
//    const uint8_t lightLevel = 15;


    // const float atlasWidth = (float)ATLAS_WIDTH;
    // const float atlasHeight = (float)ATLAS_HEIGHT;
    // // const float textureWidth = (float)(blockId >= Blocks::SASHA ? 256 : TEXTURE_WIDTH);
    // // const float textureHeight = (float)(blockId >= Blocks::SASHA ? 256 : TEXTURE_HEIGHT);
    // const float textureWidth = (float)TEXTURE_WIDTH;
    // const float textureHeight = (float)TEXTURE_HEIGHT;
    // const float textureVGap = (float)TEXTURE_VGAP;
    // const float textureHGap = (float)TEXTURE_HGAP;
    //
    // const float twStep = textureWidth / atlasWidth;
    // const float thStep = textureHeight / atlasHeight;
    //
    // const float uStep = (textureWidth + textureVGap)  / atlasWidth;
    // const float vStep = (textureHeight + textureHGap) / atlasHeight * (blockId - 1);
    //
    // const float uOffset = 0.0f / atlasWidth;
    // const float vOffset = (0.0f) / atlasHeight;
    // // const float vOffset = ((blockId >= Blocks::SASHA) ? 85.0f : 0.0f) / atlasHeight;
    // // const float uStep = ((blockId < Blocks::SASHA ? TEXTURE_WIDTH : (256)) + textureVGap)  / atlasWidth;
    // // const float vStep = ((blockId < Blocks::SASHA ? TEXTURE_HEIGHT : (256)) + textureHGap) / atlasHeight * ((blockId < Blocks::SASHA ? blockId : blockId - 5) - 1);

    const BlockModelType blockModelType = getBlockModelType(block);
    switch (direction) {
        case Direction::TOP: {
            const Vertex v0 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::TOP, blockModelType, block };
            const Vertex v1 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::TOP, blockModelType, block };
            const Vertex v2 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::TOP, blockModelType, block };
            const Vertex v3 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::TOP, blockModelType, block };

    //      const Vertex v0 = { x + size, y + size, z + size,  Direction::TOP, blockModelType };
    //		const Vertex v1 = { x + size, y + size, z - 0.0f,  Direction::TOP, blockModelType };
    //		const Vertex v2 = { x - 0.0f, y + size, z + size,  Direction::TOP, blockModelType };
    //		const Vertex v3 = { x - 0.0f, y + size, z - 0.0f,  Direction::TOP, blockModelType };

            // const Vertex v0 = { x + size, y + size, z + size,  1.0f,  uOffset + uStep * 2 + twStep, vOffset + vStep + thStep };
            // const Vertex v1 = { x + size, y + size, z - size,  1.0f,  uOffset + uStep * 2 + twStep, vOffset + vStep };
            // const Vertex v2 = { x - size, y + size, z + size,  1.0f,  uOffset + uStep * 2, vOffset + vStep + thStep };
            // const Vertex v3 = { x - size, y + size, z - size,  1.0f,  uOffset + uStep * 2, vOffset + vStep };

            PushVertex(verticesDestination, v0);
            PushVertex(verticesDestination, v1);
            PushVertex(verticesDestination, v2);
            PushVertex(verticesDestination, v3);
            break;
        }
        case Direction::BOTTOM: {
            const Vertex v0 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::BOTTOM, blockModelType, block };
            const Vertex v1 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::BOTTOM, blockModelType, block };
            const Vertex v2 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::BOTTOM, blockModelType, block };
            const Vertex v3 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::BOTTOM, blockModelType, block };

            // const Vertex v0 = { x + size, y - size, z + size,  0.55f,  uOffset + uStep * 0 + twStep, vOffset + vStep };
            // const Vertex v1 = { x - size, y - size, z + size,  0.55f,  uOffset + uStep * 0, vOffset + vStep };
            // const Vertex v2 = { x + size, y - size, z - size,  0.55f,  uOffset + uStep * 0 + twStep, vOffset + vStep + thStep };
            // const Vertex v3 = { x - size, y - size, z - size,  0.55f,  uOffset + uStep * 0, vOffset + vStep + thStep };

            PushVertex(verticesDestination, v0);
            PushVertex(verticesDestination, v1);
            PushVertex(verticesDestination, v2);
            PushVertex(verticesDestination, v3);
            break;
        }
        case Direction::NORTH: {
            const Vertex v0 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::NORTH, blockModelType, block };
            const Vertex v1 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::NORTH, blockModelType, block };
            const Vertex v2 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::NORTH, blockModelType, block };
            const Vertex v3 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::NORTH, blockModelType, block };

            // const Vertex v0 = { x + size, y + size, z + size,  0.6f,  uOffset + uStep * 1 + twStep, vOffset + vStep };
            // const Vertex v1 = { x - size, y + size, z + size,  0.6f,  uOffset + uStep * 1, vOffset + vStep };
            // const Vertex v2 = { x + size, y - size, z + size,  0.6f,  uOffset + uStep * 1 + twStep, vOffset + vStep + thStep };
            // const Vertex v3 = { x - size, y - size, z + size,  0.6f,  uOffset + uStep * 1, vOffset + vStep + thStep };

            PushVertex(verticesDestination, v0);
            PushVertex(verticesDestination, v1);
            PushVertex(verticesDestination, v2);
            PushVertex(verticesDestination, v3);
            break;
        }
        case Direction::SOUTH: {
            const Vertex v0 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::SOUTH, blockModelType, block };
            const Vertex v1 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::SOUTH, blockModelType, block };
            const Vertex v2 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::SOUTH, blockModelType, block };
            const Vertex v3 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::SOUTH, blockModelType, block };

            // const Vertex v0 = { x + size, y + size, z - size, 0.6f, uOffset + uStep * 1 + twStep, vOffset + vStep };
            // const Vertex v1 = { x - size, y + size, z - size, 0.6f, uOffset + uStep * 1, vOffset + vStep };
            // const Vertex v2 = { x + size, y - size, z - size, 0.6f, uOffset + uStep * 1 + twStep, vOffset + vStep + thStep };
            // const Vertex v3 = { x - size, y - size, z - size, 0.6f, uOffset + uStep * 1, vOffset + vStep + thStep };

            PushVertex(verticesDestination, v0);
            PushVertex(verticesDestination, v1);
            PushVertex(verticesDestination, v2);
            PushVertex(verticesDestination, v3);
            break;
        }
        case Direction::EAST: {
            const Vertex v0 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::EAST, blockModelType, block };
            const Vertex v1 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::EAST, blockModelType, block };
            const Vertex v2 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::EAST, blockModelType, block };
            const Vertex v3 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::EAST, blockModelType, block };

            // const Vertex v0 = { x + size, y + size, z + size, 0.65f, uOffset + uStep * 1 + twStep, vOffset + vStep };
            // const Vertex v1 = { x + size, y + size, z - size, 0.65f, uOffset + uStep * 1, vOffset + vStep };
            // const Vertex v2 = { x + size, y - size, z + size, 0.65f, uOffset + uStep * 1 + twStep, vOffset + vStep + thStep };
            // const Vertex v3 = { x + size, y - size, z - size, 0.65f, uOffset + uStep * 1, vOffset + vStep + thStep };

            PushVertex(verticesDestination, v0);
            PushVertex(verticesDestination, v1);
            PushVertex(verticesDestination, v2);
            PushVertex(verticesDestination, v3);
            break;
        }
        case Direction::WEST: {
            const Vertex v0 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::WEST, blockModelType, block };
            const Vertex v1 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::WEST, blockModelType, block };
            const Vertex v2 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::WEST, blockModelType, block };
            const Vertex v3 = { (uint8_t)x, (uint8_t)y, (uint8_t)z,  lightLevel, Direction::WEST, blockModelType, block };

            // const Vertex v0 = { x - size, y + size, z + size, 0.65f, uOffset + uStep * 1 + twStep, vOffset + vStep };
            // const Vertex v1 = { x - size, y + size, z - size, 0.65f, uOffset + uStep * 1, vOffset + vStep };
            // const Vertex v2 = { x - size, y - size, z + size, 0.65f, uOffset + uStep * 1 + twStep, vOffset + vStep + thStep };
            // const Vertex v3 = { x - size, y - size, z - size, 0.65f, uOffset + uStep * 1, vOffset + vStep + thStep };

            PushVertex(verticesDestination, v0);
            PushVertex(verticesDestination, v1);
            PushVertex(verticesDestination, v2);
            PushVertex(verticesDestination, v3);
            break;
        }
    }
    indicesDestination.push_back(baseIndex + 0);
    indicesDestination.push_back(baseIndex + 1);
    indicesDestination.push_back(baseIndex + 2);

    indicesDestination.push_back(baseIndex + 2);
    indicesDestination.push_back(baseIndex + 1);
    indicesDestination.push_back(baseIndex + 3);
}

void ChunkRenderer::DrawSolidBlocks() const {
    glEnable(GL_DEPTH_TEST);
    // glDepthMask(GL_TRUE);
    // glEnable(GL_BLEND);T
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (!m_FadeIn && !m_FadeOut) {
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    } else {
        // glDepthMask(GL_TRUE);
        glEnable(GL_BLEND);
        // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // glDepthMask(GL_TRUE);
    }
    m_Mesh.Draw();
}
void ChunkRenderer::DrawTransparentBlocks() const {
    if (!m_FadeIn && !m_FadeOut) {
        glDisable(GL_BLEND);
    } else {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    glDepthFunc(GL_LESS);
    // glDisable(GL_CULL_FACE);
    m_TransparentMesh.Draw();
}
void ChunkRenderer::DrawTranslucentBlocks() const {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    // glDisable(GL_CULL_FACE);
    m_TranslucentMesh.Draw();
}

// void ChunkRenderer::PrepareSolidShader(const Shader& shader) const {
//
// }

void ChunkRenderer::SortAndAllocateTranslucentFaces(const glm::vec3& cameraPos) {
    if (!m_HasTranslucent) return;

    const size_t vertexStride = 1;
    const size_t indicesPerFace = 6;

    struct FaceData {
        float distance;
        size_t startIndex;
    };

    std::vector<FaceData> visibleFaces;
    const size_t faceCount = m_TranslucentIndices.size() / indicesPerFace;
    visibleFaces.reserve(faceCount);

    for (size_t face = 0; face < faceCount; ++face) {
        const size_t baseVertexIndex = face * indicesPerFace;

        unsigned short i0 = m_TranslucentIndices[baseVertexIndex];
        unsigned short i1 = m_TranslucentIndices[baseVertexIndex + 1];
        unsigned short i2 = m_TranslucentIndices[baseVertexIndex + 2];

        const size_t idx0 = i0 * vertexStride;
        const size_t idx1 = i1 * vertexStride;
        const size_t idx2 = i2 * vertexStride;

        glm::vec3 v0 = Vertex::UnpackPos(m_TranslucentVertices[idx0]);
        glm::vec3 v1 = Vertex::UnpackPos(m_TranslucentVertices[idx1]);
        glm::vec3 v2 = Vertex::UnpackPos(m_TranslucentVertices[idx2]);
        // glm::vec3 v0(m_TranslucentVertices[idx0], m_TranslucentVertices[idx0 + 1], m_TranslucentVertices[idx0 + 2]);
        // glm::vec3 v1(m_TranslucentVertices[idx1], m_TranslucentVertices[idx1 + 1], m_TranslucentVertices[idx1 + 2]);
        // glm::vec3 v2(m_TranslucentVertices[idx2], m_TranslucentVertices[idx2 + 1], m_TranslucentVertices[idx2 + 2]);

//         // Вычисляем нормаль грани
//         glm::vec3 edge1 = v1 - v0;
//         glm::vec3 edge2 = v2 - v0;
//         glm::vec3 normal = glm::cross(edge1, edge2);
//
//         // Направление от грани к камере
//         glm::vec3 toCamera = cameraPos - v0;
//
//         // Если нормаль направлена от камеры (dot < 0) — грань не видна
//         if (glm::dot(normal, toCamera) <= 0.0f) {
//             continue;  // Пропускаем эту грань
//         }

        glm::vec3 center = (v0 + v1 + v2) / 3.0f;
        glm::vec3 worldCenter = center + glm::vec3(
            m_Chunk.m_X * CHUNK_WIDTH,
            m_Chunk.m_Y * CHUNK_HEIGHT,
            m_Chunk.m_Z * CHUNK_WIDTH
        );
        // const float dist = glm::length(worldCenter - cameraPos);
        const float dist = glm::dot(worldCenter - cameraPos, worldCenter - cameraPos);
        visibleFaces.push_back({dist, baseVertexIndex});
    }

    std::sort(visibleFaces.begin(), visibleFaces.end(), [](const FaceData& a, const FaceData& b) {
        return a.distance > b.distance;
    });


    std::vector<unsigned short> sortedIndices;
    sortedIndices.reserve(visibleFaces.size() * indicesPerFace);

    for (const FaceData& face : visibleFaces) {
        for (size_t i = 0; i < indicesPerFace; ++i) {
            sortedIndices.push_back(m_TranslucentIndices[face.startIndex + i]);
        }
    }

    m_TranslucentIndices = std::move(sortedIndices);

    const unsigned int attrs[2] = { 1, 0 };
    m_TranslucentMesh.AllocateToGpuUInt(
        m_TranslucentVertices.data(),
        m_TranslucentVertices.size(),
        m_TranslucentIndices.data(),
        m_TranslucentIndices.size(),
        attrs
    );
    // const unsigned int attrs[4] = { 3, 1, 2, 0 };
    // m_TranslucentMesh.AllocateToGpu(
    //     m_TranslucentVertices.data(),
    //     m_TranslucentVertices.size() / 6,
    //     m_TranslucentIndices.data(),
    //     m_TranslucentIndices.size(),
    //     attrs
    // );
}
// void ChunkRenderer::SortAndAllocateTranslucentFaces(const glm::vec3& cameraPos) {
//     if (!m_HasTranslucent) return;
//
//     const size_t vertexStride = 6;
//     const size_t indicesPerFace = 6;
//
//     struct FaceData {
//         float distance;
//         size_t startIndex;
//     };
//
//     std::vector<FaceData> visibleFaces;
//     const size_t faceCount = m_TranslucentIndices.size() / indicesPerFace;
//     visibleFaces.reserve(faceCount);
//
//     for (size_t face = 0; face < faceCount; ++face) {
//         size_t baseVertexIndex = face * indicesPerFace;
//
//         unsigned short i0 = m_TranslucentIndices[baseVertexIndex];
//         unsigned short i1 = m_TranslucentIndices[baseVertexIndex + 1];
//         unsigned short i2 = m_TranslucentIndices[baseVertexIndex + 2];
//
//         size_t idx0 = i0 * vertexStride;
//         size_t idx1 = i1 * vertexStride;
//         size_t idx2 = i2 * vertexStride;
//
//         glm::vec3 v0(m_TranslucentVertices[idx0], m_TranslucentVertices[idx0 + 1], m_TranslucentVertices[idx0 + 2]);
//         glm::vec3 v1(m_TranslucentVertices[idx1], m_TranslucentVertices[idx1 + 1], m_TranslucentVertices[idx1 + 2]);
//         glm::vec3 v2(m_TranslucentVertices[idx2], m_TranslucentVertices[idx2 + 1], m_TranslucentVertices[idx2 + 2]);
//
//         //         // Вычисляем нормаль грани
//         //         glm::vec3 edge1 = v1 - v0;
//         //         glm::vec3 edge2 = v2 - v0;
//         //         glm::vec3 normal = glm::cross(edge1, edge2);
//         //
//         //         // Направление от грани к камере
//         //         glm::vec3 toCamera = cameraPos - v0;
//         //
//         //         // Если нормаль направлена от камеры (dot < 0) — грань не видна
//         //         if (glm::dot(normal, toCamera) <= 0.0f) {
//         //             continue;  // Пропускаем эту грань
//         //         }
//
//         glm::vec3 center = (v0 + v1 + v2) / 3.0f;
//         glm::vec3 worldCenter = center + glm::vec3(
//             m_Chunk.m_X * CHUNK_WIDTH,
//             m_Chunk.m_Y * CHUNK_HEIGHT,
//             m_Chunk.m_Z * CHUNK_WIDTH
//         );
//         // const float dist = glm::length(worldCenter - cameraPos);
//         const float dist = glm::dot(worldCenter - cameraPos, worldCenter - cameraPos);
//         visibleFaces.push_back({dist, baseVertexIndex});
//     }
//
//     std::sort(visibleFaces.begin(), visibleFaces.end(), [](const FaceData& a, const FaceData& b) {
//         return a.distance > b.distance;
//     });
//
//
//
//     std::vector<unsigned short> sortedIndices;
//     sortedIndices.reserve(visibleFaces.size() * indicesPerFace);
//
//     for (const FaceData& face : visibleFaces) {
//         for (size_t i = 0; i < indicesPerFace; ++i) {
//             sortedIndices.push_back(m_TranslucentIndices[face.startIndex + i]);
//         }
//     }
//
//     m_TranslucentIndices = std::move(sortedIndices);
//
//     const unsigned int attrs[4] = { 3, 1, 2, 0 };
//     m_TranslucentMesh.AllocateToGpu(
//         m_TranslucentVertices.data(),
//         m_TranslucentVertices.size() / 6,
//         m_TranslucentIndices.data(),
//         m_TranslucentIndices.size(),
//         attrs
//     );
// }

void ChunkRenderer::Tick() {
    if (m_FadeIn) {
        if (m_Opacity < 0.95f) {
            m_Opacity += 0.05f;
        } else {
            if (!m_FadeOut) m_Opacity = 1.0f;
            m_FadeIn = false;
        }
    } else if (m_FadeOut) {
        if (m_Opacity > 0.05f) {
            m_Opacity -= 0.05f;
        } else {
            if (!m_FadeIn) m_Opacity = 0.0f;
            m_FadeOut = false;
        }
    } else if (m_Opacity.changed()){
    	m_Opacity.sync();
    }
}

void ChunkRenderer::StartFadeIn() {
    m_FadeIn = true;
    if (!m_FadeOut) m_Opacity = 0.0f;
    else m_FadeOut = false;
}

void ChunkRenderer::StartFadeOut() {
    m_FadeOut = true;
    if (!m_FadeIn) m_Opacity = 1.0f;
    else m_FadeIn = false;
}



const Chunk& ChunkRenderer::GetChunk() const {
    return m_Chunk;
}


// void ChunkRenderer::BindBlockTextureAtlas(const Texture& textureAtlas) const {
//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_2D, textureAtlas.GetId());
// }
//
// void ChunkRenderer::BindBlockShader(Shader& shader, const float partialTicks, const World& world) const {
//     const Camera camera = world.GetPlayerConst().camera;
//     const glm::mat4 chunkModelMatrix = GetChunkModelMatrix();
//     shader.Enable();
//     shader.SetMat4f("model", chunkModelMatrix);
//     shader.SetMat4f("view", camera.GetViewMatrix(partialTicks));
//     shader.SetMat4f("proj", projMatrix);
//     shader.SetBool("debug", ChunkRenderer::renderDebug);
// }
// void ChunkRenderer::UnindBlockShader(Shader& shader) const {
//     shader.Disable();
// }

glm::mat4 ChunkRenderer::GetChunkModelMatrix() const {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix,
        glm::vec3(
            m_Chunk.m_X * CHUNK_WIDTH,
            m_Chunk.m_Y * CHUNK_HEIGHT,
            m_Chunk.m_Z * CHUNK_DEPTH
        )
        // glm::vec3(
        //     m_Chunk.m_X * CHUNK_WIDTH + 0.5,
        //     m_Chunk.m_Y * CHUNK_HEIGHT + 0.5,
        //     m_Chunk.m_Z * CHUNK_DEPTH + 0.5
        // )
    );
    return modelMatrix;
}

