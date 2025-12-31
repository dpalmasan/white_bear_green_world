#pragma once

#include "Enemy.h"
#include "core/GameConstants.h"

class Fireball;

// Robot enemy that attacks when the player is close by spawning fireballs.
class RobotEnemy : public Enemy
{
   public:
    RobotEnemy(int w = GameConstants::Enemies::Robot::WIDTH,
               int h = GameConstants::Enemies::Robot::HEIGHT)
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
        attackFrameW    = GameConstants::Enemies::Robot::ATTACK_WIDTH;
        attackFrameH    = GameConstants::Enemies::Robot::ATTACK_HEIGHT;
    }

    // AI tick that handles detection and attack timing.
    void tickAI(float dt, const TileMap& map, const class PolarBear& bear,
                std::vector<Fireball>& fireballs, SDL_Texture* fireballTexture);

    // Override render to show attack frames when attacking.
    void render(SDL_Renderer* renderer, int camX, int camY) override;

   private:
    SDL_Texture* attackTexture = nullptr;
    int attackFrames           = GameConstants::Enemies::Robot::ATTACK_FRAMES;
    int attackFrameW           = GameConstants::Enemies::Robot::ATTACK_WIDTH;
    int attackFrameH           = GameConstants::Enemies::Robot::ATTACK_HEIGHT;
    float attackFrameTime      = GameConstants::Enemies::Robot::ATTACK_FRAME_TIME;

    bool attacking       = false;
    bool firedThisAttack = false;
    float attackTimer    = 0.0f;
    int attackFrame      = 0;
    float cooldown       = GameConstants::Enemies::Robot::FIRE_COOLDOWN;
    float cooldownTimer  = 0.0f;
};
