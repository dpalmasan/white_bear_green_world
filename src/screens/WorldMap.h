#pragma once
#include <SDL2/SDL.h>

#include <string>
#include <vector>

// Simple world map screen that renders a background image and a smooth-moving cursor
class WorldMap
{
   public:
    struct Location
    {
        std::string name;
        int x = 0;  // pixel in 256x256 map space
        int y = 0;
    };

    // Assets
    SDL_Texture* background = nullptr;  // world-map.png (256x256)
    SDL_Texture* cursorTex  = nullptr;  // map-cursor.png (32x32)

    // Locations
    std::vector<Location> locations;
    int currentIndex = 0;
    struct Neighbors
    {
        int up = -1, down = -1, left = -1, right = -1;
    };
    std::vector<Neighbors> neighbors;  // directional graph between locations

    // Cursor smooth position (in map pixel space)
    float cursorX   = 0.0f;
    float cursorY   = 0.0f;
    float targetX   = 0.0f;
    float targetY   = 0.0f;
    float moveSpeed = 300.0f;  // pixels per second in map space

    // Debug draw markers at exact node centers
    bool debug = false;

    // Cursor anchor offset in cursor texture pixels (here positive X shifts right visually)
    int cursorOffsetX = -8;  // move 8px to the right with current math
    int cursorOffsetY = 0;

    // Load assets and initialize locations
    bool load(SDL_Renderer* renderer, const std::string& assetPath);

    // Handle input events (arrow keys to move cursor between nearest nodes in direction)
    void handleEvent(const SDL_Event& e);

    // Update cursor interpolation
    void update(float dt);

    // Render map scaled to camera view, and cursor at scaled position
    void render(SDL_Renderer* renderer, int viewW, int viewH);

    // Cleanup
    void clean();

   private:
    // Choose the next location index in a given direction; returns same index if none
    int nextIndexInDirection(int dirX, int dirY) const;
};
