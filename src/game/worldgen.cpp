#include "game.h"
#include "worldgen.h"
#include <cmath>
#include <cstdlib>

namespace game
{
    WorldGenerator::WorldGenerator(unsigned int worldSeed) : seed(worldSeed)
    {
        noiseGen = new NoiseGenerator(seed);
        BiomeManager::init();
        VegetationManager::init();
        WorldLayerManager::init();
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

        return height + WORLD_SEA_LEVEL;
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
        WorldLayer layer = WorldLayerManager::getWorldLayer(worldY);
        if(layer != LAYER_CAVES && layer != LAYER_SURFACE) return false;

        double spaghettiCave = noiseGen->getCaveSpaghetti(worldX, worldY, worldZ);
        if(fabs(spaghettiCave) < 0.03) return true;

        double cheeseCave = noiseGen->getCaveCheese(worldX, worldY, worldZ);
        if(cheeseCave > 0.6) return true;

        double noiseCave = noiseGen->getCaveNoise(worldX, worldY, worldZ);
        if(noiseCave > 0.65) return true;

        return false;
    }

    bool WorldGenerator::isKithgardCave(int worldX, int worldY, int worldZ)
    {
        WorldLayer layer = WorldLayerManager::getWorldLayer(worldY);
        if(layer != LAYER_KITHGARD) return false;

        double kithgardNoise = noiseGen->getDensity(worldX, worldY, worldZ);
        double weird = noiseGen->getWeirdness(worldX, worldZ);
        
        double threshold = 0.3 + (weird * 0.3);
        if(kithgardNoise > threshold) return true;

        double spaghettiCave = noiseGen->getCaveSpaghetti(worldX, worldY, worldZ);
        if(fabs(spaghettiCave) < 0.05) return true;

        return false;
    }

