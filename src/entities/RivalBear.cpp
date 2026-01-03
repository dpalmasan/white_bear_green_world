#include "RivalBear.h"
#include "systems/TileMap.h"
#include "PolarBear.h"
#include "core/Collision.h"
#include "core/Camera.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

RivalBear::RivalBear() {}

RivalBear::~RivalBear()
{
    if (walkTexture_) SDL_DestroyTexture(walkTexture_);
    if (jumpTexture_) SDL_DestroyTexture(jumpTexture_);
    if (attackTexture_) SDL_DestroyTexture(attackTexture_);
    if (slashTexture_) SDL_DestroyTexture(slashTexture_);
}

void RivalBear::loadAssets(SDL_Renderer* renderer, const std::string& assetPath)
{
    // Not used for RivalBear - textures loaded separately in Game.cpp
}

void RivalBear::setPosition(float x, float y)
{
    x_ = x;
    y_ = y;
}

void RivalBear::loadWalkTexture(SDL_Renderer* renderer, const std::string& path)
{
    SDL_Surface* surf = IMG_Load(path.c_str());
    if (surf) {
        walkTexture_ = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }
}

void RivalBear::loadJumpTexture(SDL_Renderer* renderer, const std::string& path)
{
    SDL_Surface* surf = IMG_Load(path.c_str());
    if (surf) {
        jumpTexture_ = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }
}

void RivalBear::loadAttackTexture(SDL_Renderer* renderer, const std::string& path)
{
    SDL_Surface* surf = IMG_Load(path.c_str());
    if (surf) {
        attackTexture_ = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }
}

void RivalBear::loadSlashTexture(SDL_Renderer* renderer, const std::string& path)
{
    SDL_Surface* surf = IMG_Load(path.c_str());
    if (surf) {
        slashTexture_ = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
    }
}

void RivalBear::startIntro()
{
    // Called when camera panning finishes - start the jump sequence
    introStarted_ = true;
}

void RivalBear::setTargetPosition(float targetX)
{
    jumpTargetX_ = targetX;
}

void RivalBear::setSlashSound(Mix_Chunk* sound)
{
    slashSound_ = sound;
}

void RivalBear::setGrowlSound(Mix_Chunk* sound)
{
    growlSound_ = sound;
}

void RivalBear::playGrowlSound()
{
    if (growlSound_) {
        Mix_PlayChannel(-1, growlSound_, 0);
    }
}

bool RivalBear::isIntroActive() const
{
    // Only active after cutscene starts it AND until sequence completes
    return introStarted_ && !introDone_;
}

bool RivalBear::isIntroDone() const
{
    return introDone_;
}

void RivalBear::markIntroDone()
{
    introDone_ = true;
}

void RivalBear::markCutsceneComplete()
{
    cutsceneComplete_ = true;
}

bool RivalBear::isCutsceneComplete() const
{
    return cutsceneComplete_;
}

void RivalBear::updateIntro(float dt)
{
    // No intro animation
}

