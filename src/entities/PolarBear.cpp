#include "PolarBear.h"

#include <SDL2/SDL_image.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <typeinfo>

#include "../actions/Attack.h"
#include "movement/NormalMovementState.h"
#include "movement/SwimmingMovementState.h"
#include "movement/ClimbingMovementState.h"


// Load the sprite sheet from a PNG file
void PolarBear::loadTexture(SDL_Renderer* renderer, const std::string& filename)
{
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface)
    {
        std::cerr << "Failed to load " << filename << ": " << IMG_GetError() << "\n";
        return;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture)
    {
        std::cerr << "Failed to create texture: " << SDL_GetError() << "\n";
    }
}

bool PolarBear::getAttackWorldRect(SDL_Rect& out) const
{
    if (!currentAttack)
        return false;
    return currentAttack->getWorldRect(out);
}

// Load jump animation texture
void PolarBear::loadJumpTexture(SDL_Renderer* renderer, const std::string& filename)
{
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface)
    {
        std::cerr << "Failed to load " << filename << ": " << IMG_GetError() << "\n";
        return;
    }

    jumpTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!jumpTexture)
    {
        std::cerr << "Failed to create jump texture: " << SDL_GetError() << "\n";
    }
}

// Load attack animation texture
void PolarBear::loadAttackTexture(SDL_Renderer* renderer, const std::string& filename)
{
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface)
    {
        std::cerr << "Failed to load " << filename << ": " << IMG_GetError() << "\n";
        return;
    }

    attackTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!attackTexture)
    {
        std::cerr << "Failed to create attack texture: " << SDL_GetError() << "\n";
    }
}

// Load slash texture (textured effect)
void PolarBear::loadSlashTexture(SDL_Renderer* renderer, const std::string& filename)
{
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface)
    {
        std::cerr << "Failed to load " << filename << ": " << IMG_GetError() << "\n";
        return;
    }

    slashTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!slashTexture)
    {
        std::cerr << "Failed to create slash texture: " << SDL_GetError() << "\n";
    }
}

// Load climb animation texture
void PolarBear::loadClimbTexture(SDL_Renderer* renderer, const std::string& filename)
{
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface)
    {
        std::cerr << "Failed to load " << filename << ": " << IMG_GetError() << "\n";
        return;
    }

    climbTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!climbTexture)
    {
        std::cerr << "Failed to create climb texture: " << SDL_GetError() << "\n";
    }
    else
    {
        // Derive frame count using explicit frame width (37) when possible
        int texW = 0, texH = 0;
        SDL_QueryTexture(climbTexture, nullptr, nullptr, &texW, &texH);
        // Use specified frame size; auto-detect frames if width is a multiple of 37
        climbWidth  = 37;
        climbHeight = 47;
        if (texW > 0 && texW % climbWidth == 0)
        {
            int frames = texW / climbWidth;
            if (frames > 0)
                climbFrames = frames;
        }
        // If height differs, keep provided 47 to crop correctly
    }
}

void PolarBear::loadWaterWalkTexture(SDL_Renderer* renderer, const std::string& filename)
{
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface)
    {
        std::cerr << "Failed to load " << filename << ": " << IMG_GetError() << "\n";
        return;
    }

    waterWalkTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!waterWalkTexture)
    {
        std::cerr << "Failed to create water walk texture: " << SDL_GetError() << "\n";
    }
}

void PolarBear::loadWaterJumpTexture(SDL_Renderer* renderer, const std::string& filename)
{
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface)
    {
        std::cerr << "Failed to load " << filename << ": " << IMG_GetError() << "\n";
        return;
    }

    waterJumpTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!waterJumpTexture)
    {
        std::cerr << "Failed to create water jump texture: " << SDL_GetError() << "\n";
    }
}

