#include "RobotEnemy.h"

#include <SDL2/SDL.h>

#include <cmath>

#include "../systems/TileMap.h"
#include "Fireball.h"
#include "PolarBear.h"

void RobotEnemy::tickAI(float dt, const TileMap& map, const PolarBear& bear,
                        std::vector<Fireball>& fireballs, SDL_Texture* fireballTexture)
{
    if (!alive)
        return;

    // Face the player
    float dx = bear.x - x;
    // Default sprite faces left; flip when the player is to the right
    flipHorizontal = dx > 0;

    // Reduce cooldown
    if (cooldownTimer > 0.0f)
        cooldownTimer -= dt;

    // If currently attacking, advance animation and fire once
    if (attacking)
    {
        attackTimer += dt;
        if (attackTimer >= attackFrameTime)
        {
            attackTimer -= attackFrameTime;
            attackFrame++;
        }

        // Fire at frame 2 (third frame) once per attack
        if (!firedThisAttack && attackFrame >= 2 && fireballTexture)
        {
            firedThisAttack = true;
            Fireball fb;
            fb.texture = fireballTexture;
            fb.initFromTexture();
            fb.x  = x + width / 2 - fb.width / 2;
            fb.y  = y + height / 2 - fb.height / 2;     // spawn at robot's center
            fb.vx = (dx >= 0 ? 1.0f : -1.0f) * 150.0f;  // slower projectile
            fb.vy = 0.0f;
            fireballs.push_back(fb);
        }

        if (attackFrame >= attackFrames)
        {
            attacking       = false;
            attackFrame     = 0;
            attackTimer     = 0.0f;
            firedThisAttack = false;
            cooldownTimer   = cooldown;
        }
        return;
    }

    // Detect player within horizontal range (6 tiles) and modest vertical band
    const float range = 8.0f * map.tileSize;  // increased detection range
    float dy          = std::fabs((bear.y + bear.spriteHeight / 2.0f) - (y + height / 2.0f));
    if (std::fabs(dx) <= range && dy <= 48.0f && cooldownTimer <= 0.0f)
    {
        attacking       = true;
        attackFrame     = 0;
        attackTimer     = 0.0f;
        firedThisAttack = false;
    }
}

void RobotEnemy::render(SDL_Renderer* renderer, int camX, int camY)
{
    if (!alive)
        return;

    SDL_Texture* currentTexture = texture;
    int currentFrame            = frame;
    int frameW, frameH;

    if (attacking && attackTexture)
    {
        currentTexture = attackTexture;
        currentFrame   = attackFrame;
        frameW         = attackFrameW;  // 37 for attack
        frameH         = attackFrameH;  // 44 for attack
    }
    else
    {
        frameW = width;   // 24 for idle
        frameH = height;  // 46 for idle
    }

    if (!currentTexture)
        return;

    SDL_Rect src{currentFrame * frameW, 0, frameW, frameH};
    SDL_Rect dst{static_cast<int>(std::round(x)) - camX, static_cast<int>(std::round(y)) - camY, frameW, frameH};
    SDL_RendererFlip flip = flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, currentTexture, &src, &dst, 0.0, nullptr, flip);
}
