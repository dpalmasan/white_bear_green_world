// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// SnowRobotBoss.cpp
// Implementation of snow robot boss behavior.

#include "SnowRobotBoss.h"

#include <SDL2/SDL_image.h>

#include <cmath>
#include <cstdlib>
#include <iostream>

#include "../Explosion.h"
#include "../TileMap.h"
#include "../core/Camera.h"
#include "Fireball.h"
#include "PolarBear.h"

SnowRobotBoss::SnowRobotBoss() {}

SnowRobotBoss::~SnowRobotBoss()
{
    if (introTexture)
        SDL_DestroyTexture(introTexture);
    if (attackTexture)
        SDL_DestroyTexture(attackTexture);
    if (dashTexture)
        SDL_DestroyTexture(dashTexture);
    if (vulnerableTexture)
        SDL_DestroyTexture(vulnerableTexture);
    // Don't free shared resources (fireballTexture, explosionSound, laserSound, jetSound)
}

void SnowRobotBoss::loadAssets(SDL_Renderer* renderer, const std::string& assetPath)
{
    const std::string bossPath = assetPath + "images/bosses/snow_robot/";
    const std::string attackPath = assetPath + "images/attacks/";

    introTexture = IMG_LoadTexture(renderer, (bossPath + "boss-robot-deactivated.png").c_str());
    if (!introTexture)
        std::cerr << "Failed to load boss-robot-deactivated.png: " << IMG_GetError() << "\n";

    attackTexture = IMG_LoadTexture(renderer, (bossPath + "boss-robot-attack.png").c_str());
    if (!attackTexture)
        std::cerr << "Failed to load boss-robot-attack.png: " << IMG_GetError() << "\n";

    dashTexture = IMG_LoadTexture(renderer, (bossPath + "boss-robot-dash.png").c_str());
    if (!dashTexture)
        std::cerr << "Failed to load boss-robot-dash.png: " << IMG_GetError() << "\n";

    vulnerableTexture =
        IMG_LoadTexture(renderer, (bossPath + "boss-robot-vulnerable.png").c_str());
    if (!vulnerableTexture)
        std::cerr << "Failed to load boss-robot-vulnerable.png: " << IMG_GetError() << "\n";

    fireballTexture = IMG_LoadTexture(renderer, (attackPath + "robo-cannon.png").c_str());
    if (!fireballTexture)
        std::cerr << "Failed to load robo-cannon.png: " << IMG_GetError() << "\n";

    explosionSound = Mix_LoadWAV((assetPath + "sfx/explosion.wav").c_str());
    if (!explosionSound)
        std::cerr << "Failed to load explosion.wav: " << Mix_GetError() << "\n";

    laserSound = Mix_LoadWAV((assetPath + "sfx/laser.wav").c_str());
    if (!laserSound)
        std::cerr << "Failed to load laser.wav: " << Mix_GetError() << "\n";

    metalClashSound = Mix_LoadWAV((assetPath + "sfx/metal_clash.wav").c_str());
    if (!metalClashSound)
        std::cerr << "Failed to load metal_clash.wav: " << Mix_GetError() << "\n";

    // Jet propulsion sound
    jetSound = Mix_LoadWAV((assetPath + "sfx/jet_propulsion.wav").c_str());
    // Don't error - this sound is optional
}

void SnowRobotBoss::setPosition(float x, float y)
{
    worldX = x;
    worldY = y;
}

void SnowRobotBoss::startIntro()
{
    introActive         = true;
    introFrame          = 0;
    introTimer          = 0.0f;
    introLoopsRemaining = 3;  // Loop frames 8-11 three times
    musicStartRequested = true;
}

void SnowRobotBoss::updateIntro(float dt)
{
    if (!introActive)
        return;

    introTimer += dt;
    while (introTimer >= introFrameTime)
    {
        introTimer -= introFrameTime;
        introFrame++;
        if (introFrame >= 11)  // Total 11 frames (0-10)
        {
            if (introLoopsRemaining > 0)
            {
                introLoopsRemaining--;
                introFrame = 7;  // Loop back to frame 8 (index 7)
            }
            else
            {
                introActive = false;
                introDone   = true;
            }
        }
    }
}

