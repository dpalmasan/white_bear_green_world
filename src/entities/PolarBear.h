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

#include "../actions/Attack.h"
#include "../systems/TileMap.h"
#include "components/BearComponent.h"

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
    int spriteWidth  = 64;
    int spriteHeight = 64;
    // Current animation frame index (0-based).
    int frame = 0;
    // Number of frames in idle/walk animation.
    int numFrames = 4;
    // Number of frames in jump animation.
    int jumpFrames = 7;
    // Number of frames in attack animation.
    int attackFrames = 7;
    // Number of frames in climb animation.
    int climbFrames = 4;
    // Climb frame dimensions (pixels).
    int climbWidth  = 37;
    int climbHeight = 47;
    // Time per frame in seconds for idle/walk animations.
    float frameTime = 0.15f;
    // Accumulated time for frame counter.
    float frameTimer     = 0.0f;
    float climbFrameTime = 0.15f;  // Time per frame for climb animation

    // Attack state and animation.
    // Current attack (nullptr if not attacking).
    std::unique_ptr<Attack> currentAttack = nullptr;
    // True if currently performing an attack.
    bool isAttacking = false;
    // Attack sprite dimensions (pixels).
    int attackWidth  = 54;
    int attackHeight = 37;
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
    float damageDuration = 0.08f;
    // Counter for the blinking effect during invulnerability (0-3 seconds).
    float invulnerabilityTimer = 0.0f;
    // Duration of invulnerability after damage (3 seconds).
    float invulnerabilityDuration = 3.0f;
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
    float climbSpeed      = 60.0f;  // pixels per second when climbing
    bool climbOnRightWall = false;  // true if latched to a wall on the right side
    // Visual fine-tune to keep sprite snug to the wall
    int climbRightDrawOffset = 5;  // move sprite ~5px right when wall is on the right
    int climbLeftDrawOffset  = 0;  // no extra shift when wall is on the left

    // Health system (hearts). `maxHearts` is the capacity; `hearts` is current health.
    int maxHearts = 3;
    int hearts    = 3;

    // Horizontal input intent (-1, 0, 1) captured each frame
    float moveIntent = 0.0f;

    // Elemental state
    Element element = Element::None;

    // Baseline (non-element) dimensions/frames captured on first setElement call
    int baseWalkWidth   = 0;
    int baseWalkHeight  = 0;
    int baseJumpWidth   = 0;
    int baseJumpHeight  = 0;
    int baseNumFrames   = 0;
    int baseJumpFrames  = 0;

    // Water / swimming state
    bool inWater     = false;
    bool swimming    = false;
    bool wasSwimming = false;
    bool justExitedWater = false;
    bool swimPressed = false;
    float swimUpSpeed   = 140.0f;
    float swimSinkSpeed = 80.0f;
    float swimRunSpeed  = 70.0f;
    float runSpeed      = 60.0f;  // Ground movement speed

    // Frame metadata for water variants
    int waterWalkFrames = 4;
    int waterJumpFrames = 4;
    int waterSwimFrames = 11;
    int waterWalkWidth  = 54;
    int waterWalkHeight = 35;
    int waterJumpWidth  = 57;
    int waterJumpHeight = 37;
    int waterSwimWidth  = 54;
    int waterSwimHeight = 36;
    float swimFrameTime = 0.1f;

    // Frame metadata for wind variants
    int windWalkFrames = 4;
    int windWalkWidth  = 56;
    int windWalkHeight = 36;
    int windJumpFrames = 6;
    int windJumpWidth  = 62;
    int windJumpHeight = 41;

    // Wind detection/state
    bool inWind = false;

    // Takes damage and triggers the damage animation and invulnerability period.
    void takeDamage();

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
    void update(float dt, const TileMap& map);

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

