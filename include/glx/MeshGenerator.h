#ifndef MESH_GENERATOR_H
#define MESH_GENERATOR_H

#include "Mesh.h"

namespace MeshGenerator {
    static void allocateCubeMeshForLines(
        Mesh& mesh,

        const float minX,
        const float minY,
        const float minZ,

        const float maxX,
        const float maxY,
        const float maxZ
    ) {
        const float vertices[] = {
            // Нижняя грань (Y = minY)
            minX, minY, maxZ,  // 0: северо-восток низ (x min, y min, z max)
            maxX, minY, maxZ,  // 1: северо-запад низ (x max, y min, z max)
            maxX, minY, minZ,  // 2: юго-запад низ (x max, y min, z min)
            minX, minY, minZ,  // 3: юго-восток низ (x min, y min, z min)

            // Верхняя грань (Y = maxY)
            minX, maxY, maxZ,  // 4: северо-восток верх
            maxX, maxY, maxZ,  // 5: северо-запад верх
            maxX, maxY, minZ,  // 6: юго-запад верх
            minX, maxY, minZ  // 7: юго-восток верх
        };
        // Индексы для рёбер (12 рёбер × 2 вершины = 24 индекса)
        const unsigned short indices[] = {
            // Нижняя грань
            0, 1,  1, 2,  2, 3,  3, 0,
            // Верхняя грань
            4, 5,  5, 6,  6, 7,  7, 4,
            // Вертикальные рёбра
            0, 4,  1, 5,  2, 6,  3, 7
        };
        const unsigned int attrs[2] { 3, 0 };
        mesh.AllocateToGpu(vertices, 8, indices, 24, attrs);
        // return Mesh(vertices, 8, indices, 24, attrs);
    }

    static void allocateCubeMeshForTriangles(
        Mesh& mesh,

        const float minX,
        const float minY,
        const float minZ,

        const float maxX,
        const float maxY,
        const float maxZ
    ) {
        const float vertices[] = {
            // Нижняя грань (Y = minY)
            minX, minY, maxZ,  // 0: северо-восток низ (x min, y min, z max)
            maxX, minY, maxZ,  // 1: северо-запад низ (x max, y min, z max)
            maxX, minY, minZ,  // 2: юго-запад низ (x max, y min, z min)
            minX, minY, minZ,  // 3: юго-восток низ (x min, y min, z min)

            // Верхняя грань (Y = maxY)
            minX, maxY, maxZ,  // 4: северо-восток верх
            maxX, maxY, maxZ,  // 5: северо-запад верх
            maxX, maxY, minZ,  // 6: юго-запад верх
            minX, maxY, minZ  // 7: юго-восток верх
        };
        // Индексы для рёбер (12 рёбер × 2 вершины = 24 индекса)
        const unsigned short indices[] = {
            // Нижняя грань (Y = minY) - смотрим снизу
            0,2,1,
            0,3,2,
            // Верхняя грань (Y = maxY) - смотрим сверху
            4,5,6,
            4,6,7,
            // Передняя грань (Z = maxZ) - смотрим спереди
            0,1,5,
            0,5,4,
            // Задняя грань (Z = minZ) - смотрим сзади
            3,6,2,
            3,7,6,
            // Левая грань (X = minX) - смотрим слева
            0,4,7,
            0,7,3,
            // Правая грань (X = maxX) - смотрим справа
            1,2,6,
            1,6,5
        };
        const unsigned int attrs[2] { 3, 0 };
        mesh.AllocateToGpu(vertices, 8, indices, 36, attrs);
    }

