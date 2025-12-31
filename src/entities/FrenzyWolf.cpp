#include "FrenzyWolf.h"

#include <SDL2/SDL_image.h>

#include <algorithm>
#include <cmath>

#include "systems/TileMap.h"
#include "PolarBear.h"

void FrenzyWolf::setIdleTexture(SDL_Texture* tex)
{
    texture = tex;
    if (!texture)
        return;
    // Idle frames
    width     = GameConstants::Enemies::Wolf::IDLE_WIDTH;
    height    = GameConstants::Enemies::Wolf::HEIGHT;
    numFrames = GameConstants::Enemies::Wolf::IDLE_FRAMES;
    frameTime = GameConstants::Enemies::Wolf::FRAME_TIME;
}

void FrenzyWolf::setRunTexture(SDL_Texture* tex, float frameTimeSeconds)
{
    runTexture    = tex;
    runFrameTime  = frameTimeSeconds;
    runFrame      = 0;
    runFrameTimer = 0.0f;
    if (!runTexture)
        return;
    // Run/attack frames
    runFrameWidth = GameConstants::Enemies::Wolf::RUN_WIDTH;
    runFrames     = GameConstants::Enemies::Wolf::RUN_FRAMES;
}

void FrenzyWolf::tickAI(float dt, const TileMap& map, const PolarBear& bear)
{
    if (!alive)
        return;

    // Vision field: wolf can only see player within configurable distance
    const float VISION_DISTANCE = GameConstants::Enemies::Wolf::VISION_DISTANCE * GameConstants::Tile::DEFAULT_SIZE;
    float wolfCenterX           = x + width * 0.5f;
    float wolfCenterY           = y + height * 0.5f;
    float playerCenterX         = bear.x + bear.spriteWidth * 0.5f;
    float playerCenterY         = bear.y + bear.spriteHeight * 0.5f;

    // Calculate distance to player
    float distanceToPlayer =
        std::sqrt((playerCenterX - wolfCenterX) * (playerCenterX - wolfCenterX) +
                  (playerCenterY - wolfCenterY) * (playerCenterY - wolfCenterY));

    // If player is out of vision range, wolf idles (no movement)
    if (distanceToPlayer > VISION_DISTANCE)
    {
        // Decelerate to idle
        if (vx > 0.0f)
            vx = std::max(0.0f, vx - accel * GameConstants::Enemies::Wolf::DECEL_MULTIPLIER * dt);
        else if (vx < 0.0f)
            vx = std::min(0.0f, vx + accel * GameConstants::Enemies::Wolf::DECEL_MULTIPLIER * dt);
        running = false;
        return;
    }

    // Target position: chase distance behind the player
    const float BEHIND_DISTANCE = GameConstants::Enemies::Wolf::CHASE_BEHIND_DISTANCE * GameConstants::Tile::DEFAULT_SIZE;
    float targetX = playerCenterX - BEHIND_DISTANCE;  // target behind (to the left by default)

    // Adjust target based on wolf position relative to player
    float dx = targetX - wolfCenterX;  // distance to target position

    float dy       = std::fabs(playerCenterY - wolfCenterY);
    flipHorizontal = dx > 0;  // flip when target is to the right

    // Only chase when roughly on the same vertical band
    const float verticalBand = GameConstants::Enemies::Wolf::VERTICAL_BAND;
    float desiredDir         = 0.0f;
    if (dy <= verticalBand)
    {
        if (dx > GameConstants::Enemies::Wolf::MIN_MOVEMENT_THRESHOLD)
            desiredDir = 1.0f;
        else if (dx < -GameConstants::Enemies::Wolf::MIN_MOVEMENT_THRESHOLD)
            desiredDir = -1.0f;
    }

    float desiredSpeed = targetSpeed * desiredDir;

    // Accelerate toward desired speed, decelerate when no direction.
    if (desiredDir != 0.0f)
    {
        if (vx < desiredSpeed)
            vx = std::min(desiredSpeed, vx + accel * dt);
        else if (vx > desiredSpeed)
            vx = std::max(desiredSpeed, vx - accel * dt);
    }
    else
    {
        if (vx > 0.0f)
            vx = std::max(0.0f, vx - accel * GameConstants::Enemies::Wolf::DECEL_MULTIPLIER * dt);
        else if (vx < 0.0f)
            vx = std::min(0.0f, vx + accel * GameConstants::Enemies::Wolf::DECEL_MULTIPLIER * dt);
    }

    running = std::fabs(vx) > GameConstants::Enemies::Wolf::MIN_RUNNING_SPEED;
}

void FrenzyWolf::updateBehavior(float dt, const TileMap& map)
{
    if (!alive)
        return;

    // Move horizontally with simple collision resolution
    x += vx * dt;

    const int samples = GameConstants::Collision::COLLISION_SAMPLES;
    int collisions    = 0;

    if (vx > 0.0f)
    {
        int rightX = static_cast<int>(x + width);
        for (int i = 0; i < samples; ++i)
        {
            int h = (i * height) / (samples - 1);
            if (h >= height)
                h = height - 1;
            if (map.isSolidAtWorld(rightX, y + h, 0.0f))
                collisions++;
        }
        if (collisions >= samples * GameConstants::Collision::TIGHT_COLLISION_THRESHOLD)
        {
            x  = (rightX / map.tileSize) * map.tileSize - width;
            vx = 0.0f;
        }
    }
    else if (vx < 0.0f)
    {
        int leftX = static_cast<int>(x);
        for (int i = 0; i < samples; ++i)
        {
            int h = (i * height) / (samples - 1);
            if (h >= height)
                h = height - 1;
            if (map.isSolidAtWorld(leftX, y + h, 0.0f))
                collisions++;
        }
        if (collisions >= samples * GameConstants::Collision::TIGHT_COLLISION_THRESHOLD)
        {
            x  = (leftX / map.tileSize + 1) * map.tileSize;
            vx = 0.0f;
        }
    }

    // Advance run animation when moving
    if (running && runTexture)
    {
        runFrameTimer += dt;
        if (runFrameTimer >= runFrameTime)
        {
            runFrameTimer -= runFrameTime;
            runFrame = (runFrame + 1) % runFrames;
        }
    }
    else
    {
        runFrame      = 0;
        runFrameTimer = 0.0f;
    }
}

void FrenzyWolf::render(SDL_Renderer* renderer, int camX, int camY)
{
    if (!alive)
        return;

    SDL_Texture* tex = (running && runTexture) ? runTexture : texture;
    if (!tex)
        return;

    int frameW, frameH;
    int frameIndex;
    if (running && runTexture)
    {
        frameW     = runFrameWidth;  // 38 for run
        frameH     = height;         // 25
        frameIndex = runFrame;
    }
    else
    {
        frameW     = width;   // 34 for idle
        frameH     = height;  // 25
        frameIndex = frame;
    }

    SDL_Rect src{frameIndex * frameW, 0, frameW, frameH};
    SDL_Rect dst{static_cast<int>(std::round(x)) - camX, static_cast<int>(std::round(y)) - camY, frameW, frameH};
    SDL_RendererFlip flip = flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, tex, &src, &dst, 0.0, nullptr, flip);
}
