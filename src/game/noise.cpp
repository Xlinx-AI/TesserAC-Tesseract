#include "game.h"
#include "noise.h"
#include <cmath>
#include <cstdlib>

namespace game
{
    static const int PERMUTATION_COUNT = 256;
    static const int PERMUTATION[PERMUTATION_COUNT] = {
        151,160,137,91,90,15,
        131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
        8,99,37,240,21,10,23,
        190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
        35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,
        168, 68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,
        111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
        102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208,
        89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,
        186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,
        82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,
        183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,
        167, 43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,
        178,185, 112,104,218, 246,97,228,251,34,242,193,238,210,144,12,
        191,179,162,241, 81,51,145,235,249,14,239,107,49,192,214, 31,
        181,199,106,157,184, 84,204,176,115,121, 50,45,127,  4,150,254
    };

    double PerlinNoise::fade(double t) const
    {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    double PerlinNoise::lerp(double t, double a, double b) const
    {
        return a + t * (b - a);
    }

    double PerlinNoise::grad(int hash, double x, double y, double z) const
    {
        int h = hash & 15;
        double u = h < 8 ? x : y;
        double v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
        return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
    }

    PerlinNoise::PerlinNoise(unsigned int seed)
    {
        int permutation[256];
        for(int i = 0; i < 256; ++i) permutation[i] = PERMUTATION[i % PERMUTATION_COUNT];
        if(seed)
        {
            srand(seed);
            for(int i = 255; i > 0; --i)
            {
                int target = rand() % (i + 1);
                int temp = permutation[i];
                permutation[i] = permutation[target];
                permutation[target] = temp;
            }
        }
        for(int i = 0; i < 256; ++i)
        {
            p[i] = permutation[i];
            p[256 + i] = permutation[i];
        }
    }

    double PerlinNoise::noise(double x, double y, double z) const
    {
        int X = (int)floor(x) & 255;
        int Y = (int)floor(y) & 255;
        int Z = (int)floor(z) & 255;

        x -= floor(x);
        y -= floor(y);
        z -= floor(z);

        double u = fade(x);
        double v = fade(y);
        double w = fade(z);

        int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
        int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

        return lerp(w,
            lerp(v,
                lerp(u, grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z)),
                lerp(u, grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z))
            ),
            lerp(v,
                lerp(u, grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1)),
                lerp(u, grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1))
            )
        );
    }

    double PerlinNoise::octaveNoise(double x, double y, double z, int octaves, double persistence) const
    {
        double total = 0;
        double frequency = 1;
        double amplitude = 1;
        double maxValue = 0;

        for(int i = 0; i < octaves; ++i)
        {
            total += noise(x * frequency, y * frequency, z * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= 2;
        }

        return total / maxValue;
    }

    const int SimplexNoise::grad3[12][3] = {
        {1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
        {1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
        {0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1}
    };

    double SimplexNoise::dot(const int g[], double x, double y, double z)
    {
        return g[0]*x + g[1]*y + g[2]*z;
    }

    int SimplexNoise::fastfloor(double x) const
    {
        return x > 0 ? (int)x : (int)x - 1;
    }

    SimplexNoise::SimplexNoise(unsigned int seed)
    {
        int permute[256];
        for(int i = 0; i < 256; ++i) permute[i] = PERMUTATION[i % PERMUTATION_COUNT];
        if(seed)
        {
            srand(seed);
            for(int i = 255; i > 0; --i)
            {
                int j = rand() % (i + 1);
                int temp = permute[i];
                permute[i] = permute[j];
                permute[j] = temp;
            }
        }
        for(int i = 0; i < 256; ++i)
        {
            perm[i] = permute[i];
            perm[256 + i] = permute[i];
        }
    }

    double SimplexNoise::noise(double x, double y, double z) const
    {
        static const double F3 = 1.0/3.0;
        static const double G3 = 1.0/6.0;

        double s = (x + y + z) * F3;
        int i = fastfloor(x + s);
        int j = fastfloor(y + s);
        int k = fastfloor(z + s);

        double t = (i + j + k) * G3;
        double X0 = i - t;
        double Y0 = j - t;
        double Z0 = k - t;
        double x0 = x - X0;
        double y0 = y - Y0;
        double z0 = z - Z0;

        int i1, j1, k1;
        int i2, j2, k2;

        if(x0 >= y0)
        {
            if(y0 >= z0) { i1=1; j1=0; k1=0; i2=1; j2=1; k2=0; }
            else if(x0 >= z0) { i1=1; j1=0; k1=0; i2=1; j2=0; k2=1; }
            else { i1=0; j1=0; k1=1; i2=1; j2=0; k2=1; }
        }
        else
        {
            if(y0 < z0) { i1=0; j1=0; k1=1; i2=0; j2=1; k2=1; }
            else if(x0 < z0) { i1=0; j1=1; k1=0; i2=0; j2=1; k2=1; }
            else { i1=0; j1=1; k1=0; i2=1; j2=1; k2=0; }
        }

        double x1 = x0 - i1 + G3;
        double y1 = y0 - j1 + G3;
        double z1 = z0 - k1 + G3;
        double x2 = x0 - i2 + 2.0*G3;
        double y2 = y0 - j2 + 2.0*G3;
        double z2 = z0 - k2 + 2.0*G3;
        double x3 = x0 - 1.0 + 3.0*G3;
        double y3 = y0 - 1.0 + 3.0*G3;
        double z3 = z0 - 1.0 + 3.0*G3;

        int ii = i & 255;
        int jj = j & 255;
        int kk = k & 255;

        double n0, n1, n2, n3;

        double t0 = 0.6 - x0*x0 - y0*y0 - z0*z0;
        if(t0 < 0) n0 = 0.0;
        else
        {
            t0 *= t0;
            n0 = t0 * t0 * dot(grad3[perm[ii + perm[jj + perm[kk]]] % 12], x0, y0, z0);
        }

        double t1 = 0.6 - x1*x1 - y1*y1 - z1*z1;
        if(t1 < 0) n1 = 0.0;
        else
        {
            t1 *= t1;
            n1 = t1 * t1 * dot(grad3[perm[ii + i1 + perm[jj + j1 + perm[kk + k1]]] % 12], x1, y1, z1);
        }

        double t2 = 0.6 - x2*x2 - y2*y2 - z2*z2;
        if(t2 < 0) n2 = 0.0;
        else
        {
            t2 *= t2;
            n2 = t2 * t2 * dot(grad3[perm[ii + i2 + perm[jj + j2 + perm[kk + k2]]] % 12], x2, y2, z2);
        }

        double t3 = 0.6 - x3*x3 - y3*y3 - z3*z3;
        if(t3 < 0) n3 = 0.0;
        else
        {
            t3 *= t3;
            n3 = t3 * t3 * dot(grad3[perm[ii + 1 + perm[jj + 1 + perm[kk + 1]]] % 12], x3, y3, z3);
        }

        return 32.0 * (n0 + n1 + n2 + n3);
    }

    double SimplexNoise::octaveNoise(double x, double y, double z, int octaves, double persistence, double scale) const
    {
        double total = 0;
        double frequency = 1;
        double amplitude = 1;
        double maxValue = 0;

        for(int i = 0; i < octaves; ++i)
        {
            total += noise(x * frequency * scale, y * frequency * scale, z * frequency * scale) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= 2;
        }

        return total / maxValue;
    }

    NoiseGenerator::NoiseGenerator(unsigned int seed) :
        continentalness(seed + 1),
        erosion(seed + 2),
        peaks_valleys(seed + 3),
        temperature(seed + 4),
        humidity(seed + 5),
        weirdness(seed + 6),
        cave_noise1(seed + 7),
        cave_noise2(seed + 8),
        cave_noise3(seed + 9)
    {
    }

    double NoiseGenerator::getContinentalness(double x, double z) const
    {
        return continentalness.octaveNoise(x, 0.0, z, 6, 0.5, 0.0015);
    }

    double NoiseGenerator::getErosion(double x, double z) const
    {
        return erosion.octaveNoise(x, 0.0, z, 5, 0.6, 0.0025);
    }

    double NoiseGenerator::getPeaksValleys(double x, double z) const
    {
        return peaks_valleys.octaveNoise(x, 0.0, z, 5, 0.55, 0.002);
    }

    double NoiseGenerator::getTemperature(double x, double z) const
    {
        return temperature.octaveNoise(x, 0.0, z, 4, 0.6, 0.0005);
    }

    double NoiseGenerator::getHumidity(double x, double z) const
    {
        return humidity.octaveNoise(x, 0.0, z, 4, 0.6, 0.0005);
    }

    double NoiseGenerator::getWeirdness(double x, double z) const
    {
        return weirdness.octaveNoise(x, 0.0, z, 3, 0.5, 0.002);
    }

    double NoiseGenerator::getDensity(double x, double y, double z) const
    {
        double cont = getContinentalness(x, z);
        double erosionFactor = getErosion(x, z);
        double peaks = getPeaksValleys(x, z);
        double value = continentalness.octaveNoise(x, y, z, 5, 0.5, 0.001);
        value += peaks * 0.35;
        value -= erosionFactor * 0.2;
        value += cont * 0.5;
        value -= (y - 64.0) * 0.015;
        return value;
    }

    double NoiseGenerator::getCaveNoise(double x, double y, double z) const
    {
        return cave_noise1.octaveNoise(x, y, z, 4, 0.6, 0.01);
    }

    double NoiseGenerator::getCaveSpaghetti(double x, double y, double z) const
    {
        double a = cave_noise2.octaveNoise(x, y, z, 3, 0.7, 0.03);
        double b = cave_noise3.octaveNoise(x, y, z, 3, 0.7, 0.03);
        return a * b;
    }

    double NoiseGenerator::getCaveCheese(double x, double y, double z) const
    {
        return cave_noise1.octaveNoise(x, y, z, 5, 0.5, 0.02);
    }
}
