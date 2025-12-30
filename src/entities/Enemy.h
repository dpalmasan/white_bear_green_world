#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <string>

#include "../systems/TileMap.h"

// Base Enemy class - handles physics, collision, animation, rendering
// Derived classes override updateBehavior() for specific enemy behavior
class Enemy
{
   public:
    float x = 0.0f, y = 0.0f;
    float vx = 0.0f, vy = 0.0f;
    bool onGround    = false;
    int frame        = 0;
    int numFrames    = 1;
    float frameTime  = 0.12f;
    float frameTimer = 0.0f;

    int width = 24, height = 44;
    bool alive           = true;
    SDL_Texture* texture = nullptr;
    bool flipHorizontal  = false;

    // Fraction of height used to lower characters so opaque pixels rest on the floor
    float groundAlignInsetFrac = 0.15f;

    Enemy() {}
    virtual ~Enemy() {}

    virtual void loadTexture(SDL_Renderer* renderer, const std::string& filename)
    {
        SDL_Surface* surf = IMG_Load(filename.c_str());
        if (!surf)
            return;
        texture  = SDL_CreateTextureFromSurface(renderer, surf);
        int texW = 0, texH = 0;
        if (texture)
            SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        SDL_FreeSurface(surf);
        if (texW > 0 && width > 0)
        {
            numFrames = texW / width;
            if (numFrames < 1)
                numFrames = 1;
        }
    }

    virtual void update(float dt, const TileMap& map)
    {
        if (!alive)
            return;

        // Apply gravity
        const float GRAVITY = 1000.0f;
        vy += GRAVITY * dt;

        // Vertical movement (all enemies fall)
        y += vy * dt;
        onGround = false;

        const int vSamples = 10;
        int vCollisions    = 0;

        if (vy > 0)
        {
            // Check for ground with 30% threshold
            for (int i = 0; i < vSamples; ++i)
            {
                int w = (i * width) / (vSamples - 1);
                if (w >= width)
                    w = width - 1;
                if (map.isSolidAtWorld(x + w, y + height) ||
                    map.isCollisionDownOnlyAtWorld(x + w, y + height))
                {
                    vCollisions++;
                }
            }
            if (vCollisions >= vSamples * 0.2f)
            {
                float inset = height * groundAlignInsetFrac;
                y  = (static_cast<int>(y + height) / map.tileSize) * map.tileSize - height + inset;
                vy = 0;
                onGround = true;
            }
        }
        else if (vy < 0)
        {
            // Check for ceiling with 30% threshold
            for (int i = 0; i < vSamples; ++i)
            {
                int w = (i * width) / (vSamples - 1);
                if (w >= width)
                    w = width - 1;
                if (map.isSolidAtWorld(x + w, y))
                {
                    vCollisions++;
                }
            }
            if (vCollisions >= vSamples * 0.2f)
            {
                y  = (static_cast<int>(y) / map.tileSize + 1) * map.tileSize;
                vy = 0;
            }
        }

        // Animation
        frameTimer += dt;
        if (frameTimer >= frameTime)
        {
            frameTimer = 0.0f;
            frame      = (frame + 1) % numFrames;
        }

        // Call derived behavior (idle, patrol, chase, etc.)
        updateBehavior(dt, map);
    }

    // Override in derived classes for specific behavior
    virtual void updateBehavior(float dt, const TileMap& map)
    {
        // Base: idle (do nothing)
    }

    virtual void render(SDL_Renderer* renderer, int camX, int camY)
    {
        if (!alive)
            return;
        if (!texture)
            return;
        SDL_Rect src{frame * width, 0, width, height};
        SDL_Rect dst{static_cast<int>(x) - camX, static_cast<int>(y) - camY, width, height};
        SDL_RendererFlip flip = flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderCopyEx(renderer, texture, &src, &dst, 0.0, nullptr, flip);
    }

    SDL_Rect getAABB() const
    {
        return SDL_Rect{static_cast<int>(x), static_cast<int>(y), width, height};
    }
};