void RivalBear::updateAI(float dt, const TileMap& map, PolarBear& bear)
{
    // Update invulnerability timer
    if (invulnerabilityTimer_ > 0.0f) {
        invulnerabilityTimer_ -= dt;
        if (invulnerabilityTimer_ <= 0.0f) {
            invulnerabilityTimer_ = 0.0f;
            isFlashing_ = false;
        }
    }
    
    // Update flash animation
    if (isFlashing_) {
        flashFrame_ = static_cast<int>((invulnerabilityTimer_ * 10.0f)) % 2;
    }
    
    // During cutscene intro, use old jump behavior
    if (!introDone_) {
        if (!introStarted_) {
            return;
        }
        
        stateTimer_ += dt;
        
        // Old cutscene behavior (jump and attack robot)
        if (state_ == State::Idle && stateTimer_ >= 0.5f) {
            state_ = State::Jumping;
            stateTimer_ = 0.0f;
            float landX = jumpTargetX_;
            float distance = landX - x_;
            float flightTime = 1.15f;
            vx_ = distance / flightTime;
            vy_ = -400.0f;
            numFrames_ = 8;
            frameTime_ = 0.1f;
        } else if (state_ == State::Jumping) {
            updateJumping(dt, map);
            if (onGround_) {
                state_ = State::Attacking;
                stateTimer_ = 0.0f;
                vx_ = 0.0f;
                numFrames_ = 7;
                frameTime_ = 0.1f;
                frame_ = 0;
                frameTimer_ = 0.0f;
                slash.active = false;
            }
        } else if (state_ == State::Attacking) {
            updateAttacking(dt);
            if (frame_ == 3 && !slash.active) {
                slash.active = true;
                slash.timer = 0.0f;
                slash.facingRight = facingRight_;
                if (slashSound_) {
                    Mix_PlayChannel(-1, slashSound_, 0);
                }
                // Position slash with inset like player's slash (6 pixels from edge)
                // Calculate front point of bear symmetrically
                const int inset = 6;
                float frontX = x_ + (facingRight_ ? attackWidth_ - inset : inset);
                // Position slash relative to front point
                if (facingRight_) {
                    slash.x = frontX;
                } else {
                    slash.x = frontX - slash.width;
                }
                slash.y = y_ + attackHeight_ / 2 - slash.height / 2;
            }
            if (stateTimer_ >= 0.7f) {
                state_ = State::Done;
                stateTimer_ = 0.0f;
                frame_ = 0;
                numFrames_ = 1;
            }
        }
        
        // Update slash and frames
        if (slash.active) {
            slash.timer += dt;
            if (slash.timer >= slash.duration) {
                slash.active = false;
            }
        }
        
        frameTimer_ += dt;
        if (frameTimer_ >= frameTime_) {
            frameTimer_ = 0.0f;
            frame_++;
            if (frame_ >= numFrames_) {
                if (state_ == State::Attacking) {
                    frame_ = numFrames_ - 1;
                } else {
                    frame_ = 0;
                }
            }
        }
        return;
    }
    
    // After cutscene: Chase AI
    stateTimer_ += dt;
    
    switch (state_) {
        case State::Idle:
        case State::Done:
            // Start chasing player
            if (stateTimer_ >= 0.5f) {
                float playerX = bear.x + bear.spriteWidth / 2.0f;
                float myX = x_ + attackWidth_ / 2.0f;
                
                facingRight_ = (playerX > myX);
                
                const float walkSpeed = 80.0f;
                vx_ = facingRight_ ? walkSpeed : -walkSpeed;
                
                state_ = State::Walking;
                stateTimer_ = 0.0f;
                numFrames_ = 4;
                frameTime_ = 0.15f;
            }
            break;
            
        case State::Walking:
            updateWalking(dt, map);
            {
                float playerX = bear.x + bear.spriteWidth / 2.0f;
                float myX = x_ + attackWidth_ / 2.0f;
                float distance = std::abs(playerX - myX);
                
                if (distance < 32.0f) {
                    vx_ = 0.0f;
                    state_ = State::Attacking;
                    stateTimer_ = 0.0f;
                    frame_ = 0;
                    numFrames_ = 7;
                    frameTime_ = 0.1f;
                }
            }
            break;
            
        case State::Attacking:
            updateAttacking(dt);
            if (frame_ == 3 && !slash.active) {
                slash.active = true;
                slash.timer = 0.0f;
                slash.facingRight = facingRight_;
                
                if (slashSound_) {
                    Mix_PlayChannel(-1, slashSound_, 0);
                }
                
                // Position slash with inset like player's slash (6 pixels from edge)
                // Calculate front point of bear symmetrically
                const int inset = 6;
                float frontX = x_ + (facingRight_ ? attackWidth_ - inset : inset);
                // Position slash relative to front point
                if (facingRight_) {
                    slash.x = frontX;
                } else {
                    slash.x = frontX - slash.width;
                }
                slash.y = y_ + attackHeight_ / 2 - slash.height / 2;
            }
            if (stateTimer_ >= 0.7f) {
                // Jump backwards with random distance (6-8 tiles = 96-128 pixels)
                float randomDistance = 96.0f + (rand() % 33);  // 96 to 128
                float jumpBackDistance = facingRight_ ? -randomDistance : randomDistance;
                vx_ = jumpBackDistance / 0.6f;  // Jump duration ~0.6s
                vy_ = -300.0f;  // Smaller jump than cutscene
                
                state_ = State::Jumping;
                stateTimer_ = 0.0f;
                numFrames_ = 8;
                frameTime_ = 0.1f;
            }
            break;
            
        case State::Jumping:
            updateJumping(dt, map);
            if (onGround_) {
                vx_ = 0.0f;
                state_ = State::Idle;
                stateTimer_ = 0.0f;
                frame_ = 0;
                numFrames_ = 1;
            }
            break;
            
        case State::WalkingBack:
            // Not used anymore - using Jumping instead
            break;
    }
    
    // Update slash animation
    if (slash.active) {
        slash.timer += dt;
        
        if (slash.timer >= slash.duration) {
            slash.active = false;
        }
    }
    
    // Update animation frames
    frameTimer_ += dt;
    if (frameTimer_ >= frameTime_) {
        frameTimer_ = 0.0f;
        frame_++;
        if (frame_ >= numFrames_) {
            if (state_ == State::Attacking) {
                frame_ = numFrames_ - 1;
            } else {
                frame_ = 0;
            }
        }
    }
}

