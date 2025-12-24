// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// Attack.cpp
// Implementation of attack types.

#include "Attack.h"

#include <SDL2/SDL.h>

#include <cmath>

#include "../entities/PolarBear.h"

// ============================================================================
// SlashAttack Implementation
// ============================================================================

SlashAttack::SlashAttack(PolarBear* bear, SDL_Texture* slashTexture)
    : bear(bear), slashTexture(slashTexture)
{
    if (slashTexture)
    {
        // Query texture to get dimensions
        int w, h;
        SDL_QueryTexture(slashTexture, NULL, NULL, &w, &h);
        if (w > 0 && h > 0)
        {
            slashWidth  = w;
            slashHeight = h;
        }
    }
}

void SlashAttack::update(float dt)
{
    // Update attack sprite animation
    if (!attackFinished)
    {
        attackTimer += dt;
        if (attackTimer >= attackDuration)
        {
            attackTimer = 0.0f;
            attackFrame++;
            if (attackFrame >= attackFrames)
            {
                attackFinished = true;
            }
        }
    }

    // Update slash timer (delay before it starts, then cap at duration)
    float totalSlashTime = slashTimer + dt;
    if (totalSlashTime >= slashStartDelay && slashTimer < slashDuration + slashStartDelay)
    {
        slashTimer += dt;
    }
    else if (totalSlashTime < slashStartDelay)
    {
        slashTimer += dt;
    }
}

void SlashAttack::render(SDL_Renderer* renderer, int camX, int camY)
{
    if (!slashTexture || !bear)
        return;

    // Don't render slash until the delay has passed
    if (slashTimer < slashStartDelay)
        return;

    // Calculate slash progress within its active window
    float slashProgress = slashTimer - slashStartDelay;
    if (slashProgress >= slashDuration)
        return;

    // Progress through slash animation [0.0 to 1.0]
    float progress = 0.0f;
    if (slashDuration > 0.0f)
        progress = slashProgress / slashDuration;
    if (progress < 0.0f)
        progress = 0.0f;
    if (progress > 1.0f)
        progress = 1.0f;

    // Grow from small to full size with ease-out
    const float minScale = 0.6f;
    float eased          = 1.0f - (1.0f - progress) * (1.0f - progress);
    float overallScale   = minScale + (1.0f - minScale) * eased;

    // Screen-space positioning using bear's last drawn position
    int cy          = static_cast<int>(bear->y - camY) + bear->spriteHeight / 2;
    const int inset = 6;
    int frontX      = bear->lastDrawX + (bear->facingRight ? bear->lastDrawW - inset : inset);

    SDL_Rect dst;
    dst.w = int(slashWidth * overallScale);
    dst.h = int(slashHeight * overallScale);
    if (bear->facingRight)
        dst.x = frontX;
    else
        dst.x = frontX - dst.w;
    dst.y = cy - dst.h / 2;

    // Fade-in alpha: keep more transparency at peak
    SDL_SetTextureBlendMode(slashTexture, SDL_BLENDMODE_BLEND);
    int alpha = 120 + static_cast<int>(80 * progress);
    if (alpha > 200)
        alpha = 200;
    SDL_SetTextureAlphaMod(slashTexture, alpha);
    SDL_RenderCopyEx(renderer, slashTexture, NULL, &dst, 0.0, NULL,
                     bear->facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
    SDL_SetTextureAlphaMod(slashTexture, 255);
}

bool SlashAttack::getWorldRect(SDL_Rect& out) const
{
    if (!bear || slashTimer < slashStartDelay)
        return false;

    float slashProgress = slashTimer - slashStartDelay;
    if (slashProgress >= slashDuration)
        return false;

    float progress = 0.0f;
    if (slashDuration > 0.0f)
        progress = slashProgress / slashDuration;
    if (progress < 0.0f)
        progress = 0.0f;
    if (progress > 1.0f)
        progress = 1.0f;

    // Grow from small to full size with ease-out
    const float minScale = 0.6f;
    float eased          = 1.0f - (1.0f - progress) * (1.0f - progress);
    float overallScale   = minScale + (1.0f - minScale) * eased;

    int w = int(slashWidth * overallScale);
    int h = int(slashHeight * overallScale);

    // Compute front X in world coords
    const int inset = 6;
    int bearWorldX  = static_cast<int>(bear->x);
    int bearDrawW   = bear->spriteWidth;  // Attack sprite width is same as base
    int cy_world    = static_cast<int>(bear->y) + bear->spriteHeight / 2;

    if (bear->facingRight)
        out.x = bearWorldX + bearDrawW - inset;
    else
        out.x = bearWorldX - w + inset;

    out.w = w;
    out.h = h;
    out.y = cy_world - h / 2;
    return true;
}

bool SlashAttack::isActive() const
{
    // Attack is active if slash is still visible OR attack animation hasn't finished
    float slashEndTime = slashStartDelay + slashDuration;
    return slashTimer < slashEndTime || !attackFinished;
}
