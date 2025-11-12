#include "game.h"
#include "chunk.h"

namespace game
{
    Chunk::Chunk(ChunkCoord c) :
        coord(c),
        generated(false),
        meshBuilt(false)
    {
        clear();
    }

    Chunk::~Chunk()
    {
        mesh.clear();
    }

    void Chunk::clear()
    {
        for(int i = 0; i < CHUNK_VOLUME; ++i)
        {
            blocks[i] = Block(BLOCK_AIR);
        }
        for(int i = 0; i < CHUNK_SIZE * CHUNK_SIZE; ++i)
        {
            biomes[i] = BIOME_PLAINS;
        }
        mesh.clear();
        mesh.needsRebuild = true;
        generated = false;
        meshBuilt = false;
    }

    Block Chunk::getBlock(int x, int y, int z) const
    {
        if(!isValidCoord(x, y, z)) return Block(BLOCK_AIR);
        return blocks[toIndex(x, y, z)];
    }

    void Chunk::setBlock(int x, int y, int z, Block block)
    {
        if(!isValidCoord(x, y, z)) return;
        blocks[toIndex(x, y, z)] = block;
    }

    void Chunk::setBlock(int x, int y, int z, BlockType type)
    {
        if(!isValidCoord(x, y, z)) return;
        blocks[toIndex(x, y, z)].type = type;
    }

    BiomeType Chunk::getBiome(int x, int z) const
    {
        if(x < 0 || x >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) return BIOME_PLAINS;
        return biomes[z * CHUNK_SIZE + x];
    }

    void Chunk::setBiome(int x, int z, BiomeType biome)
    {
        if(x < 0 || x >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) return;
        biomes[z * CHUNK_SIZE + x] = biome;
    }
}
