#ifndef __WORLDLAYER_H__
#define __WORLDLAYER_H__

namespace game
{
    enum WorldLayer
    {
        LAYER_KITHGARD = 0,
        LAYER_CAVES,
        LAYER_SURFACE,
        LAYER_SKIES,
        LAYER_COUNT
    };

    enum KithgardBiomeType
    {
        KITHGARD_CATACOMBS = 0,
        KITHGARD_HALLS,
        KITHGARD_FOREST,
        KITHGARD_SWAMP,
        KITHGARD_LAKE,
        KITHGARD_OCEAN,
        KITHGARD_CRYSTAL_CAVES,
        KITHGARD_MUSHROOM_FOREST,
        KITHGARD_RUINS,
        KITHGARD_VOID,
        KITHGARD_BIOME_COUNT
    };

    struct WorldLayerData
    {
        WorldLayer layer;
        const char *name;
        int minY;
        int maxY;
        float floatingIslandChance;
    };

    struct KithgardBiomeData
    {
        KithgardBiomeType type;
        const char *name;
        float humidity;
        float weirdness;
        bool hasWater;
        bool hasClouds;
        bool hasStructures;
        float glowIntensity;
    };

    class WorldLayerManager
    {
    private:
        static WorldLayerData layers[LAYER_COUNT];
        static KithgardBiomeData kithgardBiomes[KITHGARD_BIOME_COUNT];

    public:
        static void init();
        static WorldLayer getWorldLayer(int worldY);
        static const WorldLayerData& getLayerData(WorldLayer layer);
        static const KithgardBiomeData& getKithgardBiome(KithgardBiomeType type);
        static KithgardBiomeType selectKithgardBiome(double humidity, double weirdness, double depth);
        static const char* getLayerName(WorldLayer layer);
        static const char* getKithgardBiomeName(KithgardBiomeType type);
        static bool isFloatingIsland(int worldX, int worldY, int worldZ, unsigned int seed);
    };

    static const int WORLD_SEA_LEVEL = 1200;
    static const int WORLD_KITHGARD_MIN = 0;
    static const int WORLD_KITHGARD_MAX = 575;
    static const int WORLD_CAVES_MIN = 576;
    static const int WORLD_CAVES_MAX = 1023;
    static const int WORLD_SURFACE_MIN = 1024;
    static const int WORLD_SURFACE_MAX = 1535;
    static const int WORLD_SKIES_MIN = 1536;
}

#endif
