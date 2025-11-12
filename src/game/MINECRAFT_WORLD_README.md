# Minecraft-like World Generation System

This is a comprehensive Minecraft-like voxel world generation system for the Tesseract engine.

## Features

### 1. **Infinite Chunk-Based World**
- World is divided into 16x16x256 chunks
- Chunks are loaded/unloaded dynamically based on player position
- Efficient memory management with automatic chunk cleanup

### 2. **Marching Cubes Algorithm**
- Implemented for smooth terrain generation
- Located in `marchingcubes.h` and `marchingcubes.cpp`
- Currently mesh generation uses optimized voxel rendering, but can be integrated with marching cubes for smoother terrain

### 3. **Advanced Cave Generation**
- **Spaghetti Caves**: Long, winding tunnels using noise intersection
- **Cheese Caves**: Large caverns using 3D noise
- **Cave Noise**: Additional cave variations
- Caves respect terrain height and avoid spawning near surface or bedrock

### 4. **Biome System**
The system includes 24 diverse biomes:

#### Ocean Biomes
- **Ocean**: Standard ocean, -20m depth
- **Deep Ocean**: Deeper ocean, -35m depth
- **Reef**: Shallow coral reef areas, -15m depth
- **Lake**: Inland lakes, -5m depth
- **River**: Flowing rivers, -2m depth

#### Plains & Grasslands
- **Plains**: Flat grassy areas with scattered trees
- **Hills**: Rolling hills terrain
- **Tall Grass Fields**: Dense grass with moderate trees

#### Forest Biomes
- **Forest**: Standard temperate forest
- **Birch Forest**: Birch tree dominant forest
- **Long Tree Forest**: Tall coniferous trees
- **Dark Forest**: Dense, dark forest with heavy canopy
- **High Mountain Forest**: Alpine forest on mountains

#### Hot Biomes
- **Savanna**: Hot grassland with sparse trees
- **Desert**: Sandy desert with minimal vegetation
- **Mesa**: Layered sand/stone formations

#### Wetland Biomes
- **Swamp**: Wet lowland areas
- **Mangrove Forest**: Coastal wetland forests

#### Cold Biomes
- **Taiga**: Boreal forest with snow
- **Tundra**: Frozen plains
- **Mountains**: Various heights (Small, Medium, Tall)

### 5. **Block Types**
The system supports 17 block types:
- Air
- Stone
- Dirt
- Grass
- Sand
- Water
- Wood
- Leaves
- Snow
- Ice
- Gravel
- Coal Ore
- Iron Ore
- Gold Ore
- Diamond Ore
- Clay
- Bedrock

### 6. **Noise Generation**
Multiple noise layers for realistic terrain:
- **Continentalness**: Controls ocean vs land distribution
- **Erosion**: Affects terrain smoothness/roughness
- **Peaks & Valleys**: Controls mountain/valley formation
- **Temperature**: Affects biome selection
- **Humidity**: Affects vegetation and biome type
- **Weirdness**: Adds unusual terrain features

## File Structure

```
src/game/
├── noise.h / noise.cpp              # Perlin & Simplex noise implementation
├── biome.h / biome.cpp              # Biome system and selection
├── chunk.h / chunk.cpp              # Chunk structure and management
├── marchingcubes.h / marchingcubes.cpp  # Marching Cubes algorithm
├── worldgen.h / worldgen.cpp        # World generation logic
├── voxelworld.h / voxelworld.cpp    # Main world manager
└── minecraft_integration.h / minecraft_integration.cpp  # Integration with game
```

## Usage

### Initialization

```cpp
#include "minecraft_integration.h"

// Initialize with seed
initMinecraftWorld(12345);

// Or use default seed
initMinecraftWorld();
```

### Update Loop

```cpp
// In your game update loop
void update()
{
    vec playerPos = player->o; // Get player position
    updateMinecraftWorld(playerPos);
}
```

### Rendering

```cpp
// In your render loop
void render()
{
    renderMinecraftWorld();
}
```

### Console Commands

The system provides several console commands:

```
initminecraft [seed]           # Initialize world with optional seed
placeblock x y z type          # Place a block at position
breakblock x y z               # Break a block at position
blockinfo x y z                # Get info about block and biome
```

### Block Types for Commands

- 0 = Air
- 1 = Stone
- 2 = Dirt
- 3 = Grass
- 4 = Sand
- 5 = Water
- 6 = Wood
- 7 = Leaves
- 8 = Snow
- 9 = Ice
- 10 = Gravel
- 11 = Coal Ore
- 12 = Iron Ore
- 13 = Gold Ore
- 14 = Diamond Ore
- 15 = Clay
- 16 = Bedrock

## Technical Details

### Chunk Generation Algorithm

1. **Biome Selection**: For each x,z coordinate, calculate noise values and select appropriate biome
2. **Height Map**: Generate base terrain height using continentalness, erosion, and peaks/valleys
3. **Density Field**: Create 3D density field for smooth terrain
4. **Cave Carving**: Remove blocks where cave noise indicates caves should exist
5. **Block Selection**: Choose appropriate block type based on height, biome, and position
6. **Ore Generation**: Randomly place ore veins in stone

### Performance Optimizations

- Chunks are only generated when needed
- Distant chunks are automatically unloaded
- Mesh generation is deferred until render time
- Face culling removes hidden block faces
- Efficient memory management with chunk pooling

### Biome Selection Logic

Biomes are selected based on multiple noise parameters:
1. **Continentalness** < 0.2 → Ocean biomes
2. **Erosion** < 0.15 → River biomes
3. **Temperature** + **Humidity** → Climate-based biomes
4. **Peaks & Valleys** > 0.8 → Mountain biomes
5. **Weirdness** → Special terrain features

### Cave Generation

Caves use multiple noise functions:
- **Spaghetti**: Two 3D noise functions multiplied (caves where both are near zero)
- **Cheese**: Single 3D noise function (caves where value is high)
- **Standard**: Basic cave noise for variety

## Future Enhancements

Possible improvements:
1. **Structure Generation**: Villages, dungeons, temples
2. **Tree Generation**: Different tree shapes per biome
3. **Vegetation**: Flowers, mushrooms, cacti
4. **Water Physics**: Flowing water simulation
5. **Lighting System**: Per-block lighting and shadows
6. **Block Metadata**: Additional block properties
7. **Biome Blending**: Smooth transitions between biomes
8. **Underground Features**: Ravines, mineshafts, strongholds
9. **Weather System**: Rain, snow, storms per biome
10. **Day/Night Cycle Integration**: Time-based world changes

## Notes

- The world seed determines all terrain generation
- Same seed will always generate the same world
- Chunks are stored in world coordinates
- Block coordinates use standard x,y,z system
- Y=0 is bedrock layer, Y=64 is sea level
- Render distance can be adjusted in VoxelWorld constructor (default: 6 chunks)

## Building

The new files are automatically included in the Makefile. Simply rebuild:

```bash
cd src
make clean
make
```

## Credits

Based on Minecraft's terrain generation concepts, adapted for the Tesseract engine with Marching Cubes support and advanced multi-octave noise generation.
