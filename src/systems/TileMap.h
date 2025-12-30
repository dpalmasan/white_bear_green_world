#pragma once
#include <SDL2/SDL.h>

#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

// Represents a single tile with position, ID, and attributes
struct Tile
{
    int id;                            // Tile ID from spritesheet (0-based)
    int x, y;                          // Position in the tilemap
    bool collision_down_only = false;  // Only collide from above
    bool slippery            = false;  // Slippery surface when true
    bool climbable           = false;  // True if the tile can be climbed
    bool isWater             = false;  // Water tile for swimming mechanics
    bool is_wind             = false;  // Wind tile with opacity animation
    std::string powerUp;               // Power-up type (e.g., "heart"), empty if none
    bool polarBearSpawn = false;       // Spawn marker for the player
    std::string enemyType;  // Enemy type if this is a spawn tile (e.g., "arachnoid", "robot")
    std::string boss;       // Boss type if this is a boss spawn (e.g., "snow-robot"), empty if none
    bool endOfArea = false;  // Marks end-of-area trigger tile (transparent, no collision)
};

// Represents a layer of tiles
struct Layer
{
    std::string name;
    std::vector<Tile> tiles;
    bool collider = false;  // Whether this layer participates in collision
};

// TileMap class that loads from JSON and renders using a spritesheet
class TileMap
{
   public:
    int width    = 0;
    int height   = 0;
    int tileSize = 16;

    std::vector<Layer> layers;           // Layers ordered bottom to top (first = bottom)
    SDL_Texture* spritesheet = nullptr;  // Tileset texture

    // Load map from JSON file
    bool loadFromJSON(const std::string& filename);

    // Load spritesheet texture
    bool loadSpritesheet(SDL_Renderer* renderer, const std::string& filename);

    // Render all layers to the screen
    void render(SDL_Renderer* renderer, int camX, int camY, float windTime = 0.0f) const;

    // Check if a world-space position collides with solid tiles
    // For down-only tiles, only collides if moving downward (vy > 0)
    bool isSolidAtWorld(float worldX, float worldY, float vy = 0.0f) const;

    // Check if a world-space position collides specifically with collision_down_only tiles
    // Only returns true when falling (vy > 0) and hitting a down-only tile from above
    bool isCollisionDownOnlyAtWorld(float worldX, float worldY) const;

    // Check if a world-space position sits on a slippery tile
    bool isSlipperyAtWorld(float worldX, float worldY) const;

    // Check if a world-space position is on a climbable tile
    bool isClimbableAtWorld(float worldX, float worldY) const;

    // Check if a world-space position is inside a water tile
    bool isWaterAtWorld(float worldX, float worldY) const;

    // Check if a world-space position is inside a wind tile
    bool isWindAtWorld(float worldX, float worldY) const;

    // Get all tiles marked as enemy spawn points
    std::vector<const Tile*> getEnemySpawnTiles() const;

    // Get tiles marked as power-ups
    std::vector<const Tile*> getPowerUpTiles() const;

    // Get optional polar bear spawn tile (first occurrence)
    const Tile* getPolarBearSpawnTile() const;

    // Get tiles that mark the end of area
    std::vector<const Tile*> getEndOfAreaTiles() const;

    // Get tiles marked with boss spawns
    std::vector<const Tile*> getBossTiles() const;
};