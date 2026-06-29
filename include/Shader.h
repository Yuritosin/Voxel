#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GL/gl.h>

#include <filesystem>

#include <glm/glm.hpp>

class unordered_map;

class Shader {
public:
    static Shader s_ChunkShader;
    static Shader s_ChunkTransparentShader;
    static Shader s_ChunkTranslucentShader;
    static Shader s_WorldColorShader;
    static Shader s_BlockOutlineShader;
    static Shader s_InventoryBlockShader;
    static Shader s_HandUiShader;
    static void LoadCompileAll();

    ~Shader();
    Shader() = default;

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    // Shader():
    //     enabled(false),
    //     shaderId(0) {}

    void Load(
        std::filesystem::path fragPath,
        std::filesystem::path vertPath
    );

    void SetBool(const char* name, const bool value);
    void SetInt(const char* name, const int value);
    void SetUInt(const char* name, const unsigned int value);
    void SetFloat(const char* name, const float value);
    void SetVec3i(const char* name, const glm::ivec3& value);
    void SetVec2i(const char* name, const glm::ivec2& value);
    void SetVec3f(const char* name, const glm::fvec3& value);
    void SetVec2f(const char* name, const glm::fvec2& value);

    void SetMat4f(const char* name, const glm::fmat4& value);

    void Enable();
    void Disable();
    const bool IsEnabled() const;

    void Cleanup();
protected:
    bool enabled = false;
    GLuint shaderId = 0;
    std::unordered_map<std::string, int> m_uniformCache;

    int GetUniformLocation(const char* name);
};

#endif
