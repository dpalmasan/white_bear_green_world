// Copyright 2025 Polar Bear Green World
// WindArmorComponent implementation

#include "WindArmorComponent.h"
#include "../PolarBear.h"
#include "../../systems/TileMap.h"

void WindArmorComponent::update(PolarBear& bear, float dt, const TileMap& map)
{
    // Only apply wind float if equipped with Wind armor
    if (bear.element != PolarBear::Element::Wind) {
        bear.inWind = false;
        return;
    }
    
    // Check if bear is in a wind tile
    float cx = bear.x + bear.spriteWidth * 0.5f;
    float cy = bear.y + bear.spriteHeight * 0.5f;
    
    bool inWindTile = map.isWindAtWorld(cx, cy);
    
    // Check if tile above is NOT wind (we're at the surface)
    // Check at the top quarter of the bear to allow half the bear to float at surface
    float checkAbove = bear.y + bear.spriteHeight * 0.25f;
    bool tileAboveIsWind = map.isWindAtWorld(cx, checkAbove);
    
    // Update wind state
    if (inWindTile && tileAboveIsWind)
    {
        // Only float if we're still inside a wind column (tile above is also wind)
        bear.inWind = true;
        
        // Only apply wind float if we're not jumping/moving upward from a jump
        if (bear.vy >= -150.0f)
        {
            const float windLiftSpeed = 120.0f;
            bear.vy = -windLiftSpeed;
        }
        bear.onGround = false;
    }
    else if (inWindTile && !tileAboveIsWind)
    {
        // We're at the top of the wind column - allow jumping but don't force float
        bear.inWind = true;
        bear.onGround = false;
    }
    else
    {
        bear.inWind = false;
    }
}

