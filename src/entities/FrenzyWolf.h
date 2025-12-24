#pragma once

#include <SDL2/SDL.h>

#include "Enemy.h"

class PolarBear;

// Frenzy wolf: fast chaser that runs toward the player.
class FrenzyWolf : public Enemy
{
   public:
    FrenzyWolf()
    {
        width                = 34;     // idle frame width
        height               = 25;     // frame height
        groundAlignInsetFrac = 0.15f;  // same as other enemies
    }

    // Assign shared idle texture and infer frame sizing from its dimensions.
    void setIdleTexture(SDL_Texture* tex);

    // Assign shared run (attack) texture and infer frames using current frame width.
    void setRunTexture(SDL_Texture* tex, float frameTimeSeconds = 0.08f);

    // AI tick: chooses direction toward the player and sets running state.
    void tickAI(float dt, const TileMap& map, const PolarBear& bear);

    void updateBehavior(float dt, const TileMap& map) override;
    void render(SDL_Renderer* renderer, int camX, int camY) override;

   private:
    SDL_Texture* runTexture = nullptr;
    int runFrames           = 1;
    int runFrame            = 0;
    int runFrameWidth       = 38;  // attack frames are wider
    float runFrameTime      = 0.08f;
    float runFrameTimer     = 0.0f;

    bool running      = false;
    float targetSpeed = 260.0f;   // fast chaser
    float accel       = 1200.0f;  // quick ramp-up to speed
};
