#include "World.h"

#include <cstdint>
#include <cstddef>
#include <iostream>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Window.h"

#include "math/Direction.h"

#include "glx/Frustum.h"
#include "glx/MeshGenerator.h"

#include "glx/debug/GlDebugger.h"

void World::SetBlock(const unsigned int blockId, const int x, const int y, const int z) const {
    if (y < 0 || y >= CHUNK_HEIGHT) return;

    int chunkX, chunkZ;
    GetGlobalChunkPos(chunkX, chunkZ, x, z);
    Chunk* chunk = GetChunkConst(chunkX, chunkZ);
    if (!chunk) return;

    const int localX = x - chunkX * CHUNK_WIDTH;
    const int localZ = z - chunkZ * CHUNK_DEPTH;

    chunk->SetBlock(blockId, localX, y % CHUNK_HEIGHT, localZ);
    ChunkRenderer* renderer = GetChunkRendererConst(chunkX, chunkZ);
    if (renderer) {
        renderer->RebuildMesh(*this);
    }

    if (localX == 0) RebuildMeshNeighboor(chunk, Direction::WEST);
    else if (localX == CHUNK_WIDTH - 1) RebuildMeshNeighboor(chunk, Direction::EAST);
    if (localZ == 0) RebuildMeshNeighboor(chunk, Direction::SOUTH);
    else if (localZ == CHUNK_DEPTH - 1) RebuildMeshNeighboor(chunk, Direction::NORTH);
}

unsigned int World::GetBlock(const int x, const int y, const int z) const {
    if (y < 0 || y >= CHUNK_HEIGHT) return 0;

    int chunkX, chunkZ;
    GetGlobalChunkPos(chunkX, chunkZ, x, z);
    Chunk* chunk = GetChunkConst(chunkX, chunkZ);
    if (!chunk) return 0;

    const int localX = x - chunkX * CHUNK_WIDTH;
    const int localZ = z - chunkZ * CHUNK_DEPTH;
    return chunk->GetBlock(localX, y % CHUNK_HEIGHT, localZ);
}

Chunk* World::GetChunkConst(const int x, const int z) const {
    const auto& it = m_Chunks.find(Chunk::PackPos(x, z));
    return it != m_Chunks.end() ? it->second.get() : nullptr;
}
// Chunk* World::GetChunk(const int x, const int z) {
//     Chunk* chunk = GetChunkConst(x, z);
//     if (m_GenerateChunks && !chunk) {
//         CreateChunk(x, z);
//     }
//     return m_GenerateChunks ? GetChunkConst(x, z) : chunk;
// }
ChunkRenderer* World::GetChunkRendererConst(const int x, const int z) const {
    const auto& it = m_ChunkRenderers.find(Chunk::PackPos(x, z));
    return it != m_ChunkRenderers.end() ? it->second.get() : nullptr;
}
// ChunkRenderer* World::GetChunkRenderer(const int x, const int z) {
//     ChunkRenderer* chunkRenderer = GetChunkRendererConst(x, z);
//     if (m_GenerateChunks && !chunkRenderer) {
//         CreateChunk(x, z);
//     }
//     return m_GenerateChunks ? GetChunkRendererConst(x, z) : chunkRenderer;
// }

Chunk* World::AddChunk(const int x, const int z) {
    const uint64_t packedPos = Chunk::PackPos(x, z);

    auto& chunkRef = m_Chunks[packedPos];
    chunkRef = std::make_unique<Chunk>(x, 0, z);
    auto& chunkRendererRef = m_ChunkRenderers[packedPos];
    chunkRendererRef = std::make_unique<ChunkRenderer>(*chunkRef);

    return chunkRef.get();
}

#include <cstdlib>

Chunk* World::CreateChunk(const int x, const int z, const bool notifyNeighboors) {
    Chunk* chunk = AddChunk(x, z);
    GenerateChunk(chunk);
    chunk->CalculateLighting();
    // GetChunkRendererConst(x, z)->Init(*this);
    ChunkRenderer* renderer = GetChunkRendererConst(x, z);
    renderer->Init(*this);
    //TODO Temporary
    renderer->m_Color = (rand() & 0xffffff);
    if (notifyNeighboors) {
        RebuildMeshNeighboors(chunk);
    }

    return chunk;
}

void World::GetGlobalChunkPos(int& resultX, int& resultZ, const double x, const double z) const {
    resultX = (x >= 0) ? x / CHUNK_WIDTH : (x - CHUNK_WIDTH + 1) / CHUNK_WIDTH;
    resultZ = (z >= 0) ? z / CHUNK_DEPTH : (z - CHUNK_DEPTH + 1) / CHUNK_DEPTH;
}

