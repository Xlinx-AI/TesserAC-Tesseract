#include "game.h"
#include "minecraft_integration.h"
#include "voxelworld.h"
#include "chunk.h"
#include "biome.h"
#include "worldgen.h"
#include <cstdio>

namespace game
{
    VoxelWorld *voxelWorld = NULL;

    static const char *blockNames[] = {
        "air",
        "stone",
        "dirt",
        "grass",
        "sand",
        "water",
        "wood",
        "leaves",
        "snow",
        "ice",
        "gravel",
        "coal_ore",
        "iron_ore",
        "gold_ore",
        "diamond_ore",
        "clay",
        "bedrock"
    };

    void initMinecraftWorld(unsigned int seed)
    {
        if(voxelWorld) return;
        voxelWorld = new VoxelWorld(seed, 6);
    }

    void shutdownMinecraftWorld()
    {
        if(!voxelWorld) return;
        delete voxelWorld;
        voxelWorld = NULL;
    }

    void updateMinecraftWorld(const vec &playerPos)
    {
        if(!voxelWorld) initMinecraftWorld();
        voxelWorld->update(playerPos);
    }

    void renderMinecraftWorld()
    {
        if(!voxelWorld) return;
        voxelWorld->render();
    }

    const char* getBlockName(BlockType type)
    {
        if(type < 0 || type >= BLOCK_COUNT) return "unknown";
        return blockNames[type];
    }

    void placeBlock(int worldX, int worldY, int worldZ, BlockType type)
    {
        if(!voxelWorld) initMinecraftWorld();
        voxelWorld->setBlock(worldX, worldY, worldZ, type);
    }

    void breakBlock(int worldX, int worldY, int worldZ)
    {
        if(!voxelWorld) return;
        voxelWorld->setBlock(worldX, worldY, worldZ, BLOCK_AIR);
    }

    BlockType getBlockAt(int worldX, int worldY, int worldZ)
    {
        if(!voxelWorld) initMinecraftWorld();
        return voxelWorld->getBlock(worldX, worldY, worldZ).type;
    }

    BiomeType getBiomeAt(int worldX, int worldZ)
    {
        if(!voxelWorld) initMinecraftWorld();
        return voxelWorld->getBiome(worldX, worldZ);
    }

    const char* getBiomeNameAt(int worldX, int worldZ)
    {
        return BiomeManager::getBiomeName(getBiomeAt(worldX, worldZ));
    }

    void cmdMinecraftInit(int *seed)
    {
        if(voxelWorld) shutdownMinecraftWorld();
        initMinecraftWorld(seed && *seed ? (unsigned int)(*seed) : 12345);
        conoutf("Initialized Minecraft-like world with seed %u", voxelWorld->getWorldGenerator()->getSeed());
    }

    void cmdMinecraftPlace(int *x, int *y, int *z, int *blockType)
    {
        if(!voxelWorld) initMinecraftWorld();
        if(!blockType) return;
        int bx = blockType ? *blockType : 0;
        bx = clamp(bx, 0, BLOCK_COUNT - 1);
        placeBlock(x ? *x : 0, y ? *y : 0, z ? *z : 0, (BlockType)bx);
    }

    void cmdMinecraftBreak(int *x, int *y, int *z)
    {
        if(!voxelWorld) initMinecraftWorld();
        breakBlock(x ? *x : 0, y ? *y : 0, z ? *z : 0);
    }

    void cmdMinecraftInfo(int *x, int *y, int *z)
    {
        if(!voxelWorld) initMinecraftWorld();
        BlockType block = getBlockAt(x ? *x : 0, y ? *y : 0, z ? *z : 0);
        BiomeType biome = getBiomeAt(x ? *x : 0, z ? *z : 0);
        conoutf("Position [%d, %d, %d]: block=%s biome=%s", x ? *x : 0, y ? *y : 0, z ? *z : 0, getBlockName(block), BiomeManager::getBiomeName(biome));
    }

    ICOMMAND(initminecraft, "i", (int *seed), cmdMinecraftInit(seed));
    ICOMMAND(placeblock, "iiii", (int *x, int *y, int *z, int *type), cmdMinecraftPlace(x, y, z, type));
    ICOMMAND(breakblock, "iii", (int *x, int *y, int *z), cmdMinecraftBreak(x, y, z));
    ICOMMAND(blockinfo, "iii", (int *x, int *y, int *z), cmdMinecraftInfo(x, y, z));
}
