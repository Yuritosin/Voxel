#ifndef BLOCKS_ID_H
#define BLOCKS_ID_H

#include "world/block/Block.h"

namespace Blocks {
	constexpr Block AIR = CreateBlock(0, OPAQUE_TYPE);
	constexpr Block STONE = CreateBlock(1, OPAQUE_TYPE);
	constexpr Block DIRT = CreateBlock(2, OPAQUE_TYPE);
	constexpr Block GRASS = CreateBlock(3, OPAQUE_TYPE);
	constexpr Block PLANKS = CreateBlock(4, OPAQUE_TYPE);
	constexpr Block BEDROCK = CreateBlock(5, OPAQUE_TYPE);

	constexpr Block GLASS = CreateBlock(6, TRANSPARENT_TYPE);
	constexpr Block RED_STAINED_GLASS = CreateBlock(7, TRANSLUCENT_TYPE);
	constexpr Block GREEN_STAINED_GLASS = CreateBlock(8, TRANSLUCENT_TYPE);
	constexpr Block BLUE_STAINED_GLASS = CreateBlock(9, TRANSLUCENT_TYPE);
	constexpr Block WATER = CreateBlock(10, TRANSLUCENT_TYPE);
	constexpr Block GRAVEL = CreateBlock(11, OPAQUE_TYPE);
	constexpr Block SAND = CreateBlock(12, OPAQUE_TYPE);
	constexpr Block LEAVES = CreateBlock(13, CUTOUT_TYPE);
	constexpr Block LOG = CreateBlock(14, OPAQUE_TYPE);

	constexpr Block blocks[15] = {
		AIR,
		STONE,
		DIRT,
		GRASS,
		PLANKS,
		BEDROCK,
		GLASS,
		RED_STAINED_GLASS, GREEN_STAINED_GLASS, BLUE_STAINED_GLASS,
		WATER,
		GRAVEL,
		SAND,
		LEAVES,
		LOG
	};
}


#endif
