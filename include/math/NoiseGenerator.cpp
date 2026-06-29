#include "NoiseGenerator.h"

#include <cmath>
#include <random>
#include <numeric>
#include <algorithm>
#include <functional>

NoiseGenerator::NoiseGenerator(const int seed) {
    InitGenerator(seed);
}

void NoiseGenerator::InitGenerator(const int seed) {
    m_Seed = seed;

    std::iota(m_Permutation.begin(), m_Permutation.begin() + 256, 0);

    std::mt19937 rng(seed);
    std::shuffle(m_Permutation.begin(), m_Permutation.begin() + 256, rng);

    std::copy(
        m_Permutation.begin(), m_Permutation.begin() + 256,
        m_Permutation.begin() + 256
    );
}

double NoiseGenerator::GetNoise2D(const double x, const double z) const {
    const int xi = (int)std::floor(x) & 255;
    const int zi = (int)std::floor(z) & 255;

    const double xf = x - std::floor(x);
    const double zf = z - std::floor(z);

    const double u = Fade(xf);
    const double v = Fade(zf);

    int a = m_Permutation[xi] + zi;
    int aa = m_Permutation[a];
    int ab = m_Permutation[a + 1];
    int b = m_Permutation[xi + 1] + zi;
    int ba = m_Permutation[b];
    int bb = m_Permutation[b + 1];

    double x1 = Lerp(Grad(aa, xf, zf), Grad(ba, xf - 1, zf), u);
    double x2 = Lerp(Grad(ab, xf, zf - 1), Grad(bb, xf - 1, zf - 1), u);

    return (Lerp(x1, x2, v) + 1.0) / 2.0;
}

double NoiseGenerator::GetNoiseFractal(
    const double x, const double z,
    const unsigned int octaves, const double persistence, const double lacunarity
) const {
    double amplitude = 1.0;
    double frequency = 1.0;
    double noise = 0.0;
    double maxAmplitude = 0.0;

    for (unsigned int i = 0; i < octaves; i++) {
        noise += GetNoise2D(x * frequency, z * frequency) * amplitude;
        maxAmplitude += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return noise / maxAmplitude;
}


double NoiseGenerator::Fade(const double t) const {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double NoiseGenerator::Lerp(const double a, const double b, const double t) const {
    return a + (b - a) * t;
}

double NoiseGenerator::Grad(const int hash, const double x, const double y) const {
    const int h = hash & 7;
    const double u = h < 4 ? x : y;
    const double v = h < 4 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}