    static void allocateCubeMeshForTrianglesWithUV(
        Mesh& mesh,

        const float minX,
        const float minY,
        const float minZ,

        const float maxX,
        const float maxY,
        const float maxZ
    ) {
        const float vertices[] = {
            // ========== НИЖНЯЯ ГРАНЬ (Y = minY) ==========
            minX, minY, maxZ,   0.0f, 1.0f,  // 0: левый передний
            maxX, minY, maxZ,   1.0f, 1.0f,  // 1: правый передний
            maxX, minY, minZ,   1.0f, 0.0f,  // 2: правый задний
            minX, minY, minZ,   0.0f, 0.0f,  // 3: левый задний

            // ========== ВЕРХНЯЯ ГРАНЬ (Y = maxY) ==========
            minX, maxY, maxZ,   0.0f, 1.0f,  // 4: левый передний
            maxX, maxY, maxZ,   1.0f, 1.0f,  // 5: правый передний
            maxX, maxY, minZ,   1.0f, 0.0f,  // 6: правый задний
            minX, maxY, minZ,   0.0f, 0.0f,  // 7: левый задний

            // ========== ПЕРЕДНЯЯ ГРАНЬ (Z = maxZ) ==========
            minX, minY, maxZ,   0.0f, 0.0f,  // 8: левый нижний
            maxX, minY, maxZ,   1.0f, 0.0f,  // 9: правый нижний
            maxX, maxY, maxZ,   1.0f, 1.0f,  // 10: правый верхний
            minX, maxY, maxZ,   0.0f, 1.0f,  // 11: левый верхний

            // ========== ЗАДНЯЯ ГРАНЬ (Z = minZ) ==========
            minX, minY, minZ,   0.0f, 0.0f,  // 12: левый нижний
            maxX, minY, minZ,   1.0f, 0.0f,  // 13: правый нижний
            maxX, maxY, minZ,   1.0f, 1.0f,  // 14: правый верхний
            minX, maxY, minZ,   0.0f, 1.0f,  // 15: левый верхний

            // ========== ЛЕВАЯ ГРАНЬ (X = minX) ==========
            minX, minY, maxZ,   0.0f, 0.0f,  // 16: левый нижний
            minX, minY, minZ,   1.0f, 0.0f,  // 17: правый нижний
            minX, maxY, minZ,   1.0f, 1.0f,  // 18: правый верхний
            minX, maxY, maxZ,   0.0f, 1.0f,  // 19: левый верхний

            // ========== ПРАВАЯ ГРАНЬ (X = maxX) ==========
            maxX, minY, maxZ,   0.0f, 0.0f,  // 20: левый нижний
            maxX, minY, minZ,   1.0f, 0.0f,  // 21: правый нижний
            maxX, maxY, minZ,   1.0f, 1.0f,  // 22: правый верхний
            maxX, maxY, maxZ,   0.0f, 1.0f   // 23: левый верхний
        };
        // Индексы для рёбер (12 рёбер × 2 вершины = 24 индекса)
        const unsigned short indices[] = {
            // Нижняя
            2, 1, 0,
            0, 3, 2,
            // Верхняя
            4,5,6,
            6,7,4,
            // Передняя
            8,9,10,
            10,11,8,
            // Задняя
            14,13,12,
            12,15,14,
            // Левая
            18,17,16,
            16,19,18,
            // Правая
            20,21,22,
            22,23,20
        };
        const unsigned int attrs[3] { 3, 2, 0 };
        mesh.AllocateToGpu(vertices, 24, indices, 36, attrs);
    }

    static void allocateCubeMeshForTrianglesWithUV(
        Mesh& mesh,

        const float minX,
        const float minY,
        const float minZ,

        const float maxX,
        const float maxY,
        const float maxZ,

        const float atlasWidth,
        const float atlasHeight,
        const float textureWidth,
        const float textureHeight,

        const unsigned int blockId,

        const float textureVGap = 1.0f,
        const float textureHGap = 1.0f
    ) {
        const float twStep = textureWidth / atlasWidth;
        const float thStep = textureHeight / atlasHeight;

        const float uStep = (textureWidth + textureVGap)  / atlasWidth;
        const float vStep = (textureHeight + textureHGap) / atlasHeight * (blockId - 1);

        const float vertices[] = {
            // ========== НИЖНЯЯ ГРАНЬ (Y = minY) ==========
            minX, minY, maxZ,   uStep * 2, vStep + thStep,  // 0: левый передний
            maxX, minY, maxZ,   uStep * 2 + twStep, vStep + thStep,  // 1: правый передний
            maxX, minY, minZ,   uStep * 2 + twStep, vStep,  // 2: правый задний
            minX, minY, minZ,   uStep * 2, vStep,  // 3: левый задний

            // ========== ВЕРХНЯЯ ГРАНЬ (Y = maxY) ==========
            minX, maxY, maxZ,   uStep * 2, vStep + thStep,  // 4: левый передний
            maxX, maxY, maxZ,   uStep * 2 + twStep, vStep + thStep,  // 5: правый передний
            maxX, maxY, minZ,   uStep * 2 + twStep, vStep,  // 6: правый задний
            minX, maxY, minZ,   uStep * 2, vStep,  // 7: левый задний

            // ========== ПЕРЕДНЯЯ ГРАНЬ (Z = maxZ) ==========
            minX, minY, maxZ,   uStep * 1, vStep + thStep,  // 8: левый нижний
            maxX, minY, maxZ,   uStep * 1 + twStep, vStep + thStep,  // 9: правый нижний
            maxX, maxY, maxZ,   uStep * 1 + twStep, vStep,  // 10: правый верхний
            minX, maxY, maxZ,   uStep * 1, vStep,  // 11: левый верхний

            // ========== ЗАДНЯЯ ГРАНЬ (Z = minZ) ==========
            minX, minY, minZ,   uStep * 1, vStep + thStep,  // 12: левый нижний
            maxX, minY, minZ,   uStep * 1 + twStep, vStep + thStep,  // 13: правый нижний
            maxX, maxY, minZ,   uStep * 1 + twStep, vStep,  // 14: правый верхний
            minX, maxY, minZ,   uStep * 1, vStep,  // 15: левый верхний

            // ========== ЛЕВАЯ ГРАНЬ (X = minX) ==========
            minX, minY, maxZ,   uStep * 1, vStep + thStep,  // 16: левый нижний
            minX, minY, minZ,   uStep * 1 + twStep, vStep + thStep, // 17: правый нижний
            minX, maxY, minZ,   uStep * 1 + twStep, vStep, // 18: правый верхний
            minX, maxY, maxZ,   uStep * 1, vStep,  // 19: левый верхний

            // ========== ПРАВАЯ ГРАНЬ (X = maxX) ==========
            maxX, minY, maxZ,   uStep * 1, vStep + thStep,  // 20: левый нижний
            maxX, minY, minZ,   uStep * 1 + twStep, vStep + thStep, // 21: правый нижний
            maxX, maxY, minZ,   uStep * 1 + twStep, vStep, // 22: правый верхний
            maxX, maxY, maxZ,   uStep * 1, vStep,  // 23: левый верхний
        };
        // Индексы для рёбер (12 рёбер × 2 вершины = 24 индекса)
        const unsigned short indices[] = {
            // Нижняя
            2, 1, 0,
            0, 3, 2,
            // Верхняя
            4,5,6,
            6,7,4,
            // Передняя
            8,9,10,
            10,11,8,
            // Задняя
            14,13,12,
            12,15,14,
            // Левая
            18,17,16,
            16,19,18,
            // Правая
            20,21,22,
            22,23,20
        };
        const unsigned int attrs[3] { 3, 2, 0 };
        mesh.AllocateToGpu(vertices, 24, indices, 36, attrs);
    }

