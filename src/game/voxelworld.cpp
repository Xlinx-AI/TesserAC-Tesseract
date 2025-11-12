#include "game.h"
#include "voxelworld.h"
#include "biome.h"
#include "worldgen.h"
#include "chunk.h"

namespace game
{
    VoxelWorld::VoxelWorld(unsigned int seed, int renderDist) :
        worldGen(new WorldGenerator(seed)),
        renderDistance(renderDist),
        lastPlayerChunk(0, 0)
    {
        BiomeManager::init();
    }

    VoxelWorld::~VoxelWorld()
    {
        clear();
        delete worldGen;
    }

    void VoxelWorld::clear()
    {
        enumerate(chunks, Chunk*, chunk,
        {
            delete chunk;
        });
        chunks.clear();
    }

    Chunk* VoxelWorld::getChunk(const ChunkCoord &coord)
    {
        Chunk **c = chunks.access(coord);
        return c ? *c : NULL;
    }

    Chunk* VoxelWorld::getChunk(int x, int z)
    {
        return getChunk(ChunkCoord(x, z));
    }

    Chunk* VoxelWorld::getOrCreateChunk(const ChunkCoord &coord)
    {
        Chunk *&entry = chunks[coord];
        if(!entry)
        {
            entry = new Chunk(coord);
            worldGen->generateChunk(entry);
            worldGen->generateChunkMesh(entry);
        }
        return entry;
    }

    Chunk* VoxelWorld::getOrCreateChunk(int x, int z)
    {
        return getOrCreateChunk(ChunkCoord(x, z));
    }

    Block VoxelWorld::getBlock(int worldX, int worldY, int worldZ)
    {
        ChunkCoord chunkCoord;
        int localX, localY, localZ;
        worldToLocalCoord(worldX, worldY, worldZ, chunkCoord, localX, localY, localZ);
        Chunk *chunk = getOrCreateChunk(chunkCoord);
        if(!chunk) return Block(BLOCK_AIR);
        return chunk->getBlock(localX, localY, localZ);
    }

    void VoxelWorld::setBlock(int worldX, int worldY, int worldZ, BlockType type)
    {
        setBlock(worldX, worldY, worldZ, Block(type));
    }

    void VoxelWorld::setBlock(int worldX, int worldY, int worldZ, Block block)
    {
        ChunkCoord chunkCoord;
        int localX, localY, localZ;
        worldToLocalCoord(worldX, worldY, worldZ, chunkCoord, localX, localY, localZ);
        Chunk *chunk = getOrCreateChunk(chunkCoord);
        if(!chunk) return;
        chunk->setBlock(localX, localY, localZ, block);
        chunk->markMeshDirty();
    }

    BiomeType VoxelWorld::getBiome(int worldX, int worldZ)
    {
        ChunkCoord coord;
        int localX, localY, localZ;
        worldToLocalCoord(worldX, 0, worldZ, coord, localX, localY, localZ);
        Chunk *chunk = getOrCreateChunk(coord);
        if(!chunk) return BIOME_PLAINS;
        return chunk->getBiome(localX, localZ);
    }

    void VoxelWorld::unloadDistantChunks(const ChunkCoord &playerChunk)
    {
        struct ChunkRemoval
        {
            ChunkCoord coord;
            Chunk *chunk;
        };
        vector<ChunkRemoval> toRemove;
        enumeratekt(chunks, ChunkCoord, coord, Chunk*, chunk,
        {
            int dx = coord.x - playerChunk.x;
            int dz = coord.z - playerChunk.z;
            int adx = dx < 0 ? -dx : dx;
            int adz = dz < 0 ? -dz : dz;
            if(adx > renderDistance || adz > renderDistance)
            {
                ChunkRemoval &removal = toRemove.add();
                removal.coord = coord;
                removal.chunk = chunk;
            }
        });

        loopv(toRemove)
        {
            ChunkRemoval &removal = toRemove[i];
            if(removal.chunk)
            {
                delete removal.chunk;
                chunks.remove(removal.coord);
            }
        }
    }

    void VoxelWorld::generateNearbyChunks(const ChunkCoord &playerChunk)
    {
        for(int dx = -renderDistance; dx <= renderDistance; ++dx)
        {
            for(int dz = -renderDistance; dz <= renderDistance; ++dz)
            {
                ChunkCoord coord(playerChunk.x + dx, playerChunk.z + dz);
                getOrCreateChunk(coord);
            }
        }
    }

    void VoxelWorld::update(const vec &playerPos)
    {
        ChunkCoord playerChunk = worldToChunkCoord((int)playerPos.x, (int)playerPos.z);

        if(playerChunk.x == lastPlayerChunk.x && playerChunk.z == lastPlayerChunk.z)
        {
            return;
        }

        lastPlayerChunk = playerChunk;
        generateNearbyChunks(playerChunk);
        unloadDistantChunks(playerChunk);
    }

    void VoxelWorld::render()
    {
        enumeratekt(chunks, ChunkCoord, coord, Chunk*, chunk,
        {
            if(!chunk->isMeshBuilt())
            {
                worldGen->generateChunkMesh(chunk);
            }

            const ChunkMesh &mesh = chunk->getMesh();
            if(mesh.vertices.empty()) continue;

            glBegin(GL_TRIANGLES);
            for(int idx = 0; idx < mesh.indices.length(); ++idx)
            {
                unsigned int index = mesh.indices[idx];
                if(index >= (unsigned int)mesh.vertices.length()) continue;
                const vec &normal = mesh.normals[index];
                const vec &vertex = mesh.vertices[index];
                glNormal3f(normal.x, normal.y, normal.z);
                glVertex3f(vertex.x + coord.x * CHUNK_SIZE, vertex.y, vertex.z + coord.z * CHUNK_SIZE);
            }
            glEnd();
        });
    }
}
