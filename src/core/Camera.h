#pragma once

#include <cmath>
#include "GameConstants.h"

struct Camera
{
    int x = 0;
    int y = 0;

    int width  = GameConstants::Display::LOGICAL_WIDTH;
    int height = GameConstants::Display::LOGICAL_HEIGHT;

    int worldWidth  = GameConstants::Display::LOGICAL_WIDTH;
    int worldHeight = GameConstants::Display::LOGICAL_HEIGHT;

    void setWorldSize(int w, int h)
    {
        worldWidth  = w;
        worldHeight = h;
    }

    void follow(float targetX, float targetY)
    {
        // Center camera on target
        // Round instead of truncating so odd-sized sprites don't jitter at subpixel centers
        x = static_cast<int>(std::round(targetX - width / 2.0f));
        y = static_cast<int>(std::round(targetY - height / 2.0f));

        // Clamp to world bounds
        if (x < 0)
            x = 0;
        if (y < 0)
            y = 0;
        if (x > worldWidth - width)
            x = worldWidth - width;
        if (y > worldHeight - height)
            y = worldHeight - height;
    }

    // Check if an entity is visible in the camera viewport
    bool isInViewport(float entityX, float entityY, int entityWidth, int entityHeight) const
    {
        // Add small margin to ensure entities near edge are still processed
        const int margin = GameConstants::Camera::VIEWPORT_MARGIN;
        return !(entityX + entityWidth < x - margin || entityX > x + width + margin ||
                 entityY + entityHeight < y - margin || entityY > y + height + margin);
    }
};
