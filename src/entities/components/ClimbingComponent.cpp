// Copyright 2025 Polar Bear Green World
// ClimbingComponent implementation

#include "ClimbingComponent.h"
#include "../PolarBear.h"
#include "../../TileMap.h"

#include <cmath>
#include <algorithm>

void ClimbingComponent::update(PolarBear& bear, float dt, const TileMap& map)
{
    // Only handle climbing if climbing is active
    if (!bear.isClimbing) {
        return;
    }

    // Lock horizontal movement when entering climb
    static bool firstFrame = true;
    if (firstFrame) {
        bear.vx = 0.0f;
        firstFrame = false;
    }

    handleClimbingPhysics(bear, dt, map);
    updateClimbingAnimation(bear, dt);
}

void ClimbingComponent::updateClimbVelocity(PolarBear& bear)
{
    bear.vy = bear.climbIntent * bear.climbSpeed;
    bear.vx = 0.0f;
}

void ClimbingComponent::handleLedgeMount(PolarBear& bear, float dt, const TileMap& map)
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

void ClimbingComponent::handleClimbingPhysics(PolarBear& bear, float dt, const TileMap& map)
{
    // No gravity while climbing
    updateClimbVelocity(bear);

    // Update position
    bear.y += bear.vy * dt;

    // Check for ledge mount
    handleLedgeMount(bear, dt, map);
}

void ClimbingComponent::updateClimbingAnimation(PolarBear& bear, float dt)
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
