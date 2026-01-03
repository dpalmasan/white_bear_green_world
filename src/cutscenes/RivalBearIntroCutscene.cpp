#include "RivalBearIntroCutscene.h"
#include "entities/Boss.h"
#include "entities/RivalBear.h"
#include "core/Camera.h"
#include "systems/TileMap.h"
#include <cmath>
#include <iostream>

RivalBearIntroCutscene::RivalBearIntroCutscene(Boss* boss)
    : boss_(boss), phase_(Phase::FadeIn)
{
    if (!boss_)
    {
        std::cerr << "[RivalBearIntro] Error: Boss pointer is null\n";
    }
}

void RivalBearIntroCutscene::start()
{
    phase_ = Phase::FadeIn;
    fadeTimer_ = 0.0f;
    waitTimer_ = 0.0f;
    std::cerr << "[RivalBearIntro] Cutscene started\n";
}

void RivalBearIntroCutscene::update(float dt, Camera& camera, const TileMap& map)
{
    if (!boss_) return;
    
    RivalBear* rivalBear = dynamic_cast<RivalBear*>(boss_);
    if (!rivalBear) return;
    
    switch (phase_)
    {
        case Phase::FadeIn:
        {
            // Wait for letterbox to fade in (0.5 seconds)
            fadeTimer_ += dt;
            if (fadeTimer_ >= 0.5f)
            {
                // Start panning left
                targetCameraX_ = boss_->getX() - camera.width / 2;
                if (targetCameraX_ < 0) targetCameraX_ = 0;
                phase_ = Phase::PanLeft;
                std::cerr << "[RivalBearIntro] FadeIn complete, starting PanLeft to x=" << targetCameraX_ << "\n";
            }
            break;
        }
        
        case Phase::PanLeft:
        {
            // Pan camera horizontally to center boss
            float dx = targetCameraX_ - camera.x;
            float dist = std::abs(dx);
            
            std::cerr << "[RivalBearIntro] PanLeft: camera.x=" << camera.x 
                     << " target=" << targetCameraX_ << " dist=" << dist << "\n";
            
            if (dist < 2.0f)
            {
                camera.x = targetCameraX_;
                // Start panning up
                targetCameraY_ = boss_->getY() - camera.height / 2;
                if (targetCameraY_ < 0) targetCameraY_ = 0;
                phase_ = Phase::PanUp;
                std::cerr << "[RivalBearIntro] PanLeft complete, starting PanUp to y=" << targetCameraY_ << "\n";
            }
            else
            {
                float moveAmount = cameraMoveSpeed_ * dt;
                if (moveAmount > dist) moveAmount = dist;
                camera.x += (dx < 0 ? -moveAmount : moveAmount);
                
                if (camera.x < 0) camera.x = 0;
            }
            break;
        }
        
        case Phase::PanUp:
        {
            // Pan camera vertically to center boss
            float dy = targetCameraY_ - camera.y;
            float dist = std::abs(dy);
            
            if (dist < 2.0f)
            {
                camera.y = targetCameraY_;
                // Start boss jumping
                rivalBear->startIntro();
                phase_ = Phase::BossJump;
                std::cerr << "[RivalBearIntro] PanUp complete, boss jumping\n";
            }
            else
            {
                float moveAmount = cameraMoveSpeed_ * dt;
                if (moveAmount > dist) moveAmount = dist;
                camera.y += (dy < 0 ? -moveAmount : moveAmount);
                
                if (camera.y < 0) camera.y = 0;
            }
            break;
        }
        
        case Phase::BossJump:
        {
            // Follow boss while jumping
            float targetX = boss_->getX() - camera.width / 2;
            float targetY = boss_->getY() - camera.height / 2;
            
            float followSpeed = 5.0f;
            camera.x += (targetX - camera.x) * followSpeed * dt;
            camera.y += (targetY - camera.y) * followSpeed * dt;
            
            // Clamp to world bounds
            if (camera.x < 0) camera.x = 0;
            if (camera.y < 0) camera.y = 0;
            if (camera.x > map.width * map.tileSize - camera.width)
                camera.x = map.width * map.tileSize - camera.width;
            if (camera.y > map.height * map.tileSize - camera.height)
                camera.y = map.height * map.tileSize - camera.height;
            
            // Check if boss started attacking
            if (rivalBear->slash.active)
            {
                phase_ = Phase::BossAttack;
                std::cerr << "[RivalBearIntro] Boss attacking\n";
            }
            break;
        }
        
        case Phase::BossAttack:
        {
            // Continue following during attack
            float targetX = boss_->getX() - camera.width / 2;
            float targetY = boss_->getY() - camera.height / 2;
            
            float followSpeed = 5.0f;
            camera.x += (targetX - camera.x) * followSpeed * dt;
            camera.y += (targetY - camera.y) * followSpeed * dt;
            
            // Clamp to world bounds
            if (camera.x < 0) camera.x = 0;
            if (camera.y < 0) camera.y = 0;
            if (camera.x > map.width * map.tileSize - camera.width)
                camera.x = map.width * map.tileSize - camera.width;
            if (camera.y > map.height * map.tileSize - camera.height)
                camera.y = map.height * map.tileSize - camera.height;
            
            // Wait for attack to finish
            if (!rivalBear->slash.active)
            {
                waitTimer_ = 0.0f;
                phase_ = Phase::WaitAfterSlash;
                std::cerr << "[RivalBearIntro] Attack complete, waiting 2s\n";
            }
            break;
        }
        
        case Phase::WaitAfterSlash:
        {
            // Wait 2 seconds after slash
            waitTimer_ += dt;
            if (waitTimer_ >= 2.0f)
            {
                phase_ = Phase::PlayGrowl;
                std::cerr << "[RivalBearIntro] Wait complete, playing growl\n";
            }
            break;
        }
        
        case Phase::PlayGrowl:
        {
            // Play growl sound and immediately transition
            rivalBear->playGrowlSound();
            // Reset accumulation for camera transition
            accumulatedX_ = 0.0f;
            accumulatedY_ = 0.0f;
            phase_ = Phase::TransitionToPlayer;
            std::cerr << "[RivalBearIntro] Growl played, transitioning to player (target set to " 
                     << playerTargetX_ << "," << playerTargetY_ << ")\n";
            break;
        }
        
        case Phase::TransitionToPlayer:
        {
            // Pan camera back to player at 75 px/s
            // playerTargetX/Y_ are player center coordinates
            // Use camera.follow() logic: center camera on player, then clamp to world
            int targetX = static_cast<int>(std::round(playerTargetX_ - camera.width / 2.0f));
            int targetY = static_cast<int>(std::round(playerTargetY_ - camera.height / 2.0f));
            
            // Clamp to world bounds
            if (targetX < 0) targetX = 0;
            if (targetY < 0) targetY = 0;
            if (targetX > map.width * map.tileSize - camera.width)
                targetX = map.width * map.tileSize - camera.width;
            if (targetY > map.height * map.tileSize - camera.height)
                targetY = map.height * map.tileSize - camera.height;
            
            float dx = static_cast<float>(targetX - camera.x);
            float dy = static_cast<float>(targetY - camera.y);
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist < 5.0f)
            {
                camera.x = targetX;
                camera.y = targetY;
                phase_ = Phase::StartMusic;
                std::cerr << "[RivalBearIntro] Reached player, starting music\n";
            }
            else
            {
                float speed = 75.0f;
                float moveX = (dx / dist) * speed * dt;
                float moveY = (dy / dist) * speed * dt;
                
                // Accumulate sub-pixel movement
                accumulatedX_ += moveX;
                accumulatedY_ += moveY;
                
                // Apply integer movement when we have at least 1 pixel
                int deltaX = static_cast<int>(accumulatedX_);
                int deltaY = static_cast<int>(accumulatedY_);
                
                if (deltaX != 0 || deltaY != 0) {
                    camera.x += deltaX;
                    camera.y += deltaY;
                    accumulatedX_ -= deltaX;
                    accumulatedY_ -= deltaY;
                    std::cerr << "[RivalBearIntro] Moved camera by (" << deltaX << "," << deltaY 
                             << ") to (" << camera.x << "," << camera.y << ")\n";
                }
            }
            break;
        }
        
        case Phase::StartMusic:
        {
            // Signal cutscene complete - Game.cpp will handle music/cleanup
            rivalBear->markCutsceneComplete();
            phase_ = Phase::Complete;
            std::cerr << "[RivalBearIntro] Cutscene complete\n";
            break;
        }
        
        case Phase::Complete:
            // Do nothing, cutscene is complete
            break;
    }
}

bool RivalBearIntroCutscene::isComplete() const
{
    return phase_ == Phase::Complete;
}

void RivalBearIntroCutscene::setPlayerTarget(float x, float y)
{
    playerTargetX_ = x;
    playerTargetY_ = y;
}
