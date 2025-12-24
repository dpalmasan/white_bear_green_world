// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// Attack.h
// Base class for all attack types, allowing flexible attack composition.

#pragma once
#include <SDL2/SDL.h>

#include <memory>
#include <string>

// Forward declaration.
class PolarBear;

// Base class for all attacks.
// Attacks manage their own animation state, rendering, and collision.
class Attack
{
   public:
    virtual ~Attack() = default;

    // Update the attack state (animations, timers, etc.) by dt seconds.
    virtual void update(float dt) = 0;

    // Render the attack effect (slash, particles, etc.) to the screen.
    // camX, camY are camera offsets.
    virtual void render(SDL_Renderer* renderer, int camX, int camY) = 0;

    // Get the world-space bounding box for collision detection.
    // Returns true if the attack's collision is active; fills out with AABB.
    virtual bool getWorldRect(SDL_Rect& out) const = 0;

    // Called when the attack button is released (for charge/hold attacks).
    // Default does nothing; override to handle release logic.
    virtual void onRelease() {}

    // Returns true if this attack is still active and should continue.
    virtual bool isActive() const = 0;
};

// Slash attack: quick melee with a textured slash sprite.
class SlashAttack : public Attack
{
   public:
    // Constructor: pass the bear and slash texture.
    // The bear is used to get position, facing direction, and sprite dimensions.
    SlashAttack(PolarBear* bear, SDL_Texture* slashTexture);

    void update(float dt) override;
    void render(SDL_Renderer* renderer, int camX, int camY) override;
    bool getWorldRect(SDL_Rect& out) const override;
    bool isActive() const override;

    // Getters for rendering the attack sprite in PolarBear.
    int getAttackFrame() const { return attackFrame; }
    int getAttackFrameCount() const { return attackFrames; }

   private:
    PolarBear* bear;
    SDL_Texture* slashTexture;

    // Slash animation state.
    float slashTimer = 0.0f;
    const float slashStartDelay =
        0.16f;  // Delay before slash appears (let attack sprite start first).
    const float slashDuration = 0.2f;  // Duration of the slash effect (0.2s).

    // Attack sprite animation state.
    int attackFrame            = 0;
    float attackTimer          = 0.0f;
    const int attackFrames     = 7;
    const float attackDuration = 0.08f;  // Per-frame duration (~0.56s total).
    bool attackFinished        = false;  // Track if attack animation is complete.

    // Slash sprite dimensions.
    int slashWidth  = 48;
    int slashHeight = 24;
};
