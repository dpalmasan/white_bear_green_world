#include "TileMap.h"

#include <SDL2/SDL_image.h>

#include <fstream>
#include <iostream>

// Load map data from JSON file
bool TileMap::loadFromJSON(const std::string& filename)
{
    std::ifstream f(filename);
    if (!f.is_open())
    {
        std::cerr << "Failed to open map file: " << filename << "\n";
        return false;
    }

    json j;
    try
    {
        f >> j;
    }
    catch (const json::exception& e)
    {
        std::cerr << "JSON parse error: " << e.what() << "\n";
        return false;
    }

    // Read map dimensions
    tileSize = j["tileSize"];
    width    = j["mapWidth"];
    height   = j["mapHeight"];

    // Read layers (ordered bottom to top in the JSON)
    layers.clear();
    for (const auto& jLayer : j["layers"])
    {
        Layer layer;
        layer.name     = jLayer["name"];
        layer.collider = jLayer.value("collider", false);

        // Read tiles in this layer
        for (const auto& jTile : jLayer["tiles"])
        {
            Tile tile;
            // Parse ID (could be string or int)
            if (jTile["id"].is_string())
                tile.id = std::stoi(jTile["id"].get<std::string>());
            else
                tile.id = jTile["id"].get<int>();

            tile.x = jTile["x"];
            tile.y = jTile["y"];

            // Check for attributes
            if (jTile.contains("attributes"))
            {
                const auto& attrs = jTile["attributes"];
                // Check for collision_down_only attribute
                if (attrs.contains("collision_down_only"))
                {
                    tile.collision_down_only = attrs["collision_down_only"];
                }
                // Check for slippery attribute
                if (attrs.contains("slippery"))
                {
                    tile.slippery = attrs["slippery"];
                }
                // Check for climbable attribute
                if (attrs.contains("climbable"))
                {
                    tile.climbable = attrs["climbable"];
                }
                // Check for water attribute
                if (attrs.contains("is_water"))
                {
                    tile.isWater = attrs["is_water"].get<bool>();
                }
                // Check for wind attribute
                if (attrs.contains("is_wind"))
                {
                    tile.is_wind = attrs["is_wind"].get<bool>();
                }
                // Check for enemy attribute
                if (attrs.contains("enemy"))
                {
                    tile.enemyType = attrs["enemy"].get<std::string>();
                }
                // Check for power-up attribute
                if (attrs.contains("power_up"))
                {
                    tile.powerUp = attrs["power_up"].get<std::string>();
                }
                // Check for end-of-area attribute
                if (attrs.contains("end_of_area"))
                {
                    tile.endOfArea = attrs["end_of_area"].get<bool>();
                }
                // Check for polar bear spawn marker
                if (attrs.contains("polar_bear_spawn"))
                {
                    tile.polarBearSpawn = attrs["polar_bear_spawn"];
                }
                // Check for boss attribute
                if (attrs.contains("boss"))
                {
                    tile.boss = attrs["boss"].get<std::string>();
                }
                // Check for flipped attribute
                if (attrs.contains("flipped"))
                {
                    tile.flipped = attrs["flipped"];
                }
                // Check for cutscene attribute (string ID)
                if (attrs.contains("cutscene"))
                {
                    tile.cutscene = attrs["cutscene"].get<std::string>();
                    std::cerr << "[TileMap] Loaded cutscene tile at (" << tile.x << "," << tile.y 
                              << ") cutscene=" << tile.cutscene << "\n";
                }
                // Check for event attribute
                if (attrs.contains("event"))
                {
                    tile.event = attrs["event"].get<std::string>();
                }
            }

            layer.tiles.push_back(tile);
        }

        layers.push_back(layer);
    }

    return true;
}

// Load the spritesheet texture
bool TileMap::loadSpritesheet(SDL_Renderer* renderer, const std::string& filename)
{
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface)
    {
        std::cerr << "Failed to load spritesheet " << filename << ": " << IMG_GetError() << "\n";
        return false;
    }

    spritesheet = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!spritesheet)
    {
        std::cerr << "Failed to create spritesheet texture: " << SDL_GetError() << "\n";
        return false;
    }

    return true;
}