void SnowRobotBoss::updateAI(float dt, const TileMap& map, PolarBear& player)
{
    // Clear pending projectiles at start of frame
    pendingFireballs.clear();
    pendingExplosions.clear();

    // Allow death states to run even before intro is done
    if (!introDone && state != BossDying && state != BossDisappearing && state != BossDead)
        return;

    // Store player position for rendering flip (only if still alive)
    if (alive)
        lastPlayerX = player.x;

    // Tick invulnerability timer
    if (invulnerableTimer > 0.0f)
        invulnerableTimer = std::max(0.0f, invulnerableTimer - dt);

    // Do NOT clear hitThisAttack on player attack end; immunity lasts until next vulnerable phase

    // Handle general boss lifecycle state
    switch (state)
    {
        case BossIntro:
            updateIntro(dt);
            break;
        case BossActive:
            // Only process attack phases when boss is alive
            if (alive)
            {
                switch (phase)
                {
                    case SnowPhaseIdle:
                        updateIdleState(dt);
                        break;
                    case SnowPhaseDecision:
                        updateDecisionState(dt);
                        break;
                    case SnowPhaseAttack:
                        updateAttackState(dt, player, pendingFireballs);
                        break;
                    case SnowPhaseDashPrep:
                        updateDashPrepState(dt, player);
                        break;
                    case SnowPhaseDashMove:
                        updateDashMoveState(dt, map);
                        break;
                    case SnowPhaseVulnerable:
                        updateVulnerableState(dt);
                        break;
                }
            }
            break;
        case BossDying:
            updateDyingState(dt, pendingExplosions);
            break;
        case BossDisappearing:
            updateDisappearingState(dt);
            break;
        case BossDead:
            updateDeadState(dt);
            break;
    }
}

void SnowRobotBoss::updateIdleState(float dt)
{
    // Idle uses first frame of boss-robot-attack.png
    animFrame = 0;
    decisionTimer += dt;
    if (decisionTimer >= decisionInterval)
    {
        decisionTimer = 0.0f;
        phase         = SnowPhaseDecision;
    }
}

void SnowRobotBoss::updateDecisionState(float dt)
{
    float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    if (r <= shootProb)
    {
        bool canDash       = (bulletsShot >= 3);
        
        // When health is 1 or 2, guarantee two triple shots per phase before
        // using the weighted probabilities (Dash 1/5, Triple 3/5, Single 1/5).
        if (health == 1 || health == 2)
        {
            if (forcedTripleShotsRemaining > 0)
            {
                // Consume a forced triple-shot
                phase                 = SnowPhaseAttack;
                animFrame             = 1;
                animTimer             = 0.0f;
                attackFired           = false;
                fireTripleFireballs   = true;
                forcedTripleShotsRemaining--;
            }
            else
            {
                float choice = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);

                if (choice < 0.2f)
                {
                    // Dash attack
                    phase                 = SnowPhaseDashPrep;
                    animFrame             = 0;
                    animTimer             = 0.0f;
                    dashPrepLoops         = 0;
                    levitateOffset        = 0.0f;
                    fireTripleFireballs   = false;
                }
                else if (choice < 0.8f)
                {
                    // Triple fireball attack
                    phase                 = SnowPhaseAttack;
                    animFrame             = 1;
                    animTimer             = 0.0f;
                    attackFired           = false;
                    fireTripleFireballs   = true;
                }
                else
                {
                    // Single fireball attack
                    phase                 = SnowPhaseAttack;
                    animFrame             = 1;
                    animTimer             = 0.0f;
                    attackFired           = false;
                    fireTripleFireballs   = false;
                }
            }
        }
        else
        {
            // Normal probability logic for health > 2
            float dashProb     = canDash ? (1.0f / 3.0f) : 0.0f;
            float attackChoice = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);

            if (canDash && attackChoice < dashProb)
            {
                phase          = SnowPhaseDashPrep;
                animFrame      = 0;
                animTimer      = 0.0f;
                dashPrepLoops  = 0;
                levitateOffset = 0.0f;
            }
            else
            {
                phase       = SnowPhaseAttack;
                animFrame   = 1;
                animTimer   = 0.0f;
                attackFired = false;
            }
        }
    }
    else
    {
        phase = SnowPhaseIdle;
    }

    // Degrade probability only when health > 2 (don't degrade at low health)
    if (health > 2)
    {
        if (shootProb > 0.51f)
            shootProb = 0.5f;
        else if (shootProb > 0.34f)
            shootProb = 1.0f / 3.0f;
    }
    else
    {
        // Keep decisions active at low health
        shootProb = 1.0f;
    }
}

