// Copyright 2025 Polar Bear Green World
// SwimmingComponent implementation

#include "SwimmingComponent.h"
#include "../PolarBear.h"
#include "../../TileMap.h"

#include <algorithm>

void SwimmingComponent::update(PolarBear& bear, float dt, const TileMap& map)
{
    // Only handle swimming if water armor is equipped
    if (!bear.isWaterEquipped()) {
        return;
    }

    // Detect water coverage
    int waterTiles = bear.waterCoverageCount(map);
    bool inWater = (waterTiles >= 4);
    
    // Update swimming state
    bool wasSwimming = bear.swimming;
    bear.setSwimmingState(inWater, bear.swimPressed);
    
    // Handle swimming physics and animation
    if (bear.swimming) {
        detectWaterGround(bear, map);
        updateSwimmingVelocity(bear, dt);
        handleSwimmingPhysics(bear, dt, map);
        updateSwimmingAnimation(bear, dt);
    }
    // Handle exit from water
    else if (wasSwimming && bear.justExitedWater) {
        if (!bear.onGround) {
            bear.vy = -336.0f;
            bear.onGround = false;
        }
        bear.justExitedWater = false;
    }
}

void SwimmingComponent::detectWaterGround(PolarBear& bear, const TileMap& map)
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

void SwimmingComponent::updateSwimmingVelocity(PolarBear& bear, float dt)
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

void SwimmingComponent::handleSwimmingPhysics(PolarBear& bear, float dt, const TileMap& map)
{
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

void SwimmingComponent::updateSwimmingAnimation(PolarBear& bear, float dt)
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
