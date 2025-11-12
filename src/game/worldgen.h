#ifndef __WORLDGEN_H__
#define __WORLDGEN_H__

#include "chunk.h"
#include "noise.h"
#include "biome.h"
#include "marchingcubes.h"
#include "worldlayer.h"
#include "vegetation.h"

namespace game
{
    class WorldGenerator
    {
    private:
        NoiseGenerator *noiseGen;
        unsigned int seed;

        double getTerrainHeight(int worldX, int worldZ);
        double getSurfaceDensity(int worldX, int worldY, int worldZ);
        bool isCave(int worldX, int worldY, int worldZ);
        bool isKithgardCave(int worldX, int worldY, int worldZ);
        BlockType selectBlockType(int worldX, int worldY, int worldZ, BiomeType biome, double surfaceDistance, WorldLayer layer);
        void generateOres(Chunk *chunk);
        void generateVegetation(Chunk *chunk);
        void generateSkyIslands(Chunk *chunk);
        void generateKithgardFeatures(Chunk *chunk);

    public:
        WorldGenerator(unsigned int worldSeed);
        ~WorldGenerator();

        void generateChunk(Chunk *chunk);
        void generateChunkMesh(Chunk *chunk);
        
        unsigned int getSeed() const { return seed; }
    };
}

#endif