void SnowRobotBoss::updateAttackState(float dt, PolarBear& player, std::vector<Fireball>& fireballs)
{
    animTimer += dt;
    while (animTimer >= attackFrameTime)
    {
        animTimer -= attackFrameTime;
        animFrame++;

        // Determine if we should fire triple or single fireball
        bool tripleAttack = (health <= 2 && fireTripleFireballs);
        std::vector<int> fireFrames;
        if (tripleAttack)
        {
            fireFrames = {3, 5, 7};  // Triple: fire at frames 3, 5, 7 for rapid sequence
        }
        else
        {
            fireFrames = {5};  // Single: fire at frame 5
        }

        // Check if we should fire this frame
        bool shouldFire = false;
        for (int fireFrame : fireFrames)
        {
            if (animFrame == fireFrame && !attackFired)
            {
                shouldFire = true;
                break;
            }
        }

        if (shouldFire)
        {
            Fireball fb;
            fb.texture = fireballTexture;
            fb.frames  = 5;
            // Scale size based on health: 0.5 base, up to 1.2 at low health
            float healthRatio = static_cast<float>(health) / 5.0f;
            fb.renderScale    = 0.5f + (1.0f - healthRatio) * 0.7f;
            fb.fromBoss       = true;
            fb.initFromTexture();

            float bcx = worldX + 32.0f;
            float bcy = worldY + 32.0f;
            float px  = player.x + player.spriteWidth * 0.5f;
            float py  = player.y + player.spriteHeight * 0.5f;
            float dx  = px - bcx;
            float dy  = py - bcy;
            float len = std::sqrt(dx * dx + dy * dy);
            if (len < 1e-3f)
                len = 1.0f;
            dx /= len;
            dy /= len;

            // Scale speed based on health: 400 base, up to 600 at low health
            float speed = 400.0f + (1.0f - healthRatio) * 200.0f;
            fb.vx       = dx * speed;
            fb.vy       = dy * speed;
            fb.x        = bcx - fb.width * 0.5f;
            fb.y        = bcy - fb.height * 0.5f;
            fireballs.push_back(fb);
            bulletsShot++;
            if (laserSound)
                Mix_PlayChannel(-1, laserSound, 0);
        }

        // Mark as fired only after all fireballs are shot
        if (tripleAttack && animFrame > 7)
        {
            attackFired = true;
        }
        else if (!tripleAttack && animFrame > 5)
        {
            attackFired = true;
        }

        if (animFrame > 9)
        {
            phase       = SnowPhaseIdle;
            animFrame   = 0;
            attackFired = false;
            fireTripleFireballs = false;
            break;
        }
    }
}

void SnowRobotBoss::updateDashPrepState(float dt, PolarBear& player)
{
    if (animFrame < 5)
    {
        float progress = static_cast<float>(animFrame + 1) / 5.0f;
        levitateOffset = -24.0f * progress;
    }
    else
    {
        levitateOffset = -24.0f;
    }

    animTimer += dt;
    while (animTimer >= dashFrameTime)
    {
        animTimer -= dashFrameTime;
        animFrame++;

        if (animFrame >= 5)
        {
            if (dashPrepLoops < 3)
            {
                dashPrepLoops++;
                animFrame = 3;
            }
            else
            {
                phase        = SnowPhaseDashMove;
                animFrame    = 5;
                animTimer    = 0.0f;
                dashDistance = 0.0f;
                dashTarget   = 200.0f;  // 200 pixels

                float bearCX = player.x + player.spriteWidth * 0.5f;
                float bossCX = worldX + 32.0f;
                float dir    = (bearCX > bossCX) ? 1.0f : -1.0f;
                // When health is 2, dash faster (600 instead of 400)
                float dashSpeed = (health == 2) ? 600.0f : 400.0f;
                dashVX       = dir * dashSpeed;
                if (jetSound)
                    Mix_PlayChannel(-1, jetSound, 0);
                break;
            }
        }
    }
}

