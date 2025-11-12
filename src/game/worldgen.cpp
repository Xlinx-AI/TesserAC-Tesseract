#include "game.h"
#include "worldgen.h"
#include <cmath>
#include <cstdlib>

namespace game
{
    WorldGenerator::WorldGenerator(unsigned int worldSeed) : seed(worldSeed)
    {
        noiseGen = new NoiseGenerator(seed);
    }

    WorldGenerator::~WorldGenerator()
    {
        delete noiseGen;
    }

    double WorldGenerator::getTerrainHeight(int worldX, int worldZ)
    {
        double cont = noiseGen->getContinentalness(worldX, worldZ);
        double erosion = noiseGen->getErosion(worldX, worldZ);
        double peaks = noiseGen->getPeaksValleys(worldX, worldZ);
        double temp = noiseGen->getTemperature(worldX, worldZ);
        double humid = noiseGen->getHumidity(worldX, worldZ);
        double weirdness = noiseGen->getWeirdness(worldX, worldZ);

        BiomeType biome = BiomeManager::selectBiome(cont, erosion, peaks, temp, humid, weirdness);
        const BiomeData &biomeData = BiomeManager::getBiome(biome);

        double baseHeight = biomeData.baseHeight;
        double heightVar = biomeData.heightVariation;

        double height = baseHeight + peaks * heightVar * 0.5;
        height -= erosion * heightVar * 0.3;
        height += weirdness * 5.0;

        return height + 64.0;
    }

    double WorldGenerator::getSurfaceDensity(int worldX, int worldY, int worldZ)
    {
        double terrainHeight = getTerrainHeight(worldX, worldZ);
        double density = noiseGen->getDensity(worldX, worldY, worldZ);

        double heightDiff = worldY - terrainHeight;
        density -= heightDiff * 0.1;

        return density;
    }

    bool WorldGenerator::isCave(int worldX, int worldY, int worldZ)
    {
        if(worldY < 5) return false;
        if(worldY > 240) return false;

        double spaghettiCave = noiseGen->getCaveSpaghetti(worldX, worldY, worldZ);
        if(fabs(spaghettiCave) < 0.03) return true;

        double cheeseCave = noiseGen->getCaveCheese(worldX, worldY, worldZ);
        if(cheeseCave > 0.6) return true;

        double noiseCave = noiseGen->getCaveNoise(worldX, worldY, worldZ);
        if(noiseCave > 0.65) return true;

        return false;
    }

    BlockType WorldGenerator::selectBlockType(int worldX, int worldY, int worldZ, BiomeType biome, double surfaceDistance)
    {
        const BiomeData &biomeData = BiomeManager::getBiome(biome);

        if(worldY <= 1) return BLOCK_BEDROCK;

        if(surfaceDistance < -5.0)
        {
            double stoneNoise = noiseGen->getCaveNoise(worldX, worldY, worldZ);
            if(stoneNoise > 0.3 && worldY < 20) return BLOCK_GRAVEL;
            return BLOCK_STONE;
        }

        if(biome == BIOME_OCEAN || biome == BIOME_DEEP_OCEAN || biome == BIOME_LAKE || biome == BIOME_RIVER)
        {
            if(surfaceDistance < -1.0) return BLOCK_SAND;
            if(surfaceDistance < 0.0) return BLOCK_GRAVEL;
            return BLOCK_STONE;
        }

        if(biome == BIOME_DESERT || biome == BIOME_MESA)
        {
            if(surfaceDistance < 0.5) return BLOCK_SAND;
            if(surfaceDistance < 4.0) return BLOCK_SAND;
            return BLOCK_STONE;
        }

        if(biome == BIOME_TUNDRA && worldY > 120)
        {
            if(surfaceDistance < 1.0) return BLOCK_SNOW;
            if(surfaceDistance < 3.0) return BLOCK_DIRT;
            return BLOCK_STONE;
        }

        if(worldY > 140 && (biome == BIOME_TALL_MOUNTAINS || biome == BIOME_MEDIUM_MOUNTAINS))
        {
            if(surfaceDistance < 0.5) return BLOCK_SNOW;
            return BLOCK_STONE;
        }

        if(surfaceDistance < 1.0) return BLOCK_GRASS;
        if(surfaceDistance < 4.0) return BLOCK_DIRT;

        return BLOCK_STONE;
    }

