// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// ClimbingMovementState.h
// Wall climbing state with vertical-only movement.

#pragma once

#include "MovementState.h"

class TileMap;
class PolarBear;

class ClimbingMovementState : public MovementState
{
   public:
    void updatePhysics(PolarBear& bear, float dt, const TileMap& map) override;
    void updateAnimation(PolarBear& bear, float dt) override;
    void onEnter(PolarBear& bear) override;
    const char* getName() const override { return "Climbing"; }

   private:
    void updateClimbVelocity(PolarBear& bear);
    void handleLedgeMount(PolarBear& bear, float dt, const TileMap& map);
};
