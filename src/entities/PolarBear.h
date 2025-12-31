// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// polarbear.h
// Player character class that handles movement, jumping, attacking, and
// animation frame selection.

#pragma once
#include <SDL2/SDL.h>

#include <memory>
#include <string>

#include "actions/Attack.h"
#include "systems/TileMap.h"
#include "components/BearComponent.h"
#include "core/GameConstants.h"

// Forward declarations.
class Attack;
class BearComponent;

// Player character class.
// Manages position, velocity, animation state, and attack mechanics.
// The bear can move left/right, jump, and attack with a crescent slash effect.
class PolarBear
{
   public:
    enum class Element
    {
        None,
        Water,
        Fire,
        Earth,
        Wind
    };
    // Position in world space.
    float x, y;
    // Velocity in world space.
    float vx, vy;
    // True if the bear is standing on ground.
    bool onGround;
    // True if the bear is facing right, false if facing left.
    // This is independent of movement direction and freezes during attacks.
    bool facingRight = true;

    // Sprite sheet textures for different states.
    SDL_Texture* texture       = nullptr;  // Idle/walk animation.
    SDL_Texture* jumpTexture   = nullptr;  // Jump animation texture.
    SDL_Texture* attackTexture = nullptr;  // Attack animation texture.
    SDL_Texture* slashTexture  = nullptr;  // Textured slash effect for attacks.
    SDL_Texture* climbTexture  = nullptr;  // Climb animation texture.

    // Elemental variants (currently water)
    SDL_Texture* waterWalkTexture = nullptr;
    SDL_Texture* waterJumpTexture = nullptr;
    SDL_Texture* waterSwimTexture = nullptr;

    // Elemental variants: wind
    SDL_Texture* windWalkTexture = nullptr;
    SDL_Texture* windJumpTexture = nullptr;

    // Sprite dimensions (pixels).
    int spriteWidth  = GameConstants::Player::DEFAULT_WIDTH;
    int spriteHeight = GameConstants::Player::DEFAULT_HEIGHT;
    // Current animation frame index (0-based).
    int frame = 0;
    // Number of frames in idle/walk animation.
    int numFrames = GameConstants::Player::WALK_FRAMES;
    // Number of frames in jump animation.
    int jumpFrames = GameConstants::Player::JUMP_FRAMES;
    // Number of frames in attack animation.
    int attackFrames = GameConstants::Player::ATTACK_FRAMES;
    // Number of frames in climb animation.
    int climbFrames = GameConstants::Player::CLIMB_FRAMES;
    // Climb frame dimensions (pixels).
    int climbWidth  = GameConstants::Player::CLIMB_WIDTH;
    int climbHeight = GameConstants::Player::CLIMB_HEIGHT;
    // Time per frame in seconds for idle/walk animations.
    float frameTime = GameConstants::Player::WALK_FRAME_TIME;
    // Accumulated time for frame counter.
    float frameTimer     = 0.0f;
    float climbFrameTime = GameConstants::Player::CLIMB_FRAME_TIME;

    // Attack state and animation.
    // Current attack (nullptr if not attacking).
    std::unique_ptr<Attack> currentAttack = nullptr;
    // True if currently performing an attack.
    bool isAttacking = false;
    // Attack sprite dimensions (pixels).
    int attackWidth  = GameConstants::Player::ATTACK_WIDTH;
    int attackHeight = GameConstants::Player::ATTACK_HEIGHT;
    // Last rendered sprite position in screen coordinates.
    // Used to align the slash effect with the bear's actual draw position.
    int lastDrawX = 0;  // Screen X position of sprite.
    int lastDrawW = 0;  // Screen width of sprite.
    int lastDrawY = 0;  // Screen Y position of sprite.
    int lastDrawH = 0;  // Screen height of sprite.

    // Damage state and animation.
    // True if currently playing the damage animation.
    bool isDamaged = false;
    // Current frame in the damage animation (reversed jump frames).
    int damageFrame = 0;
    // Elapsed time in the current damage frame.
    float damageTimer = 0.0f;
    // Time per damage frame in seconds.
    float damageDuration = GameConstants::Player::DAMAGE_FRAME_DURATION;
    // Counter for the blinking effect during invulnerability (0-3 seconds).
    float invulnerabilityTimer = 0.0f;
    // Duration of invulnerability after damage (3 seconds).
    float invulnerabilityDuration = GameConstants::Player::INVULNERABILITY_DURATION;
    // True if currently invulnerable (immune to damage).
    bool isInvulnerable = false;
    // Direction the bear was facing when damaged (for knockback direction).
    // True if facing right when damage occurred.
    bool damageFacingRight = true;
    // True if currently being knocked back; disables all input until landing.
    bool isKnockedBack = false;

    // Climbing capability and state
    bool canClimb         = false;  // unlocked via dev flag
    bool isClimbing       = false;  // currently climbing
    float climbIntent     = 0.0f;   // vertical input intent for climbing (-1,0,1)
    float climbSpeed      = GameConstants::Player::CLIMB_SPEED;
    bool climbOnRightWall = false;  // true if latched to a wall on the right side
    // Visual fine-tune to keep sprite snug to the wall
    int climbRightDrawOffset = GameConstants::Player::CLIMB_RIGHT_DRAW_OFFSET;
    int climbLeftDrawOffset  = GameConstants::Player::CLIMB_LEFT_DRAW_OFFSET;

    // Horizontal input intent (-1, 0, 1) captured each frame
    float moveIntent = 0.0f;
    
    // Jump button held state (for wind float mechanic)
    bool jumpHeld = false;