void World::Init(const Window& window) {
    m_TextureAtlasBlocks.Load(
        "./resources/textures/block_atlas.png"
    );

    // Crosshair
    m_CrosshairShader.Load(
        std::filesystem::path("./resources/shaders/crosshair_frag.glsl"),
        std::filesystem::path("./resources/shaders/crosshair_vert.glsl")
    );
    const float crosshairVertices[8] {
        -0.005f, 0.0f,
        0.0046f, 0.0f,

        0.0f, -0.009f,
        0.0f, 0.0075f
    };
    const unsigned short crosshairIndices[4] { 0, 1, 2, 3 };
    const unsigned int crosshairAttrs[2] = { 2, 0 };
    m_CrosshairMesh.AllocateToGpu(
        crosshairVertices, 4, crosshairIndices, 4, crosshairAttrs
    );
    // <...>

    // SkyBox
    m_SkyboxShader.Load(
        std::filesystem::path("./resources/shaders/skybox_frag.glsl"),
        std::filesystem::path("./resources/shaders/skybox_vert.glsl")
    );
    m_SkyboxTexture.Load(
        "./resources/textures/skybox_cubemap.jpg"
    );
    MeshGenerator::allocateSkyboxMeshForTrianglesWithUV(
        m_SkyboxMesh,
        -1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        // 800.0f, 600.0f,
        // 200.0f, 200.f
        // 2048.0f, 1536.0f,
        // 512.0f, 512.f
        4096.0f, 3072.0f,
        1024.0f, 1024.0f
    );
    // <...>

    GenerateWorld();

    if (IS_CHUNK_RENDERER_DEBUG) {
        std::cout << "Common size of all chunks meshes: " << ChunkRenderer::format_size_as_str(ChunkRenderer::commonSize) << '\n';
    }
    // chunkRenderer.Init(window);
    GetPlayer().Init(window);
    GetPlayer().SetPos(0.0, 50.0, 0.0);
    // GetPlayer().SetPos(0.0, 100.0, 0.0);

    // m_VisibleChunks.reserve(m_RenderRadius * m_RenderRadius * 4);
}

void World::RebuildMeshNeighboors(Chunk* chunk) const {
    if (chunk) {
        const int x = chunk->m_X;
        const int z = chunk->m_Z;
        ChunkRenderer* eastRenderer = GetChunkRendererConst(x + 1, z);
        if (eastRenderer) eastRenderer->RebuildMesh(*this);
        ChunkRenderer* westRenderer = GetChunkRendererConst(x - 1, z);
        if (westRenderer) westRenderer->RebuildMesh(*this);
        ChunkRenderer* northRenderer = GetChunkRendererConst(x, z + 1);
        if (northRenderer) northRenderer->RebuildMesh(*this);
        ChunkRenderer* southRenderer = GetChunkRendererConst(x, z - 1);
        if (southRenderer) southRenderer->RebuildMesh(*this);
    }
}

void World::RebuildMeshNeighboor(Chunk* chunk, const Direction direction) const {
    if (chunk) {
        const int chunkX = chunk->m_X;
        const int chunkZ = chunk->m_Z;
        switch (direction) {
            case Direction::EAST: {
                ChunkRenderer* eastRenderer = GetChunkRendererConst(chunkX + 1, chunkZ);
                if (eastRenderer) eastRenderer->RebuildMesh(*this);
                break;
            }
            case Direction::WEST: {
                ChunkRenderer* westRenderer = GetChunkRendererConst(chunkX - 1, chunkZ);
                if (westRenderer) westRenderer->RebuildMesh(*this);
                break;
            }
            case Direction::NORTH: {
                ChunkRenderer* northRenderer = GetChunkRendererConst(chunkX, chunkZ + 1);
                if (northRenderer) northRenderer->RebuildMesh(*this);
                break;
            }
            case Direction::SOUTH: {
                ChunkRenderer* southRenderer = GetChunkRendererConst(chunkX, chunkZ - 1);
                if (southRenderer) southRenderer->RebuildMesh(*this);
                break;
            }
            default: { break; }
        }
    }
}


#include <cmath>