    static void allocateCubeMeshForTrianglesWithFaceNumeration(
        Mesh& mesh,

        const float minX,
        const float minY,
        const float minZ,

        const float maxX,
        const float maxY,
        const float maxZ
    ) {
        const float vertices[] = {
            // ========== НИЖНЯЯ ГРАНЬ (Y = minY) ==========
            minX, minY, maxZ,   0.0f,  // 0: левый передний
            maxX, minY, maxZ,   0.0f,  // 1: правый передний
            maxX, minY, minZ,   0.0f,  // 2: правый задний
            minX, minY, minZ,   0.0f,  // 3: левый задний

            // ========== ВЕРХНЯЯ ГРАНЬ (Y = maxY) ==========
            minX, maxY, maxZ,   1.0f,  // 4: левый передний
            maxX, maxY, maxZ,   1.0f,  // 5: правый передний
            maxX, maxY, minZ,   1.0f,  // 6: правый задний
            minX, maxY, minZ,   1.0f,  // 7: левый задний

            // ========== ПЕРЕДНЯЯ ГРАНЬ (Z = maxZ) ==========
            minX, minY, maxZ,   2.0f,  // 8: левый нижний
            maxX, minY, maxZ,   2.0f,  // 9: правый нижний
            maxX, maxY, maxZ,   2.0f,  // 10: правый верхний
            minX, maxY, maxZ,   2.0f,  // 11: левый верхний

            // ========== ЗАДНЯЯ ГРАНЬ (Z = minZ) ==========
            minX, minY, minZ,   3.0f,  // 12: левый нижний
            maxX, minY, minZ,   3.0f,  // 13: правый нижний
            maxX, maxY, minZ,   3.0f,  // 14: правый верхний
            minX, maxY, minZ,   3.0f,  // 15: левый верхний

            // ========== ЛЕВАЯ ГРАНЬ (X = minX) ==========
            minX, minY, maxZ,   4.0f,  // 16: левый нижний
            minX, minY, minZ,   4.0f,  // 17: правый нижний
            minX, maxY, minZ,   4.0f,  // 18: правый верхний
            minX, maxY, maxZ,   4.0f,  // 19: левый верхний

            // ========== ПРАВАЯ ГРАНЬ (X = maxX) ==========
            maxX, minY, maxZ,   5.0f,  // 20: левый нижний
            maxX, minY, minZ,   5.0f,  // 21: правый нижний
            maxX, maxY, minZ,   5.0f,  // 22: правый верхний
            maxX, maxY, maxZ,   5.0f   // 23: левый верхний
        };
        // Индексы для рёбер (12 рёбер × 2 вершины = 24 индекса)
        const unsigned short indices[] = {
            // Нижняя
            2, 1, 0,
            0, 3, 2,
            // Верхняя
            4,5,6,
            6,7,4,
            // Передняя
            8,9,10,
            10,11,8,
            // Задняя
            14,13,12,
            12,15,14,
            // Левая
            18,17,16,
            16,19,18,
            // Правая
            20,21,22,
            22,23,20
        };
        const unsigned int attrs[3] { 3, 1, 0 };
        mesh.AllocateToGpu(vertices, 24, indices, 36, attrs);
    }

