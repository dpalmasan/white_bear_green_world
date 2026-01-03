#pragma once

#include "Cutscene.h"
#include <memory>

class Boss;

// Cutscene: Camera pans to rival bear, then follows it as it jumps and attacks robot
class RivalBearIntroCutscene : public GameCutscene
{
public:
    RivalBearIntroCutscene(Boss* boss);
    
    void start() override;
    void update(float dt, Camera& camera, const TileMap& map) override;
    bool isComplete() const override;
    std::string getId() const override { return "rival-bear-intro1"; }
    
    // Set player position for camera return
    void setPlayerTarget(float x, float y);
    
private:
    enum class Phase
    {
        FadeIn,              // Wait for letterbox fade
        PanLeft,             // Pan camera left to boss
        PanUp,               // Pan camera up to center boss
        BossJump,            // Wait for boss to jump
        BossAttack,          // Wait for boss to attack
        WaitAfterSlash,      // 2 second wait after slash
        PlayGrowl,           // Play growl sound
        TransitionToPlayer,  // Pan camera back to player
        StartMusic,          // Start boss fight music
        Complete             // All done
    };
    
    Boss* boss_;  // Non-owning pointer to boss
    Phase phase_;
    
    // Camera panning state
    float cameraMoveSpeed_ = 50.0f;  // pixels per second
    float targetCameraX_ = 0.0f;
    float targetCameraY_ = 0.0f;
    float fadeTimer_ = 0.0f;  // Timer for FadeIn phase
    float waitTimer_ = 0.0f;  // Timer for wait states
    float playerTargetX_ = 0.0f;  // Target camera position for player
    float playerTargetY_ = 0.0f;
    float accumulatedX_ = 0.0f;  // Accumulated sub-pixel movement
    float accumulatedY_ = 0.0f;
};
