#ifndef WORLD_H
#define WORLD_H

#include <memory>
#include <vector>
#include <cstdint>
#include <unordered_map>

#include "Chunk.h"

#include "Camera.h"

#include "Player.h"

#include <glm/glm.hpp>

#include "glx/Mesh.h"
#include "glx/Texture.h"
#include "glx/ChunkRenderer.h"

#include "world/block/Block.h"
#include "world/block/Blocks.h"

#include "math/NoiseGenerator.h"

class Window;

class World {
public:
    ~World() { Cleanup(); }
    // World() = default;
    World(const int generatorSeed):
        m_GeneratorSeed(generatorSeed),
        m_HeightNoiseGenerator(m_GeneratorSeed),
        m_DetailNoiseGenerator(m_GeneratorSeed + 1000) {}
        // m_MountainNoiseGenerator(m_GeneratorSeed + 2000) {}

    // Chunk* GetChunk(const int x, const int z);
    Chunk* GetChunkConst(const int x, const int z) const;
    // ChunkRenderer* GetChunkRenderer(const int x, const int z);
    ChunkRenderer* GetChunkRendererConst(const int x, const int z) const;
    Chunk* AddChunk(const int x, const int z);
    Chunk* CreateChunk(const int x, const int z, const bool notifyNeighboors = true);

    void GetGlobalChunkPos(int& resultX, int& resultZ, const double x, const double z) const;

    void RebuildMeshNeighboors(Chunk* chunk) const;
    void RebuildMeshNeighboor(Chunk* chunk, const Direction direction) const;
    int GetGeneratorHeight(const int worldX, const int worldZ) const;
    void GenerateChunkDefault(Chunk* chunk) const;
    void GenerateChunk(Chunk* chunk) const;

    std::vector<ChunkRenderer*> GetVisibleChunks();

    Player& GetPlayer();
    const Player& GetPlayerConst() const;

    bool IsSolidBlock(const Block block) const;

    void SetBlock(const unsigned int blockId, const int x, const int y, const int z) const;
    unsigned int GetBlock(const int x, const int y, const int z) const;

    void Init(const Window& window);
    void Draw(const float partialTicks, const Window& window);
    void Update();

    void GenerateWorld();

    void KeyPressed(const Window& window, const int key);
    void KeyReleased(const Window& window, const int key);

    void MousePressed(const Window& window, const int button);
    void MouseReleased(const Window& window, const int button);
    void MouseWheelScrolled(const Window& window, const double xOffset, const double yOffset);

    void MouseMoved(const Window& window, const double xPos, const double yPos);
    // Camera GetCamera() const;

    unsigned int GetGlobalTicks() const;

    Texture GetBlockTextureAtals() const;

    void Cleanup();
private:
    Player m_Player;
    Texture m_TextureAtlasBlocks;

    Mesh m_SkyboxMesh;
    Shader m_SkyboxShader;
    Texture m_SkyboxTexture;

    const int m_GeneratorSeed;
    NoiseGenerator m_HeightNoiseGenerator;
    NoiseGenerator m_DetailNoiseGenerator;
    // NoiseGenerator m_MountainNoiseGenerator;

    Mesh m_CrosshairMesh;
    Shader m_CrosshairShader;
    bool m_ShowCrosshair = true;

    unsigned int m_GlobalTicks = 0;

    // int m_RenderRadius = 1;
//     int m_RenderRadius = 2;
//    int m_RenderRadius = 4;
    int m_RenderRadius = 12;
    bool m_GenerateChunks = true;
    std::vector<ChunkRenderer*> m_VisibleChunks;
    std::vector<ChunkRenderer*> m_FadeOutChunks;
    std::unordered_map<uint64_t, std::unique_ptr<Chunk>> m_Chunks;
    std::unordered_map<uint64_t, std::unique_ptr<ChunkRenderer>> m_ChunkRenderers;
};

#endif