void PolarBear::loadWaterSwimTexture(SDL_Renderer* renderer, const std::string& filename)
{
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface)
    {
        std::cerr << "Failed to load " << filename << ": " << IMG_GetError() << "\n";
        return;
    }

    waterSwimTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!waterSwimTexture)
    {
        std::cerr << "Failed to create water swim texture: " << SDL_GetError() << "\n";
    }
}

void PolarBear::loadWindWalkTexture(SDL_Renderer* renderer, const std::string& filename)
{
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface)
    {
        std::cerr << "Failed to load " << filename << ": " << IMG_GetError() << "\n";
        return;
    }

    windWalkTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!windWalkTexture)
    {
        std::cerr << "Failed to create wind walk texture: " << SDL_GetError() << "\n";
    }
}

void PolarBear::loadWindJumpTexture(SDL_Renderer* renderer, const std::string& filename)
{
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface)
    {
        std::cerr << "Failed to load " << filename << ": " << IMG_GetError() << "\n";
        return;
    }

    windJumpTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!windJumpTexture)
    {
        std::cerr << "Failed to create wind jump texture: " << SDL_GetError() << "\n";
    }
}

void PolarBear::setElement(Element e)
{
    // Capture baseline once (non-element frames/dimensions)
    if (baseWalkWidth == 0)
    {
        baseWalkWidth  = spriteWidth;
        baseWalkHeight = spriteHeight;
        baseNumFrames  = numFrames;
        baseJumpFrames = jumpFrames;
        // Default jump uses spriteWidth/Height; allow override later if needed
        baseJumpWidth  = spriteWidth;
        baseJumpHeight = spriteHeight;
    }

    element = e;

    if (element == Element::Water)
    {
        // Use water variants when available
        if (waterWalkTexture) {
            texture = waterWalkTexture;
            spriteHeight = waterWalkHeight;
            spriteWidth  = waterWalkWidth;
        }
        if (waterJumpTexture) {
            spriteWidth  = waterJumpWidth;
            spriteHeight = waterJumpHeight;
        }
            jumpTexture = waterJumpTexture;

        numFrames    = waterWalkFrames;
        jumpFrames   = waterJumpFrames;

        // Reset animation state
        frame      = 0;
        frameTimer = 0.0f;
    }
    else if (element == Element::Wind)
    {
        // Use wind variants when available
        if (windWalkTexture)
            texture = windWalkTexture;
        if (windJumpTexture)
            jumpTexture = windJumpTexture;

        // Set sprite box to wind frame size if available
        spriteWidth  = windJumpWidth;
        spriteHeight = windJumpHeight;
        numFrames    = windWalkFrames;
        jumpFrames   = windJumpFrames;

        // Reset animation state
        frame      = 0;
        frameTimer = 0.0f;
    }
    else
    {
        // Revert to baseline non-element dimensions/textures
        spriteWidth  = (baseWalkWidth > 0) ? baseWalkWidth : spriteWidth;
        spriteHeight = (baseWalkHeight > 0) ? baseWalkHeight : spriteHeight;
        numFrames    = (baseNumFrames > 0) ? baseNumFrames : numFrames;
        jumpFrames   = (baseJumpFrames > 0) ? baseJumpFrames : jumpFrames;
        // Keep texture/jumpTexture as originally loaded defaults
        frame      = 0;
        frameTimer = 0.0f;
    }
}

int PolarBear::waterCoverageCount(const TileMap& map) const
{
    // Sample center and four corners slightly inset; require 4/5 samples in water (~80%).
    const float inset = 2.0f;
    float left        = x + inset;
    float right       = x + spriteWidth - inset;
    float top         = y + inset;
    float bottom      = y + spriteHeight - inset;
    float cx          = x + spriteWidth * 0.5f;
    float cy          = y + spriteHeight * 0.5f;

    int waterCount = 0;
    waterCount += map.isWaterAtWorld(left, top) ? 1 : 0;
    waterCount += map.isWaterAtWorld(right, top) ? 1 : 0;
    waterCount += map.isWaterAtWorld(left, bottom) ? 1 : 0;
    waterCount += map.isWaterAtWorld(right, bottom) ? 1 : 0;
    waterCount += map.isWaterAtWorld(cx, cy) ? 1 : 0;

    return waterCount;  // up to 5 samples
}

