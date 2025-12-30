// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// SwimmingMovementState.cpp

#include "SwimmingMovementState.h"

#include <algorithm>
#include <iostream>

#include "entities/PolarBear.h"
#include "systems/TileMap.h"

void SwimmingMovementState::onEnter(PolarBear& bear)
{
    bear.wasSwimming = bear.swimming;
}

void SwimmingMovementState::onExit(PolarBear& bear)
{
    // When exiting water at the surface (not grounded), give an upward impulse
    if (bear.justExitedWater && !bear.onGround)
    {
        bear.vy              = -336.0f;
        bear.onGround        = false;
        bear.justExitedWater = false;
    }
    else if (bear.justExitedWater)
    {
        bear.justExitedWater = false;
    }
}

void SwimmingMovementState::detectWaterGround(PolarBear& bear, const TileMap& map)
{
    // When swimming downward, check if we're touching solid ground through water
    if (bear.vy >= 0)
    {
        const int vSamples = 10;
        int vCollisions    = 0;

        for (int i = 0; i < vSamples; ++i)
        {
            int w = (i * bear.spriteWidth) / (vSamples - 1);
            if (w >= bear.spriteWidth)
                w = bear.spriteWidth - 1;

            if (map.isSolidAtWorld(bear.x + w, bear.y + bear.spriteHeight, 1.0f) ||
                map.isCollisionDownOnlyAtWorld(bear.x + w, bear.y + bear.spriteHeight))
            {
                vCollisions++;
            }
        }

        if (vCollisions >= vSamples * 0.2f)
        {
            bear.onGround = true;
        }
        else
        {
            bear.onGround = false;
        }
    }
}

void SwimmingMovementState::updateSwimmingVelocity(PolarBear& bear, float dt)
{
    // If resting at bottom, lock all velocity
    bool restingInWater = bear.onGround && !bear.swimPressed;

    if (restingInWater)
    {
        bear.vy = 0.0f;
        bear.vx = 0.0f;
    }
    else if (bear.swimPressed)
    {
        bear.vy = -bear.swimUpSpeed;
        bear.vx = bear.moveIntent * bear.swimRunSpeed;
    }
    else
    {
        bear.vy = bear.swimSinkSpeed;
        bear.vx = bear.moveIntent * bear.swimRunSpeed;
    }
}

void SwimmingMovementState::updatePhysics(PolarBear& bear, float dt, const TileMap& map)
{
    // No gravity in water - use swimming physics instead
    detectWaterGround(bear, map);
    updateSwimmingVelocity(bear, dt);
    
    // Horizontal movement with collision
    bear.x += bear.vx * dt;

    const int samples = 10;
    int collisions    = 0;
    for (int i = 0; i < samples; ++i)
    {
        int h = (i * bear.spriteHeight) / (samples - 1);
        if (h >= bear.spriteHeight)
            h = bear.spriteHeight - 1;

        if (bear.vx > 0)
        {
            if (map.isSolidAtWorld(bear.x + bear.spriteWidth, bear.y + h))
                collisions++;
        }
        else if (bear.vx < 0)
        {
            if (map.isSolidAtWorld(bear.x, bear.y + h))
                collisions++;
        }
    }

    if (collisions >= samples * 0.3f)
    {
        if (bear.vx > 0)
        {
            bear.x  = (static_cast<int>(bear.x + bear.spriteWidth) / map.tileSize) * map.tileSize - bear.spriteWidth;
            bear.vx = 0;
        }
        else if (bear.vx < 0)
        {
            bear.x  = (static_cast<int>(bear.x) / map.tileSize + 1) * map.tileSize;
            bear.vx = 0;
        }
    }

    // Vertical movement
    bool restingInWater = bear.onGround && !bear.swimPressed;
    if (!restingInWater)
    {
        bear.y += bear.vy * dt;
    }
}

void SwimmingMovementState::updateAnimation(PolarBear& bear, float dt)
{
    // When resting on the bottom, stay idle (no swim animation)
    if (bear.onGround)
    {
        bear.frame      = 0;
        bear.frameTimer = 0.0f;
    }
    else
    {
        bear.frameTimer += dt;
        if (bear.frameTimer >= bear.swimFrameTime)
        {
            bear.frameTimer = 0.0f;
            bear.frame      = (bear.frame + 1) % std::max(1, bear.waterSwimFrames);
        }
    }
}
