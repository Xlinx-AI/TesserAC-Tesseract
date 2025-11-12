#include "vegetation.h"

namespace game
{
    VegetationData VegetationManager::vegetation[VEG_COUNT];
    vector<VegetationType> VegetationManager::biomeVegetation[BIOME_COUNT];

    template<int N>
    static inline void assignVegetation(vector<VegetationType> &list, const VegetationType (&types)[N])
    {
        list.shrink(0);
        loopi(N) list.add(types[i]);
    }

    void VegetationManager::init()
    {
        vegetation[VEG_GRASS] = {VEG_GRASS, "grass", 0, 256, 0.35f, true, false, false};
        vegetation[VEG_TALL_GRASS] = {VEG_TALL_GRASS, "tall_grass", 0, 256, 0.25f, true, false, false};
        vegetation[VEG_FERN] = {VEG_FERN, "fern", 0, 256, 0.18f, true, false, false};
        vegetation[VEG_FLOWER_RED] = {VEG_FLOWER_RED, "flower_red", 0, 256, 0.08f, true, false, false};
        vegetation[VEG_FLOWER_YELLOW] = {VEG_FLOWER_YELLOW, "flower_yellow", 0, 256, 0.08f, true, false, false};
        vegetation[VEG_FLOWER_BLUE] = {VEG_FLOWER_BLUE, "flower_blue", 0, 256, 0.07f, true, false, false};
        vegetation[VEG_FLOWER_WHITE] = {VEG_FLOWER_WHITE, "flower_white", 0, 256, 0.07f, true, false, false};
        vegetation[VEG_MUSHROOM_RED] = {VEG_MUSHROOM_RED, "mushroom_red", -32, 96, 0.05f, true, false, false};
        vegetation[VEG_MUSHROOM_BROWN] = {VEG_MUSHROOM_BROWN, "mushroom_brown", -32, 96, 0.05f, true, false, false};
        vegetation[VEG_CACTUS] = {VEG_CACTUS, "cactus", 0, 256, 0.06f, true, false, false};
        vegetation[VEG_DEAD_BUSH] = {VEG_DEAD_BUSH, "dead_bush", 0, 256, 0.05f, true, false, false};
        vegetation[VEG_BUSH] = {VEG_BUSH, "bush", 0, 256, 0.09f, true, false, false};
        vegetation[VEG_REED] = {VEG_REED, "reed", 0, 256, 0.07f, false, true, false};
        vegetation[VEG_SEAWEED] = {VEG_SEAWEED, "seaweed", -64, 64, 0.1f, false, true, false};
        vegetation[VEG_LILY_PAD] = {VEG_LILY_PAD, "lily_pad", 0, 256, 0.04f, false, true, false};
        vegetation[VEG_VINE] = {VEG_VINE, "vine", 0, 256, 0.06f, false, false, false};
        vegetation[VEG_MOSS] = {VEG_MOSS, "moss", -128, 96, 0.12f, true, false, false};
        vegetation[VEG_LICHEN] = {VEG_LICHEN, "lichen", -128, 96, 0.08f, false, false, false};
        vegetation[VEG_WHEAT_GRASS] = {VEG_WHEAT_GRASS, "wheat_grass", 0, 256, 0.05f, true, false, false};
        vegetation[VEG_BAMBOO] = {VEG_BAMBOO, "bamboo", 0, 256, 0.04f, true, false, false};
        vegetation[VEG_SHRUB] = {VEG_SHRUB, "shrub", 0, 256, 0.1f, true, false, false};
        vegetation[VEG_BERRY_BUSH] = {VEG_BERRY_BUSH, "berry_bush", 0, 256, 0.06f, true, false, false};
        vegetation[VEG_TUNDRA_GRASS] = {VEG_TUNDRA_GRASS, "tundra_grass", 64, 256, 0.07f, true, false, false};
        vegetation[VEG_ICE_FLOWER] = {VEG_ICE_FLOWER, "ice_flower", 96, 256, 0.04f, true, false, false};
        vegetation[VEG_DESERT_FLOWER] = {VEG_DESERT_FLOWER, "desert_flower", 0, 256, 0.03f, true, false, false};
        vegetation[VEG_SAVANNA_BUSH] = {VEG_SAVANNA_BUSH, "savanna_bush", 0, 256, 0.07f, true, false, false};
        vegetation[VEG_JUNGLE_FLOWER] = {VEG_JUNGLE_FLOWER, "jungle_flower", 0, 256, 0.09f, true, false, false};
        vegetation[VEG_MANGROVE_ROOT] = {VEG_MANGROVE_ROOT, "mangrove_root", 0, 256, 0.12f, true, true, false};
        vegetation[VEG_CORAL] = {VEG_CORAL, "coral", -64, 32, 0.08f, false, true, false};
        vegetation[VEG_KELP] = {VEG_KELP, "kelp", -64, 32, 0.1f, false, true, false};
        vegetation[VEG_UNDERGROUND_MUSHROOM_BLUE] = {VEG_UNDERGROUND_MUSHROOM_BLUE, "underground_mushroom_blue", -256, 64, 0.1f, true, false, true};
        vegetation[VEG_UNDERGROUND_MUSHROOM_PURPLE] = {VEG_UNDERGROUND_MUSHROOM_PURPLE, "underground_mushroom_purple", -256, 64, 0.08f, true, false, true};
        vegetation[VEG_UNDERGROUND_VINE] = {VEG_UNDERGROUND_VINE, "underground_vine", -256, 64, 0.12f, false, false, true};
        vegetation[VEG_UNDERGROUND_MOSS] = {VEG_UNDERGROUND_MOSS, "underground_moss", -256, 64, 0.15f, true, false, true};
        vegetation[VEG_CRYSTAL_FLOWER] = {VEG_CRYSTAL_FLOWER, "crystal_flower", -256, 32, 0.05f, true, false, true};
        vegetation[VEG_GLOWING_MUSHROOM] = {VEG_GLOWING_MUSHROOM, "glowing_mushroom", -256, 64, 0.06f, true, false, true};
        vegetation[VEG_UNDERGROUND_FERN] = {VEG_UNDERGROUND_FERN, "underground_fern", -256, 64, 0.08f, true, false, true};
        vegetation[VEG_CAVE_GRASS] = {VEG_CAVE_GRASS, "cave_grass", -256, 64, 0.12f, true, false, false};
        vegetation[VEG_LIMINAL_FLOWER] = {VEG_LIMINAL_FLOWER, "liminal_flower", -256, 32, 0.05f, true, false, true};
        vegetation[VEG_SURREAL_TREE_SMALL] = {VEG_SURREAL_TREE_SMALL, "surreal_tree_small", -256, 32, 0.03f, true, false, true};
        vegetation[VEG_KITHGARD_MOSS] = {VEG_KITHGARD_MOSS, "kithgard_moss", -256, 0, 0.14f, true, false, true};
        vegetation[VEG_KITHGARD_VINE] = {VEG_KITHGARD_VINE, "kithgard_vine", -256, 0, 0.15f, false, false, true};
        vegetation[VEG_ETHEREAL_PLANT] = {VEG_ETHEREAL_PLANT, "ethereal_plant", -256, 0, 0.05f, true, false, true};
        vegetation[VEG_FLOATING_SPORE] = {VEG_FLOATING_SPORE, "floating_spore", -256, 0, 0.04f, false, false, true};
        vegetation[VEG_CLOUD_GRASS] = {VEG_CLOUD_GRASS, "cloud_grass", 160, 256, 0.04f, true, false, false};
        vegetation[VEG_SKY_FLOWER] = {VEG_SKY_FLOWER, "sky_flower", 160, 256, 0.03f, true, false, false};
        vegetation[VEG_LAPUTA_VINE] = {VEG_LAPUTA_VINE, "laputa_vine", 180, 256, 0.05f, false, false, true};

        for(int i = 0; i < BIOME_COUNT; ++i)
        {
            biomeVegetation[i].shrink(0);
        }

        {
            const VegetationType plains[] = {
                VEG_GRASS, VEG_TALL_GRASS, VEG_FLOWER_RED, VEG_FLOWER_YELLOW, VEG_FLOWER_BLUE,
                VEG_FLOWER_WHITE, VEG_FERN, VEG_BERRY_BUSH, VEG_SHRUB, VEG_WHEAT_GRASS,
                VEG_BUSH, VEG_LICHEN, VEG_MOSS
            };
            assignVegetation(biomeVegetation[BIOME_PLAINS], plains);
        }

        {
            const VegetationType hills[] = {
                VEG_GRASS, VEG_TALL_GRASS, VEG_FERN, VEG_FLOWER_RED, VEG_FLOWER_BLUE,
                VEG_FLOWER_WHITE, VEG_BERRY_BUSH, VEG_BUSH, VEG_LICHEN, VEG_MOSS,
                VEG_SHRUB, VEG_WHEAT_GRASS, VEG_TALL_GRASS
            };
            assignVegetation(biomeVegetation[BIOME_HILLS], hills);
        }

        {
            const VegetationType tallGrassFields[] = {
                VEG_GRASS, VEG_TALL_GRASS, VEG_FERN, VEG_FLOWER_RED, VEG_FLOWER_YELLOW,
                VEG_FLOWER_BLUE, VEG_FLOWER_WHITE, VEG_WHEAT_GRASS, VEG_SHRUB, VEG_BERRY_BUSH,
                VEG_BUSH, VEG_MOSS, VEG_LICHEN, VEG_FLOWER_BLUE
            };
            assignVegetation(biomeVegetation[BIOME_TALL_GRASS_FIELDS], tallGrassFields);
        }

        {
            const VegetationType savanna[] = {
                VEG_GRASS, VEG_SAVANNA_BUSH, VEG_TALL_GRASS, VEG_SHRUB, VEG_DEAD_BUSH,
                VEG_FLOWER_YELLOW, VEG_FLOWER_RED, VEG_FLOWER_WHITE, VEG_BUSH,
                VEG_MOSS, VEG_WHEAT_GRASS
            };
            assignVegetation(biomeVegetation[BIOME_SAVANNA], savanna);
        }

        {
            const VegetationType desert[] = {
                VEG_CACTUS, VEG_DEAD_BUSH, VEG_DESERT_FLOWER, VEG_SAVANNA_BUSH, VEG_SHRUB,
                VEG_LICHEN, VEG_BERRY_BUSH
            };
            assignVegetation(biomeVegetation[BIOME_DESERT], desert);
        }

        {
            const VegetationType swamp[] = {
                VEG_GRASS, VEG_MOSS, VEG_LICHEN, VEG_REED, VEG_LILY_PAD, VEG_MUSHROOM_BROWN,
                VEG_MUSHROOM_RED, VEG_SHRUB, VEG_BERRY_BUSH, VEG_BUSH, VEG_VINE,
                VEG_BAMBOO, VEG_TALL_GRASS, VEG_FERN
            };
            assignVegetation(biomeVegetation[BIOME_SWAMP], swamp);
        }

        {
            const VegetationType mangroveForest[] = {
                VEG_MANGROVE_ROOT, VEG_GRASS, VEG_MOSS, VEG_LICHEN, VEG_REED,
                VEG_LILY_PAD, VEG_VINE, VEG_FERN, VEG_SHRUB, VEG_BERRY_BUSH,
                VEG_BUSH, VEG_BAMBOO, VEG_JUNGLE_FLOWER, VEG_TALL_GRASS,
                VEG_FLOWER_YELLOW, VEG_FLOWER_RED
            };
            assignVegetation(biomeVegetation[BIOME_MANGROVE_FOREST], mangroveForest);
        }

        {
            const VegetationType forest[] = {
                VEG_GRASS, VEG_TALL_GRASS, VEG_FERN, VEG_FLOWER_RED, VEG_FLOWER_BLUE,
                VEG_FLOWER_WHITE, VEG_FLOWER_YELLOW, VEG_BERRY_BUSH, VEG_BUSH,
                VEG_SHRUB, VEG_MUSHROOM_RED, VEG_MUSHROOM_BROWN, VEG_LICHEN,
                VEG_MOSS, VEG_WHEAT_GRASS
            };
            assignVegetation(biomeVegetation[BIOME_FOREST], forest);
        }

        {
            const VegetationType birchForest[] = {
                VEG_GRASS, VEG_TALL_GRASS, VEG_FERN, VEG_FLOWER_RED, VEG_FLOWER_BLUE,
                VEG_FLOWER_WHITE, VEG_FLOWER_YELLOW, VEG_BERRY_BUSH, VEG_BUSH,
                VEG_SHRUB, VEG_MUSHROOM_RED, VEG_MUSHROOM_BROWN, VEG_LICHEN,
                VEG_MOSS, VEG_WHEAT_GRASS, VEG_FLOWER_WHITE
            };
            assignVegetation(biomeVegetation[BIOME_BIRCH_FOREST], birchForest);
        }

        {
            const VegetationType longTreeForest[] = {
                VEG_GRASS, VEG_TALL_GRASS, VEG_FERN, VEG_FLOWER_RED, VEG_FLOWER_BLUE,
                VEG_FLOWER_WHITE, VEG_BERRY_BUSH, VEG_BUSH, VEG_SHRUB, VEG_MOSS,
                VEG_LICHEN, VEG_MUSHROOM_RED, VEG_MUSHROOM_BROWN, VEG_BAMBOO,
                VEG_VINE, VEG_JUNGLE_FLOWER, VEG_WHEAT_GRASS
            };
            assignVegetation(biomeVegetation[BIOME_LONG_TREE_FOREST], longTreeForest);
        }

        {
            const VegetationType darkForest[] = {
                VEG_GRASS, VEG_MOSS, VEG_LICHEN, VEG_MUSHROOM_RED, VEG_MUSHROOM_BROWN,
                VEG_UNDERGROUND_MOSS, VEG_UNDERGROUND_MUSHROOM_BLUE, VEG_UNDERGROUND_MUSHROOM_PURPLE,
                VEG_UNDERGROUND_VINE, VEG_UNDERGROUND_FERN, VEG_GLOWING_MUSHROOM,
                VEG_BERRY_BUSH, VEG_BUSH, VEG_SHRUB
            };
            assignVegetation(biomeVegetation[BIOME_DARK_FOREST], darkForest);
        }

        {
            const VegetationType taiga[] = {
                VEG_GRASS, VEG_TALL_GRASS, VEG_FERN, VEG_TUNDRA_GRASS, VEG_MOSS,
                VEG_LICHEN, VEG_BERRY_BUSH, VEG_SHRUB, VEG_FLOWER_WHITE, VEG_FLOWER_BLUE,
                VEG_BUSH, VEG_WHEAT_GRASS
            };
            assignVegetation(biomeVegetation[BIOME_TAIGA], taiga);
        }

        {
            const VegetationType tundra[] = {
                VEG_TUNDRA_GRASS, VEG_ICE_FLOWER, VEG_MOSS, VEG_LICHEN, VEG_SHRUB,
                VEG_BERRY_BUSH, VEG_FLOWER_WHITE, VEG_FLOWER_BLUE, VEG_FLOWER_RED
            };
            assignVegetation(biomeVegetation[BIOME_TUNDRA], tundra);
        }

        {
            const VegetationType mesa[] = {
                VEG_CACTUS, VEG_DEAD_BUSH, VEG_DESERT_FLOWER, VEG_SAVANNA_BUSH,
                VEG_SHRUB, VEG_LICHEN, VEG_BERRY_BUSH
            };
            assignVegetation(biomeVegetation[BIOME_MESA], mesa);
        }

        {
            const VegetationType ocean[] = {
                VEG_SEAWEED, VEG_KELP, VEG_CORAL, VEG_REED, VEG_LILY_PAD
            };
            assignVegetation(biomeVegetation[BIOME_OCEAN], ocean);
        }

        {
            const VegetationType deepOcean[] = {
                VEG_SEAWEED, VEG_KELP, VEG_CORAL
            };
            assignVegetation(biomeVegetation[BIOME_DEEP_OCEAN], deepOcean);
        }

        {
            const VegetationType reef[] = {
                VEG_CORAL, VEG_SEAWEED, VEG_KELP, VEG_REED, VEG_LILY_PAD,
                VEG_MANGROVE_ROOT
            };
            assignVegetation(biomeVegetation[BIOME_REEF], reef);
        }

        {
            const VegetationType lake[] = {
                VEG_GRASS, VEG_REED, VEG_LILY_PAD, VEG_SEAWEED, VEG_MOSS,
                VEG_LICHEN, VEG_FERN, VEG_FLOWER_BLUE, VEG_FLOWER_WHITE,
                VEG_SHRUB, VEG_BERRY_BUSH, VEG_BUSH
            };
            assignVegetation(biomeVegetation[BIOME_LAKE], lake);
        }

        {
            const VegetationType river[] = {
                VEG_REED, VEG_LILY_PAD, VEG_GRASS, VEG_FERN, VEG_MOSS,
                VEG_LICHEN, VEG_SHRUB, VEG_BERRY_BUSH, VEG_BUSH, VEG_FLOWER_RED,
                VEG_FLOWER_YELLOW
            };
            assignVegetation(biomeVegetation[BIOME_RIVER], river);
        }

        {
            const VegetationType smallMountains[] = {
                VEG_GRASS, VEG_TALL_GRASS, VEG_FERN, VEG_FLOWER_RED,
                VEG_FLOWER_BLUE, VEG_FLOWER_WHITE, VEG_BERRY_BUSH, VEG_BUSH,
                VEG_SHRUB, VEG_MOSS, VEG_LICHEN
            };
            assignVegetation(biomeVegetation[BIOME_SMALL_MOUNTAINS], smallMountains);
        }

        {
            const VegetationType mediumMountains[] = {
                VEG_GRASS, VEG_TALL_GRASS, VEG_FERN, VEG_FLOWER_WHITE,
                VEG_FLOWER_BLUE, VEG_TUNDRA_GRASS, VEG_MOSS, VEG_LICHEN,
                VEG_BERRY_BUSH, VEG_SHRUB
            };
            assignVegetation(biomeVegetation[BIOME_MEDIUM_MOUNTAINS], mediumMountains);
        }

        {
            const VegetationType tallMountains[] = {
                VEG_TUNDRA_GRASS, VEG_ICE_FLOWER, VEG_MOSS, VEG_LICHEN,
                VEG_SHRUB, VEG_BERRY_BUSH, VEG_FLOWER_WHITE
            };
            assignVegetation(biomeVegetation[BIOME_TALL_MOUNTAINS], tallMountains);
        }

        {
            const VegetationType highMountainForest[] = {
                VEG_GRASS, VEG_TALL_GRASS, VEG_FERN, VEG_FLOWER_WHITE,
                VEG_FLOWER_BLUE, VEG_TUNDRA_GRASS, VEG_MOSS, VEG_LICHEN,
                VEG_BERRY_BUSH, VEG_SHRUB
            };
            assignVegetation(biomeVegetation[BIOME_HIGH_MOUNTAIN_FOREST], highMountainForest);
        }
    }

    const VegetationData& VegetationManager::getVegetation(VegetationType type)
    {
        return vegetation[type];
    }

    const vector<VegetationType>& VegetationManager::getBiomeVegetation(BiomeType biome)
    {
        return biomeVegetation[biome];
    }

    const char* VegetationManager::getVegetationName(VegetationType type)
    {
        if(type < 0 || type >= VEG_COUNT) return "unknown";
        return vegetation[type].name;
    }
}