void SnowRobotBoss::updateDashMoveState(float dt, const TileMap& map)
{
    float deltaX = dashVX * dt;
    worldX += deltaX;
    dashDistance += std::abs(deltaX);

    animTimer += dt;
    while (animTimer >= dashFrameTime)
    {
        animTimer -= dashFrameTime;
        if (animFrame < 15)
            animFrame++;
    }

    if (dashDistance >= dashTarget || animFrame >= 15)
    {
        phase           = SnowPhaseVulnerable;
        animFrame       = 0;
        animTimer       = 0.0f;
        levitateOffset  = 0.0f;
        dashVX          = 0.0f;
        vulnerableLoops = 0;
        hitThisAttack   = false;  // allow one hit in this vulnerable phase
    }
}

void SnowRobotBoss::updateVulnerableState(float dt)
{
    animTimer += dt;
    while (animTimer >= vulnerableFrameTime)
    {
        animTimer -= vulnerableFrameTime;
        animFrame++;
        // Sequence: 1-9 once (0-8), then 10-12 x3 loops (9-11), then 13-16 (12-15)
        if (animFrame <= 8)
        {
            // Initial run 0-8
        }
        else if (animFrame > 11)
        {
            if (vulnerableLoops < 3)
            {
                // Loop 9-11 three times
                vulnerableLoops++;
                animFrame = 9;
            }
            else if (animFrame < 12)
            {
                // Start final sequence explicitly at 12 after loops complete
                animFrame = 12;
            }
        }

        if (animFrame >= 16)
        {
            phase     = SnowPhaseIdle;
            animFrame = 0;  // Reset to first attack frame on idle
            break;
        }
    }
}

void SnowRobotBoss::updateDyingState(float dt, std::vector<Explosion>& explosions)
{
    // BossDying: Explosion animation with music stopped and inputs disabled
    if (deathStage == 1)
    {
        deathTimer += dt;
        while (deathTimer >= deathFrameTime)
        {
            deathTimer -= deathFrameTime;
            animFrame++;

            if (animFrame < 8)
                animFrame = 8;
            if (animFrame > 11)
            {
                animFrame = 8;
                deathLoops++;
                deathFrameTime *= 0.85f;

                for (int i = 0; i < 3; i++)
                {
                    Explosion ex;
                    float offsetX = (std::rand() % 64) - 32;
                    float offsetY = (std::rand() % 64) - 32;
                    ex.x          = worldX + 32.0f + offsetX;
                    ex.y          = worldY + 32.0f + offsetY;
                    ex.timer      = 0.0f;
                    explosions.push_back(ex);
                }
                if (explosionSound)
                    Mix_PlayChannel(-1, explosionSound, 0);

                if (deathLoops >= 10)
                {
                    // Transition to disappearing state
                    state     = BossDisappearing;
                    fadeTimer = 0.0f;
                    fadeAlpha = 0;
                }
            }
        }
    }
}

void SnowRobotBoss::updateDisappearingState(float dt)
{
    // BossDisappearing: Fade to black and hide boss sprite
    fadeTimer += dt;
    fadeAlpha = static_cast<int>((fadeTimer / fadeOutDuration) * 255.0f);

    if (fadeAlpha >= 255)
    {
        fadeAlpha = 255;
        // Transition to dead state
        state     = BossDead;
        fadeTimer = 0.0f;
        alive     = false;  // Mark boss as dead (removes sprite)
        // Keep inputs disabled until Game signals unlock
    }
}

void SnowRobotBoss::updateDeadState(float dt)
{
    // BossDead: Fade back in and re-enable inputs
    fadeTimer += dt;
    fadeAlpha = 255 - static_cast<int>((fadeTimer / fadeInDuration) * 255.0f);

    if (fadeAlpha <= 0)
    {
        fadeAlpha      = 0;
        inputsDisabled = false;  // Safety: re-enable inputs when fade-in completes
        // Stay in BossDead state; do not return to idle or allow further attacks
        deathStage = 0;
    }
}