int World::GetGeneratorHeight(const int worldX, const int worldZ) const {
    const double continent = m_HeightNoiseGenerator.GetNoiseFractal(
        // worldX * 0.02, worldZ * 0.02, 3, 0.375, 2.25 // Classic
        worldX * 0.02, worldZ * 0.02, 3, 0.355, 1.0
        // worldX * 0.03, worldZ * 0.03, 3, 0.375, 2.25

        // worldX * 0.025, worldZ * 0.025, 3, 0.39, 2.2
        // worldX * 0.025, worldZ * 0.025, 3, 0.5, 2.0
        // worldX * 0.01, worldZ * 0.01, 3, 0.5, 2.0
    );
    // const double hills = m_DetailNoiseGenerator.GetNoiseFractal(
    //     worldX * 0.004, worldZ * 0.004, 4, 0.5, 2.0
    // );
    // const double details = m_DetailNoiseGenerator.GetNoiseFractal(
    //     worldX * 0.04, worldZ * 0.04, 3, 0.5, 2.0
    // );
    // const double mountainMask = m_MountainNoiseGenerator.GetNoiseFractal(
    //     worldX * 0.003, worldZ * 0.003, 6, 0.5, 2.0
    // );

    // double mountainStrength = 0.0;
    // if (mountainMask > 0.4) {
    //     mountainStrength = (mountainMask - 0.4) / 0.4;
    //     mountainStrength = std::min(1.0, std::max(0.0, mountainStrength));
    //     mountainStrength = mountainStrength * mountainStrength * (3 - 2 * mountainStrength);
    // }
    // const double mountainShape = m_MountainNoiseGenerator.GetNoiseFractal(
    //     worldX * 0.008, worldZ * 0.008, 5, 0.5, 2.0
    // );

    // const double mountains = mountainStrength * mountainShape * 25.0;

    const int baseHeight = 8;
    const int heightVariation = (int)(
        // continent * 40.0
        // continent * 20.0
        continent * 20.0
        // hills * 8.0 +
        // details * 4.0
    );

    return std::max(
        // std::min(baseHeight + baseHeight + (int)mountains, CHUNK_HEIGHT - 1),
        std::min(baseHeight + heightVariation, CHUNK_HEIGHT - 1),
        0
    );
}

// void World::GenerateChunkDefault(Chunk* chunk) const {
//     if (chunk) {
//         for (int zz = 0; zz < CHUNK_DEPTH; zz++) {
//             for (int xx = 0; xx < CHUNK_WIDTH; xx++) {
//                 const int worldX = chunk->m_X * CHUNK_WIDTH + xx;
//                 const int worldZ = chunk->m_Z * CHUNK_DEPTH + zz;
//
//                 const int height = GetGeneratorHeight(worldX, worldZ);
//
//                 const double detailNoise = m_DetailNoiseGenerator.GetNoise2D(
//                     worldX * 0.08, worldZ * 0.08
//                 );
//
//                 const double treeNoise = m_HeightNoiseGenerator.GetNoise2D(
//                     worldX * 0.1, worldZ * 0.1
//                 );
//
//                 const int waterLevel = 22;
//
//                 // Заполняем водой ВЕСЬ столбец до waterLevel, независимо от height
//                 for (int wy = 0; wy < waterLevel; wy++) {
//                     chunk->SetBlock(BlockIds::WATER, xx, wy, zz);
//                 }
//
//                 // Поверх воды генерируем землю
//                 for (int yy = waterLevel; yy < height; yy++) {
//                     unsigned int blockId = BlockIds::STONE;
//
//                     if (yy == 0) {
//                         blockId = BlockIds::BEDROCK;
//                     }
//                     else if (yy > height - 4) {
//                         blockId = BlockIds::DIRT;
//                     }
//
//                     if (yy > 0 && yy < height - 4 && detailNoise > 0.9) {
//                         blockId = BlockIds::GRAVEL;
//                     }
//
//                     chunk->SetBlock(blockId, xx, yy, zz);
//                 }
//
//                 if (height > waterLevel) {
//                     chunk->SetBlock(BlockIds::GRASS, xx, height - 1, zz);
//                 }
//
//                 // Деревья
//                 if (height >= waterLevel && height < 35) {
//                     if (treeNoise > 0.85 && chunk->GetBlock(xx, height - 1, zz) == BlockIds::GRASS) {
//                         if (xx >= 2 && xx < CHUNK_WIDTH - 2 && zz >= 2 && zz < CHUNK_DEPTH - 2) {
//                             for (int ty = 0; ty < 4; ty++) {
//                                 chunk->SetBlock(BlockIds::PLANKS, xx, height + ty, zz);
//                             }
//
//                             for (int ly = 2; ly < 5; ly++) {
//                                 for (int lx = -2; lx <= 2; lx++) {
//                                     for (int lz = -2; lz <= 2; lz++) {
//                                         if (lx == 0 && lz == 0 && ly < 4) continue;
//                                         if (abs(lx) == 2 && abs(lz) == 2) continue;
//                                         const int leafY = height + ly;
//                                         if (leafY < CHUNK_HEIGHT) {
//                                             chunk->SetBlock(BlockIds::LEAVES, xx + lx, leafY, zz + lz);
//                                         }
//                                     }
//                                 }
//                             }
//                         }
//                     }
//                 }
//             }
//         }
//     }
// }