    void WorldGenerator::generateChunk(Chunk *chunk)
    {
        ChunkCoord coord = chunk->getCoord();
        int chunkWorldX = coord.x * CHUNK_SIZE;
        int chunkWorldZ = coord.z * CHUNK_SIZE;

        for(int x = 0; x < CHUNK_SIZE; ++x)
        {
            for(int z = 0; z < CHUNK_SIZE; ++z)
            {
                int worldX = chunkWorldX + x;
                int worldZ = chunkWorldZ + z;

                double cont = noiseGen->getContinentalness(worldX, worldZ);
                double erosion = noiseGen->getErosion(worldX, worldZ);
                double peaks = noiseGen->getPeaksValleys(worldX, worldZ);
                double temp = noiseGen->getTemperature(worldX, worldZ);
                double humid = noiseGen->getHumidity(worldX, worldZ);
                double weirdness = noiseGen->getWeirdness(worldX, worldZ);

                BiomeType biome = BiomeManager::selectBiome(cont, erosion, peaks, temp, humid, weirdness);
                chunk->setBiome(x, z, biome);

                double terrainHeight = getTerrainHeight(worldX, worldZ);

                for(int y = 0; y < CHUNK_HEIGHT; ++y)
                {
                    int worldY = y;
                    double surfaceDistance = terrainHeight - worldY;

                    bool isAir = worldY > terrainHeight;
                    bool cave = !isAir && isCave(worldX, worldY, worldZ);

                    if(isAir || cave)
                    {
                        if(worldY < 63 && (biome == BIOME_OCEAN || biome == BIOME_DEEP_OCEAN || 
                            biome == BIOME_LAKE || biome == BIOME_RIVER))
                        {
                            chunk->setBlock(x, y, z, BLOCK_WATER);
                        }
                        else
                        {
                            chunk->setBlock(x, y, z, BLOCK_AIR);
                        }
                    }
                    else
                    {
                        BlockType blockType = selectBlockType(worldX, worldY, worldZ, biome, surfaceDistance);
                        chunk->setBlock(x, y, z, blockType);
                    }
                }
            }
        }

        generateOres(chunk);
        chunk->markGenerated();
    }

    void WorldGenerator::generateOres(Chunk *chunk)
    {
        ChunkCoord coord = chunk->getCoord();
        int chunkWorldX = coord.x * CHUNK_SIZE;
        int chunkWorldZ = coord.z * CHUNK_SIZE;

        srand(seed ^ ((chunkWorldX << 16) ^ chunkWorldZ));

        for(int i = 0; i < 8; ++i)
        {
            int x = rand() % CHUNK_SIZE;
            int y = rand() % 128 + 1;
            int z = rand() % CHUNK_SIZE;
            Block block = chunk->getBlock(x, y, z);
            if(block.type == BLOCK_STONE)
            {
                int oreType = rand() % 100;
                if(oreType < 60) chunk->setBlock(x, y, z, BLOCK_COAL_ORE);
                else if(oreType < 85) chunk->setBlock(x, y, z, BLOCK_IRON_ORE);
                else if(oreType < 95) chunk->setBlock(x, y, z, BLOCK_GOLD_ORE);
                else chunk->setBlock(x, y, z, BLOCK_DIAMOND_ORE);
            }
        }
    }

