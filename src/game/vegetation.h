#ifndef __VEGETATION_H__
#define __VEGETATION_H__

#include "biome.h"
#include "cube.h"

namespace game
{
    enum VegetationType
    {
        VEG_GRASS = 0,
        VEG_TALL_GRASS,
        VEG_FERN,
        VEG_FLOWER_RED,
        VEG_FLOWER_YELLOW,
        VEG_FLOWER_BLUE,
        VEG_FLOWER_WHITE,
        VEG_MUSHROOM_RED,
        VEG_MUSHROOM_BROWN,
        VEG_CACTUS,
        VEG_DEAD_BUSH,
        VEG_BUSH,
        VEG_REED,
        VEG_SEAWEED,
        VEG_LILY_PAD,
        VEG_VINE,
        VEG_MOSS,
        VEG_LICHEN,
        VEG_WHEAT_GRASS,
        VEG_BAMBOO,
        VEG_SHRUB,
        VEG_BERRY_BUSH,
        VEG_TUNDRA_GRASS,
        VEG_ICE_FLOWER,
        VEG_DESERT_FLOWER,
        VEG_SAVANNA_BUSH,
        VEG_JUNGLE_FLOWER,
        VEG_MANGROVE_ROOT,
        VEG_CORAL,
        VEG_KELP,
        VEG_UNDERGROUND_MUSHROOM_BLUE,
        VEG_UNDERGROUND_MUSHROOM_PURPLE,
        VEG_UNDERGROUND_VINE,
        VEG_UNDERGROUND_MOSS,
        VEG_CRYSTAL_FLOWER,
        VEG_GLOWING_MUSHROOM,
        VEG_UNDERGROUND_FERN,
        VEG_CAVE_GRASS,
        VEG_LIMINAL_FLOWER,
        VEG_SURREAL_TREE_SMALL,
        VEG_KITHGARD_MOSS,
        VEG_KITHGARD_VINE,
        VEG_ETHEREAL_PLANT,
        VEG_FLOATING_SPORE,
        VEG_CLOUD_GRASS,
        VEG_SKY_FLOWER,
        VEG_LAPUTA_VINE,
        VEG_COUNT
    };

    struct VegetationData
    {
        VegetationType type;
        const char *name;
        int minHeight;
        int maxHeight;
        float spawnChance;
        bool needsSolidGround;
        bool needsWater;
        bool glows;
    };

    class VegetationManager
    {
    private:
        static VegetationData vegetation[VEG_COUNT];
        static vector<VegetationType> biomeVegetation[BIOME_COUNT];
        
    public:
        static void init();
        static const VegetationData& getVegetation(VegetationType type);
        static const vector<VegetationType>& getBiomeVegetation(BiomeType biome);
        static const char* getVegetationName(VegetationType type);
    };
}

#endif
