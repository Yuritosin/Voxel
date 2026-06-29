#ifndef CHUNK_H
#define CHUNK_H

#include <array>
#include <cmath>
#include <queue>
#include <utility>
#include <cstdint>

#include <glm/glm.hpp>

#include "options/Options.h"

#include "world/block/Block.h"
#include "world/block/Blocks.h"

// #include "math/NoiseGenerator.h"
class World;
class NoiseGenerator;

class Chunk {
public:
    static uint64_t PackPos(const int x, const int z);
    static void UnpackPos(const uint64_t packedPos, int& x, int& z);
    static std::pair<int, int> UnpackPos(const uint64_t packedPos);

    const int m_X;
    const int m_Y;
    const int m_Z;
    const double m_GlobalCenterX;
    const double m_GlobalCenterY;
    const double m_GlobalCenterZ;
    int m_MaxPlacedHeight = 0;

    ~Chunk();
    Chunk(const int x, const int y, const int z);

    // void Update(const World& world); // Not using yet

    void CalculateLighting();
//    void RecalcSkyLightAtPos(const unsigned int x, const unsigned int y, const unsigned z);
    inline void CalcSkyLightAtPos(const unsigned int x, const unsigned z, const bool debug = true);
    void RecalcBlockLight(const unsigned int x, const unsigned int y, const unsigned z);
    uint8_t GetSkyLightLevel(const unsigned int x, const unsigned int y, const unsigned z);
    uint8_t GetBlockLightLevel(const unsigned int x, const unsigned int y, const unsigned z);
    uint8_t GetLightLevel(const unsigned int x, const unsigned int y, const unsigned z);

    void SetBlockUnsafe(const Block block, const int index);
    void SetBlockUnsafe(const Block block, const int x, const int y, const int z);
    void SetBlock(const Block block, const int x, const int y, const int z);
    Block GetBlock(const int x, const int y, const int z) const;

    glm::vec3 GetCenter() const;

    void MarkDirty();
    bool IsDirty() const;

    void Cleanup();
private:
    bool m_Dirty = false;

    std::queue<uint8_t> m_LightQueue;
    const unsigned int m_LightQueueBatch = 256;

    // std::array<unsigned int, CHUNK_SIZE> blocks = {0};
    uint8_t m_SkyLight[CHUNK_SIZE] = { false };
    Block blocks[CHUNK_SIZE] = { };
};

#endif
