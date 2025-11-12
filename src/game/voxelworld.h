#ifndef __VOXELWORLD_H__
#define __VOXELWORLD_H__

#include "chunk.h"
#include "worldgen.h"

namespace game
{
    typedef hashtable<ChunkCoord, Chunk*> chunkmap;

    class VoxelWorld
    {
    private:
        chunkmap chunks;
        WorldGenerator *worldGen;
        int renderDistance;
        ChunkCoord lastPlayerChunk;

    public:
        VoxelWorld(unsigned int seed, int renderDist = 8);
        ~VoxelWorld();

        void update(const vec &playerPos);
        void render();

        Chunk* getChunk(const ChunkCoord &coord);
        Chunk* getChunk(int x, int z);
        Chunk* getOrCreateChunk(const ChunkCoord &coord);
        Chunk* getOrCreateChunk(int x, int z);

        Block getBlock(int worldX, int worldY, int worldZ);
        void setBlock(int worldX, int worldY, int worldZ, BlockType type);
        void setBlock(int worldX, int worldY, int worldZ, Block block);

        BiomeType getBiome(int worldX, int worldZ);

        void unloadDistantChunks(const ChunkCoord &playerChunk);
        void generateNearbyChunks(const ChunkCoord &playerChunk);

        static ChunkCoord worldToChunkCoord(int worldX, int worldZ);
        static void worldToLocalCoord(int worldX, int worldY, int worldZ, ChunkCoord &chunk, int &localX, int &localY, int &localZ);

        WorldGenerator* getWorldGenerator() { return worldGen; }
        int getChunkCount() const { return chunks.numelems; }
        void clear();
    };

    inline ChunkCoord VoxelWorld::worldToChunkCoord(int worldX, int worldZ)
    {
        int chunkX = worldX >= 0 ? worldX / CHUNK_SIZE : (worldX - CHUNK_SIZE + 1) / CHUNK_SIZE;
        int chunkZ = worldZ >= 0 ? worldZ / CHUNK_SIZE : (worldZ - CHUNK_SIZE + 1) / CHUNK_SIZE;
        return ChunkCoord(chunkX, chunkZ);
    }

    inline void VoxelWorld::worldToLocalCoord(int worldX, int worldY, int worldZ, ChunkCoord &chunk, int &localX, int &localY, int &localZ)
    {
        chunk = worldToChunkCoord(worldX, worldZ);
        localX = worldX - chunk.x * CHUNK_SIZE;
        localZ = worldZ - chunk.z * CHUNK_SIZE;
        localY = worldY;
        if(localX < 0) localX += CHUNK_SIZE;
        if(localZ < 0) localZ += CHUNK_SIZE;
    }
}

#endif
