#pragma once

#include <SDL2/SDL.h>

#include "../TileMap.h"

struct Fireball
{
    float x           = 0.0f;
    float y           = 0.0f;
    float vx          = 0.0f;
    float vy          = 0.0f;
    int width         = 0;      // collision/render width (scaled)
    int height        = 0;      // collision/render height (scaled)
    int srcWidth      = 0;      // frame width in the texture
    int srcHeight     = 0;      // frame height in the texture
    float renderScale = 0.35f;  // shrink further to ease dodging

    int frames       = 5;
    int frame        = 0;
    float frameTimer = 0.0f;
    float frameTime  = 0.1f;

    bool alive           = true;
    SDL_Texture* texture = nullptr;
    // Identify origin for damage gating (boss vs regular enemies)
    bool fromBoss = false;

    void initFromTexture()
    {
        if (!texture)
            return;
        int texW = 0, texH = 0;
        SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
        if (frames > 0)
            srcWidth = texW / frames;
        srcHeight = texH;

        // Apply scale to make the projectile easier to dodge
        width  = std::max(1, static_cast<int>(srcWidth * renderScale));
        height = std::max(1, static_cast<int>(srcHeight * renderScale));
    }

    void update(float dt, const TileMap& map)
    {
        if (!alive)
            return;

        x += vx * dt;
        y += vy * dt;

        frameTimer += dt;
        if (frameTimer >= frameTime)
        {
            frameTimer -= frameTime;
            frame = (frame + 1) % frames;
        }

        // Simple collision against solid tiles (ignore down-only since projectile moves
        // horizontally)
        float testPoints[4][2] = {
            {x, y}, {x + width - 1, y}, {x, y + height - 1}, {x + width - 1, y + height - 1}};
        for (auto& p : testPoints)
        {
            if (map.isSolidAtWorld(p[0], p[1]))
            {
                alive = false;
                break;
            }
        }
    }

    void render(SDL_Renderer* renderer, int camX, int camY)
    {
        if (!alive || !texture)
            return;
        SDL_Rect src{frame * srcWidth, 0, srcWidth, srcHeight};
        SDL_Rect dst{static_cast<int>(x) - camX, static_cast<int>(y) - camY, width, height};
        SDL_RenderCopy(renderer, texture, &src, &dst);
    }
};