void RivalBear::updateWalking(float dt, const TileMap& map)
{
    x_ += vx_ * dt;
    applyGravity(dt, map);
}

void RivalBear::updateJumping(float dt, const TileMap& map)
{
    // Horizontal movement with wall collision
    float newX = x_ + vx_ * dt;
    
    // Get current dimensions
    int currentWidth = jumpWidth_;
    int currentHeight = jumpHeight_;
    
    // Check left and right walls
    bool hitWall = false;
    if (vx_ < 0) {  // Moving left
        float leftEdge = newX;
        float topCheck = y_ + currentHeight * 0.3f;
        float bottomCheck = y_ + currentHeight * 0.7f;
        if (map.isSolidAtWorld(leftEdge, topCheck, 0.0f) || 
            map.isSolidAtWorld(leftEdge, bottomCheck, 0.0f)) {
            hitWall = true;
        }
    } else if (vx_ > 0) {  // Moving right
        float rightEdge = newX + currentWidth;
        float topCheck = y_ + currentHeight * 0.3f;
        float bottomCheck = y_ + currentHeight * 0.7f;
        if (map.isSolidAtWorld(rightEdge, topCheck, 0.0f) || 
            map.isSolidAtWorld(rightEdge, bottomCheck, 0.0f)) {
            hitWall = true;
        }
    }
    
    if (!hitWall) {
        x_ = newX;
    } else {
        vx_ = 0.0f;  // Stop horizontal movement on wall hit
    }
    
    applyGravity(dt, map);
}

void RivalBear::updateAttacking(float dt)
{
    // Stay in place during attack
    vx_ = 0.0f;
}

void RivalBear::applyGravity(float dt, const TileMap& map)
{
    const float GRAVITY = 1000.0f;
    vy_ += GRAVITY * dt;
    
    y_ += vy_ * dt;
    onGround_ = false;
    
    // Get current dimensions based on state
    int currentWidth = walkWidth_;
    int currentHeight = walkHeight_;
    if (state_ == State::Jumping) {
        currentWidth = jumpWidth_;
        currentHeight = jumpHeight_;
    } else if (state_ == State::Attacking) {
        currentWidth = attackWidth_;
        currentHeight = attackHeight_;
    }
    
    // Ground collision
    if (vy_ > 0) {
        float bottom = y_ + currentHeight;
        float checkX = x_ + currentWidth / 2;
        
        if (map.isSolidAtWorld(checkX, bottom, vy_)) {
            int floorTileY = static_cast<int>(bottom / map.tileSize);
            y_ = floorTileY * map.tileSize - currentHeight;
            vy_ = 0.0f;
            onGround_ = true;
        }
    }
}

void RivalBear::render(SDL_Renderer* renderer, const Camera& camera)
{
    int camX = camera.x;
    int camY = camera.y;
    
    SDL_Texture* currentTexture = nullptr;
    int currentWidth = walkWidth_;
    int currentHeight = walkHeight_;
    
    switch (state_) {
        case State::Idle:
        case State::Walking:
            currentTexture = walkTexture_;
            currentWidth = walkWidth_;
            currentHeight = walkHeight_;
            break;
        case State::Jumping:
            currentTexture = jumpTexture_;
            currentWidth = jumpWidth_;
            currentHeight = jumpHeight_;
            break;
        case State::Attacking:
            currentTexture = attackTexture_;
            currentWidth = attackWidth_;
            currentHeight = attackHeight_;
            break;
        case State::Done:
            currentTexture = walkTexture_;
            currentWidth = walkWidth_;
            currentHeight = walkHeight_;
            break;
    }
    
    if (!currentTexture) return;
    
    SDL_Rect src{frame_ * currentWidth, 0, currentWidth, currentHeight};
    SDL_Rect dst{
        static_cast<int>(std::round(x_)) - camX,
        static_cast<int>(std::round(y_)) - camY,
        currentWidth,
        currentHeight
    };
    
    // Sprite art is drawn facing LEFT by default, so flip when facing RIGHT
    SDL_RendererFlip flip = facingRight_ ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    
    // Apply flash effect if invulnerable
    if (isFlashing_ && flashFrame_ == 1) {
        SDL_SetTextureColorMod(currentTexture, 255, 100, 100);  // Red tint
    }
    
    SDL_RenderCopyEx(renderer, currentTexture, &src, &dst, 0.0, nullptr, flip);
    
    // Reset color mod
    if (isFlashing_) {
        SDL_SetTextureColorMod(currentTexture, 255, 255, 255);
    }
}

