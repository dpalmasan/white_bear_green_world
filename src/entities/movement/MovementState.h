// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// MovementState.h
// Abstract base class for polar bear movement states.
// Each state encapsulates physics, collision, and animation logic for a specific movement type.

#pragma once

#include <SDL2/SDL.h>

class TileMap;
class PolarBear;

// Abstract base class for movement states.
// Each state manages its own physics rules, collision handling, and animation transitions.
class MovementState
{
   public:
    virtual ~MovementState() = default;

    // Update physics for this frame
    virtual void updatePhysics(PolarBear& bear, float dt, const TileMap& map) = 0;

    // Update animation for this frame
    virtual void updateAnimation(PolarBear& bear, float dt) = 0;

    // Called when bear transitions from another state to this one
    virtual void onEnter(PolarBear& bear) {}

    // Called when bear transitions away from this state
    virtual void onExit(PolarBear& bear) {}

    // Get human-readable state name for debugging
    virtual const char* getName() const = 0;
};