    BlockType WorldGenerator::selectBlockType(int worldX, int worldY, int worldZ, BiomeType biome, double surfaceDistance, WorldLayer layer)
    {
        const BiomeData &biomeData = BiomeManager::getBiome(biome);

        if(layer == LAYER_KITHGARD)
        {
            if(worldY <= WORLD_KITHGARD_MIN + 1) return BLOCK_BEDROCK;
            
            double weird = noiseGen->getWeirdness(worldX, worldZ);
            if(surfaceDistance < -5.0)
            {
                if(weird > 0.6) return BLOCK_KITHGARD_CRYSTAL;
                return BLOCK_KITHGARD_STONE;
            }
            
            if(surfaceDistance < 1.0)
            {
                if(weird > 0.8) return BLOCK_KITHGARD_GLOWSTONE;
                return BLOCK_KITHGARD_SOIL;
            }
            if(surfaceDistance < 4.0) return BLOCK_KITHGARD_SOIL;
            
            return BLOCK_KITHGARD_STONE;
        }

        if(layer == LAYER_SKIES)
        {
            if(surfaceDistance < -3.0) return BLOCK_FLOATSTONE;
            if(surfaceDistance < 1.0) return BLOCK_SKY_GRASS;
            if(surfaceDistance < 3.0) return BLOCK_SKY_DIRT;
            return BLOCK_FLOATSTONE;
        }

        if(worldY <= WORLD_KITHGARD_MIN + 1) return BLOCK_BEDROCK;

        if(surfaceDistance < -5.0)
        {
            double stoneNoise = noiseGen->getCaveNoise(worldX, worldY, worldZ);
            if(stoneNoise > 0.3 && worldY < WORLD_SEA_LEVEL - 200) return BLOCK_GRAVEL;
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

        if(biome == BIOME_TUNDRA && worldY > WORLD_SEA_LEVEL + 50)
        {
            if(surfaceDistance < 1.0) return BLOCK_SNOW;
            if(surfaceDistance < 3.0) return BLOCK_DIRT;
            return BLOCK_STONE;
        }

        if(worldY > WORLD_SEA_LEVEL + 100 && (biome == BIOME_TALL_MOUNTAINS || biome == BIOME_MEDIUM_MOUNTAINS))
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
                    WorldLayer layer = WorldLayerManager::getWorldLayer(worldY);
                    double surfaceDistance = terrainHeight - worldY;

                    bool isAir = false;
                    bool cave = false;

                    if(layer == LAYER_SKIES)
                    {
                        bool island = WorldLayerManager::isFloatingIsland(worldX, worldY, worldZ, seed);
                        double density = noiseGen->getDensity(worldX * 0.5, worldY * 0.5, worldZ * 0.5);
                        double altFactor = (worldY - WORLD_SKIES_MIN) / 512.0;
                        if(altFactor > 0.4) altFactor = 0.4;
                        double threshold = 0.6 - altFactor;
                        if(island && density > threshold)
                        {
                            isAir = false;
                            cave = false;
                            surfaceDistance = (double)(WORLD_SKIES_MIN - worldY);
                        }
                        else
                        {
                            isAir = true;
                        }
                    }
                    else if(layer == LAYER_KITHGARD)
                    {
                        isAir = false;
                        cave = isKithgardCave(worldX, worldY, worldZ);
                        surfaceDistance = (double)(WORLD_KITHGARD_MAX - worldY);
                    }
                    else
                    {
                        isAir = worldY > terrainHeight;
                        cave = !isAir && isCave(worldX, worldY, worldZ);
                    }

                    if(isAir || cave)
                    {
                        if(layer == LAYER_KITHGARD && worldY < WORLD_KITHGARD_MIN + 100)
                        {
                            chunk->setBlock(x, y, z, BLOCK_KITHGARD_WATER);
                        }
                        else if(layer == LAYER_SURFACE || layer == LAYER_CAVES)
                        {
                            if(worldY < WORLD_SEA_LEVEL && (biome == BIOME_OCEAN || biome == BIOME_DEEP_OCEAN || 
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
                            chunk->setBlock(x, y, z, BLOCK_AIR);
                        }
                    }
                    else
                    {
                        BlockType blockType = selectBlockType(worldX, worldY, worldZ, biome, surfaceDistance, layer);
                        chunk->setBlock(x, y, z, blockType);
                    }
                }
            }
        }

        generateOres(chunk);
        generateVegetation(chunk);
        generateSkyIslands(chunk);
        generateKithgardFeatures(chunk);
        chunk->markGenerated();
    }

