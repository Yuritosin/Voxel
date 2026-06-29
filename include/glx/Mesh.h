#ifndef MESH_H
#define MESH_H

#include <cstdint>
#include <cstddef>

#include <GL/glew.h>

class Mesh {
public:
    ~Mesh();
    Mesh() = default;
    Mesh(
        const float* data, const unsigned int vertexCount,
        const unsigned short* indices, const unsigned int indexCount,
        const unsigned int* attributes
    ) {
        AllocateToGpu(data, vertexCount, indices, indexCount, attributes);
    }

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void AllocateToGpu(
        const float* data, const unsigned int vertexCount,
        const unsigned short* indices, const unsigned int indexCount,
        const unsigned int* attributes
    );
    void AllocateToGpuUInt(
        const uint32_t* data, const unsigned int vertexCount,
        const unsigned short* indices, const unsigned int indexCount,
        const unsigned int* attributes
    );

    void Draw(const GLenum mode = GL_TRIANGLES) const;

    void Bind() const;
    void Unbind() const;

    inline GLuint GetVao() const;
    inline bool IsGpuAllocated() const;

    void Cleanup();

    // Mesh(Mesh&& other) noexcept
    // : vao(other.vao), vbo(other.vbo), m_VertexCount(other.m_VertexCount) {
    //
    //     // std::cout << "Mesh MOVED: VAO=" << vao << '\n';
    //
    //     other.vao = 0;
    //     other.vbo = 0;
    //     other.m_VertexCount = 0;
    // }
    //
    // Mesh& operator=(Mesh&& other) noexcept {
    //     if (this != &other) {
    //         Cleanup();
    //         vao = other.vao;
    //         vbo = other.vbo;
    //         m_VertexCount = other.m_VertexCount;
    //         other.vao = 0;
    //         other.vbo = 0;
    //         other.m_VertexCount = 0;
    //         // std::cout << "Mesh MOVED (assign)\n";
    //     }
    //     return *this;
    // }
private:
    GLuint m_Vao = 0;
    GLuint m_Vbo = 0;
    GLuint m_Ibo = 0;
    GLenum m_IboType = GL_UNSIGNED_SHORT;

    unsigned int m_IndexCount = 0;
    unsigned int m_VertexCount = 0;

    bool m_IsIBO = false;

    unsigned int PrepareGpuAllocation(
        const unsigned int vertexCount,
        const unsigned int indexCount,
        const unsigned short* indices,
        const unsigned int* attributes,
        const bool useEbo = true
    );

    // void PushAttributes(
    //     const GLenum type,
    //     const unsigned int* attributes,
    //     const size_t typeSize,
    //     const unsigned int vertexSize
    // ) const;
};

#endif
