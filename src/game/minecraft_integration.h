#ifndef __MINECRAFT_INTEGRATION_H__
#define __MINECRAFT_INTEGRATION_H__

#include "voxelworld.h"

namespace game
{
    extern VoxelWorld *voxelWorld;

    void initMinecraftWorld(unsigned int seed = 12345);
    void shutdownMinecraftWorld();
    void updateMinecraftWorld(const vec &playerPos);
    void renderMinecraftWorld();

    const char* getBlockName(BlockType type);
    void placeBlock(int worldX, int worldY, int worldZ, BlockType type);
    void breakBlock(int worldX, int worldY, int worldZ);
    BlockType getBlockAt(int worldX, int worldY, int worldZ);
    BiomeType getBiomeAt(int worldX, int worldZ);
    const char* getBiomeNameAt(int worldX, int worldZ);

    void cmdMinecraftInit(int *seed);
    void cmdMinecraftPlace(int *x, int *y, int *z, int *blockType);
    void cmdMinecraftBreak(int *x, int *y, int *z);
    void cmdMinecraftInfo(int *x, int *y, int *z);
}

#endif
