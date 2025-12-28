// Copyright 2025 Polar Bear Green World
// Component-based architecture for PolarBear
// Base interface for all bear components (movement, armor, abilities)

#pragma once

// Forward declarations
class PolarBear;
class TileMap;

class BearComponent {
public:
    virtual ~BearComponent() = default;
    
    // Called every frame with bear reference, delta time, and tilemap
    virtual void update(PolarBear& bear, float dt, const TileMap& map) = 0;
};
