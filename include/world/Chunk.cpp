#include "Chunk.h"

static const int64_t offsetBits = 1LL << 31;

static int UnpackX(const uint64_t packedPos) {
    return (int)((int64_t)(packedPos >> 32) - offsetBits);
}
static int UnpackZ(const uint64_t packedPos) {
    return (int)((int64_t)(packedPos & 0xffffffff) - offsetBits);
}

uint64_t Chunk::PackPos(const int x, const int z) {
    const int64_t nx = (int64_t)x + offsetBits;
    const int64_t nz = (int64_t)z + offsetBits;
    return ((uint64_t)nx << 32) | (uint64_t)nz;
}

void Chunk::UnpackPos(const uint64_t packedPos, int& x, int& z) {
    x = UnpackX(packedPos);
    z = UnpackZ(packedPos);
}
std::pair<int, int> Chunk::UnpackPos(const uint64_t packedPos) {
    return std::make_pair(UnpackX(packedPos), UnpackZ(packedPos));
}


Chunk::~Chunk() {
    Cleanup();
}

#include <cmath>
#include <cstdlib>
#include <algorithm>

Chunk::Chunk(const int x, const int y, const int z):
    m_X(x), m_Y(y), m_Z(z),
    m_GlobalCenterX(m_X * CHUNK_WIDTH + static_cast<double>(CHUNK_WIDTH) / 2.0),
    m_GlobalCenterY(m_Y * CHUNK_HEIGHT + static_cast<double>(CHUNK_HEIGHT) / 2.0),
    m_GlobalCenterZ(m_Z * CHUNK_WIDTH + static_cast<double>(CHUNK_WIDTH) / 2.0) {
    // std::fill(std::begin(blocks), std::end(blocks), 1);
    // for (int zz = 0; zz < CHUNK_DEPTH; zz ++) {
    //     for (int yy = 0; yy < 16; yy ++) {
    //         for (int xx = 0; xx < CHUNK_WIDTH; xx ++) {
    //             SetBlockUnsafe(1, xx, yy, zz);
    //         }
    //     }
    // }
    // std::generate(blocks, blocks + CHUNK_SIZE, []() { return (rand() % 32 / 31); });
    // std::generate(blocks, blocks + CHUNK_SIZE, []() { return 1 - (rand() % 64 / 63); });
}

//#include <iostream>
void Chunk::CalculateLighting() {
	for (int zz = 0; zz < CHUNK_WIDTH; zz ++) {
		for (int xx = 0; xx < CHUNK_WIDTH; xx ++) {
			CalcSkyLightAtPos(xx, zz);
		}
	}
}
inline void Chunk::CalcSkyLightAtPos(const unsigned int x, const unsigned z, const bool debug) {
	bool virgin = true;
	int8_t light = 15;
	for (int yy = CHUNK_HEIGHT - 1; yy >= 0; yy --) {
		const Block block = GetBlock(x, yy, z);
		const unsigned int index = x + yy * CHUNK_WIDTH + z * (CHUNK_WIDTH * CHUNK_HEIGHT);
		if (debug) {
//			std::cout << "yy: " << yy <<
//				", light: " << (int)light <<
//				", blockId: " << GetRawId(block) <<
//				", IsAir: " << IsAir(block) <<
//				", IsOpaque: " << IsOpaque(block) <<
//				", IsTransparent: " << IsTransparent(block) <<
//				", IsTranslucent: " << IsTranslucent(block) <<
//				", virgin: " << virgin <<
//				", x: " << x <<
//				", z: " << z <<
//				'\n';
		}
		if (light <= 0) {
			m_SkyLight[index] = 0;
			continue;
		}
		if (virgin) {
			if (IsAir(block)) {
				m_SkyLight[index] = light;
			} else if (!IsOpaque(block)) {
				m_SkyLight[index] = light;
				light--;
			} else if (IsOpaque(block)) {
				virgin = false;
				m_SkyLight[index] = light;
				light --;
			}
		} else {
			m_SkyLight[index] = light;
			light--;
		}
	}
}
void Chunk::RecalcBlockLight(const unsigned int x, const unsigned int y, const unsigned z) {
	//TODO Not implemented yet
}

//#include <iostream>
uint8_t Chunk::GetSkyLightLevel(const unsigned int x, const unsigned int y, const unsigned z) {
	const unsigned int index = x + y * CHUNK_WIDTH + z * (CHUNK_WIDTH * CHUNK_HEIGHT);
//	const auto l = m_SkyLight[index] ? 15 : 0;
//	std::cout << "Try to get sky light at [x: " << x << ", y: " << y << ", z: " << z << "], level: " << l << '\n';
//	return m_SkyLight[index] ? 15 : 0;
	return m_SkyLight[index];
}
uint8_t Chunk::GetBlockLightLevel(const unsigned int x, const unsigned int y, const unsigned z) {
	return 0;
}
uint8_t Chunk::GetLightLevel(const unsigned int x, const unsigned int y, const unsigned z) {
	return std::max(GetSkyLightLevel(x, y, z), GetBlockLightLevel(x, y, z));
}

void Chunk::SetBlockUnsafe(const Block block, const int index) {
    blocks[index] = block;

    const unsigned int z = index / (CHUNK_WIDTH * CHUNK_HEIGHT);
    const unsigned int remainder = index % (CHUNK_WIDTH * CHUNK_HEIGHT);
    const int y = remainder / CHUNK_WIDTH;
    const unsigned int x = remainder % CHUNK_WIDTH;

    m_MaxPlacedHeight = std::max(m_MaxPlacedHeight, y);
//    RecalcSkyLightAtPos(x, y, z);
    CalcSkyLightAtPos(x, z);
    MarkDirty();
}
void Chunk::SetBlockUnsafe(const Block block, const int x, const int y, const int z) {
    SetBlockUnsafe(block, x + y * CHUNK_WIDTH + z * (CHUNK_WIDTH * CHUNK_HEIGHT));
}

void Chunk::SetBlock(const Block block, const int x, const int y, const int z) {
    const int index = x + y * CHUNK_WIDTH + z * (CHUNK_WIDTH * CHUNK_HEIGHT);
    if (index >= 0 && index < CHUNK_SIZE) {
        SetBlockUnsafe(block, index);
    }
}

Block Chunk::GetBlock(const int x, const int y, const int z) const {
    if (x >= CHUNK_WIDTH || x < 0) return 0;
    if (y >= CHUNK_HEIGHT || y < 0) return 0;
    if (z >= CHUNK_DEPTH || z < 0) return 0;

    const int index = x + y * CHUNK_WIDTH + z * (CHUNK_WIDTH * CHUNK_HEIGHT);
    if (index >= 0 && index < CHUNK_SIZE) {
        return blocks[index];
    }
    return Blocks::AIR;
}

glm::vec3 Chunk::GetCenter() const {
    return glm::vec3(m_GlobalCenterX, m_GlobalCenterY, m_GlobalCenterZ);
}

void Chunk::MarkDirty() {
    m_Dirty = true;
}

bool Chunk::IsDirty() const {
    return m_Dirty;
}


void Chunk::Cleanup() {
}
