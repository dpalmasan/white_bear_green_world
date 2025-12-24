// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// Explosion.h
// Explosion effect data structure.

#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <SDL2/SDL.h>

// Explosion effect data structure.
// Used to show visual feedback when the player hits an enemy with the slash.
struct Explosion
{
    float x, y;             // World position of the explosion.
    float timer    = 0.0f;  // Elapsed time since explosion started.
    float duration = 0.5f;  // Total duration of explosion effect in seconds.

    // Returns true if the explosion has finished.
    bool done() const { return timer >= duration; }

    // Updates the explosion timer by delta time.
    void update(float dt) { timer += dt; }

    // Renders the explosion as an expanding circle with fade-out.
    void render(SDL_Renderer* renderer, int camX, int camY)
    {
        float p = timer / duration;
        if (p > 1.0f)
            p = 1.0f;
        int r     = int(10 + 40 * p);
        int alpha = int(255 * (1.0f - p));

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 180, 60, alpha);

        // Draw filled circle using horizontal lines (much faster than pixel-by-pixel)
        int centerX = static_cast<int>(x) - camX;
        int centerY = static_cast<int>(y) - camY;

        for (int dy = -r; dy <= r; ++dy)
        {
            int width = static_cast<int>(SDL_sqrt(r * r - dy * dy));
            SDL_Rect rect{centerX - width, centerY + dy, width * 2, 1};
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
};

#endif  // EXPLOSION_H
