// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// Boss.h
// Abstract base class for boss enemies with complex behavior patterns.

#ifndef BOSS_H
#define BOSS_H

#include <SDL2/SDL.h>

#include <memory>
#include <vector>

// Forward declarations
class TileMap;
class PolarBear;
struct Fireball;
struct Camera;
struct Explosion;

// Boss state enumeration
enum BossState
{
    BossIdle         = 0,
    BossDecision     = 1,
    BossAttack       = 2,
    BossDashPrep     = 3,
    BossDashMove     = 4,
    BossVulnerable   = 5,
    BossDying        = 6,
    BossDisappearing = 7,
    BossDead         = 8
};

// Abstract base class for all boss enemies
class Boss
{
   public:
    virtual ~Boss() = default;

    // Core boss lifecycle methods
    virtual void loadAssets(SDL_Renderer* renderer, const std::string& assetPath)               = 0;
    virtual void updateAI(float dt, const TileMap& map, PolarBear& player,
                          std::vector<Fireball>& fireballs, std::vector<Explosion>& explosions) = 0;
    virtual void render(SDL_Renderer* renderer, const Camera& camera)                           = 0;

    // Boss interaction methods
    virtual void takeDamage(int amount)                    = 0;
    virtual bool isDead() const                            = 0;
    virtual bool isVulnerable() const                      = 0;
    virtual void getCollisionRect(SDL_Rect& outRect) const = 0;

    // Intro/death sequence hooks
    virtual void startIntro()          = 0;
    virtual void updateIntro(float dt) = 0;
    virtual bool isIntroActive() const = 0;
    virtual bool isIntroDone() const   = 0;

    // Position accessors
    virtual float getX() const                 = 0;
    virtual float getY() const                 = 0;
    virtual void setPosition(float x, float y) = 0;

    // Music control (optional override)
    virtual bool shouldStartMusic() const { return false; }
    virtual bool shouldStopMusic() const { return false; }
    virtual bool shouldLoopMusic() const { return false; }
    virtual bool shouldDisableInputs() const { return false; }
    virtual void enableInputs() {}

    // Fade effects for death sequence
    virtual int getFadeAlpha() const { return 0; }

    // Whether the boss can currently damage the player (e.g., not during death phases)
    // Default: true. Concrete bosses should override to disable damage during
    // dying/disappearing/dead.
    virtual bool canDamagePlayer() const { return true; }

   protected:
    Boss() = default;
};

#endif  // BOSS_H