void World::GenerateChunk(Chunk* chunk) const {
    // GenerateChunkDefault(chunk);
    if (chunk) {
        unsigned int blockId = 0;
        for (int zz = 0; zz < CHUNK_WIDTH; zz ++) {
            for (int xx = 0; xx < CHUNK_WIDTH; xx ++) {
                const int worldX = chunk->m_X * CHUNK_WIDTH + xx;
                const int worldZ = chunk->m_Z * CHUNK_WIDTH + zz;
                const int height = GetGeneratorHeight(worldX, worldZ);

                const int waterLevel = 16;
                for (int yy = 0; yy < waterLevel; yy ++) {
                    chunk->SetBlock(Blocks::WATER, xx, yy, zz);
                }
                for (int yy = 0; yy < height; yy ++) {
                    if (yy == height - 1) {
                        if (chunk->GetBlock(xx, yy + 1, zz) == Blocks::WATER) {
                            if (yy >= waterLevel - 1 - 1) {
                                blockId = Blocks::GRAVEL;
                            } else {
                                blockId = Blocks::DIRT;
                            }
                        } else {
                            blockId = Blocks::GRASS;
                        }
                        // blockId = BlockIds::GRASS;
                        // blockId = BlockIds::RED_STAINED_GLASS;
                        // blockId = BlockIds::BLUE_STAINED_GLASS;
                        // blockId = BlockIds::GLASS_CLASSIC;
                    } else if (yy > height - 4 - 1) {
                        // blockId = BlockIds::BLUE_STAINED_GLASS;
                        blockId = Blocks::DIRT;
                        // blockId = BlockIds::GLASS_CLASSIC;
                    } else if (yy == 0) {
                        blockId = Blocks::BEDROCK;
                    } else {
                        // blockId = BlockIds::BLUE_STAINED_GLASS;
                        blockId = Blocks::STONE;
                        // blockId = BlockIds::GLASS_CLASSIC;
                        // blockId = BlockIds::GREEN_STAINED_GLASS;
                    }
                    chunk->SetBlock(blockId, xx, yy, zz);
                }

                const double treeNoise = m_HeightNoiseGenerator.GetNoise2D(
                    // worldX * 0.5, worldZ * 0.5
                    worldX * 0.45, worldZ * 0.45
                    // worldX, worldZ
                );
                if (treeNoise >= 0.8 && chunk->GetBlock(xx, height - 1, zz) == Blocks::GRASS) {
                // if (treeNoise >= 0.95 && chunk->GetBlock(xx, height - 1, zz) == BlockIds::GRASS) {
                // if (treeNoise <= 0.995 && chunk->GetBlock(xx, height - 1, zz) == BlockIds::GRASS) {
                    if (xx >= 5 && xx < CHUNK_WIDTH - 5 && zz >= 5 && zz < CHUNK_WIDTH - 5) {
                        for (int ly = 2; ly < 6; ly++) {
                            for (int lx = -2; lx <= 2; lx++) {
                                for (int lz = -2; lz <= 2; lz++) {
                                    // if (lx == 0 && lz == 0 && ly < 5) continue;
                                    if (ly == 5 && (glm::abs(lx) == 2 || glm::abs(lz) == 2)) continue;
                                    if (glm::abs(lx) == 2 && glm::abs(lz) == 2) continue;
                                    // if (abs(lx) == 2 && abs(lz) == 2) continue;
                                    const int leafY = height + ly;
                                    if (leafY < CHUNK_HEIGHT) {
                                        chunk->SetBlock(Blocks::LEAVES, xx + lx, leafY, zz + lz);
                                    }
                                }
                            }
                        }
                        for (int ty = 0; ty < 4; ty++) {
                            chunk->SetBlock(Blocks::LOG, xx, height + ty, zz);
                        }
                    }
                }
            }
        }
    }
    // if (chunk) {
    //     const int height = 16;
    //     for (int zz = 0; zz < CHUNK_DEPTH; zz ++) {
    //         for (int xx = 0; xx < CHUNK_WIDTH; xx ++) {
    //             for (int yy = 0; yy < height; yy ++) {
    //                 chunk->SetBlock(1, xx, yy, zz);
    //             }
    //         }
    //     }
    // }

    // if (chunk) {
    //     unsigned int blockId = BlockIds::AIR;
    //     for (int zz = 0; zz < CHUNK_WIDTH; zz ++) {
    //         for (int xx = 0; xx < CHUNK_WIDTH; xx ++) {
    //             const int worldX = chunk->m_X * CHUNK_WIDTH + xx;
    //             const int worldZ = chunk->m_Z * CHUNK_WIDTH + zz;
    //             const int height = GetGeneratorHeight(worldX, worldZ);
    //             // const int waterLevel = 16;
    //             // for (int yy = 0; yy < waterLevel; yy ++) {
    //             //     chunk->SetBlock(BlockIds::WATER, xx, yy, zz);
    //             // }
    //             for (int yy = 0; yy < height; yy ++) {
    //                 if (yy == height - 1) {
    //                     // if (chunk->GetBlock(xx, yy + 1, zz) == BlockIds::WATER) {
    //                     //     if (yy >= waterLevel - 1 - 1) {
    //                     //         blockId = BlockIds::GRAVEL;
    //                     //     } else {
    //                     //         blockId = BlockIds::DIRT;
    //                     //     }
    //                     // } else {
    //                     //     blockId = BlockIds::GRASS;
    //                     // }
    //                     blockId = BlockIds::GRASS;
    //                     // blockId = BlockIds::RED_STAINED_GLASS;
    //                     // blockId = BlockIds::BLUE_STAINED_GLASS;
    //                 } else if (yy > height - 4 - 1) {
    //                     blockId = BlockIds::DIRT;
    //                     // blockId = BlockIds::BLUE_STAINED_GLASS;
    //                 } else if (yy == 0) {
    //                     blockId = BlockIds::BEDROCK;
    //                 } else {
    //                     blockId = BlockIds::STONE;
    //                     // blockId = BlockIds::GREEN_STAINED_GLASS;
    //                 }
    //                 chunk->SetBlock(blockId, xx, yy, zz);
    //             }
    //         }
    //     }
    // }
}