void PolarBear::setSwimmingState(bool inWaterNow, bool swimButtonPressed)
{
    wasSwimming = swimming;
    inWater     = inWaterNow;
    swimPressed = swimButtonPressed;

    if (isWaterEquipped() && inWater)
    {
        swimming = true;
        if (!wasSwimming)
        {
            frame      = 0;
            frameTimer = 0.0f;
        }
    }
    else
    {
        if (wasSwimming)
            justExitedWater = true;
        swimming = false;
    }
}

void PolarBear::startAttack()
{
    if (!isAttacking && !currentAttack)
    {
        isAttacking   = true;
        currentAttack = std::make_unique<SlashAttack>(this, slashTexture);
    }
}

void PolarBear::onAttackRelease()
{
    if (currentAttack)
    {
        currentAttack->onRelease();
    }
}

void PolarBear::takeDamage()
{
    // Ignore damage if invulnerable
    if (isInvulnerable)
        return;

    // Store the direction the bear was facing when damaged
    damageFacingRight = facingRight;

    // Start damage animation and invulnerability
    isDamaged            = true;
    damageFrame          = jumpFrames - 1;  // Start from last frame (will count backwards)
    damageTimer          = 0.0f;
    isInvulnerable       = true;
    invulnerabilityTimer = 0.0f;
    isKnockedBack        = true;  // Disable input during knockback

    // Lose one heart on damage (not below zero)
    if (hearts > 0)
        hearts -= 1;

    // Apply knockback velocity (opposite to facing direction) and upward jump
    // Knockback speed and jump height
    const float knockbackSpeed = 150.0f;   // Knockback distance
    const float jumpPower      = -250.0f;  // Reduced jump height for knockback

    // Knockback moves in opposite direction of where bear was facing
    vx = facingRight ? -knockbackSpeed : knockbackSpeed;
    vy = jumpPower;  // Smaller jump upward
}

void PolarBear::transitionToMovementState(std::unique_ptr<MovementState> newState)
{
    if (currentMovementState)
    {
        currentMovementState->onExit(*this);
    }
    currentMovementState = std::move(newState);
    if (currentMovementState)
    {
        currentMovementState->onEnter(*this);
    }
}

void PolarBear::updateMovementState()
{
    // Determine which movement state we should be in
    
    // Climbing takes priority
    if (isClimbing && currentMovementState.get() != nullptr)
    {
        if (typeid(*currentMovementState) != typeid(ClimbingMovementState))
        {
            transitionToMovementState(std::make_unique<ClimbingMovementState>());
        }
    }
    // Then swimming
    else if (swimming && isWaterEquipped())
    {
        if (currentMovementState.get() == nullptr || 
            typeid(*currentMovementState) != typeid(SwimmingMovementState))
        {
            transitionToMovementState(std::make_unique<SwimmingMovementState>());
        }
    }
    // Finally normal movement
    else
    {
        if (currentMovementState.get() == nullptr || 
            typeid(*currentMovementState) != typeid(NormalMovementState))
        {
            transitionToMovementState(std::make_unique<NormalMovementState>());
        }
    }
}

// Render the attack effect (slash, particles, etc.)
void PolarBear::renderAttack(SDL_Renderer* renderer, int camX, int camY)
{
    if (currentAttack)
    {
        currentAttack->render(renderer, camX, camY);
    }
}

