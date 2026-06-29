#include "Shader.h"

#include <iostream>

#include <unordered_map>

#include <fstream>
#include <sstream>
#include <filesystem>

#include <string>
#include <string_view>

#include <glm/ext.hpp>

#include <GL/glew.h>
#include <GL/gl.h>

const std::string ReadFileAsString(const std::filesystem::path& path);
void PrintShaderError(const GLuint shaderId, const char* const shaderPath, std::string_view errorMessage);
void PrintProgramError(const GLuint shaderId, std::string_view errorMessage);

Shader Shader::s_ChunkShader;
Shader Shader::s_ChunkTransparentShader;
Shader Shader::s_ChunkTranslucentShader;
Shader Shader::s_WorldColorShader;
Shader Shader::s_BlockOutlineShader;
Shader Shader::s_InventoryBlockShader;
Shader Shader::s_HandUiShader;

void Shader::LoadCompileAll() {
    s_ChunkShader.Load(
        std::filesystem::path("./resources/shaders/chunk_frag.glsl"),
        std::filesystem::path("./resources/shaders/chunk_vert.glsl")
    );
    s_ChunkTransparentShader.Load(
        std::filesystem::path("./resources/shaders/chunk_transparent_frag.glsl"),
        std::filesystem::path("./resources/shaders/chunk_transparent_vert.glsl")
    );
    s_ChunkTranslucentShader.Load(
        std::filesystem::path("./resources/shaders/chunk_translucent_frag.glsl"),
        std::filesystem::path("./resources/shaders/chunk_translucent_vert.glsl")
    );
    s_WorldColorShader.Load(
        std::filesystem::path("./resources/shaders/world_color_frag.glsl"),
        std::filesystem::path("./resources/shaders/world_color_vert.glsl")
    );
    s_BlockOutlineShader.Load(
        std::filesystem::path("./resources/shaders/block_outline_frag.glsl"),
        std::filesystem::path("./resources/shaders/block_outline_vert.glsl")
    );
    s_InventoryBlockShader.Load(
        std::filesystem::path("./resources/shaders/inventory_block_frag.glsl"),
        std::filesystem::path("./resources/shaders/inventory_block_vert.glsl")
    );
    s_HandUiShader.Load(
        std::filesystem::path("./resources/shaders/hand_ui_frag.glsl"),
        std::filesystem::path("./resources/shaders/hand_ui_vert.glsl")
    );
}


Shader::~Shader() {
    Cleanup();
}

void Shader::Load(
    std::filesystem::path fragPath,
    std::filesystem::path vertPath
) {
    const std::string fragShaderSource = ReadFileAsString(fragPath);
    const std::string vertShaderSource = ReadFileAsString(vertPath);
    const GLchar* fragShaderCode = fragShaderSource.c_str();
    const GLchar* vertShaderCode = vertShaderSource.c_str();

    const GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(fragShader, 1, &fragShaderCode, nullptr);
    glShaderSource(vertShader, 1, &vertShaderCode, nullptr);

    GLint status;
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
    if (!status) {
        PrintShaderError(fragShader, fragPath.string().c_str(), "Compile fragment shader error: ");
        return;
    }
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &status);
    if (!status) {
        PrintShaderError(vertShader, vertPath.string().c_str(), "Compile vertex shader error: ");
        return;
    }

    shaderId = glCreateProgram();

    glAttachShader(shaderId, fragShader);
    glAttachShader(shaderId, vertShader);

    glLinkProgram(shaderId);
    glGetProgramiv(shaderId, GL_LINK_STATUS, &status);
    if (!status) {
        PrintProgramError(shaderId, "Shader program linking error: ");
        return;
    }
    glValidateProgram(shaderId);
    glGetProgramiv(shaderId, GL_VALIDATE_STATUS, &status);
    if (!status) {
        PrintProgramError(shaderId, "Shader program validation error: ");
        return;
    }

    glDeleteShader(fragShader);
    glDeleteShader(vertShader);
}

