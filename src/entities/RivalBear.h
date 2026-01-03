#pragma once

#include "Boss.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

class TileMap;
class PolarBear;

// Rival Bear boss - appears in cutscenes as a rival character
// Performs cinematic sequences: walk, jump, and slash attack
class RivalBear : public Boss
{
public:
    RivalBear();
    virtual ~RivalBear();

    // Boss interface implementation
    void loadAssets(SDL_Renderer* renderer, const std::string& assetPath) override;
    void updateAI(float dt, const TileMap& map, PolarBear& bear) override;
    void render(SDL_Renderer* renderer, const Camera& camera) override;
    void renderSlash(SDL_Renderer* renderer, const Camera& camera);  // Render slash separately (after player)
    void setPosition(float x, float y) override;
    void updateIntro(float dt) override;
    void startIntro() override;
    bool isIntroActive() const override;
    bool isIntroDone() const override;
    void markIntroDone();  // Called when camera returns to player
    void markCutsceneComplete();  // Called when cutscene ends
    bool isCutsceneComplete() const;  // Check if cutscene is done
    void takeDamage(int amount) override;
    bool isDead() const override;
    bool isVulnerable() const override;
    bool canDamagePlayer() const override;
    bool shouldDisableInputs() const override;
    bool shouldStopMusic() const override;
    void enableInputs() override;
    void getCollisionRect(SDL_Rect& out) const override;
    float getX() const override;
    float getY() const override;
    int getFadeAlpha() const override;
    const char* getName() const override;
    
    // Texture loading
    void loadWalkTexture(SDL_Renderer* renderer, const std::string& path);
    void loadJumpTexture(SDL_Renderer* renderer, const std::string& path);
    void loadAttackTexture(SDL_Renderer* renderer, const std::string& path);
    void loadSlashTexture(SDL_Renderer* renderer, const std::string& path);
    
    // Slash attack (larger than player's for better visibility)
    struct SlashAttack {
        bool active = false;
        float x = 0.0f;
        float y = 0.0f;
        int width = 64;   // Match texture width
        int height = 64;  // Match texture height (square aspect ratio)
        bool facingRight = true;
        float timer = 0.0f;
        float duration = 0.3f;
    };
    
    SlashAttack slash;
    
    // Get slash world rectangle for collision detection
    bool getSlashWorldRect(SDL_Rect& out) const;
    
    // Set facing direction
    void setFacingRight(bool right);
    
    // Set target position for jump (robot location)
    void setTargetPosition(float targetX);
    
    // Set sound effects
    void setSlashSound(Mix_Chunk* sound);
    void setGrowlSound(Mix_Chunk* sound);
    void playGrowlSound();

private:
    enum class State {
        Idle,
        Walking,
        Jumping,
        Attacking,
        WalkingBack,  // Walking back after attack
        Done
    };
    
    State state_ = State::Idle;
    float stateTimer_ = 0.0f;
    
    float x_ = 0.0f;
    float y_ = 0.0f;
    float vx_ = 0.0f;
    float vy_ = 0.0f;
    bool onGround_ = false;
    bool facingRight_ = true;
    bool introStarted_ = false;  // Becomes true when cutscene starts boss AI
    bool introDone_ = false;  // Becomes true when attack sequence completes
    bool cutsceneComplete_ = false;  // Becomes true when cutscene finishes (before camera transition)
    
    // Damage and invulnerability
    float invulnerabilityTimer_ = 0.0f;
    bool isFlashing_ = false;
    int flashFrame_ = 0;
    
    int health_ = 1;
    
    // Sprite dimensions
    int walkWidth_ = 56;
    int walkHeight_ = 33;
    int jumpWidth_ = 57;
    int jumpHeight_ = 35;
    int attackWidth_ = 51;
    int attackHeight_ = 36;
    
    // Textures
    SDL_Texture* walkTexture_ = nullptr;
    SDL_Texture* jumpTexture_ = nullptr;
    SDL_Texture* attackTexture_ = nullptr;
    SDL_Texture* slashTexture_ = nullptr;
    
    // Animation
    int frame_ = 0;
    int numFrames_ = 1;
    float frameTime_ = 0.15f;
    float frameTimer_ = 0.0f;
    
    // Target for cutscene
    float targetX_ = 0.0f;
    float targetY_ = 0.0f;
    float jumpTargetX_ = -10.0f;
    float walkBackStartX_ = 0.0f;  // Track starting position for walk-back distance
    
    // Sound effects
    Mix_Chunk* slashSound_ = nullptr;
    Mix_Chunk* growlSound_ = nullptr;
    float postAttackTimer_ = 0.0f;
    
    void updateWalking(float dt, const TileMap& map);
    void updateJumping(float dt, const TileMap& map);
    void updateAttacking(float dt);
    void applyGravity(float dt, const TileMap& map);
};