// Render all layers using the spritesheet
void TileMap::render(SDL_Renderer* renderer, int camX, int camY, float windTime) const
{
    if (!spritesheet)
        return;

    // Query spritesheet dimensions to calculate tiles per row
    int sheetW = 0, sheetH = 0;
    SDL_QueryTexture(spritesheet, nullptr, nullptr, &sheetW, &sheetH);

    if (sheetW <= 0 || tileSize <= 0)
        return;

    int tilesPerRow = sheetW / tileSize;

    // Render layers in reverse order (last layer in array = bottom, first layer = top)
    for (auto it = layers.rbegin(); it != layers.rend(); ++it)
    {
        const auto& layer = *it;
        for (const auto& tile : layer.tiles)
        {
            // Skip rendering tiles that are markers (enemy spawns, player spawns, power-ups,
            // boss spawns, end tiles, cutscene triggers, event objects)
            if (!tile.enemyType.empty() || tile.polarBearSpawn || !tile.powerUp.empty() ||
                !tile.boss.empty() || tile.endOfArea || !tile.cutscene.empty() || !tile.event.empty())
                continue;

            // Calculate source rect from tile ID
            // Tiles are numbered from 0, ordered left-to-right, top-to-bottom
            int srcX = (tile.id % tilesPerRow) * tileSize;
            int srcY = (tile.id / tilesPerRow) * tileSize;

            SDL_Rect src{srcX, srcY, tileSize, tileSize};

            // Calculate destination rect in screen space
            SDL_Rect dst{tile.x * tileSize - camX, tile.y * tileSize - camY, tileSize, tileSize};

            // Apply wind animation if this is a wind tile
            if (tile.is_wind)
            {
                // Animate opacity between 50-70% (127-178 in 0-255 range)
                float opacityFactor = 0.6f + 0.1f * std::sin(windTime * 3.14159f);
                Uint8 opacity = static_cast<Uint8>(opacityFactor * 255.0f);
                SDL_SetTextureAlphaMod(spritesheet, opacity);
                
                // Add slight shake/wobble (±1-2 pixels)
                int shakeX = static_cast<int>(2.0f * std::sin(windTime * 4.0f));
                int shakeY = static_cast<int>(1.5f * std::cos(windTime * 3.5f));
                dst.x += shakeX;
                dst.y += shakeY;
            }

            SDL_RenderCopy(renderer, spritesheet, &src, &dst);

            // Reset alpha if we modified it
            if (tile.is_wind)
            {
                SDL_SetTextureAlphaMod(spritesheet, 255);
            }
        }
    }
}

