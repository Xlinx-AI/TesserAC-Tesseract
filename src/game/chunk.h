#ifndef __CHUNK_H__
#define __CHUNK_H__

#include "cube.h"
#include "biome.h"

namespace game
{
    static const int CHUNK_SIZE = 16;
    static const int CHUNK_HEIGHT = 256;
    static const int CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT;

    enum BlockType
    {
        BLOCK_AIR = 0,
        BLOCK_STONE,
        BLOCK_DIRT,
        BLOCK_GRASS,
        BLOCK_SAND,
        BLOCK_WATER,
        BLOCK_WOOD,
        BLOCK_LEAVES,
        BLOCK_SNOW,
        BLOCK_ICE,
        BLOCK_GRAVEL,
        BLOCK_COAL_ORE,
        BLOCK_IRON_ORE,
        BLOCK_GOLD_ORE,
        BLOCK_DIAMOND_ORE,
        BLOCK_CLAY,
        BLOCK_BEDROCK,
        BLOCK_COUNT
    };

    struct Block
    {
        BlockType type;
        unsigned char data;

        Block() : type(BLOCK_AIR), data(0) {}
        Block(BlockType t) : type(t), data(0) {}
    };

    struct ChunkCoord
    {
        int x, z;

        ChunkCoord() : x(0), z(0) {}
        ChunkCoord(int _x, int _z) : x(_x), z(_z) {}

        bool operator==(const ChunkCoord &other) const
        {
            return x == other.x && z == other.z;
        }

        bool operator<(const ChunkCoord &other) const
        {
            if(x != other.x) return x < other.x;
            return z < other.z;
        }

        unsigned int hash() const
        {
            unsigned int h = 0x811c9dc5;
            h = (h ^ (unsigned int)x) * 0x01000193;
            h = (h ^ (unsigned int)z) * 0x01000193;
            return h;
        }
    };

    struct ChunkMesh
    {
        vector<vec> vertices;
        vector<vec> normals;
        vector<vec2> texcoords;
        vector<unsigned int> indices;
        bool needsRebuild;

        ChunkMesh() : needsRebuild(true) {}

        void clear()
        {
            vertices.shrink(0);
            normals.shrink(0);
            texcoords.shrink(0);
            indices.shrink(0);
        }
    };

    class Chunk
    {
    private:
        ChunkCoord coord;
        Block blocks[CHUNK_VOLUME];
        BiomeType biomes[CHUNK_SIZE * CHUNK_SIZE];
        ChunkMesh mesh;
        bool generated;
        bool meshBuilt;

    public:
        Chunk(ChunkCoord c);
        ~Chunk();

        ChunkCoord getCoord() const { return coord; }
        bool isGenerated() const { return generated; }
        bool isMeshBuilt() const { return meshBuilt; }

        Block getBlock(int x, int y, int z) const;
        void setBlock(int x, int y, int z, Block block);
        void setBlock(int x, int y, int z, BlockType type);

        BiomeType getBiome(int x, int z) const;
        void setBiome(int x, int z, BiomeType biome);

        void markGenerated() { generated = true; }
        void markMeshDirty() { mesh.needsRebuild = true; meshBuilt = false; }
        void markMeshBuilt() { meshBuilt = true; mesh.needsRebuild = false; }

        ChunkMesh& getMesh() { return mesh; }
        const ChunkMesh& getMesh() const { return mesh; }

        void clear();

        static int toIndex(int x, int y, int z);
        static bool isValidCoord(int x, int y, int z);
    };

    inline int Chunk::toIndex(int x, int y, int z)
    {
        return y * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + x;
    }

    inline bool Chunk::isValidCoord(int x, int y, int z)
    {
        return x >= 0 && x < CHUNK_SIZE &&
               y >= 0 && y < CHUNK_HEIGHT &&
               z >= 0 && z < CHUNK_SIZE;
    }

    inline uint hthash(const ChunkCoord &coord)
    {
        return coord.hash();
    }

    inline bool htcmp(const ChunkCoord &a, const ChunkCoord &b)
    {
        return a == b;
    }
}

#endif