void World::GenerateWorld() {
    const int size = m_RenderRadius * 2;
//     const int size = 4;
    const int length = size * size;
//     const int length = size;

    m_Chunks.reserve(length);
    m_ChunkRenderers.reserve(length);
    for (int i = 0; i < length; i ++) {
//        const int chunkX = i % size - size / 2;
//        const int chunkZ = i / size - size / 2;
        const int chunkX = i % (size / 2) - size / 4;
		const int chunkZ = i / (size / 2) - size / 4;

        Chunk* chunk = AddChunk(chunkX, chunkZ);
        GenerateChunk(chunk);
        chunk->CalculateLighting();

        //TODO For tests (Remove later)
        // if (i == 8) {
        //     ChunkRenderer* render = GetChunkRendererConst(chunkX, chunkZ);
        //     if (render) render->m_Color = 0xCBCB00;
        //     if (chunk) {
        //         chunk->SetBlock(1, 6, 20, 1);
        //         for (int j = 0; j < 3 * 3; j ++) {
        //             chunk->SetBlock(1, j % 3, 21, j / 3);
        //         }
        //         for (int j = 0; j < 4 * 4; j ++) {
        //             chunk->SetBlock(1, 5 + j % 4, 22, 5 + j / 4);
        //         }
        //         for (int j = 0; j < 3 * 3; j ++) {
        //             chunk->SetBlock(1, 10 + j % 3, 23, 10 + j / 3);
        //         }
        //         chunk->SetBlock(1, 14, 24, 14);
        //     }
        // }
    }
    for (auto& [_, renderer] : m_ChunkRenderers) {
        renderer->Init(*this);
        m_VisibleChunks.emplace_back(renderer.get());
    }
    // for (auto& [_, chunk] : chunks) {
    //     if (chunk && chunk->IsDirty()) {
    //         ChunkRenderer* renderer = GetChunkRendererConst(chunk->m_X, chunk->m_Z);
    //         if (renderer) {
    //             renderer->RebuildMesh(*this);
    //         }
    //     }
    // }
}

