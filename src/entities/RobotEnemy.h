#pragma once

#include "Enemy.h"

class Fireball;

// Robot enemy that attacks when the player is close by spawning fireballs.
class RobotEnemy : public Enemy
{
   public:
    RobotEnemy(int w = 24, int h = 46)
    {
        width                = w;
        height               = h;
        groundAlignInsetFrac = 0.0f;  // sprite already adjusted for floor alignment
    }

    // Provide attack texture (separate from idle/texture) and frame timing.
    void setAttackTexture(SDL_Texture* tex, int frames, float frameTimeSeconds)
    {
        attackTexture   = tex;
        attackFrames    = frames;
        attackFrameTime = frameTimeSeconds;
        attackFrameW    = 37;  // attack frames are 37x44
        attackFrameH    = 44;
    }

    // AI tick that handles detection and attack timing.
    void tickAI(float dt, const TileMap& map, const class PolarBear& bear,
                std::vector<Fireball>& fireballs, SDL_Texture* fireballTexture);

    // Override render to show attack frames when attacking.
    void render(SDL_Renderer* renderer, int camX, int camY) override;

   private:
    SDL_Texture* attackTexture = nullptr;
    int attackFrames           = 6;
    int attackFrameW           = 37;  // attack frame width
    int attackFrameH           = 44;  // attack frame height
    float attackFrameTime      = 0.1f;

    bool attacking       = false;
    bool firedThisAttack = false;
    float attackTimer    = 0.0f;
    int attackFrame      = 0;
    float cooldown       = 3.0f;  // minimum wait between attacks
    float cooldownTimer  = 0.0f;
};