    // Elemental state
    Element element = Element::None;

    // Baseline (non-element) dimensions/frames/textures captured on first setElement call
    int baseWalkWidth   = 0;
    int baseWalkHeight  = 0;
    int baseJumpWidth   = 0;
    int baseJumpHeight  = 0;
    int baseNumFrames   = 0;
    int baseJumpFrames  = 0;
    SDL_Texture* baseWalkTexture = nullptr;  // Original walk texture
    SDL_Texture* baseJumpTexture = nullptr;  // Original jump texture

    // Water / swimming state
    bool inWater     = false;
    bool swimming    = false;
    bool wasSwimming = false;
    bool justExitedWater = false;
    bool swimPressed = false;
    float swimUpSpeed   = GameConstants::Player::SWIM_UP_SPEED;
    float swimSinkSpeed = GameConstants::Player::SWIM_SINK_SPEED;
    float swimRunSpeed  = GameConstants::Player::SWIM_RUN_SPEED;
    float runSpeed      = GameConstants::Player::RUN_SPEED;

    // Frame metadata for water variants
    int waterWalkFrames = GameConstants::Player::WATER_WALK_FRAMES;
    int waterJumpFrames = GameConstants::Player::WATER_JUMP_FRAMES;
    int waterSwimFrames = GameConstants::Player::WATER_SWIM_FRAMES;
    int waterWalkWidth  = GameConstants::Player::WATER_WALK_WIDTH;
    int waterWalkHeight = GameConstants::Player::WATER_WALK_HEIGHT;
    int waterJumpWidth  = GameConstants::Player::WATER_JUMP_WIDTH;
    int waterJumpHeight = GameConstants::Player::WATER_JUMP_HEIGHT;
    int waterSwimWidth  = GameConstants::Player::WATER_SWIM_WIDTH;
    int waterSwimHeight = GameConstants::Player::WATER_SWIM_HEIGHT;
    float swimFrameTime = GameConstants::Player::SWIM_FRAME_TIME;

    // Frame metadata for wind variants
    int windWalkFrames = GameConstants::Player::WIND_WALK_FRAMES;
    int windWalkWidth  = GameConstants::Player::WIND_WALK_WIDTH;
    int windWalkHeight = GameConstants::Player::WIND_WALK_HEIGHT;
    int windJumpFrames = GameConstants::Player::WIND_JUMP_FRAMES;
    int windJumpWidth  = GameConstants::Player::WIND_JUMP_WIDTH;
    int windJumpHeight = GameConstants::Player::WIND_JUMP_HEIGHT;

    // Wind detection/state
    bool inWind = false;

    // Takes damage and triggers the damage animation and invulnerability period.
    void takeDamage(class GameState& state);

    // Computes the world-space bounding box of the current attack.
    // Returns true if an attack is active and fills `out` with AABB coordinates.
    bool getAttackWorldRect(SDL_Rect& out) const;

    // Loads the idle/walk sprite sheet from a PNG file.
    void loadTexture(SDL_Renderer* renderer, const std::string& filename);

    // Loads the jump animation sprite sheet from a PNG file.
    void loadJumpTexture(SDL_Renderer* renderer, const std::string& filename);

    // Loads the attack animation sprite sheet from a PNG file.
    void loadAttackTexture(SDL_Renderer* renderer, const std::string& filename);

    // Loads the slash effect texture from a PNG file.
    // Stored in slashTexture for attack creation.
    void loadSlashTexture(SDL_Renderer* renderer, const std::string& filename);

    // Loads the climb animation sprite sheet from a PNG file.
    void loadClimbTexture(SDL_Renderer* renderer, const std::string& filename);

    // Load water-specific textures
    void loadWaterWalkTexture(SDL_Renderer* renderer, const std::string& filename);
    void loadWaterJumpTexture(SDL_Renderer* renderer, const std::string& filename);
    void loadWaterSwimTexture(SDL_Renderer* renderer, const std::string& filename);

    // Load wind-specific textures
    void loadWindWalkTexture(SDL_Renderer* renderer, const std::string& filename);
    void loadWindJumpTexture(SDL_Renderer* renderer, const std::string& filename);

    // Element helpers
    void setElement(Element e);
    bool isWaterEquipped() const { return element == Element::Water; }
    bool isWindEquipped() const { return element == Element::Wind; }

    // Water detection and swimming control
    int waterCoverageCount(const TileMap& map) const;
    void setSwimmingState(bool inWater, bool swimButtonPressed);
    bool isSwimming() const { return swimming; }

    // Wind detection helper
    int windCoverageCount(const TileMap& map) const;

    // Initiates a slash attack.
    void startAttack();

    // Called when attack button is released (for charge/hold attacks).
    void onAttackRelease();

    // Updates position, velocity, collision, and animation state.
    void update(float dt, const TileMap& map, class GameState& state);

    // Renders the bear sprite to the screen, selecting appropriate texture based on state.
    void render(SDL_Renderer* renderer, int camX, int camY, SDL_RendererFlip flip = SDL_FLIP_NONE);

    // Renders the attack effect (slash, particles, etc.).
    void renderAttack(SDL_Renderer* renderer, int camX, int camY);

    // Short window after mounting from climb where horizontal input is ignored
    bool ledgeMounting       = false;
    float ledgeMountTimer    = 0.0f;
    float ledgeMountDuration = 0.25f;

    // Component-based system (replaces state machine)
    std::vector<std::unique_ptr<BearComponent>> components;
    void addComponent(std::unique_ptr<BearComponent> component);
    void clearComponents();
};