int Shader::GetUniformLocation(const char* name) {
    auto uniformCacheIterator = m_uniformCache.find(name);
    if (uniformCacheIterator != m_uniformCache.end()) {
        return uniformCacheIterator->second;
    }

    const GLint uniformLocation = glGetUniformLocation(shaderId, name);
    if (uniformLocation == -1) {
        std::cerr << "Unable to found uniform " << name << "\n";
    } else {
        m_uniformCache[name] = uniformLocation;
    }
    return uniformLocation;
    // if (IsEnabled()) {
    //     auto uniformCacheIterator = m_uniformCache.find(name);
    //     if (uniformCacheIterator != m_uniformCache.end()) {
    //         return uniformCacheIterator->second;
    //     }
    //
    //     const GLint uniformLocation = glGetUniformLocation(shaderId, name);
    //     if (uniformLocation == -1) {
    //         std::cerr << "Unable to found uniform " << name << "\n";
    //     } else {
    //         m_uniformCache[name] = uniformLocation;
    //     }
    //     return uniformLocation;
    // }
    // std::cerr << "Try to get uniform " << name << " on disabled shader " << "\n";
    // return -1;
}

void Shader::SetBool(const char* name, const bool value) {
    SetInt(name, value);
}

void Shader::SetInt(const char* name, const int value) {
    const GLint location = GetUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

void Shader::SetUInt(const char* name, const unsigned int value) {
    const GLint location = GetUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

void Shader::SetFloat(const char* name, const float value) {
    // GLint currentShader;
    // glGetIntegerv(GL_CURRENT_PROGRAM, &currentShader);
    // std::cout << "Current program: " << currentShader << ", "
    //     << "Expeceted program: " << shaderId << '\n';

    const GLint location = GetUniformLocation(name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void Shader::SetVec3i(const char* name, const glm::ivec3& value) {
    const GLint location = GetUniformLocation(name);
    if (location != -1) {
        glUniform3iv(location, 1, glm::value_ptr(value));
    }
}

void Shader::SetVec2i(const char* name, const glm::ivec2& value) {
    const GLint location = GetUniformLocation(name);
    if (location != -1) {
        glUniform2iv(location, 1, glm::value_ptr(value));
    }
}

void Shader::SetVec3f(const char* name, const glm::fvec3& value) {
    const GLint location = GetUniformLocation(name);
    if (location != -1) {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::SetVec2f(const char* name, const glm::fvec2& value) {
    const GLint location = GetUniformLocation(name);
    if (location != -1) {
        glUniform2fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::SetMat4f(const char* name, const glm::fmat4& value) {
    const GLint location = GetUniformLocation(name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void Shader::Enable() {
    if (shaderId == 0) {
        std::cerr << "Try to use unloaded shader" << '\n';
        return;
    }
    if (IsEnabled()) return;
    glUseProgram(shaderId);
    enabled = true;
}

void Shader::Disable() {
    if (!IsEnabled()) return;
    glUseProgram(GL_FALSE);
    enabled = false;
}

const bool Shader::IsEnabled() const {
    return enabled;
}

void Shader::Cleanup() {
    if (shaderId != 0) {
        glDeleteProgram(shaderId);
        shaderId = 0;
    }
}



const std::string ReadFileAsString(const std::filesystem::path& path) {
    const std::ifstream ifs(path, std::ios::in);
    if (!ifs) {
        std::cerr << "Unable to open " << path << "\n";
        return "";
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

void PrintShaderError(
    const GLuint shaderId,
    const char* const shaderPath,
    std::string_view errorMessage
) {
    GLchar bufferLog[1024];
    glGetShaderInfoLog(shaderId, 1024, nullptr, bufferLog);
    std::cerr << errorMessage;
    std::cerr << " (" << shaderPath << ')' << '\n';
    std::cerr << bufferLog << '\n';
}

void PrintProgramError(
    const GLuint shaderId,
    std::string_view errorMessage
) {
    GLchar bufferLog[1024];
    glGetProgramInfoLog(shaderId, 1024, nullptr, bufferLog);
    std::cerr << errorMessage << "\n";
    std::cerr << bufferLog << "\n";
}
