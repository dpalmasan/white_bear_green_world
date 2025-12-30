// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// ClimbingMovementState.cpp

#include "ClimbingMovementState.h"

#include <cmath>

#include "../PolarBear.h"
#include "../../systems/TileMap.h"

void ClimbingMovementState::onEnter(PolarBear& bear)
{
    // Lock horizontal movement while climbing
    bear.vx = 0.0f;
}

void ClimbingMovementState::updateClimbVelocity(PolarBear& bear)
{
    bear.vy = bear.climbIntent * bear.climbSpeed;
    bear.vx = 0.0f;
}

void ClimbingMovementState::handleLedgeMount(PolarBear& bear, float dt, const TileMap& map)
{
    // When climbing upward, check if we've reached the top of climbable surface
    if (bear.climbIntent < 0.0f)
    {
        float midY      = bear.y + bear.spriteHeight / 2.0f;
        float headY     = bear.y + 1.0f;
        float leftX     = bear.x - 1.0f;
        float rightX    = bear.x + bear.spriteWidth + 1.0f;
        float sideXMid  = bear.climbOnRightWall ? rightX : leftX;
        float sideXHead = sideXMid;

        bool midAdj  = map.isClimbableAtWorld(sideXMid, midY);
        bool headAdj = map.isClimbableAtWorld(sideXHead, headY);

        // If adjacent at mid but not at head, we reached the top
        if (midAdj && !headAdj)
        {
            int tileX = static_cast<int>(sideXMid) / map.tileSize;
            int tileY = static_cast<int>(midY) / map.tileSize;

            float topOfTile = tileY * map.tileSize;
            bear.y          = topOfTile - bear.spriteHeight - 3.0f;

            if (bear.climbOnRightWall)
            {
                bear.x = tileX * map.tileSize;
            }
            else
            {
                bear.x = (tileX + 1) * map.tileSize - bear.spriteWidth;
            }

            bear.isClimbing  = false;
            bear.climbIntent = 0.0f;
            bear.vy          = 0.0f;
            bear.vx          = 0.0f;
            bear.onGround    = false;

            bear.ledgeMounting   = true;
            bear.ledgeMountTimer = bear.ledgeMountDuration;
        }
    }
}

void ClimbingMovementState::updatePhysics(PolarBear& bear, float dt, const TileMap& map)
{
    // No gravity while climbing
    updateClimbVelocity(bear);

    // Update position
    bear.y += bear.vy * dt;

    // Check for ledge mount
    handleLedgeMount(bear, dt, map);
}

void ClimbingMovementState::updateAnimation(PolarBear& bear, float dt)
{
    // Animate climb when moving vertically; otherwise show first frame
    if (std::abs(bear.climbIntent) > 0.0f)
    {
        bear.frameTimer += dt;
        if (bear.frameTimer >= bear.climbFrameTime)
        {
            bear.frameTimer = 0.0f;
            bear.frame      = (bear.frame + 1) % std::max(1, bear.climbFrames);
        }
    }
    else
    {
        bear.frame = 0;
    }
}
