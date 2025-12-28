// Copyright 2025 Polar Bear Green World
// SwimmingComponent - Handles water detection and swimming mechanics

#pragma once

#include "BearComponent.h"

class SwimmingComponent : public BearComponent {
public:
    void update(PolarBear& bear, float dt, const TileMap& map) override;

private:
    void detectWaterGround(PolarBear& bear, const TileMap& map);
    void updateSwimmingVelocity(PolarBear& bear, float dt);
    void handleSwimmingPhysics(PolarBear& bear, float dt, const TileMap& map);
    void updateSwimmingAnimation(PolarBear& bear, float dt);
};