void PolarBear::update(float dt, const TileMap& map)
{
    // Update movement state machine
    updateMovementState();

    // Delegate physics and animation to current state
    if (currentMovementState)
    {
        currentMovementState->updatePhysics(*this, dt, map);
        currentMovementState->updateAnimation(*this, dt);
    }

    // --- Update ledge mount timer ---
    if (ledgeMounting)
    {
        ledgeMountTimer -= dt;
        if (ledgeMountTimer <= 0.0f)
        {
            ledgeMounting   = false;
            ledgeMountTimer = 0.0f;
        }
    }

    // --- Clamp to world bounds (map edges collide) ---
    const float worldW = static_cast<float>(map.width * map.tileSize);
    const float worldH = static_cast<float>(map.height * map.tileSize);

    if (worldW > 0.0f)
    {
        if (x < 0.0f)
        {
            x  = 0.0f;
            vx = 0.0f;
        }
        else if (x + spriteWidth > worldW)
        {
            x  = worldW - spriteWidth;
            vx = 0.0f;
        }
    }

    if (worldH > 0.0f)
    {
        if (y < 0.0f)
        {
            y  = 0.0f;
            vy = 0.0f;
        }
        else if (y > worldH)
        {
            // Bear fell off the world - kill it
            hearts = 0;
        }
    }

    // --- Attack animation ---
    if (currentAttack)
    {
        currentAttack->update(dt);
        if (!currentAttack->isActive())
        {
            isAttacking = false;
            currentAttack.reset();
        }
    }

    // --- Damage animation and invulnerability ---
    if (isDamaged)
    {
        damageTimer += dt;
        if (damageTimer >= damageDuration)
        {
            damageTimer = 0.0f;
            damageFrame--;  // Count backwards through frames
            if (damageFrame < 0)
            {
                isDamaged   = false;
                damageFrame = 0;
            }
        }
    }

    if (isInvulnerable)
    {
        invulnerabilityTimer += dt;
        if (invulnerabilityTimer >= invulnerabilityDuration)
        {
            isInvulnerable       = false;
            invulnerabilityTimer = 0.0f;
        }
    }
}