void World::Draw(const float partialTicks, const Window& window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Skybox
    if (!ChunkRenderer::renderDebug) {
        glm::mat4 view = glm::mat4(glm::mat3(m_Player.GetViewMatrix(partialTicks)));
        glm::mat4 proj = Camera::GetProjectionMatrix();
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        m_SkyboxShader.Enable();
        // m_SkyboxShader.SetMat4f("model", model);
        m_SkyboxShader.SetMat4f("view", view);
        m_SkyboxShader.SetMat4f("proj", proj);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_SkyboxTexture.GetId());
        m_SkyboxMesh.Draw();
        m_SkyboxShader.Disable();
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
    }
    // <...>

    std::vector<ChunkRenderer*> chunksToRender = m_VisibleChunks;

    // for (const ChunkRenderer* renderer : chunksToRender) {
    //     if (!renderer) continue;
    //
    //     const glm::mat4 chunkModelMatrix = renderer->GetChunkModelMatrix();
    //     m_ChunkShader.SetMat4f("model", chunkModelMatrix);
    //
    //     renderer->DrawSolidBlocks();
    // }

    // std::sort(chunksToRender.begin(), chunksToRender.end(), [this](const ChunkRenderer* prev, const ChunkRenderer* now) {
    //     const double playerPrevDist = glm::distance(m_Player.camera.GetPos(), prev->GetChunk().GetCenter());
    //     const double playerNowDist = glm::distance(m_Player.camera.GetPos(), now->GetChunk().GetCenter());
    //     return playerPrevDist > playerNowDist;
    // });

    const Camera camera = GetPlayerConst().camera;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureAtlasBlocks.GetId());

    Shader* chunkShader = nullptr;
    for (int j = 0; j < 3; j ++) {
        if (j == 0) {
        	chunkShader = &Shader::s_ChunkShader;
        }
//        else if (j == 1) {
//        	chunkShader = &Shader::s_ChunkTransparentShader;
//        } if (j == 2) {
//            chunkShader = &Shader::s_ChunkTranslucentShader;
//        }
        chunkShader->Enable();
        chunkShader->SetMat4f("view", m_Player.GetViewMatrix(partialTicks));
        chunkShader->SetMat4f("proj", camera.GetProjectionMatrix());
        chunkShader->SetInt("uAtlasWidth", ATLAS_WIDTH);
        chunkShader->SetInt("uAtlasHeight", ATLAS_HEIGHT);
        chunkShader->SetInt("uTextureWidth", TEXTURE_WIDTH);
		chunkShader->SetInt("uTextureHeight", TEXTURE_HEIGHT);
//        chunkShader->SetFloat("uChunkWidth", CHUNK_WIDTH);
//        chunkShader->SetFloat("uChunkHeight", CHUNK_HEIGHT);
        chunkShader->SetBool("debug", ChunkRenderer::renderDebug);
        for (size_t i = 0; i < chunksToRender.size(); i ++) {
            const ChunkRenderer* renderer = chunksToRender[i];
            if (!renderer || renderer->m_FrustumCulledFlag) continue;
            auto fadeOutIt = std::find(m_FadeOutChunks.begin(), m_FadeOutChunks.end(), renderer);
            if (!renderer->m_FadeOut && fadeOutIt != m_FadeOutChunks.end()) continue;

            if (j == 1 && !renderer->m_HasTransparent) continue;
            else if (j == 2 && !renderer->m_HasTranslucent) continue;

            const glm::mat4 chunkModelMatrix = renderer->GetChunkModelMatrix();
            chunkShader->SetMat4f("model", chunkModelMatrix);
            chunkShader->SetFloat("uOpacity", glm::mix(renderer->m_Opacity.prev(), renderer->m_Opacity.now(), partialTicks));
//            chunkShader->SetFloat("uOpacity", renderer->m_Opacity);

            if (j == 0) {
                renderer->DrawSolidBlocks();
            } else if (j == 1 && renderer->m_HasTransparent) {
                renderer->DrawTransparentBlocks();
            } else if (j == 2 && renderer->m_HasTranslucent) {
                renderer->DrawTranslucentBlocks();
            }
        }
        chunkShader->Disable();
    }
    // chunkShader->Disable();
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    if (m_ShowCrosshair) {
        m_CrosshairShader.Enable();
        m_CrosshairMesh.Draw(GL_LINES);
        m_CrosshairShader.Disable();
    }

    m_Player.Draw(partialTicks, *this, window);
    // m_Player.Draw(window);
    // const auto& it = chunks.find();
    // for (auto& chunkRenderer : chunkRenderers) {
    //     chunkRenderer->Draw(m_ChunkShader, *this);
    // }

//     std::cout << player.camera.m_Pos.x << ", " << player.camera.m_Pos.z << '\n';
//
    // m_ChunkRenderer.Draw(m_ChunkShader, *this);

    GlDebugger::glCheckError();
}