void RivalBear::renderSlash(SDL_Renderer* renderer, const Camera& camera)
{
    int camX = camera.x;
    int camY = camera.y;
    
    // Render slash effect with scaling animation like player's slash
    if (slash.active && slashTexture_) {
        // Calculate progress through animation
        float progress = slash.timer / slash.duration;
        if (progress > 1.0f) progress = 1.0f;
        
        // Scale from 0.6 to 1.0 with ease-out (like player's slash)
        const float minScale = 0.6f;
        float eased = 1.0f - (1.0f - progress) * (1.0f - progress);
        float scale = minScale + (1.0f - minScale) * eased;
        
        int scaledWidth = static_cast<int>(slash.width * scale);
        int scaledHeight = static_cast<int>(slash.height * scale);
        
        // When facing left, keep right edge anchored as slash grows
        int renderX = static_cast<int>(std::round(slash.x));
        if (!slash.facingRight) {
            // Right edge of full-size slash is at slash.x + slash.width
            // Keep this point fixed and grow left
            renderX = static_cast<int>(std::round(slash.x + slash.width - scaledWidth));
        }
        
        SDL_Rect slashDst{
            renderX - camX,
            static_cast<int>(std::round(slash.y)) - camY,
            scaledWidth,
            scaledHeight
        };
        
        // Alpha fade-in (120-200 like player's slash)
        SDL_SetTextureBlendMode(slashTexture_, SDL_BLENDMODE_BLEND);
        int alpha = 120 + static_cast<int>(80 * progress);
        if (alpha > 200) alpha = 200;
        SDL_SetTextureAlphaMod(slashTexture_, alpha);
        
        // Bright red color for rival bear slash
        SDL_SetTextureColorMod(slashTexture_, 255, 50, 50);
        
        // Slash sprite is drawn facing RIGHT by default, flip when facing LEFT
        SDL_RendererFlip slashFlip = slash.facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
        // Use NULL source rect to render entire texture (like player)
        SDL_RenderCopyEx(renderer, slashTexture_, NULL, &slashDst, 0.0, nullptr, slashFlip);
        
        // Reset mods
        SDL_SetTextureAlphaMod(slashTexture_, 255);
        SDL_SetTextureColorMod(slashTexture_, 255, 255, 255);
    }
}

bool RivalBear::getSlashWorldRect(SDL_Rect& out) const
{
    if (!slash.active) return false;
    
    out.x = static_cast<int>(slash.x);
    out.y = static_cast<int>(slash.y);
    out.w = slash.width;
    out.h = slash.height;
    return true;
}

void RivalBear::takeDamage(int amount)
{
    // Ignore damage during cutscene or if invulnerable
    if (!introDone_ || invulnerabilityTimer_ > 0.0f) {
        return;
    }
    
    // Flash and growl
    isFlashing_ = true;
    flashFrame_ = 0;
    invulnerabilityTimer_ = 3.0f;
    playGrowlSound();
}

bool RivalBear::isDead() const
{
    return false;  // Never dies in cutscene
}

bool RivalBear::isVulnerable() const
{
    // Vulnerable after cutscene and when not in invulnerability period
    return introDone_ && invulnerabilityTimer_ <= 0.0f;
}

bool RivalBear::canDamagePlayer() const
{
    return false;  // Body collision doesn't damage; only slash does
}

bool RivalBear::shouldDisableInputs() const
{
    return false;  // Inputs already disabled by cutscene
}

bool RivalBear::shouldStopMusic() const
{
    return false;
}

void RivalBear::enableInputs()
{
    // No-op
}

void RivalBear::getCollisionRect(SDL_Rect& out) const
{
    out.x = static_cast<int>(x_);
    out.y = static_cast<int>(y_);
    out.w = walkWidth_;
    out.h = walkHeight_;
}

float RivalBear::getX() const
{
    return x_;
}

float RivalBear::getY() const
{
    return y_;
}

int RivalBear::getFadeAlpha() const
{
    return 0;  // No fade effect
}

const char* RivalBear::getName() const
{
    return "rival-bear";
}

void RivalBear::setFacingRight(bool right)
{
    facingRight_ = right;
}