    void WorldGenerator::generateOres(Chunk *chunk)
    {
        ChunkCoord coord = chunk->getCoord();
        int chunkWorldX = coord.x * CHUNK_SIZE;
        int chunkWorldZ = coord.z * CHUNK_SIZE;

        srand(seed ^ ((chunkWorldX << 16) ^ chunkWorldZ));

        for(int i = 0; i < 24; ++i)
        {
            int x = rand() % CHUNK_SIZE;
            int y = rand() % WORLD_SURFACE_MAX;
            int z = rand() % CHUNK_SIZE;
            if(!Chunk::isValidCoord(x, y, z)) continue;
            Block block = chunk->getBlock(x, y, z);
            if(block.type == BLOCK_STONE)
            {
                int oreType = rand() % 100;
                if(oreType < 50) chunk->setBlock(x, y, z, BLOCK_COAL_ORE);
                else if(oreType < 80) chunk->setBlock(x, y, z, BLOCK_IRON_ORE);
                else if(oreType < 92) chunk->setBlock(x, y, z, BLOCK_GOLD_ORE);
                else chunk->setBlock(x, y, z, BLOCK_DIAMOND_ORE);
            }
            else if(block.type == BLOCK_KITHGARD_STONE)
            {
                int rare = rand() % 100;
                if(rare < 20) chunk->setBlock(x, y, z, BLOCK_KITHGARD_METAL);
                else if(rare < 40) chunk->setBlock(x, y, z, BLOCK_KITHGARD_CRYSTAL);
                else if(rare < 55) chunk->setBlock(x, y, z, BLOCK_KITHGARD_GLOWSTONE);
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

    void WorldGenerator::generateVegetation(Chunk *chunk)
    {
        ChunkCoord coord = chunk->getCoord();
        int baseX = coord.x * CHUNK_SIZE;
        int baseZ = coord.z * CHUNK_SIZE;

        for(int x = 0; x < CHUNK_SIZE; ++x)
        {
            for(int z = 0; z < CHUNK_SIZE; ++z)
            {
                int worldX = baseX + x;
                int worldZ = baseZ + z;
                unsigned int columnSeed = seed ^ (unsigned int)(worldX * 374761393) ^ (unsigned int)(worldZ * 668265263);

                BiomeType biome = chunk->getBiome(x, z);
                const std::vector<VegetationType> &vegList = VegetationManager::getBiomeVegetation(biome);

                if(!vegList.empty())
                {
                    int topY = -1;
                    for(int y = std::min(CHUNK_HEIGHT - 2, WORLD_SURFACE_MAX); y >= WORLD_CAVES_MAX; --y)
                    {
                        Block below = chunk->getBlock(x, y, z);
                        if(below.type == BLOCK_AIR || below.type == BLOCK_WATER) continue;
                        if(below.type == BLOCK_VEGETATION) continue;
                        topY = y;
                        break;
                    }

                    if(topY >= 0)
                    {
                        Block ground = chunk->getBlock(x, topY, z);
                        if(ground.type == BLOCK_GRASS || ground.type == BLOCK_DIRT || ground.type == BLOCK_SAND || ground.type == BLOCK_CLAY)
                        {
                            int targetY = topY + 1;
                            if(targetY < CHUNK_HEIGHT)
                            {
                                Block above = chunk->getBlock(x, targetY, z);
                                if(above.type == BLOCK_AIR)
                                {
                                    int attempts = 1 + (columnSeed % 3);
                                    for(int i = 0; i < attempts; ++i)
                                    {
                                        VegetationType veg = vegList[(columnSeed + i * 1013904223u) % vegList.size()];
                                        const VegetationData &vegData = VegetationManager::getVegetation(veg);
                                        if(targetY < vegData.minHeight || targetY > vegData.maxHeight) continue;

                                        if(vegData.needsWater)
                                        {
                                            bool nearWater = false;
                                            static const int offsets[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
                                            for(int j = 0; j < 4; ++j)
                                            {
                                                int nx = x + offsets[j][0];
                                                int nz = z + offsets[j][1];
                                                if(!Chunk::isValidCoord(nx, topY, nz)) continue;
                                                Block neighbour = chunk->getBlock(nx, topY, nz);
                                                if(neighbour.type == BLOCK_WATER || neighbour.type == BLOCK_KITHGARD_WATER)
                                                {
                                                    nearWater = true;
                                                    break;
                                                }
                                            }
                                            if(!nearWater) continue;
                                        }

                                        if(vegData.needsSolidGround && !(ground.type == BLOCK_GRASS || ground.type == BLOCK_DIRT || ground.type == BLOCK_SAND || ground.type == BLOCK_CLAY))
                                        {
                                            continue;
                                        }

                                        Block vegetationBlock;
                                        vegetationBlock.type = BLOCK_VEGETATION;
                                        vegetationBlock.data = (unsigned char)veg;
                                        chunk->setBlock(x, targetY, z, vegetationBlock);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                // Kithgard vegetation
                int kTop = -1;
                for(int y = WORLD_KITHGARD_MAX; y >= WORLD_KITHGARD_MIN; --y)
                {
                    Block ground = chunk->getBlock(x, y, z);
                    if(ground.type == BLOCK_KITHGARD_SOIL)
                    {
                        Block above = (y + 1 < CHUNK_HEIGHT) ? chunk->getBlock(x, y + 1, z) : Block(BLOCK_AIR);
                        if(above.type == BLOCK_AIR)
                        {
                            kTop = y;
                            break;
                        }
                    }
                }

                if(kTop >= 0)
                {
                    int targetY = kTop + 1;
                    if(targetY < CHUNK_HEIGHT)
                    {
                        Block above = chunk->getBlock(x, targetY, z);
                        if(above.type == BLOCK_AIR)
                        {
                            double humid = noiseGen->getHumidity(worldX, worldZ);
                            double weird = noiseGen->getWeirdness(worldX, worldZ);
                            KithgardBiomeType kBiome = WorldLayerManager::selectKithgardBiome(humid, weird, (double)kTop);

                            std::vector<VegetationType> kVeg;
                            switch(kBiome)
                            {
                                case KITHGARD_FOREST:
                                    kVeg = {VEG_KITHGARD_MOSS, VEG_KITHGARD_VINE, VEG_ETHEREAL_PLANT, VEG_UNDERGROUND_FERN, VEG_UNDERGROUND_MUSHROOM_BLUE, VEG_GLOWING_MUSHROOM};
                                    break;
                                case KITHGARD_SWAMP:
                                    kVeg = {VEG_KITHGARD_MOSS, VEG_UNDERGROUND_VINE, VEG_UNDERGROUND_MOSS, VEG_UNDERGROUND_FERN, VEG_LIMINAL_FLOWER};
                                    break;
                                case KITHGARD_LAKE:
                                case KITHGARD_OCEAN:
                                    kVeg = {VEG_UNDERGROUND_VINE, VEG_UNDERGROUND_MOSS, VEG_FLOATING_SPORE, VEG_ETHEREAL_PLANT};
                                    break;
                                case KITHGARD_CRYSTAL_CAVES:
                                    kVeg = {VEG_CRYSTAL_FLOWER, VEG_GLOWING_MUSHROOM, VEG_LIMINAL_FLOWER, VEG_FLOATING_SPORE};
                                    break;
                                case KITHGARD_MUSHROOM_FOREST:
                                    kVeg = {VEG_UNDERGROUND_MUSHROOM_BLUE, VEG_UNDERGROUND_MUSHROOM_PURPLE, VEG_GLOWING_MUSHROOM, VEG_UNDERGROUND_MOSS};
                                    break;
                                case KITHGARD_RUINS:
                                    kVeg = {VEG_KITHGARD_MOSS, VEG_KITHGARD_VINE, VEG_ETHEREAL_PLANT, VEG_FLOATING_SPORE};
                                    break;
                                case KITHGARD_CATACOMBS:
                                case KITHGARD_HALLS:
                                case KITHGARD_VOID:
                                default:
                                    kVeg = {VEG_KITHGARD_MOSS, VEG_UNDERGROUND_MOSS, VEG_UNDERGROUND_VINE, VEG_FLOATING_SPORE};
                                    break;
                            }

                            if(!kVeg.empty())
                            {
                                VegetationType veg = kVeg[columnSeed % kVeg.size()];
                                const VegetationData &vegData = VegetationManager::getVegetation(veg);
                                if(targetY >= vegData.minHeight && targetY <= vegData.maxHeight)
                                {
                                    Block vegetationBlock;
                                    vegetationBlock.type = BLOCK_VEGETATION;
                                    vegetationBlock.data = (unsigned char)veg;
                                    chunk->setBlock(x, targetY, z, vegetationBlock);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void WorldGenerator::generateSkyIslands(Chunk *chunk)
    {
        ChunkCoord coord = chunk->getCoord();
        int baseX = coord.x * CHUNK_SIZE;
        int baseZ = coord.z * CHUNK_SIZE;

        for(int x = 0; x < CHUNK_SIZE; ++x)
        {
            for(int z = 0; z < CHUNK_SIZE; ++z)
            {
                int worldX = baseX + x;
                int worldZ = baseZ + z;
                unsigned int columnSeed = seed ^ (unsigned int)(worldX * 1103515245) ^ (unsigned int)(worldZ * 12345);

                int topY = -1;
                for(int y = CHUNK_HEIGHT - 2; y >= WORLD_SKIES_MIN; --y)
                {
                    Block block = chunk->getBlock(x, y, z);
                    if(block.type == BLOCK_FLOATSTONE || block.type == BLOCK_SKY_DIRT || block.type == BLOCK_SKY_GRASS)
                    {
                        topY = y;
                        break;
                    }
                }

                if(topY < 0) continue;

                Block topBlock = chunk->getBlock(x, topY, z);
                if(topBlock.type == BLOCK_FLOATSTONE)
                {
                    chunk->setBlock(x, topY, z, BLOCK_SKY_GRASS);
                }

                for(int depth = 1; depth <= 3; ++depth)
                {
                    int ny = topY - depth;
                    if(ny < WORLD_SKIES_MIN) break;
                    Block block = chunk->getBlock(x, ny, z);
                    if(block.type == BLOCK_FLOATSTONE)
                    {
                        chunk->setBlock(x, ny, z, BLOCK_SKY_DIRT);
                    }
                }

                int plantY = topY + 1;
                if(plantY < CHUNK_HEIGHT)
                {
                    Block above = chunk->getBlock(x, plantY, z);
                    if(above.type == BLOCK_AIR)
                    {
                        std::vector<VegetationType> skyVeg = {VEG_CLOUD_GRASS, VEG_SKY_FLOWER, VEG_LAPUTA_VINE, VEG_FLOATING_SPORE};
                        VegetationType veg = skyVeg[columnSeed % skyVeg.size()];
                        const VegetationData &vegData = VegetationManager::getVegetation(veg);
                        if(plantY >= vegData.minHeight && plantY <= vegData.maxHeight)
                        {
                            Block vegetationBlock;
                            vegetationBlock.type = BLOCK_VEGETATION;
                            vegetationBlock.data = (unsigned char)veg;
                            chunk->setBlock(x, plantY, z, vegetationBlock);
                        }
                    }
                }
            }
        }
    }

    void WorldGenerator::generateKithgardFeatures(Chunk *chunk)
    {
        ChunkCoord coord = chunk->getCoord();
        int baseX = coord.x * CHUNK_SIZE;
        int baseZ = coord.z * CHUNK_SIZE;

        for(int x = 0; x < CHUNK_SIZE; ++x)
        {
            for(int z = 0; z < CHUNK_SIZE; ++z)
            {
                int worldX = baseX + x;
                int worldZ = baseZ + z;
                unsigned int columnSeed = seed ^ (unsigned int)(worldX * 2654435761u) ^ (unsigned int)(worldZ * 805459861u);
                double humid = noiseGen->getHumidity(worldX, worldZ);
                double weird = noiseGen->getWeirdness(worldX, worldZ);

                for(int y = WORLD_KITHGARD_MIN; y <= WORLD_KITHGARD_MAX; ++y)
                {
                    Block block = chunk->getBlock(x, y, z);
                    if(block.type == BLOCK_KITHGARD_STONE)
                    {
                        if((columnSeed + y) % 97 == 0)
                        {
                            chunk->setBlock(x, y, z, BLOCK_KITHGARD_METAL);
                        }
                        else if((columnSeed + y) % 71 == 0)
                        {
                            chunk->setBlock(x, y, z, BLOCK_KITHGARD_CRYSTAL);
                        }
                    }
                    else if(block.type == BLOCK_AIR)
                    {
                        KithgardBiomeType kBiome = WorldLayerManager::selectKithgardBiome(humid, weird, (double)y);
                        const KithgardBiomeData &data = WorldLayerManager::getKithgardBiome(kBiome);

                        if(data.hasWater && y < WORLD_KITHGARD_MIN + 90 && ((columnSeed + y) % 23) < 3)
                        {
                            chunk->setBlock(x, y, z, BLOCK_KITHGARD_WATER);
                        }
                        else if(data.hasClouds && y > WORLD_KITHGARD_MAX - 30 && ((columnSeed + y) % 37) < 5)
                        {
                            chunk->setBlock(x, y, z, BLOCK_KITHGARD_CLOUD);
                        }
                        else if(data.glowIntensity > 0.7f && ((columnSeed + y) % 113) == 0)
                        {
                            chunk->setBlock(x, y, z, BLOCK_KITHGARD_GLOWSTONE);
                        }
                    }
                }
            }
        }
    }
}
