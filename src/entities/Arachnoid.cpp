#include "Arachnoid.h"

#include <algorithm>

#include "core/GameConstants.h"

void Arachnoid::updateBehavior(float dt, const TileMap& map)
{
    if (!alive)
        return;

    // Move horizontally
    x += vx * dt;

    // Simple wall collision: if blocked, push out and reverse
    const int samples = GameConstants::Collision::COLLISION_SAMPLES;
    int collisions    = 0;

    if (vx > 0)
    {
        // Check right edge
        int rightX = static_cast<int>(x + width);
        for (int i = 0; i < samples; ++i)
        {
            int h = (i * height) / (samples - 1);
            if (h >= height)
                h = height - 1;
            if (map.isSolidAtWorld(rightX, y + h, 0.0f))
                collisions++;
        }
        if (collisions >= samples * GameConstants::Collision::TIGHT_COLLISION_THRESHOLD)
        {
            x  = (rightX / map.tileSize) * map.tileSize - width;
            vx = -vx;
        }
    }
    else if (vx < 0)
    {
        int leftX = static_cast<int>(x);
        for (int i = 0; i < samples; ++i)
        {
            int h = (i * height) / (samples - 1);
            if (h >= height)
                h = height - 1;
            if (map.isSolidAtWorld(leftX, y + h, 0.0f))
                collisions++;
        }
        if (collisions >= samples * GameConstants::Collision::TIGHT_COLLISION_THRESHOLD)
        {
            x  = (leftX / map.tileSize + 1) * map.tileSize;
            vx = -vx;
        }
    }

    // Edge detection: if no ground in front, reverse
    int footY     = static_cast<int>(y + height);
    bool groundOK = false;
    // Check multiple points along width for ground
    const int edgeChecks = GameConstants::Enemies::Arachnoid::EDGE_CHECKS;
    for (int i = 0; i < edgeChecks; ++i)
    {
        int checkX = static_cast<int>(vx > 0 ? x + width + i : x - edgeChecks + i);
        if (map.isSolidAtWorld(checkX, footY + 1, 1.0f) ||
            map.isCollisionDownOnlyAtWorld(checkX, footY + 1))
        {
            groundOK = true;
            break;
        }
    }
    if (!groundOK)
    {
        vx = -vx;
    }

    // Update facing flip: texture faces left by default; flip only when moving right
    flipHorizontal = (vx > 0);
}

void Arachnoid::render(SDL_Renderer* renderer, int camX, int camY)
{
    if (!alive || !texture)
        return;

    SDL_Rect src{frame * width, 0, width, height};
    SDL_Rect dst{static_cast<int>(std::round(x)) - camX,
                 static_cast<int>(std::round(y)) - camY + GameConstants::Enemies::Arachnoid::RENDER_Y_OFFSET,
                 width, height};
    SDL_RendererFlip flip = flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, texture, &src, &dst, 0.0, nullptr, flip);
}
