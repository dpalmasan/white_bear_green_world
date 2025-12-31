#pragma once

#include <SDL2/SDL.h>

#include "Enemy.h"
#include "core/GameConstants.h"

class PolarBear;

// Frenzy wolf: fast chaser that runs toward the player.
class FrenzyWolf : public Enemy
{
   public:
    FrenzyWolf()
    {
        width                = GameConstants::Enemies::Wolf::IDLE_WIDTH;
        height               = GameConstants::Enemies::Wolf::HEIGHT;
        groundAlignInsetFrac = GameConstants::Enemies::Wolf::GROUND_INSET;
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
    int runFrameWidth       = GameConstants::Enemies::Wolf::RUN_WIDTH;
    float runFrameTime      = GameConstants::Enemies::Wolf::FRAME_TIME;
    float runFrameTimer     = 0.0f;

    bool running      = false;
    float targetSpeed = GameConstants::Enemies::Wolf::TARGET_SPEED;
    float accel       = GameConstants::Enemies::Wolf::ACCELERATION;
};
