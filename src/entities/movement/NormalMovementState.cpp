// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// NormalMovementState.cpp

#include "NormalMovementState.h"

#include <algorithm>

#include "entities/PolarBear.h"
#include "systems/TileMap.h"

void NormalMovementState::onEnter(PolarBear& bear)
{
    // Reset any velocity when entering normal state
}

void NormalMovementState::applyGravity(PolarBear& bear, float dt)
{
    const float GRAVITY = 1000.0f;
    
    // Wind glide: reduce gravity when in wind form, descending, and holding jump
    float gravityMultiplier = 1.0f;
    if (bear.element == PolarBear::Element::Wind && bear.jumpHeld && bear.vy > 0.0f)
    {
        gravityMultiplier = 0.3f;  // 30% gravity for strong glide effect
    }
    
    bear.vy += GRAVITY * gravityMultiplier * dt;
}

void NormalMovementState::applyHorizontalMovement(PolarBear& bear, float dt, const TileMap& map)
{
    // Detect slipperiness at the feet
    float footY        = bear.y + bear.spriteHeight;
    float footCenter   = bear.x + bear.spriteWidth / 2.0f;
    float footLeft     = bear.x + 2.0f;
    float footRight    = bear.x + bear.spriteWidth - 2.0f;
    bool onSlippery    = bear.onGround && (map.isSlipperyAtWorld(footCenter, footY) ||
                                        map.isSlipperyAtWorld(footLeft, footY) ||
                                        map.isSlipperyAtWorld(footRight, footY));
    const float runspd = 75.0f;

    if (onSlippery)
    {
        const float slipMaxSpeed = 165.0f;
        const float slipAccel    = 260.0f;
        const float slipFriction = 40.0f;

        float desired = bear.moveIntent * slipMaxSpeed;
        if (bear.moveIntent != 0.0f)
        {
            float delta = slipAccel * dt;
            if (bear.vx < desired)
                bear.vx = std::min(bear.vx + delta, desired);
            else if (bear.vx > desired)
                bear.vx = std::max(bear.vx - delta, desired);
        }
        else
        {
            float delta = slipFriction * dt;
            if (bear.vx > 0.0f)
                bear.vx = std::max(0.0f, bear.vx - delta);
            else if (bear.vx < 0.0f)
                bear.vx = std::min(0.0f, bear.vx + delta);
        }
    }
    else
    {
        bear.vx = bear.moveIntent * runspd;
    }
}

void NormalMovementState::updatePhysics(PolarBear& bear, float dt, const TileMap& map)
{
    applyGravity(bear, dt);
    applyHorizontalMovement(bear, dt, map);
    
    // Update position with collision detection
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
    bear.y += bear.vy * dt;

    bear.onGround = false;

    const int vSamples = 10;
    int vCollisions    = 0;

    for (int i = 0; i < vSamples; ++i)
    {
        int w = (i * bear.spriteWidth) / (vSamples - 1);
        if (w >= bear.spriteWidth)
            w = bear.spriteWidth - 1;

        if (bear.vy > 0)
        {
            if (map.isSolidAtWorld(bear.x + w, bear.y + bear.spriteHeight, bear.vy) ||
                map.isCollisionDownOnlyAtWorld(bear.x + w, bear.y + bear.spriteHeight))
            {
                vCollisions++;
            }
        }
        else if (bear.vy < 0)
        {
            if (map.isSolidAtWorld(bear.x + w, bear.y))
            {
                vCollisions++;
            }
        }
    }

    if (vCollisions >= vSamples * 0.2f)
    {
        if (bear.vy > 0)
        {
            bear.y  = (static_cast<int>(bear.y + bear.spriteHeight) / map.tileSize) * map.tileSize - bear.spriteHeight;
            bear.vy = 0;
            bear.onGround = true;
            bear.isKnockedBack = false;
        }
        else if (bear.vy < 0)
        {
            bear.y  = (static_cast<int>(bear.y) / map.tileSize + 1) * map.tileSize;
            bear.vy = 0;
        }
    }
}

void NormalMovementState::updateAnimation(PolarBear& bear, float dt)
{
    if (bear.vx != 0)
    {
        bear.frameTimer += dt;
        if (bear.frameTimer >= bear.frameTime)
        {
            bear.frameTimer = 0.0f;
            bear.frame      = (bear.frame + 1) % bear.numFrames;
        }
    }
    else
    {
        bear.frame = 0;  // idle frame
    }
}
