#include "PolarBear.h"

#include <SDL2/SDL_image.h>

#include <algorithm>
#include <cmath>
#include <iostream>

#include "../actions/Attack.h"

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
        if (waterWalkTexture)
            texture = waterWalkTexture;
        if (waterJumpTexture)
            jumpTexture = waterJumpTexture;

        // Set sprite box to encompass the largest water frame (jump height)
        spriteWidth  = waterJumpWidth;
        spriteHeight = waterJumpHeight;
        numFrames    = waterWalkFrames;
        jumpFrames   = waterJumpFrames;

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
    const bool useSwimming = swimming && isWaterEquipped();

    // If we just exited water at the surface, give an upward impulse to breach
    // Only jump out if we're near the top of water (not at the bottom)
    if (justExitedWater && !onGround)
    {
        vy              = -336.0f;
        onGround        = false;
        justExitedWater = false;
    }
    else if (justExitedWater)
    {
        // Clear flag if we're on ground (don't jump from bottom)
        justExitedWater = false;
    }

    // --- Climbing check: if climbing, override gravity and vertical velocity ---
    if (!useSwimming && isClimbing)
    {
        // No gravity while actively climbing
        vy = climbIntent * climbSpeed;
        // Stick to surface: no horizontal movement while climbing
        vx = 0.0f;

        // If climbing upward and reaching the top of the climbable surface, mount onto top
        if (climbIntent < 0.0f)
        {
            float midY      = y + spriteHeight / 2.0f;
            float headY     = y + 1.0f;  // sample just inside top of head
            float leftX     = x - 1.0f;
            float rightX    = x + spriteWidth + 1.0f;
            float sideXMid  = climbOnRightWall ? rightX : leftX;
            float sideXHead = sideXMid;

            bool midAdj  = map.isClimbableAtWorld(sideXMid, midY);
            bool headAdj = map.isClimbableAtWorld(sideXHead, headY);

            // If adjacent at mid but not at head, we reached the top
            if (midAdj && !headAdj)
            {
                // Calculate the tile position of the climbable surface
                int tileX = static_cast<int>(sideXMid) / map.tileSize;
                int tileY = static_cast<int>(midY) / map.tileSize;

                // Position bear on top of the tile, slightly higher so gravity settles it
                float topOfTile = tileY * map.tileSize;
                y               = topOfTile - spriteHeight - 3.0f;

                // Move bear horizontally onto the tile
                // Center the bear on the tile or align to tile edge based on which side we're
                // climbing
                if (climbOnRightWall)
                {
                    // Wall is on right, move bear onto that tile
                    x = tileX * map.tileSize;
                }
                else
                {
                    // Wall is on left, move bear onto that tile (align to right edge of tile)
                    x = (tileX + 1) * map.tileSize - spriteWidth;
                }

                // Exit climbing state and let gravity handle final positioning
                isClimbing  = false;
                climbIntent = 0.0f;
                vy          = 0.0f;
                vx          = 0.0f;
                onGround    = false;  // Let physics detect ground

                // Begin ledge mount window to disable input during transition
                ledgeMounting   = true;
                ledgeMountTimer = ledgeMountDuration;
            }
        }
    }
    else if (!useSwimming)
    {
        // --- Apply gravity ---
        const float GRAVITY = 1000.0f;
        vy += GRAVITY * dt;
    }
    else
    {
        // Swimming: override gravity with swim rise/sink and set horizontal swim speed
        isClimbing = false;
        if (onGround && !swimPressed)
        {
            // Resting at bottom: completely lock velocity
            vy = 0.0f;
            vx = 0.0f;
        }
        else if (swimPressed)
        {
            vy = -swimUpSpeed;
            vx = moveIntent * swimRunSpeed;
        }
        else
        {
            vy = swimSinkSpeed;
            vx = moveIntent * swimRunSpeed;
        }
    }

    // --- Horizontal intent -> velocity (supports slippery surfaces) ---
    // Skip applying moveIntent while climbing (vx already set to 0 above)
    if (!isClimbing && !useSwimming)
    {
        // Detect slipperiness at the feet (one pixel above the sole to stay within tile)
        // Sample at the tile directly beneath the feet (landing puts feet exactly at tile top)
        float footY        = y + spriteHeight;
        float footCenter   = x + spriteWidth / 2.0f;
        float footLeft     = x + 2.0f;                // sample near left foot
        float footRight    = x + spriteWidth - 2.0f;  // sample near right foot
        bool onSlippery    = onGround && (map.isSlipperyAtWorld(footCenter, footY) ||
                                       map.isSlipperyAtWorld(footLeft, footY) ||
                                       map.isSlipperyAtWorld(footRight, footY));
        const float runspd = 75.0f;  // base ground speed

        if (onSlippery)
        {
            // Faster top speed, gradual accel; very light friction so bear keeps sliding when
            // released
            const float slipMaxSpeed = 165.0f;
            const float slipAccel    = 260.0f;  // px/s^2 while holding
            const float slipFriction = 40.0f;   // px/s^2 when released (very low)

            float desired = moveIntent * slipMaxSpeed;
            if (moveIntent != 0.0f)
            {
                float delta = slipAccel * dt;
                if (vx < desired)
                    vx = std::min(vx + delta, desired);
                else if (vx > desired)
                    vx = std::max(vx - delta, desired);
            }
            else
            {
                // Decay slowly to mimic sliding; keep sign until friction bleeds it out
                float delta = slipFriction * dt;
                if (vx > 0.0f)
                    vx = std::max(0.0f, vx - delta);
                else if (vx < 0.0f)
                    vx = std::min(0.0f, vx + delta);
            }
        }
        else
        {
            // Regular ground/air movement snaps to intended speed
            vx = moveIntent * runspd;
        }
    }  // end if (!isClimbing)

    // --- Horizontal movement with collision ---
    x += vx * dt;

    // Check horizontal collisions (left and right edges)
    // Require at least 80% of height to collide for blocking
    const int samples = 10;
    int collisions    = 0;
    for (int i = 0; i < samples; ++i)
    {
        int h = (i * spriteHeight) / (samples - 1);
        if (h >= spriteHeight)
            h = spriteHeight - 1;

        if (vx > 0)
        {
            // Moving right - check right edge
            if (map.isSolidAtWorld(x + spriteWidth, y + h))
                collisions++;
        }
        else if (vx < 0)
        {
            // Moving left - check left edge
            if (map.isSolidAtWorld(x, y + h))
                collisions++;
        }
    }

    // Block movement if 30% or more of the body collides
    if (collisions >= samples * 0.3f)
    {
        if (vx > 0)
        {
            x  = (static_cast<int>(x + spriteWidth) / map.tileSize) * map.tileSize - spriteWidth;
            vx = 0;
        }
        else if (vx < 0)
        {
            x  = (static_cast<int>(x) / map.tileSize + 1) * map.tileSize;
            vx = 0;
        }
    }

    // --- Vertical movement with collision ---
    // When swimming, we need to detect solid ground even through water tiles
    if (useSwimming && vy >= 0)
    {
        // Check if we're touching ground while sinking
        const int vSamples = 10;
        int vCollisions    = 0;
        
        for (int i = 0; i < vSamples; ++i)
        {
            int w = (i * spriteWidth) / (vSamples - 1);
            if (w >= spriteWidth)
                w = spriteWidth - 1;
            
            if (map.isSolidAtWorld(x + w, y + spriteHeight, 1.0f) ||
                map.isCollisionDownOnlyAtWorld(x + w, y + spriteHeight))
            {
                vCollisions++;
            }
        }
        
        if (vCollisions >= vSamples * 0.2f)
        {
            onGround = true;
        }
        else
        {
            onGround = false;
        }
    }
    
    bool restingInWater = useSwimming && onGround && !swimPressed;

    if (!restingInWater)
    {
        y += vy * dt;

        if (!useSwimming)
            onGround = false;

        // Check vertical collisions (top and bottom edges) with 30% threshold
        const int vSamples = 10;
        int vCollisions    = 0;

        for (int i = 0; i < vSamples; ++i)
        {
            int w = (i * spriteWidth) / (vSamples - 1);
            if (w >= spriteWidth)
                w = spriteWidth - 1;

            if (vy > 0)
            {
                // Moving down - check bottom edge for regular collisions and down-only platforms
                if (map.isSolidAtWorld(x + w, y + spriteHeight, vy) ||
                    map.isCollisionDownOnlyAtWorld(x + w, y + spriteHeight))
                {
                    vCollisions++;
                }
            }
            else if (vy < 0)
            {
                // Moving up - check top edge
                if (map.isSolidAtWorld(x + w, y))
                {
                    vCollisions++;
                }
            }
        }

        if (vCollisions >= vSamples * 0.2f)
        {
            if (vy > 0)
            {
                y  = (static_cast<int>(y + spriteHeight) / map.tileSize) * map.tileSize - spriteHeight;
                vy = 0;
                if (!useSwimming)
                    onGround = true;
                // Clear knockback state when landing
                isKnockedBack = false;
            }
            else if (vy < 0)
            {
                y  = (static_cast<int>(y) / map.tileSize + 1) * map.tileSize;
                vy = 0;
            }
        }
    }
    else
    {
        // Resting at bottom in water: lock position completely
        vy = 0.0f;
        vx = 0.0f;
        // No position update - stay completely still
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

    // --- Animation ---
    if (useSwimming)
    {
        // When resting on the bottom, stay idle (no swim animation)
        if (onGround)
        {
            frame      = 0;
            frameTimer = 0.0f;
        }
        else
        {
            frameTimer += dt;
            if (frameTimer >= swimFrameTime)
            {
                frameTimer = 0.0f;
                frame      = (frame + 1) % std::max(1, waterSwimFrames);
            }
        }
    }
    else if (isClimbing)
    {
        // Animate climb when moving vertically; otherwise show first frame
        if (std::abs(climbIntent) > 0.0f)
        {
            frameTimer += dt;
            if (frameTimer >= climbFrameTime)
            {
                frameTimer = 0.0f;
                frame      = (frame + 1) % std::max(1, climbFrames);
            }
        }
        else
        {
            frame = 0;
        }
    }
    else
    {
        // Walking/idle animation
        if (vx != 0)
        {
            frameTimer += dt;
            if (frameTimer >= frameTime)
            {
                frameTimer = 0.0f;
                frame      = (frame + 1) % numFrames;
            }
        }
        else
        {
            frame = 0;  // idle frame
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
