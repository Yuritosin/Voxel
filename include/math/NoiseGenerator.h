#ifndef NOISE_GENERATOR_H
#define NOISE_GENERATOR_H

#include <array>

class NoiseGenerator {
public:
    ~NoiseGenerator() = default;
    NoiseGenerator(const int seed);

    void InitGenerator(const int seed);

    double GetNoise2D(const double x, const double z) const;
    double GetNoiseFractal(
        const double x, const double z,
        const unsigned int octaves, const double persistence, const double lacunarity
    ) const;
private:
    int m_Seed;
    std::array<int, 512> m_Permutation;

    double Fade(const double t) const;
    double Lerp(const double a, const double b, const double t) const;
    double Grad(const int hash, const double x, const double y) const;
};

#endif
