#include "worldlayer.h"
#include "noise.h"
#include <cmath>
#include <cstdlib>

namespace game
{
    static double clampd(double value, double minVal, double maxVal)
    {
        if(value < minVal) return minVal;
        if(value > maxVal) return maxVal;
        return value;
    }

    static double mind(double a, double b)
    {
        return a < b ? a : b;
    }

    WorldLayerData WorldLayerManager::layers[LAYER_COUNT];
    KithgardBiomeData WorldLayerManager::kithgardBiomes[KITHGARD_BIOME_COUNT];

    void WorldLayerManager::init()
    {
        layers[LAYER_KITHGARD] = {LAYER_KITHGARD, "kithgard", WORLD_KITHGARD_MIN, WORLD_KITHGARD_MAX, 0.0f};
        layers[LAYER_CAVES] = {LAYER_CAVES, "caves", WORLD_CAVES_MIN, WORLD_CAVES_MAX, 0.0f};
        layers[LAYER_SURFACE] = {LAYER_SURFACE, "surface", WORLD_SURFACE_MIN, WORLD_SURFACE_MAX, 0.02f};
        layers[LAYER_SKIES] = {LAYER_SKIES, "skies", WORLD_SKIES_MIN, WORLD_SKIES_MIN + 512, 0.15f};

        kithgardBiomes[KITHGARD_CATACOMBS] = {KITHGARD_CATACOMBS, "catacombs", 0.2f, 0.8f, false, false, true, 0.2f};
        kithgardBiomes[KITHGARD_HALLS] = {KITHGARD_HALLS, "halls", 0.3f, 0.6f, false, true, true, 0.4f};
        kithgardBiomes[KITHGARD_FOREST] = {KITHGARD_FOREST, "forest", 0.6f, 0.4f, true, true, true, 0.5f};
        kithgardBiomes[KITHGARD_SWAMP] = {KITHGARD_SWAMP, "swamp", 0.9f, 0.7f, true, true, true, 0.45f};
        kithgardBiomes[KITHGARD_LAKE] = {KITHGARD_LAKE, "lake", 0.7f, 0.5f, true, false, false, 0.35f};
        kithgardBiomes[KITHGARD_OCEAN] = {KITHGARD_OCEAN, "ocean", 0.8f, 0.6f, true, true, false, 0.6f};
        kithgardBiomes[KITHGARD_CRYSTAL_CAVES] = {KITHGARD_CRYSTAL_CAVES, "crystal_caves", 0.4f, 0.9f, false, false, true, 0.9f};
        kithgardBiomes[KITHGARD_MUSHROOM_FOREST] = {KITHGARD_MUSHROOM_FOREST, "mushroom_forest", 0.65f, 0.5f, true, true, true, 0.7f};
        kithgardBiomes[KITHGARD_RUINS] = {KITHGARD_RUINS, "ruins", 0.5f, 0.8f, false, false, true, 0.3f};
        kithgardBiomes[KITHGARD_VOID] = {KITHGARD_VOID, "void", 0.1f, 1.0f, false, false, false, 0.95f};
    }

    WorldLayer WorldLayerManager::getWorldLayer(int worldY)
    {
        if(worldY >= WORLD_SKIES_MIN) return LAYER_SKIES;
        if(worldY >= WORLD_SURFACE_MIN) return LAYER_SURFACE;
        if(worldY >= WORLD_CAVES_MIN) return LAYER_CAVES;
        return LAYER_KITHGARD;
    }

    const WorldLayerData& WorldLayerManager::getLayerData(WorldLayer layer)
    {
        return layers[layer];
    }

    const KithgardBiomeData& WorldLayerManager::getKithgardBiome(KithgardBiomeType type)
    {
        return kithgardBiomes[type];
    }

    KithgardBiomeType WorldLayerManager::selectKithgardBiome(double humidity, double weirdness, double depth)
    {
        double h = clampd(humidity * 0.5 + 0.5, 0.0, 1.0);
        double w = clampd(weirdness * 0.5 + 0.5, 0.0, 1.0);
        double range = (double)(WORLD_KITHGARD_MAX - WORLD_KITHGARD_MIN);
        if(range < 1.0) range = 1.0;
        double d = clampd((depth - WORLD_KITHGARD_MIN) / range, 0.0, 1.0);

        if(w > 0.85) return KITHGARD_VOID;
        if(w > 0.7 && h < 0.4) return KITHGARD_CRYSTAL_CAVES;
        if(h > 0.8 && w > 0.6) return KITHGARD_OCEAN;
        if(h > 0.7 && w < 0.5) return KITHGARD_SWAMP;
        if(h > 0.7) return KITHGARD_LAKE;
        if(d > 0.6 && h > 0.6) return KITHGARD_FOREST;
        if(d > 0.4 && w > 0.5) return KITHGARD_RUINS;
        if(d < 0.3 && w < 0.4) return KITHGARD_CATACOMBS;
        if(w < 0.5) return KITHGARD_HALLS;
        return KITHGARD_MUSHROOM_FOREST;
    }

    const char* WorldLayerManager::getLayerName(WorldLayer layer)
    {
        switch(layer)
        {
            case LAYER_KITHGARD: return "kithgard";
            case LAYER_CAVES: return "caves";
            case LAYER_SURFACE: return "surface";
            case LAYER_SKIES: return "skies";
            default: return "unknown";
        }
    }

    const char* WorldLayerManager::getKithgardBiomeName(KithgardBiomeType type)
    {
        if(type < 0 || type >= KITHGARD_BIOME_COUNT) return "unknown";
        return kithgardBiomes[type].name;
    }

    bool WorldLayerManager::isFloatingIsland(int worldX, int worldY, int worldZ, unsigned int seed)
    {
        if(worldY < WORLD_SKIES_MIN) return false;
        unsigned int hash = seed;
        hash ^= (unsigned int)(worldX * 374761393 + worldZ * 668265263);
        hash = (hash ^ (hash >> 13)) * 1274126177u;
        double chance = (double)(hash % 10000) / 10000.0;
        double baseChance = layers[LAYER_SKIES].floatingIslandChance;
        double altitudeFactor = mind(1.0, (double)(worldY - WORLD_SKIES_MIN) / 512.0);
        double finalChance = baseChance * (0.5 + altitudeFactor);
        return chance < finalChance;
    }
}
