// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// idleenemy.h
// Simple enemy type that falls under gravity and animates in place.
// Useful as a placeholder or for stationary enemies.

#pragma once
#include "Enemy.h"

// IdleEnemy derived class.
// A simple enemy that falls under gravity and plays idle animation.
// No special behavior—just physics, collision, and animation.
// Dimensions are customizable via constructor for different sprite sizes.
class IdleEnemy : public Enemy
{
   public:
    // Constructor with customizable dimensions.
    // Parameters:
    //   w: sprite width in pixels (default 24).
    //   h: sprite height in pixels (default 44).
    IdleEnemy(int w = 24, int h = 44)
    {
        width  = w;
        height = h;
    }
    virtual ~IdleEnemy() {}

    // No behavior override needed; uses base Enemy's idle behavior.
    // The enemy falls under gravity, animates, and responds to collisions.
};
