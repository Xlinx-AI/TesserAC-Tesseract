#include "game.h"
#include "biome.h"
#include "noise.h"

namespace game
{
    BiomeData BiomeManager::biomes[BIOME_COUNT];

    static float clampf(float value, float minVal, float maxVal)
    {
        if(value < minVal) return minVal;
        if(value > maxVal) return maxVal;
        return value;
    }

    void BiomeManager::init()
    {
        biomes[BIOME_OCEAN] = { BIOME_OCEAN, "ocean", -20.0f, 4.0f, 0.8f, 0.9f, 0, 0, 0.2f };
        biomes[BIOME_DEEP_OCEAN] = { BIOME_DEEP_OCEAN, "deep_ocean", -35.0f, 6.0f, 0.6f, 0.9f, 0, 0, 0.4f };
        biomes[BIOME_REEF] = { BIOME_REEF, "reef", -15.0f, 3.0f, 0.9f, 1.0f, 2, 1, 0.1f };
        biomes[BIOME_LAKE] = { BIOME_LAKE, "lake", -5.0f, 2.0f, 0.7f, 0.8f, 1, 0, 0.2f };
        biomes[BIOME_RIVER] = { BIOME_RIVER, "river", -2.0f, 1.0f, 0.6f, 0.8f, 1, 0, 0.15f };
        biomes[BIOME_PLAINS] = { BIOME_PLAINS, "plains", 5.0f, 8.0f, 0.7f, 0.6f, 5, 2, 0.5f };
        biomes[BIOME_HILLS] = { BIOME_HILLS, "hills", 15.0f, 14.0f, 0.6f, 0.5f, 4, 3, 0.6f };
        biomes[BIOME_TALL_GRASS_FIELDS] = { BIOME_TALL_GRASS_FIELDS, "tall_grass_fields", 5.0f, 6.0f, 0.8f, 0.7f, 7, 3, 0.4f };
        biomes[BIOME_SAVANNA] = { BIOME_SAVANNA, "savanna", 3.0f, 4.0f, 1.0f, 0.2f, 2, 2, 0.3f };
        biomes[BIOME_DESERT] = { BIOME_DESERT, "desert", 4.0f, 5.0f, 1.2f, 0.05f, 0, 0, 0.1f };
        biomes[BIOME_SWAMP] = { BIOME_SWAMP, "swamp", -1.0f, 2.0f, 0.7f, 1.0f, 6, 1, 0.35f };
        biomes[BIOME_MANGROVE_FOREST] = { BIOME_MANGROVE_FOREST, "mangrove_forest", -1.0f, 1.5f, 0.8f, 1.0f, 7, 5, 0.4f };
        biomes[BIOME_FOREST] = { BIOME_FOREST, "forest", 7.0f, 9.0f, 0.6f, 0.7f, 6, 6, 0.55f };
        biomes[BIOME_BIRCH_FOREST] = { BIOME_BIRCH_FOREST, "birch_forest", 7.0f, 8.0f, 0.55f, 0.65f, 6, 6, 0.5f };
        biomes[BIOME_LONG_TREE_FOREST] = { BIOME_LONG_TREE_FOREST, "long_tree_forest", 10.0f, 12.0f, 0.65f, 0.8f, 5, 8, 0.55f };
        biomes[BIOME_DARK_FOREST] = { BIOME_DARK_FOREST, "dark_forest", 6.0f, 7.5f, 0.55f, 0.9f, 7, 7, 0.6f };
        biomes[BIOME_TAIGA] = { BIOME_TAIGA, "taiga", 8.0f, 9.0f, 0.4f, 0.5f, 4, 6, 0.65f };
        biomes[BIOME_TUNDRA] = { BIOME_TUNDRA, "tundra", 5.0f, 6.0f, 0.2f, 0.4f, 1, 1, 0.7f };
        biomes[BIOME_SMALL_MOUNTAINS] = { BIOME_SMALL_MOUNTAINS, "small_mountains", 30.0f, 20.0f, 0.4f, 0.5f, 2, 1, 0.75f };
        biomes[BIOME_MEDIUM_MOUNTAINS] = { BIOME_MEDIUM_MOUNTAINS, "medium_mountains", 55.0f, 35.0f, 0.3f, 0.4f, 1, 1, 0.85f };
        biomes[BIOME_TALL_MOUNTAINS] = { BIOME_TALL_MOUNTAINS, "tall_mountains", 90.0f, 50.0f, 0.2f, 0.3f, 0, 0, 0.95f };
        biomes[BIOME_HIGH_MOUNTAIN_FOREST] = { BIOME_HIGH_MOUNTAIN_FOREST, "high_mountain_forest", 75.0f, 35.0f, 0.35f, 0.5f, 1, 3, 0.9f };
        biomes[BIOME_MESA] = { BIOME_MESA, "mesa", 35.0f, 15.0f, 0.8f, 0.2f, 0, 1, 0.6f };
    }