void World::Update() {
    frst::Frustum frustum = frst::ExtractFrustum(
        m_Player.camera.GetProjectionMatrix(),
        m_Player.GetViewMatrix()
    );

    const glm::vec3& playerCamPos = m_Player.camera.GetPos();

    // std::for_each(m_VisibleChunks.begin(), m_VisibleChunks.end(), [](ChunkRenderer* renderer) {
    //     renderer->m_InRenderList = false;
    // });
    // m_VisibleChunks.clear();
    std::vector<ChunkRenderer*> visibleChunks;
    // m_VisibleChunks.shrink_to_fit();
    int playerChunkX, playerChunkZ;
    GetGlobalChunkPos(playerChunkX, playerChunkZ, m_Player.GetX(), m_Player.GetZ());
    for (int xx = 0; xx < m_RenderRadius * 2; xx ++) {
        for (int zz = 0; zz < m_RenderRadius * 2; zz ++) {
            const int dx = xx - m_RenderRadius;
            const int dz = zz - m_RenderRadius;
            const int dPlayerChunkX = playerChunkX + dx;
            const int dPlayerChunkZ = playerChunkZ + dz;
            if (dx * dx + dz * dz >= m_RenderRadius * m_RenderRadius) continue;

            // const float minX = dPlayerChunkX * CHUNK_WIDTH;
            // const float minZ = dPlayerChunkZ * CHUNK_WIDTH;
            // const float maxX = minX + CHUNK_WIDTH;
            // const float maxZ = minZ + CHUNK_WIDTH;
            // const float minY = 0.0f;
            // const float maxY = minY + CHUNK_HEIGHT;
            //
            // if (!frustum.IsInAABB(minX, minY, minZ, maxX, maxY, maxZ)) continue;

            ChunkRenderer* renderer = GetChunkRendererConst(dPlayerChunkX, dPlayerChunkZ);
            if (!renderer) {
                if (!m_GenerateChunks) continue;

                Chunk* chunk = CreateChunk(dPlayerChunkX, dPlayerChunkZ);
                if (!chunk) continue;

                renderer = GetChunkRendererConst(chunk->m_X, chunk->m_Z);
            }

            // if (renderer) renderer->Draw(partialTicks, m_ChunkShader, m_TextureAtlasBlocks, *this);
            if (renderer) {
                const float minX = dPlayerChunkX * CHUNK_WIDTH;
                const float minZ = dPlayerChunkZ * CHUNK_WIDTH;
                const float maxX = minX + CHUNK_WIDTH;
                const float maxZ = minZ + CHUNK_WIDTH;
                const float minY = 0.0f;
                const float maxY = minY + renderer->GetChunk().m_MaxPlacedHeight + 6.0f;
                // const float maxY = minY + CHUNK_HEIGHT;

                renderer->m_FrustumCulledFlag = !frustum.IsInAABB(minX, minY, minZ, maxX, maxY, maxZ);
                if (!renderer->m_FrustumCulledFlag) {
                    if (m_GlobalTicks % 10 == 0) {
                        renderer->SortAndAllocateTranslucentFaces(playerCamPos);
                    }
                }

                visibleChunks.emplace_back(renderer);
                // m_VisibleChunks.emplace_back(renderer);
                // // renderer->m_InRenderList = true;
                // renderer->DrawSolidBlocks(partialTicks, *this);
            }
        }
    }

    std::vector<ChunkRenderer*> fadeOutChunks;
    for (ChunkRenderer* renderer : m_VisibleChunks) {
        auto it = std::find(visibleChunks.begin(), visibleChunks.end(), renderer);
        if (it == visibleChunks.end()) {
            auto fadeOutIt = std::find(m_FadeOutChunks.begin(), m_FadeOutChunks.end(), renderer);
            if (fadeOutIt == m_FadeOutChunks.end()) {
                renderer->m_InRenderList = false;
                // if (!renderer->m_FadeOut) {
                renderer->StartFadeOut();
                // }
            }
            fadeOutChunks.emplace_back(renderer);
            // renderer->StartFadeOut();
            // visibleChunks.emplace_back(renderer);
        }
    }
    for (ChunkRenderer* renderer : visibleChunks) {
        auto visibleIt = std::find(m_VisibleChunks.begin(), m_VisibleChunks.end(), renderer);
        if (visibleIt == m_VisibleChunks.end()) {
            renderer->m_InRenderList = true;
            // if (!renderer->m_FadeIn) {
            renderer->StartFadeIn();
            // }
        } else {
            auto fadeOutIt = std::find(m_FadeOutChunks.begin(), m_FadeOutChunks.end(), renderer);
            if (fadeOutIt != m_FadeOutChunks.end()) {
                renderer->m_InRenderList = true;
                renderer->StartFadeIn();
            }
        }
    }

    //TODO REMOVE
    // std::cout << "visibleChunks: ";
    // if (visibleChunks.empty()) {
    //     std::cout << "empty\n";
    // } else {
    //     for (ChunkRenderer* renderer : visibleChunks) {
    //         std::cout << renderer << '\n';
    //     }
    // }
    // std::cout << "m_VisibleChunks:";
    // if (m_VisibleChunks.empty()) {
    //     std::cout << "empty\n";
    // } else {
    //     for (ChunkRenderer* renderer : m_VisibleChunks) {
    //         std::cout << renderer << '\n';
    //     }
    // }
    //<...>

    m_FadeOutChunks.clear();
    for (ChunkRenderer* renderer : fadeOutChunks) {
        if (renderer->m_FadeOut) m_FadeOutChunks.emplace_back(renderer);
    }

    std::sort(visibleChunks.begin(), visibleChunks.end(), [&playerCamPos](const ChunkRenderer* prev, const ChunkRenderer* now) {
        const Chunk& prevChunk = prev->GetChunk();
        const Chunk& nowChunk = now->GetChunk();

        const float prevDistX = prevChunk.m_GlobalCenterX - playerCamPos.x;
        const float prevDistZ = prevChunk.m_GlobalCenterZ - playerCamPos.z;
        const float nowDistX = nowChunk.m_GlobalCenterX - playerCamPos.x;
        const float nowDistZ = nowChunk.m_GlobalCenterZ - playerCamPos.z;

        return (prevDistX * prevDistX + prevDistZ * prevDistZ) > (nowDistX * nowDistX + nowDistZ * nowDistZ);
    });

    for (ChunkRenderer* renderer : m_FadeOutChunks) {
        visibleChunks.emplace_back(renderer);
        // std::cout << "Fucking fade out chunk: " << renderer->m_Opacity << "; " << renderer->m_FadeOut << '\n';
    }

    for (ChunkRenderer* renderer : visibleChunks) {
        // if (renderer->m_FadeOut) {
        //     std::cout << "Fucking fade out chunk(s): " << renderer->m_Opacity << "; " << renderer->m_FadeOut << '\n';
        // }

        // std::cout << "Fucking all chunk(s): "
        //     << "Opacity: " << renderer->m_Opacity << "; "
        //     << "FadeOut: " << (renderer->m_FadeOut ? "true" : "false") << "; "
        //     << "FadeIn: " << (renderer->m_FadeIn ? "true" : "false") << "; "
        //     << '\n';

        // if (!renderer->m_FrustumCulledFlag) renderer->Tick();
        renderer->Tick();
    }

    m_VisibleChunks = std::move(visibleChunks);

    GetPlayer().Update(*this);

    m_GlobalTicks ++;
}