    void WorldGenerator::generateChunkMesh(Chunk *chunk)
    {
        if(!chunk->isGenerated()) return;

        ChunkMesh &mesh = chunk->getMesh();
        mesh.clear();

        for(int x = 0; x < CHUNK_SIZE; ++x)
        {
            for(int z = 0; z < CHUNK_SIZE; ++z)
            {
                for(int y = 0; y < CHUNK_HEIGHT; ++y)
                {
                    Block block = chunk->getBlock(x, y, z);
                    if(block.type == BLOCK_AIR || block.type == BLOCK_WATER) continue;

                    bool drawTop = (y == CHUNK_HEIGHT - 1) || 
                                   (chunk->getBlock(x, y + 1, z).type == BLOCK_AIR);
                    bool drawBottom = (y == 0) || 
                                      (chunk->getBlock(x, y - 1, z).type == BLOCK_AIR);
                    bool drawLeft = (x == 0) || 
                                    (chunk->getBlock(x - 1, y, z).type == BLOCK_AIR);
                    bool drawRight = (x == CHUNK_SIZE - 1) || 
                                     (chunk->getBlock(x + 1, y, z).type == BLOCK_AIR);
                    bool drawFront = (z == CHUNK_SIZE - 1) || 
                                     (chunk->getBlock(x, y, z + 1).type == BLOCK_AIR);
                    bool drawBack = (z == 0) || 
                                    (chunk->getBlock(x, y, z - 1).type == BLOCK_AIR);

                    if(!drawTop && !drawBottom && !drawLeft && !drawRight && !drawFront && !drawBack)
                        continue;

                    float fx = (float)x;
                    float fy = (float)y;
                    float fz = (float)z;

                    unsigned int baseIdx = mesh.vertices.length();

                    if(drawTop)
                    {
                        mesh.vertices.add(vec(fx, fy + 1, fz));
                        mesh.vertices.add(vec(fx + 1, fy + 1, fz));
                        mesh.vertices.add(vec(fx + 1, fy + 1, fz + 1));
                        mesh.vertices.add(vec(fx, fy + 1, fz + 1));
                        vec normal(0, 1, 0);
                        for(int i = 0; i < 4; ++i) mesh.normals.add(normal);
                        mesh.indices.add(baseIdx);
                        mesh.indices.add(baseIdx + 1);
                        mesh.indices.add(baseIdx + 2);
                        mesh.indices.add(baseIdx);
                        mesh.indices.add(baseIdx + 2);
                        mesh.indices.add(baseIdx + 3);
                        baseIdx += 4;
                    }

                    if(drawBottom)
                    {
                        mesh.vertices.add(vec(fx, fy, fz));
                        mesh.vertices.add(vec(fx, fy, fz + 1));
                        mesh.vertices.add(vec(fx + 1, fy, fz + 1));
                        mesh.vertices.add(vec(fx + 1, fy, fz));
                        vec normal(0, -1, 0);
                        for(int i = 0; i < 4; ++i) mesh.normals.add(normal);
                        mesh.indices.add(baseIdx);
                        mesh.indices.add(baseIdx + 1);
                        mesh.indices.add(baseIdx + 2);
                        mesh.indices.add(baseIdx);
                        mesh.indices.add(baseIdx + 2);
                        mesh.indices.add(baseIdx + 3);
                        baseIdx += 4;
                    }

                    if(drawLeft)
                    {
                        mesh.vertices.add(vec(fx, fy, fz));
                        mesh.vertices.add(vec(fx, fy + 1, fz));
                        mesh.vertices.add(vec(fx, fy + 1, fz + 1));
                        mesh.vertices.add(vec(fx, fy, fz + 1));
                        vec normal(-1, 0, 0);
                        for(int i = 0; i < 4; ++i) mesh.normals.add(normal);
                        mesh.indices.add(baseIdx);
                        mesh.indices.add(baseIdx + 1);
                        mesh.indices.add(baseIdx + 2);
                        mesh.indices.add(baseIdx);
                        mesh.indices.add(baseIdx + 2);
                        mesh.indices.add(baseIdx + 3);
                        baseIdx += 4;
                    }

                    if(drawRight)
                    {
                        mesh.vertices.add(vec(fx + 1, fy, fz));
                        mesh.vertices.add(vec(fx + 1, fy, fz + 1));
                        mesh.vertices.add(vec(fx + 1, fy + 1, fz + 1));
                        mesh.vertices.add(vec(fx + 1, fy + 1, fz));
                        vec normal(1, 0, 0);
                        for(int i = 0; i < 4; ++i) mesh.normals.add(normal);
                        mesh.indices.add(baseIdx);
                        mesh.indices.add(baseIdx + 1);
                        mesh.indices.add(baseIdx + 2);
                        mesh.indices.add(baseIdx);
                        mesh.indices.add(baseIdx + 2);
                        mesh.indices.add(baseIdx + 3);
                        baseIdx += 4;
                    }

                    if(drawFront)
                    {
                        mesh.vertices.add(vec(fx, fy, fz + 1));
                        mesh.vertices.add(vec(fx, fy + 1, fz + 1));
                        mesh.vertices.add(vec(fx + 1, fy + 1, fz + 1));
                        mesh.vertices.add(vec(fx + 1, fy, fz + 1));
                        vec normal(0, 0, 1);
                        for(int i = 0; i < 4; ++i) mesh.normals.add(normal);
                        mesh.indices.add(baseIdx);
                        mesh.indices.add(baseIdx + 1);
                        mesh.indices.add(baseIdx + 2);
                        mesh.indices.add(baseIdx);
                        mesh.indices.add(baseIdx + 2);
                        mesh.indices.add(baseIdx + 3);
                        baseIdx += 4;
                    }

                    if(drawBack)
                    {
                        mesh.vertices.add(vec(fx, fy, fz));
                        mesh.vertices.add(vec(fx + 1, fy, fz));
                        mesh.vertices.add(vec(fx + 1, fy + 1, fz));
                        mesh.vertices.add(vec(fx, fy + 1, fz));
                        vec normal(0, 0, -1);
                        for(int i = 0; i < 4; ++i) mesh.normals.add(normal);
                        mesh.indices.add(baseIdx);
                        mesh.indices.add(baseIdx + 1);
                        mesh.indices.add(baseIdx + 2);
                        mesh.indices.add(baseIdx);
                        mesh.indices.add(baseIdx + 2);
                        mesh.indices.add(baseIdx + 3);
                        baseIdx += 4;
                    }
                }
            }
        }

        chunk->markMeshBuilt();
    }
}
