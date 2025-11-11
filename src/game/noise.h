#ifndef __NOISE_H__
#define __NOISE_H__

#include "cube.h"

namespace game
{
    class PerlinNoise
    {
    private:
        int p[512];
        
        double fade(double t) const;
        double lerp(double t, double a, double b) const;
        double grad(int hash, double x, double y, double z) const;
        
    public:
        PerlinNoise(unsigned int seed = 0);
        double noise(double x, double y, double z) const;
        double octaveNoise(double x, double y, double z, int octaves, double persistence) const;
    };
    
    class SimplexNoise
    {
    private:
        int perm[512];
        
        static const int grad3[12][3];
        static double dot(const int g[], double x, double y, double z);
        int fastfloor(double x) const;
        
    public:
        SimplexNoise(unsigned int seed = 0);
        double noise(double x, double y, double z) const;
        double octaveNoise(double x, double y, double z, int octaves, double persistence, double scale = 1.0) const;
    };
    
    class NoiseGenerator
    {
    private:
        SimplexNoise continentalness;
        SimplexNoise erosion;
        SimplexNoise peaks_valleys;
        SimplexNoise temperature;
        SimplexNoise humidity;
        SimplexNoise weirdness;
        SimplexNoise cave_noise1;
        SimplexNoise cave_noise2;
        SimplexNoise cave_noise3;
        
    public:
        NoiseGenerator(unsigned int seed);
        
        double getContinentalness(double x, double z) const;
        double getErosion(double x, double z) const;
        double getPeaksValleys(double x, double z) const;
        double getTemperature(double x, double z) const;
        double getHumidity(double x, double z) const;
        double getWeirdness(double x, double z) const;
        double getDensity(double x, double y, double z) const;
        double getCaveNoise(double x, double y, double z) const;
        double getCaveSpaghetti(double x, double y, double z) const;
        double getCaveCheese(double x, double y, double z) const;
    };
}

#endif