void World::KeyPressed(const Window& window, const int key) {
    if (key == GLFW_KEY_F4) {
        ChunkRenderer::ToggleRenderDebug();
    }
    if (key == GLFW_KEY_F1) {
        m_ShowCrosshair = !m_ShowCrosshair;
    }
    if (key == GLFW_KEY_F5) {
        m_GenerateChunks = !m_GenerateChunks;
        std::cout << "Chunks generation: " << m_GenerateChunks << '\n';
    }

    GetPlayer().KeyPressed(window, key);
}
void World::KeyReleased(const Window& window, const int key) {
    GetPlayer().KeyReleased(window, key);
}

void World::MousePressed(const Window& window, const int button) {
    GetPlayer().MousePressed(window, *this, button);
}

void World::MouseReleased(const Window& window, const int button) {
    GetPlayer().MouseReleased(window, *this, button);
}
void World::MouseWheelScrolled(const Window& window, const double xOffset, const double yOffset) {
    GetPlayer().MouseWheelScrolled(window, xOffset, yOffset);
}


void World::MouseMoved(const Window& window, const double xPos, const double yPos) {
    GetPlayer().MouseMoved(window, xPos, yPos);
}

std::vector<ChunkRenderer*> World::GetVisibleChunks() {
    return m_VisibleChunks;
}


Player& World::GetPlayer() {
    return m_Player;
}
const Player& World::GetPlayerConst() const {
    return m_Player;
}

bool World::IsSolidBlock(const Block block) const {
    return block != Blocks::AIR && block != Blocks::WATER;
}

unsigned int World::GetGlobalTicks() const { return m_GlobalTicks; }

Texture World::GetBlockTextureAtals() const {
    return m_TextureAtlasBlocks;
}

void World::Cleanup() {
}
