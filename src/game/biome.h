#ifndef __BIOME_H__
#define __BIOME_H__

namespace game
{
    enum BiomeType
    {
        BIOME_OCEAN = 0,
        BIOME_DEEP_OCEAN,
        BIOME_REEF,
        BIOME_LAKE,
        BIOME_RIVER,
        BIOME_PLAINS,
        BIOME_HILLS,
        BIOME_TALL_GRASS_FIELDS,
        BIOME_SAVANNA,
        BIOME_DESERT,
        BIOME_SWAMP,
        BIOME_MANGROVE_FOREST,
        BIOME_FOREST,
        BIOME_BIRCH_FOREST,
        BIOME_LONG_TREE_FOREST,
        BIOME_DARK_FOREST,
        BIOME_TAIGA,
        BIOME_TUNDRA,
        BIOME_SMALL_MOUNTAINS,
        BIOME_MEDIUM_MOUNTAINS,
        BIOME_TALL_MOUNTAINS,
        BIOME_HIGH_MOUNTAIN_FOREST,
        BIOME_MESA,
        BIOME_COUNT
    };

    struct BiomeData
    {
        BiomeType type;
        const char *name;
        float baseHeight;
        float heightVariation;
        float temperature;
        float humidity;
        int grassDensity;
        int treeDensity;
        float erosionResistance;
    };

    class BiomeManager
    {
    private:
        static BiomeData biomes[BIOME_COUNT];
        
    public:
        static void init();
        static const BiomeData& getBiome(BiomeType type);
        static BiomeType selectBiome(double continentalness, double erosion, double peaks_valleys, 
                                     double temperature, double humidity, double weirdness);
        static const char* getBiomeName(BiomeType type);
    };
}

#endif