void SnowRobotBoss::takeDamage(int amount)
{
    // Only allow damage when vulnerable (in vulnerable phase, frame >= 9, not yet hit)
    if (state != BossActive || phase != SnowPhaseVulnerable || animFrame < 9 || hitThisAttack)
    {
        // Play metal clash sound whenever damage is rejected
        if (metalClashSound)
            Mix_PlayChannel(-1, metalClashSound, 0);
        return;
    }

    health -= amount;
    hitThisAttack = true;

    // Reset forced triple-shot count when entering low-health thresholds
    if (health == 2 || health == 1)
    {
        forcedTripleShotsRemaining = 2;
    }

    if (health <= 0)
    {
        state              = BossDying;
        animFrame          = 8;
        deathTimer         = 0.0f;
        deathLoops         = 0;
        deathFrameTime     = 0.30f;
        deathStage         = 1;
        musicStopRequested = true;
        inputsDisabled     = true;  // Disable inputs on death
    }
}

bool SnowRobotBoss::isDead() const
{
    return !alive;
}

bool SnowRobotBoss::isVulnerable() const
{
    // Consider boss vulnerable only after frame 9 (during loop frames 10-12) and if not yet hit
    return state == BossActive && phase == SnowPhaseVulnerable && animFrame >= 9 && !hitThisAttack;
}

void SnowRobotBoss::getCollisionRect(SDL_Rect& outRect) const
{
    outRect.x = static_cast<int>(worldX);
    outRect.y = static_cast<int>(worldY + levitateOffset);
    outRect.w = 64;
    outRect.h = 64;
}

void SnowRobotBoss::render(SDL_Renderer* renderer, const Camera& camera)
{
    const int frameW = 64;
    const int frameH = 64;
    int drawX        = static_cast<int>(worldX) - camera.x;
    int drawY        = static_cast<int>(worldY + levitateOffset) - camera.y;
    SDL_Rect dst{drawX, drawY, frameW, frameH};

    // Flip robot when player is to the right
    float bossCenterX     = worldX + 32.0f;
    SDL_RendererFlip flip = (lastPlayerX > bossCenterX) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    // Apply red tint based on damage; flicker slightly when immune in vulnerable
    float damageRatio = (5 - health) / 5.0f;
    Uint8 greenBlue   = static_cast<Uint8>(255 * (1.0f - damageRatio * 0.8f));
    if (phase == SnowPhaseVulnerable && hitThisAttack)
    {
        // Simple flicker effect: toggle dim each frame
        static bool dimToggle = false;
        dimToggle             = !dimToggle;
        if (dimToggle)
            greenBlue = static_cast<Uint8>(greenBlue * 0.6f);
    }

    SDL_Texture* texToUse = nullptr;
    int frameIdx          = 0;

    if (introActive && introTexture)
    {
        texToUse = introTexture;
        frameIdx = introFrame;
    }
    else if (!introDone && introTexture)
    {
        // Before intro starts, show first frame of deactivated texture
        texToUse = introTexture;
        frameIdx = 0;
    }
    else if (phase == SnowPhaseDashPrep || phase == SnowPhaseDashMove)
    {
        texToUse = dashTexture;
        frameIdx = animFrame;
    }
    else if (phase == SnowPhaseVulnerable || state == BossDying || state == BossDisappearing)
    {
        texToUse = vulnerableTexture;
        frameIdx = animFrame;
    }
    else
    {
        texToUse = attackTexture;
        frameIdx = animFrame;
    }

    if (texToUse)
    {
        SDL_Rect src{frameIdx * frameW, 0, frameW, frameH};
        SDL_SetTextureColorMod(texToUse, 255, greenBlue, greenBlue);
        SDL_RenderCopyEx(renderer, texToUse, &src, &dst, 0.0, nullptr, flip);
        SDL_SetTextureColorMod(texToUse, 255, 255, 255);
    }
}

bool SnowRobotBoss::shouldStartMusic() const
{
    return musicStartRequested;
}

bool SnowRobotBoss::shouldStopMusic() const
{
    return musicStopRequested;
}

bool SnowRobotBoss::shouldLoopMusic() const
{
    return musicLoopRequested;
}
void SnowRobotBoss::spawnProjectiles(std::vector<Fireball>& fireballs)
{
    // Transfer all pending fireballs to the Game's fireball list
    for (auto& fb : pendingFireballs)
    {
        fireballs.push_back(fb);
    }
    pendingFireballs.clear();
}

void SnowRobotBoss::spawnExplosions(std::vector<Explosion>& explosions)
{
    // Transfer all pending explosions to the Game's explosion list
    for (auto& exp : pendingExplosions)
    {
        explosions.push_back(exp);
    }
    pendingExplosions.clear();
}