// Render the polar bear at the given camera offset
void PolarBear::render(SDL_Renderer* renderer, int camX, int camY, SDL_RendererFlip flip)
{
    // Check if we should skip rendering due to invulnerability blinking
    if (isInvulnerable)
    {
        // Blink effect: alternate visibility every 150ms (about 3.3 blinks per second)
        float blinkPeriod = 0.15f;
        float blinkPhase  = fmodf(invulnerabilityTimer, blinkPeriod * 2.0f);
        if (blinkPhase >= blinkPeriod)
        {
            // Skip rendering during this blink cycle to create the blinking effect
            return;
        }
    }

    // Determine which texture and frame to use
    const bool useSwimming = swimming && isWaterEquipped();
    SDL_Texture* currentTexture = texture;
    int currentFrame            = frame;
    int currentNumFrames        = numFrames;
    int currentWidth            = spriteWidth;
    int currentHeight           = spriteHeight;
    double rotationAngle        = 0.0;
    if (isWaterEquipped())
    {
        currentNumFrames = waterWalkFrames;
        currentWidth     = waterWalkWidth;
        currentHeight    = waterWalkHeight;
    }

    if (isWindEquipped())   
    {
        currentNumFrames = windWalkFrames;
        currentWidth     = windWalkWidth;
        currentHeight    = windWalkHeight;
    }

    if (isDamaged && jumpTexture)
    {
        // Use damage animation (reversed jump frames with counter-clockwise rotation)
        currentTexture   = jumpTexture;
        currentFrame     = damageFrame;
        currentNumFrames = jumpFrames;
        currentWidth     = spriteWidth;  // Jump frames use sprite dimensions
        currentHeight    = spriteHeight;
        // Rotate left (counter-clockwise) based on facing direction
        // If was facing right, rotate left (-angle); if facing left, rotate right (+angle)
        rotationAngle = damageFacingRight ? -25.0 : 25.0;
    }
    else if (isAttacking && attackTexture)
    {
        // Use attack sprite with frame from current attack
        currentTexture   = attackTexture;
        currentNumFrames = attackFrames;
        currentWidth     = attackWidth;
        currentHeight    = attackHeight;

        // Get frame from the attack object (cast to SlashAttack for now)
        if (currentAttack)
        {
            SlashAttack* slash = static_cast<SlashAttack*>(currentAttack.get());
            currentFrame       = slash->getAttackFrame();
        }
        else
        {
            currentFrame = 0;
        }
    }
    else if (useSwimming && waterSwimTexture)
    {
        currentTexture   = waterSwimTexture;
        currentNumFrames = waterSwimFrames;
        currentWidth     = waterSwimWidth;
        currentHeight    = waterSwimHeight;
        currentFrame     = frame;
    }
    else if (isClimbing && climbTexture)
    {
        // Use climb animation when climbing
        currentTexture   = climbTexture;
        currentNumFrames = climbFrames;
        currentWidth     = climbWidth;
        currentHeight    = climbHeight;
        // Frame already advanced in update
    }
    else if (!onGround && jumpTexture)
    {
        // Use jump animation when airborne
        currentTexture   = jumpTexture;
        currentNumFrames = jumpFrames;
        if (isWaterEquipped())
        {
            currentWidth  = waterJumpWidth;
            currentHeight = waterJumpHeight;
        }

        if (isWindEquipped())
        {
            currentWidth  = windJumpWidth;
            currentHeight = windJumpHeight;
        }

        // Map vertical velocity to frame with specific ranges:
        // Ascending (vy < 0): frames 1-3
        // Peak (vy ~= 0): frame 4
        // Descending (vy > 0): frames 5-7
        float maxUpVelocity   = 320.0f;  // Match further reduced jump impulse
        float maxDownVelocity = 600.0f;  // Terminal velocity during fall
        float peakThreshold   = 40.0f;   // Velocity range around 0 for peak

        if (vy < -peakThreshold)
        {
            // Going up: map -500 to -50 to frames 1-3
            currentFrame = 1 + static_cast<int>(
                                   ((maxUpVelocity + vy) / (maxUpVelocity - peakThreshold)) * 2.0f);
            currentFrame = std::min(currentFrame, 3);
        }
        else if (vy >= -peakThreshold && vy <= peakThreshold)
        {
            // At peak: use frame 4
            currentFrame = 4;
        }
        else
        {
            // Going down: map 50 to +600 to frames 5-7
            currentFrame = 5 + static_cast<int>((vy / maxDownVelocity) * 2.0f);
            currentFrame = std::min(currentFrame, 6);
        }
    }

    if (!currentTexture)
        return;

    // Clamp frame to available frames (handles short sheets like 4-frame water jump)
    if (currentNumFrames > 0)
    {
        currentFrame = std::max(0, std::min(currentFrame, currentNumFrames - 1));
    }

    SDL_Rect src;
    src.x = currentFrame * currentWidth;  // current animation frame
    src.y = 0;
    src.w = currentWidth;
    src.h = currentHeight;

    SDL_Rect dest;
    dest.x = static_cast<int>(x - camX);
    dest.y = static_cast<int>(y - camY);
    dest.w = currentWidth;
    dest.h = currentHeight;

    // When climbing and latched to right wall, shift draw so right edge aligns with physics box
    if (isClimbing)
    {
        if (climbOnRightWall)
        {
            int dx = (spriteWidth - currentWidth) + climbRightDrawOffset;  // e.g., 14 - 3
            dest.x += dx;
        }
        else
        {
            dest.x += climbLeftDrawOffset;
        }
    }

    // store last drawn dest for precise effect alignment
    lastDrawX = dest.x;
    lastDrawY = dest.y;
    lastDrawW = dest.w;
    lastDrawH = dest.h;

    // Use rotation center point at the center of the sprite
    SDL_Point center;
    center.x = currentWidth / 2;
    center.y = currentHeight / 2;

    SDL_RenderCopyEx(renderer, currentTexture, &src, &dest, rotationAngle, &center, flip);
}