// Copyright 2025 Polar Bear Green World
// ClimbingComponent - Handles climbing mechanics

#pragma once

#include "BearComponent.h"

class ClimbingComponent : public BearComponent {
public:
    void update(PolarBear& bear, float dt, const TileMap& map) override;

private:
    void updateClimbVelocity(PolarBear& bear);
    void handleLedgeMount(PolarBear& bear, float dt, const TileMap& map);
    void handleClimbingPhysics(PolarBear& bear, float dt, const TileMap& map);
    void updateClimbingAnimation(PolarBear& bear, float dt);
};
