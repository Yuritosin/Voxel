#ifndef OPTIONS_H
#define OPTIONS_H

constexpr double UPDATES_PER_SECOND = 20.0;
constexpr double FRAMES_PER_SECOND = 60.0;

constexpr int CHUNK_HORIZONTAL_SIZE = 16;
constexpr int CHUNK_WIDTH = CHUNK_HORIZONTAL_SIZE;
constexpr int CHUNK_HEIGHT = 128;
// constexpr int CHUNK_HEIGHT = CHUNK_HORIZONTAL_SIZE;
// constexpr int CHUNK_HEIGHT = CHUNK_HORIZONTAL_SIZE * 2;
constexpr int CHUNK_DEPTH = CHUNK_HORIZONTAL_SIZE;
constexpr int CHUNK_SIZE = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH;

// ### TEXTURES ###
// constexpr int ATLAS_WIDTH = 2048;
// constexpr int ATLAS_HEIGHT = 2048;
//constexpr int ATLAS_WIDTH = 512;
//constexpr int ATLAS_HEIGHT = 512;
constexpr int ATLAS_WIDTH = 1024;
constexpr int ATLAS_HEIGHT = 1024;
constexpr int TEXTURE_WIDTH = 32;
constexpr int TEXTURE_HEIGHT = 32;
constexpr int TEXTURE_VGAP = 1;
constexpr int TEXTURE_HGAP = 1;
// ### <...> ###

constexpr bool PERFOMANCE_OUTPUT = true;

#endif
