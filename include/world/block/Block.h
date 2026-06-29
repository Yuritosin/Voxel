// world/block/Block.h
#ifndef INCLUDE_WORLD_BLOCK_H_
#define INCLUDE_WORLD_BLOCK_H_

#include <cstdint>

using Block = uint32_t;

static constexpr uint8_t OPAQUE_TYPE = 0b00;
static constexpr uint8_t CUTOUT_TYPE = 0b01;
static constexpr uint8_t TRANSPARENT_TYPE = 0b10;
static constexpr uint8_t TRANSLUCENT_TYPE = 0b11;

constexpr Block CreateBlock(const uint32_t blockId, const uint8_t type) {
	return (blockId << (2)) | (type & (4 - 1));
}

constexpr uint32_t GetRawId(const Block block) { return block >> 2; }
constexpr uint8_t GetType(const Block block) { return block & 0b11; }

constexpr bool IsAir(const Block block) { return GetRawId(block) == 0; }

constexpr bool IsOpaque(const Block block) { return !IsAir(block) && GetType(block) == OPAQUE_TYPE; }
constexpr bool IsCutout(const Block block) { return !IsAir(block) && GetType(block) == CUTOUT_TYPE; }
constexpr bool IsTransparent(const Block block) { return IsCutout(block) || (GetType(block) == TRANSPARENT_TYPE); }
constexpr bool IsTranslucent(const Block block) { return !IsAir(block) && GetType(block) == TRANSLUCENT_TYPE; }

//class Block { // REMOVE THAT SHIT
//public:
//	static constexpr unsigned char OPAQUE_TYPE = 0b00;
//	static constexpr unsigned char CUTOUT_TYPE = 0b01;
//	static constexpr unsigned char TRANSPARENT_TYPE = 0b10;
//	static constexpr unsigned char TRANSLUCENT_TYPE = 0b11;
//
//	constexpr Block(
//		unsigned char blockId = 0,
//		unsigned char type = Block::OPAQUE_TYPE
//	) : m_Data(
//		((blockId & (64 - 1)) << (8 - 6)) | (type & (4 - 1))
//	) {}
//
////	bool operator!=(const unsigned int blockId) const {
////		return GetRawId() != blockId;
////	}
////	operator unsigned char() const { return m_Data; }
//
//	constexpr unsigned char GetData() const { return m_Data; }
//	constexpr unsigned char GetRawId() const { return m_Data >> (8 - 6); }
//	constexpr unsigned char GetType() const { return m_Data & 0b11; }
//
//	constexpr Block Cutout() const {
//		return Block(GetRawId(), Block::CUTOUT_TYPE);
//	}
//	constexpr Block Transparent() const {
//		return Block(GetRawId(), Block::TRANSPARENT_TYPE);
//	}
//	constexpr Block Translucent() const {
//		return Block(GetRawId(), Block::TRANSLUCENT_TYPE);
//	}
//
//	constexpr bool IsOpaque() const { return GetType() == Block::OPAQUE_TYPE; }
//	constexpr bool IsCutout() const { return GetType() == Block::CUTOUT_TYPE; }
//	constexpr bool IsTransparent() const { return IsCutout() || (GetType() == Block::TRANSPARENT_TYPE); }
//	constexpr bool IsTranslucent() const { return GetType() == Block::TRANSLUCENT_TYPE; }
//
//	constexpr bool IsAir() const { return GetRawId() == 0; }
//
//private:
//	unsigned char m_Data;
//};

#endif
