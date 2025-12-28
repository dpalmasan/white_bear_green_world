// Copyright 2025 Polar Bear Green World
// MovementComponent - Handles basic physics, collision, and animation
// Replaces NormalMovementState logic in component architecture

#pragma once

#include "BearComponent.h"

class MovementComponent : public BearComponent {
public:
    void update(PolarBear& bear, float dt, const TileMap& map) override;

private:
    void applyGravity(PolarBear& bear, float dt);
    void applyHorizontalMovement(PolarBear& bear, float dt, const TileMap& map);
    void handleCollision(PolarBear& bear, float dt, const TileMap& map);
    void updateAnimation(PolarBear& bear, float dt);
};
