// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// SwimmingMovementState.h
// Water element movement state with swim physics and no gravity.

#pragma once

#include "MovementState.h"

class TileMap;
class PolarBear;

class SwimmingMovementState : public MovementState
{
   public:
    void updatePhysics(PolarBear& bear, float dt, const TileMap& map) override;
    void updateAnimation(PolarBear& bear, float dt) override;
    void onEnter(PolarBear& bear) override;
    void onExit(PolarBear& bear) override;
    const char* getName() const override { return "Swimming"; }

   private:
    void detectWaterGround(PolarBear& bear, const TileMap& map);
    void updateSwimmingVelocity(PolarBear& bear, float dt);
};