    static void allocateSkyboxMeshForTrianglesWithUV(
        Mesh& mesh,

        const float minX,
        const float minY,
        const float minZ,

        const float maxX,
        const float maxY,
        const float maxZ,

        const float atlasWidth,
        const float atlasHeight,
        const float textureWidth,
        const float textureHeight
    ) {
        const float twStep = textureWidth / atlasWidth;
        const float thStep = textureHeight / atlasHeight;

        const float uStep = twStep;
        const float vStep = thStep;

        const float vertices[] = {
            // ========== НИЖНЯЯ ГРАНЬ (Y = minY) ==========
            minX, minY, maxZ,   uStep * 1, vStep * 2 + thStep,  // 0: левый передний
            maxX, minY, maxZ,   uStep * 1 + twStep, vStep * 2 + thStep,  // 1: правый передний
            maxX, minY, minZ,   uStep * 1 + twStep, vStep * 2,  // 2: правый задний
            minX, minY, minZ,   uStep * 1, vStep * 2,  // 3: левый задний

            // ========== ВЕРХНЯЯ ГРАНЬ (Y = maxY) ==========
            minX, maxY, maxZ,   uStep * 1 + twStep, vStep * 0 + thStep,  // 4: левый передний
            maxX, maxY, maxZ,   uStep * 1, vStep * 0 + thStep,  // 5: правый передний
            maxX, maxY, minZ,   uStep * 1, vStep * 0,  // 6: правый задний
            minX, maxY, minZ,   uStep * 1 + twStep, vStep * 0,  // 7: левый задний

            // ========== ПЕРЕДНЯЯ ГРАНЬ (Z = maxZ) ==========
            minX, minY, maxZ,   uStep * 1, vStep * 1 + thStep,  // 8: левый нижний
            maxX, minY, maxZ,   uStep * 1 + twStep, vStep * 1 + thStep,  // 9: правый нижний
            maxX, maxY, maxZ,   uStep * 1 + twStep, vStep * 1,  // 10: правый верхний
            minX, maxY, maxZ,   uStep * 1, vStep * 1,  // 11: левый верхний

            // ========== ЗАДНЯЯ ГРАНЬ (Z = minZ) ==========
            minX, minY, minZ,   uStep * 3 + twStep, vStep * 1 + thStep,  // 12: левый нижний
            maxX, minY, minZ,   uStep * 3, vStep * 1 + thStep,  // 13: правый нижний
            maxX, maxY, minZ,   uStep * 3, vStep * 1,  // 14: правый верхний
            minX, maxY, minZ,   uStep * 3 + twStep, vStep * 1,  // 15: левый верхний

            // ========== ЛЕВАЯ ГРАНЬ (X = minX) ==========
            minX, minY, maxZ,   uStep * 0 + twStep, vStep * 1 + thStep,  // 16: левый нижний
            minX, minY, minZ,   uStep * 0, vStep * 1 + thStep, // 17: правый нижний
            minX, maxY, minZ,   uStep * 0, vStep * 1, // 18: правый верхний
            minX, maxY, maxZ,   uStep * 0 + twStep, vStep * 1,  // 19: левый верхний

            // ========== ПРАВАЯ ГРАНЬ (X = maxX) ==========
            maxX, minY, maxZ,   uStep * 2, vStep * 1 + thStep,  // 20: левый нижний
            maxX, minY, minZ,   uStep * 2 + twStep, vStep * 1 + thStep, // 21: правый нижний
            maxX, maxY, minZ,   uStep * 2 + twStep, vStep * 1, // 22: правый верхний
            maxX, maxY, maxZ,   uStep * 2, vStep * 1,  // 23: левый верхний
        };
        // Индексы для рёбер (12 рёбер × 2 вершины = 24 индекса)
        const unsigned short indices[] = {
            // Нижняя
            2, 1, 0,
            0, 3, 2,
            // Верхняя
            4,5,6,
            6,7,4,
            // Передняя
            8,9,10,
            10,11,8,
            // Задняя
            12,13,14,
            14,15,12,
            // Левая
            16,17,18,
            18,19,16,
            // Правая
            20,21,22,
            22,23,20
        };
        const unsigned int attrs[3] { 3, 2, 0 };
        mesh.AllocateToGpu(vertices, 24, indices, 36, attrs);
    }
};

#endif