    const BiomeData& BiomeManager::getBiome(BiomeType type)
    {
        return biomes[type];
    }

    BiomeType BiomeManager::selectBiome(double continentalness, double erosion, double peaks_valleys, double temperature, double humidity, double weirdness)
    {
        double cont = clampf((float)continentalness * 0.5f + 0.5f, 0.0f, 1.0f);
        double eros = clampf((float)erosion * 0.5f + 0.5f, 0.0f, 1.0f);
        double peaks = clampf((float)peaks_valleys * 0.5f + 0.5f, 0.0f, 1.0f);
        double temp = clampf((float)temperature * 0.5f + 0.5f, 0.0f, 1.0f);
        double humid = clampf((float)humidity * 0.5f + 0.5f, 0.0f, 1.0f);
        double weird = clampf((float)weirdness * 0.5f + 0.5f, 0.0f, 1.0f);

        if(cont < 0.2)
        {
            if(weird > 0.7) return BIOME_REEF;
            return cont < 0.1 ? BIOME_DEEP_OCEAN : BIOME_OCEAN;
        }

        if(cont < 0.3 && eros < 0.3) return BIOME_LAKE;
        if(eros < 0.15) return BIOME_RIVER;

        if(temp > 0.8 && humid < 0.3) return BIOME_DESERT;
        if(temp > 0.85 && humid < 0.5) return BIOME_SAVANNA;

        if(peaks > 0.8)
        {
            if(cont > 0.8) return BIOME_TALL_MOUNTAINS;
            if(cont > 0.6) return BIOME_MEDIUM_MOUNTAINS;
            return BIOME_SMALL_MOUNTAINS;
        }

        if(cont > 0.7 && peaks > 0.6)
        {
            if(humid > 0.4) return BIOME_HIGH_MOUNTAIN_FOREST;
            return BIOME_SMALL_MOUNTAINS;
        }

        if(temp < 0.3)
        {
            if(humid < 0.4) return BIOME_TUNDRA;
            return BIOME_TAIGA;
        }

        if(humid > 0.75)
        {
            if(temp < 0.5) return BIOME_DARK_FOREST;
            if(temp < 0.7) return BIOME_LONG_TREE_FOREST;
            return BIOME_SWAMP;
        }

        if(humid > 0.6)
        {
            if(temp < 0.55) return BIOME_BIRCH_FOREST;
            return BIOME_FOREST;
        }

        if(humid > 0.45)
        {
            if(temp < 0.6) return BIOME_HILLS;
            return BIOME_TALL_GRASS_FIELDS;
        }

        if(weird > 0.8) return BIOME_MESA;

        return BIOME_PLAINS;
    }

    const char* BiomeManager::getBiomeName(BiomeType type)
    {
        if(type < 0 || type >= BIOME_COUNT) return "unknown";
        return biomes[type].name;
    }
}
