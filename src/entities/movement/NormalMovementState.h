// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// NormalMovementState.h
// Standard ground-based movement state with gravity, jumping, and knockback.

#pragma once

#include "MovementState.h"

class TileMap;
class PolarBear;

class NormalMovementState : public MovementState
{
   public:
    void updatePhysics(PolarBear& bear, float dt, const TileMap& map) override;
    void updateAnimation(PolarBear& bear, float dt) override;
    void onEnter(PolarBear& bear) override;
    const char* getName() const override { return "Normal"; }

   private:
    // Helper methods for physics calculations
    void applyGravity(PolarBear& bear, float dt);
    void applyHorizontalMovement(PolarBear& bear, float dt, const TileMap& map);
};
