#ifndef CHUNK_RENDERER_H
#define CHUNK_RENDERER_H

#define IS_CHUNK_RENDERER_DEBUG (false)

#include <cstdint>

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>

#include <GLFW/glfw3.h>

#include "Shader.h"
#include "glx/Mesh.h"
#include "glx/Texture.h"

#include "world/Chunk.h"

#include "math/mmrvl.h"
#include "math/Direction.h"

#include <world/block/Block.h>
#include <world/block/Blocks.h>

class World;
class Window;

enum class BlockModelType {
    CUBE, CUBE_ALL, CUBE_TOP_BOTTOM, CUBE_COLUMN
};

struct Vertex {
    static glm::vec3 UnpackPos(const uint32_t packedData);
    static float UnpackX(const uint32_t packedData);
    static float UnpackY(const uint32_t packedData);
    static float UnpackZ(const uint32_t packedData);
    static uint8_t UnpackLightLevel(const uint32_t packedData);
    static Direction UnpackFaceIndex(const uint32_t packedData);
    static BlockModelType UnpackModelType(const uint32_t packedData);
    static Block UnpackBlock(const uint32_t packedData);

    Vertex(
//        const float x,
//        const float y,
//        const float z,
		const uint8_t x,
		const uint8_t y,
		const uint8_t z,
		const uint8_t lightLevel,
        const Direction face,
        const BlockModelType modelType,
		const Block block
    ): m_X(x), m_Y(y), m_Z(z), m_LightLevel(lightLevel), m_Face(face), m_ModelType(modelType), m_Block(block) {}
    // ): m_X(x), m_Y(y), m_Z(z), m_U(u), m_V(v), m_Bright(bright) {}
//    const float m_X, m_Y, m_Z;
    const uint8_t m_X, m_Y, m_Z;
    const uint8_t m_LightLevel;
    const Direction m_Face;
    const BlockModelType m_ModelType;
    const Block m_Block;
    // float m_U, m_V;
    // float m_Bright = 1.0f;

    uint32_t Pack() const;
};

#include <string>
#include <sstream>

class ChunkRenderer {
public:
    inline static int commonSize = 0;
    static std::string format_size_as_str(const int sizeBytes) {
        std::ostringstream oss;
        oss << sizeBytes << " Bytes";
        if (sizeBytes >= 1024) oss << " (" << (sizeBytes / 1024) << " KBytes)";
        if (sizeBytes / 1024 >= 1024) oss << " (" << (sizeBytes / 1024 / 1024) << " MBytes)";
        if (sizeBytes / 1024 / 1024 >= 1024) oss << " (" << (sizeBytes / 1024 / 1024 / 1024) << " GBytes)";
        return oss.str();
    }

    inline static bool renderDebug = false;
    static void ToggleRenderDebug();

    static BlockModelType getBlockModelType(const Block block);

    ~ChunkRenderer() = default;
    ChunkRenderer(Chunk& chunk): m_Chunk(chunk) {}

    void Init(const World& world);
    // void BindBlockTextureAtlas(const Texture& textureAtlas) const;
    // void BindBlockShader(Shader& shader, const float partialTicks, const World& world) const;
    // void UnindBlockShader(Shader& shader) const;
    void Tick();
    void DrawSolidBlocks() const;
    void DrawTransparentBlocks() const;
    void DrawTranslucentBlocks() const;
    // void PrepareSolidShader(const Shader& shader) const;
    // void PrepareTransparentShader() const;
    // void PrepareTranslucentShader() const;
    void SortAndAllocateTranslucentFaces(const glm::vec3& cameraPos);

    void StartFadeIn();
    void StartFadeOut();

    const Chunk& GetChunk() const;

    glm::mat4 GetChunkModelMatrix() const;

    void RebuildMesh(const World& world);

    int m_Color = 0xffffff; //XXX UNUSED
    bool m_HasTransparent = false;
    bool m_HasTranslucent = false;
//
//    float m_Opacity = 1.0f;
    mmr::mmrvl<float> m_Opacity = 1.0f;
    bool m_FadeIn = false;
    bool m_FadeOut = false;

    bool m_InRenderList = false;
    bool m_FrustumCulledFlag = false;
protected:
    // void PushVertex(std::vector<float>& destination, const Vertex& vertex);
    void PushVertex(std::vector<uint32_t>& destination, const Vertex& vertex);
    void PushFace(
        // std::vector<float>& verticesDestination,
        std::vector<unsigned int>& verticesDestination,
        std::vector<unsigned short>& indicesDestination,
        const int x, const int y, const int z, const Direction direction, const Block blockId
    );

    bool IsFaceNeed(const World& world, const Block currentBlock, const int x, const int y, const int z) const;
private:
    Mesh m_Mesh;
    Mesh m_TransparentMesh;
    Mesh m_TranslucentMesh;

    // std::vector<float> m_Vertices;
    std::vector<unsigned int> m_Vertices;
    std::vector<unsigned short> m_Indices;
    // std::vector<float> m_TransparentVertices;
    std::vector<unsigned int> m_TransparentVertices;
    std::vector<unsigned short> m_TransparentIndices;
    // std::vector<float> m_TranslucentVertices;
    std::vector<unsigned int> m_TranslucentVertices;
    std::vector<unsigned short> m_TranslucentIndices;

    Chunk& m_Chunk;
};

#endif