// Check if a world position collides with any solid tile
bool TileMap::isSolidAtWorld(float worldX, float worldY, float vy) const
{
    if (worldX < 0 || worldY < 0)
        return false;

    int tileX = static_cast<int>(worldX) / tileSize;
    int tileY = static_cast<int>(worldY) / tileSize;

    if (tileX < 0 || tileX >= width || tileY < 0 || tileY >= height)
        return false;

    // Check all layers for collision
    for (const auto& layer : layers)
    {
        // Check if any tile in this layer occupies this position
        for (const auto& tile : layer.tiles)
        {
            if (tile.x == tileX && tile.y == tileY)
            {
                // Skip marker tiles (enemy spawns, player spawn, power-ups, end tiles) - they
                // should not collide
                if (!tile.enemyType.empty() || tile.polarBearSpawn || !tile.powerUp.empty() ||
                    tile.endOfArea)
                    continue;

                // Water tiles are non-blocking
                if (tile.isWater)
                    continue;

                // If tile is down-only, treat it as collision regardless of layer collider flag
                // Only collide when actively falling (vy > 0)
                if (tile.collision_down_only)
                {
                    return vy > 0;
                }

                // For regular tiles, require the layer to be marked as collider
                if (layer.collider)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

// Check if a world-space position collides specifically with collision_down_only tiles
bool TileMap::isCollisionDownOnlyAtWorld(float worldX, float worldY) const
{
    if (worldX < 0 || worldY < 0)
        return false;

    int tileX = static_cast<int>(worldX) / tileSize;
    int tileY = static_cast<int>(worldY) / tileSize;

    if (tileX < 0 || tileX >= width || tileY < 0 || tileY >= height)
        return false;

    // Check all layers for collision_down_only tiles (attribute overrides collider flag)
    for (const auto& layer : layers)
    {
        // Check if any tile in this layer occupies this position
        for (const auto& tile : layer.tiles)
        {
            if (tile.x == tileX && tile.y == tileY && tile.collision_down_only)
            {
                // Skip marker tiles
                if (!tile.enemyType.empty() || tile.polarBearSpawn || !tile.powerUp.empty() ||
                    tile.endOfArea)
                    continue;

                return true;
            }
        }
    }

    return false;
}

// Check if a world position rests on a slippery tile
bool TileMap::isSlipperyAtWorld(float worldX, float worldY) const
{
    if (worldX < 0 || worldY < 0)
        return false;

    int tileX = static_cast<int>(worldX) / tileSize;
    int tileY = static_cast<int>(worldY) / tileSize;

    if (tileX < 0 || tileX >= width || tileY < 0 || tileY >= height)
        return false;

    for (const auto& layer : layers)
    {
        for (const auto& tile : layer.tiles)
        {
            if (tile.x == tileX && tile.y == tileY)
            {
                // Marker tiles should not affect movement properties
                if (!tile.enemyType.empty() || tile.polarBearSpawn || !tile.powerUp.empty() ||
                    tile.endOfArea)
                    continue;

                if (tile.slippery)
                    return true;
            }
        }
    }

    return false;
}

// Check if a world position is on a climbable tile
bool TileMap::isClimbableAtWorld(float worldX, float worldY) const
{
    if (worldX < 0 || worldY < 0)
        return false;

    int tileX = static_cast<int>(worldX) / tileSize;
    int tileY = static_cast<int>(worldY) / tileSize;

    if (tileX < 0 || tileX >= width || tileY < 0 || tileY >= height)
        return false;

    for (const auto& layer : layers)
    {
        for (const auto& tile : layer.tiles)
        {
            if (tile.x == tileX && tile.y == tileY)
            {
                // Marker tiles should not affect movement properties
                if (!tile.enemyType.empty() || tile.polarBearSpawn || !tile.powerUp.empty() ||
                    tile.endOfArea)
                    continue;

                if (tile.climbable)
                    return true;
            }
        }
    }

    return false;
}

// Check if a world position is inside a water tile
bool TileMap::isWaterAtWorld(float worldX, float worldY) const
{
    if (worldX < 0 || worldY < 0)
        return false;

    int tileX = static_cast<int>(worldX) / tileSize;
    int tileY = static_cast<int>(worldY) / tileSize;

    if (tileX < 0 || tileX >= width || tileY < 0 || tileY >= height)
        return false;

    for (const auto& layer : layers)
    {
        for (const auto& tile : layer.tiles)
        {
            if (tile.x == tileX && tile.y == tileY)
            {
                // Marker tiles should not affect water checks
                if (!tile.enemyType.empty() || tile.polarBearSpawn || !tile.powerUp.empty() ||
                    tile.endOfArea)
                    continue;

                if (tile.isWater)
                    return true;
            }
        }
    }

    return false;
}

// Check if a world position is inside a wind tile
bool TileMap::isWindAtWorld(float worldX, float worldY) const
{
    if (worldX < 0 || worldY < 0)
        return false;

    int tileX = static_cast<int>(worldX) / tileSize;
    int tileY = static_cast<int>(worldY) / tileSize;

    if (tileX < 0 || tileX >= width || tileY < 0 || tileY >= height)
        return false;

    for (const auto& layer : layers)
    {
        for (const auto& tile : layer.tiles)
        {
            if (tile.x == tileX && tile.y == tileY)
            {
                // Marker tiles should not affect wind checks
                if (!tile.enemyType.empty() || tile.polarBearSpawn || !tile.powerUp.empty() ||
                    tile.endOfArea)
                    continue;

                if (tile.is_wind)
                    return true;
            }
        }
    }

    return false;
}

// Get all tiles marked as enemy spawn points
std::vector<const Tile*> TileMap::getEnemySpawnTiles() const
{
    std::vector<const Tile*> spawnTiles;
    for (const auto& layer : layers)
    {
        for (const auto& tile : layer.tiles)
        {
            if (!tile.enemyType.empty())
            {
                spawnTiles.push_back(&tile);
            }
        }
    }
    return spawnTiles;
}

// Get tiles marked as power-ups
std::vector<const Tile*> TileMap::getPowerUpTiles() const
{
    std::vector<const Tile*> tilesOut;
    for (const auto& layer : layers)
    {
        for (const auto& tile : layer.tiles)
        {
            if (!tile.powerUp.empty())
                tilesOut.push_back(&tile);
        }
    }
    return tilesOut;
}

// Get optional polar bear spawn tile
const Tile* TileMap::getPolarBearSpawnTile() const
{
    for (const auto& layer : layers)
    {
        for (const auto& tile : layer.tiles)
        {
            if (tile.polarBearSpawn)
                return &tile;
        }
    }
    return nullptr;
}

// Get tiles marked as end-of-area triggers
std::vector<const Tile*> TileMap::getEndOfAreaTiles() const
{
    std::vector<const Tile*> out;
    for (const auto& layer : layers)
    {
        for (const auto& tile : layer.tiles)
        {
            if (tile.endOfArea)
                out.push_back(&tile);
        }
    }
    return out;
}

// Get tiles marked as boss spawns
std::vector<const Tile*> TileMap::getBossTiles() const
{
    std::vector<const Tile*> out;
    for (const auto& layer : layers)
    {
        for (const auto& tile : layer.tiles)
        {
            if (!tile.boss.empty())
                out.push_back(&tile);
        }
    }
    return out;
}

// Get tiles marked as event objects
std::vector<const Tile*> TileMap::getEventTiles() const
{
    std::vector<const Tile*> out;
    for (const auto& layer : layers)
    {
        for (const auto& tile : layer.tiles)
        {
            if (!tile.event.empty())
                out.push_back(&tile);
        }
    }
    return out;
}
