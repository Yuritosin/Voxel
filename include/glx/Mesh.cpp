#include "Mesh.h"

#include <GL/gl.h>

#include <string>
#include <iostream>

Mesh::~Mesh() {
    // std::cout << "Mesh Destroyed! VAO=" << m_Vao << '\n';
    Cleanup();
}

unsigned int Mesh::PrepareGpuAllocation(
    const unsigned int vertexCount,
    const unsigned int indexCount,
    const unsigned short* indices,
    const unsigned int* attributes,
    const bool useEbo //=true
) {
    if (IsGpuAllocated()) Cleanup();
    m_VertexCount = vertexCount;
    m_IndexCount = indexCount;

    unsigned int vertexSize = 0;
    for (int i = 0; attributes[i]; i ++) vertexSize += attributes[i];

    glGenVertexArrays(1, &m_Vao);
    Bind();

    if (useEbo) {
        m_IsIBO = true;
        glGenBuffers(1, &m_Ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLushort), indices, GL_STATIC_DRAW);
    }

    return vertexSize;
}
// void Mesh::PushAttributes(
//     const GLenum type,
//     const unsigned int* attributes,
//     const size_t typeSize,
//     const unsigned int vertexSize
// ) const {
//     unsigned int stride = 0;
//     for (int i = 0; attributes[i]; i ++) {
//         const unsigned int attributeSize = attributes[i];
//         if (type == GL_FLOAT) {
//             glVertexAttribPointer(i, attributeSize, type, GL_FALSE, vertexSize * typeSize, (void*)(stride * typeSize));
//         } else {
//             glVertexAttribIPointer(i, attributeSize, type, vertexSize * typeSize, (void*)(stride * typeSize));
//         }
//         glEnableVertexAttribArray(i);
//
//         stride += attributeSize;
//     }
// }

void Mesh::AllocateToGpu(
    const float* data, const unsigned int vertexCount,
    const unsigned short* indices, const unsigned int indexCount,
    const unsigned int* attributes
) {
    unsigned int vertexSize = PrepareGpuAllocation(vertexCount, indexCount, indices, attributes);
    const size_t typeSize = sizeof(GLfloat);

    glGenBuffers(1, &m_Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glBufferData(GL_ARRAY_BUFFER, m_VertexCount * vertexSize * typeSize, data, GL_STATIC_DRAW);

    unsigned int stride = 0;
    for (int i = 0; attributes[i]; i ++) {
        const unsigned int attributeSize = attributes[i];

        glVertexAttribPointer(i, attributeSize, GL_FLOAT, GL_FALSE, vertexSize * typeSize, (void*)(stride * typeSize));
        glEnableVertexAttribArray(i);

        stride += attributeSize;
    }

    // PushAttributes(GL_FLOAT, attributes, typeSize, vertexSize);

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));
    // // glEnableVertexArrayAttrib(0);
    // glEnableVertexAttribArray(0);
    //
    // glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    // // glEnableVertexArrayAttrib(0);
    // glEnableVertexAttribArray(1);
    //
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(4 * sizeof(GLfloat)));
    // // glEnableVertexArrayAttrib(0);
    // glEnableVertexAttribArray(2);

    Unbind();
}

void Mesh::AllocateToGpuUInt(
    const uint32_t* data, const unsigned int vertexCount,
    const unsigned short* indices, const unsigned int indexCount,
    const unsigned int* attributes
) {
    unsigned int vertexSize = PrepareGpuAllocation(vertexCount, indexCount, indices, attributes);
    const size_t typeSize = sizeof(GLuint);

    glGenBuffers(1, &m_Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glBufferData(GL_ARRAY_BUFFER, m_VertexCount * vertexSize * typeSize, data, GL_STATIC_DRAW);

    unsigned int stride = 0;
    for (int i = 0; attributes[i]; i ++) {
        const unsigned int attributeSize = attributes[i];

        glVertexAttribIPointer(i, attributeSize, GL_UNSIGNED_INT, vertexSize * typeSize, (void*)(stride * typeSize));
        glEnableVertexAttribArray(i);

        stride += attributeSize;
    }

    // PushAttributes(GL_UNSIGNED_INT, attributes, typeSize, vertexSize);
}


void Mesh::Draw(const GLenum mode) const {
    Bind();
    if (m_IsIBO) {
        glDrawElements(mode, m_IndexCount, m_IboType, nullptr);
    } else {
        glDrawArrays(mode, 0, m_VertexCount);
    }

    // const GLenum err = glGetError();
    // while (err != GL_NO_ERROR) {
    //     std::cout << "OpenGL error in Mesh::Draw: " << glewGetErrorString(err) << " (" << err << ')' << '\n';
    // }

    // Bind();
    // glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
    // Unbind();
}

void Mesh::Bind() const {
    if (IsGpuAllocated()) {
        glBindVertexArray(m_Vao);
        // if (m_Ibo != 0) {
        //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ibo);
        // }
    } else {
        std::cerr << "Try to bind unallocated mesh" << '\n';
    }
}
void Mesh::Unbind() const {
    // if (m_Ibo != 0) {
    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // }
    glBindVertexArray(0);
}

inline GLuint Mesh::GetVao() const {
    return m_Vao;
}

inline bool Mesh::IsGpuAllocated() const {
    return GetVao() != 0;
}


void Mesh::Cleanup() {
    if (IsGpuAllocated()) {
        if (m_Ibo != 0) {
            m_IsIBO = false;
            glDeleteBuffers(1, &m_Ibo);
            m_Ibo = 0;
        }
        if (m_Vbo != 0) {
            glDeleteBuffers(1, &m_Vbo);
            m_Vbo = 0;
        }
        glDeleteVertexArrays(1, &m_Vao);
        m_Vao = 0;
    }
}
