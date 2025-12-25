// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// SnowRobotBoss.h
// Snow robot boss with dash attack, vulnerable states, and death sequence.

#ifndef SNOW_ROBOT_BOSS_H
#define SNOW_ROBOT_BOSS_H

#include <SDL2/SDL_mixer.h>

#include "Boss.h"

// Forward declare Explosion (defined in Game.h)
struct Explosion;

// Snow robot-specific attack state machine (internal to this boss)
enum SnowRobotBossPhase
{
    SnowPhaseIdle        = 0,
    SnowPhaseDecision    = 1,
    SnowPhaseAttack      = 2,
    SnowPhaseDashPrep    = 3,
    SnowPhaseDashMove    = 4,
    SnowPhaseVulnerable  = 5
};

class SnowRobotBoss : public Boss
{
   public:
    SnowRobotBoss();
    virtual ~SnowRobotBoss();

    // Boss interface implementation
    void loadAssets(SDL_Renderer* renderer, const std::string& assetPath) override;
    void updateAI(float dt, const TileMap& map, PolarBear& player) override;
    void render(SDL_Renderer* renderer, const Camera& camera) override;

    // Snow robot-specific projectile spawning
    void spawnProjectiles(std::vector<Fireball>& fireballs);
    void spawnExplosions(std::vector<Explosion>& explosions);

    void takeDamage(int amount) override;
    bool isDead() const override;
    bool isVulnerable() const override;
    void getCollisionRect(SDL_Rect& outRect) const override;

    void startIntro() override;
    void updateIntro(float dt) override;
    bool isIntroActive() const override { return introActive; }
    bool isIntroDone() const override { return introDone; }

    float getX() const override { return worldX; }
    float getY() const override { return worldY; }
    void setPosition(float x, float y) override;

    // Music control
    bool shouldStartMusic() const override;
    bool shouldStopMusic() const override;
    bool shouldLoopMusic() const override;
    bool canDamagePlayer() const override
    {
        return state != BossDying && state != BossDisappearing && state != BossDead;
    }

    // Gameplay state
    bool isAlive() const { return alive; }
    int getHealth() const { return health; }
    bool shouldDisableInputs() const { return inputsDisabled; }
    int getFadeAlpha() const override { return fadeAlpha; }
    void enableInputs() override { inputsDisabled = false; }

   private:
    // Position and rendering
    float worldX         = 0.0f;
    float worldY         = 0.0f;
    float levitateOffset = 0.0f;
    float lastPlayerX    = 0.0f;  // Track player position for flipping

    // Textures
    SDL_Texture* introTexture      = nullptr;
    SDL_Texture* attackTexture     = nullptr;
    SDL_Texture* dashTexture       = nullptr;
    SDL_Texture* vulnerableTexture = nullptr;
    SDL_Texture* fireballTexture   = nullptr;

    // Sound effects
    Mix_Chunk* explosionSound  = nullptr;
    Mix_Chunk* laserSound      = nullptr;
    Mix_Chunk* jetSound        = nullptr;
    Mix_Chunk* metalClashSound = nullptr;

    // State machine
    BossState state = BossActive;           // General boss lifecycle state
    SnowRobotBossPhase phase = SnowPhaseIdle;  // Snow robot attack pattern state
    int animFrame   = 0;
    float animTimer = 0.0f;

    // Health and status
    int health          = 5;
    bool alive          = true;
    bool hitThisAttack  = false;
    bool inputsDisabled = false;  // Disable inputs when boss dies

    // Intro sequence
    bool introActive           = false;
    bool introDone             = false;
    int introFrame             = 0;
    float introTimer           = 0.0f;
    int introLoopsRemaining    = 0;
    const int introFrames      = 11;  // Frames 1-7 once, then 8-11 three times
    const float introFrameTime = 0.15f;

    // Decision making
    float decisionTimer          = 0.0f;
    const float decisionInterval = 1.0f;
    float shootProb              = 1.0f;
    int bulletsShot              = 0;

    // Attack behavior
    bool attackFired            = false;
    const float attackFrameTime = 0.15f;

    // Dash attack
    float dashVX              = 0.0f;
    float dashDistance        = 0.0f;
    float dashTarget          = 0.0f;
    int dashPrepLoops         = 0;
    const float dashFrameTime = 0.15f;

    // Vulnerable state
    int vulnerableLoops             = 0;
    const float vulnerableFrameTime = 0.15f;
    // Post-hit invulnerability to prevent rapid multi-hits
    float invulnerableTimer          = 0.0f;
    const float invulnerableDuration = 0.6f;

    // Death sequence
    float deathTimer     = 0.0f;
    float deathFrameTime = 0.30f;
    int deathLoops       = 0;
    int deathStage       = 0;

    // Fade effects
    int fadeAlpha               = 0;  // 0 = transparent, 255 = opaque
    float fadeTimer             = 0.0f;
    const float fadeInDuration  = 1.0f;  // 1 second fade in
    const float fadeOutDuration = 1.0f;  // 1 second fade out

    // Music control flags
    bool musicStartRequested = false;
    bool musicStopRequested  = false;
    bool musicLoopRequested  = false;

    // Pending projectiles created during updateAI (transferred via spawn methods)
    std::vector<Fireball> pendingFireballs;
    std::vector<Explosion> pendingExplosions;

    // Helper methods
    void updateIdleState(float dt);
    void updateDecisionState(float dt);
    void updateAttackState(float dt, PolarBear& player, std::vector<Fireball>& fireballs);
    void updateDashPrepState(float dt, PolarBear& player);
    void updateDashMoveState(float dt, const TileMap& map);
    void updateVulnerableState(float dt);
    void updateDyingState(float dt, std::vector<Explosion>& explosions);
    void updateDisappearingState(float dt);
    void updateDeadState(float dt);
};

#endif  // SNOW_ROBOT_BOSS_H
