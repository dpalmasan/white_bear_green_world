// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// game.h
// Main game class that manages the game loop, initialization, rendering,
// and coordination between game objects (player, enemies, camera, etc.).

#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <memory>
#include <string>
#include <vector>

#include "Config.h"
#include "Cutscene.h"
#include "Explosion.h"
#include "TileMap.h"
#include "TitleScreen.h"
#include "WorldMap.h"
#include "core/Camera.h"
#include "core/Input.h"
#include "entities/Arachnoid.h"
#include "entities/Boss.h"
#include "entities/Enemy.h"
#include "entities/Fireball.h"
#include "entities/FrenzyWolf.h"
#include "entities/IdleEnemy.h"
#include "entities/PolarBear.h"
#include "entities/RobotEnemy.h"
#include "entities/SnowRobotBoss.h"

// Main Game class.
// Manages SDL initialization, asset loading, input handling, game updates,
// and rendering. Coordinates player, enemies, camera, and collision detection.
class Game
{
   public:
    // Game running state.
    bool running = true;
    // Pause state.
    bool paused = false;

    // SDL rendering objects.
    SDL_Window* window     = nullptr;
    SDL_Renderer* renderer = nullptr;

    // Window dimensions and camera zoom.
    int windowWidth  = 640;
    int windowHeight = 640;
    float cameraZoom = 2.0f;  // Render scale; camera view = window size / zoom

    // Background texture for the current level.
    SDL_Texture* backgroundTexture = nullptr;
    SDL_Texture* menuTexture       = nullptr;  // Pause menu image

    // HUD: Health bar (hearts) texture and layout.
    SDL_Texture* heartTexture = nullptr;  // spritesheet with 2 frames: full (0), empty (1)
    int heartFrameW           = 0;        // single frame width (sheet width / 2)
    int heartFrameH           = 0;        // frame height
    int heartRows             = 1;        // number of rows for layout
    int heartCols             = 5;        // number of columns for layout
    int heartMargin           = 8;        // margin from top-left of screen
    int heartSpacing          = 6;        // spacing between hearts
    int heartPixelHeight      = 24;       // desired on-screen heart height in pixels (scaled)

    // Core game objects.
    TileMap map;                                  // The tilemap for collision and rendering.
    PolarBear polarBear;                          // The player character.
    Camera camera;                                // Camera for tracking the player.
    Input input;                                  // Centralized input handling
    std::vector<std::unique_ptr<Enemy>> enemies;  // List of active enemies.

    std::vector<Explosion> explosions;  // Active explosion effects.
    std::vector<Fireball> fireballs;    // Active enemy projectiles.
    struct PowerUp
    {
        float x  = 0.0f;
        float y  = 0.0f;
        float vy = 0.0f;  // Vertical velocity for gravity
        std::string type;
        bool collected  = false;
        float glowPhase = 0.0f;  // For pulsating glow animation
    };
    std::vector<PowerUp> powerUps;   // Active power-ups (e.g., hearts)
    std::vector<SDL_Rect> endAreas;  // End-of-area trigger rectangles

    // Path to game assets (sprites, maps, etc.).
    const std::string ASSET_PATH = "../../assets/";  // Deprecated; use config.assetPath instead
    std::string stageName        = "stage1";         // Stage to load (e.g., "stage1", "dev_stage").
    Config config;                                   // Centralized configuration

    Mix_Music* backgroundMusic = nullptr;  // Background music for the current stage.
    Mix_Music* powerUpMusic    = nullptr;  // Short cue played on pickup
    Mix_Music* bossMusic       = nullptr;  // Boss music for snow-robot intro
    Mix_Music* mapMusic        = nullptr;  // Music for world map screen

    // Sound effects
    Mix_Chunk* slashSound     = nullptr;
    Mix_Chunk* explosionSound = nullptr;

    // Shared enemy textures
    SDL_Texture* robotAttackTexture     = nullptr;
    SDL_Texture* roboFireballTexture    = nullptr;
    SDL_Texture* frenzyWolfIdleTex      = nullptr;
    SDL_Texture* frenzyWolfRunTex       = nullptr;
    SDL_Texture* arachnoidTexture       = nullptr;  // Arachnoid spider enemy
    SDL_Texture* bossSnowRobotTex       = nullptr;  // Boss texture for snow-robot
    SDL_Texture* bossRobotAttackTex     = nullptr;  // Boss attack/idle texture
    SDL_Texture* bossRobotDashTex       = nullptr;  // Boss dash attack texture (16 frames)
    SDL_Texture* bossRobotVulnerableTex = nullptr;  // Boss vulnerable texture (16 frames)
    SDL_Texture* endSceneTexture        = nullptr;  // End scene overlay
    Mix_Music* endSceneMusic            = nullptr;  // Scene music at end of area

    // Intro and title screen
    Cutscene introCutscene;
    TitleScreen titleScreen;
    bool showIntroCutscene     = true;   // Start with intro sequence
    bool showTitleScreen       = false;  // Show after intro completes
    bool inCutsceneToTitleFade = false;  // Transitioning with fade to black
    float fadeToBlackTimer     = 0.0f;
    float fadeToBlackDuration  = 1.0f;   // 1 second fade to black

    // Title screen -> stage fade sequence
    bool titleFadingOut     = false;  // Fading to black from title
    bool titleFadingIn      = false;  // Fading in to stage after title
    float titleFadeTimer    = 0.0f;   // Accumulated time for title fades
    float titleFadeDuration = 2.0f;   // 2 seconds for out and 2 for in

    // Boss management (using interface pattern)
    std::unique_ptr<Boss> boss;     // Current boss instance
    bool bossHasSpawn     = false;  // True if boss spawn tile exists
    bool bossAlive        = false;  // True while boss is active
    bool bossSlashHit     = false;  // Track if current slash already hit boss
    bool bossMusicStarted = false;  // Boss music started (one-shot)
    bool bossMusicLooped  = false;  // Boss music switched to looping after finish
    bool cameraLocked     = false;  // lock viewport while bossAlive
    int lockCamX          = 0;
    int lockCamY          = 0;

    // Camera transition for boss intro
    bool cameraTransitioning = false;  // smooth move to lock position
    bool cameraUnlocking     = false;  // smooth unlock after boss death
    int targetCamX           = 0;
    int targetCamY           = 0;

    // World Map screen
    WorldMap worldMap;

    // Initializes SDL, creates the window and renderer.
    // Returns true on success, false if initialization fails.
    bool init();

    // Loads all game assets (sprites, textures) and initializes game objects.
    void loadAssets();

    // Processes keyboard input and updates player movement/actions.
    void handleInput();

    // Updates game state by delta time (physics, animations, collision detection).
    void update(float dt);

    // Renders all game objects to the screen.
    void render();

    // Main game loop that runs until the game exits.
    void run();

    // Cleans up SDL resources and closes the window.
    void clean();

    // Pause specifically for pickup sequence; disables gameplay without showing pause menu.
    bool pauseForPickup        = false;
    float pickupMusicDelay     = 0.5f;   // Delay before playing pickup music
    float pickupMusicTimer     = 0.0f;   // Timer for pickup music delay
    bool pickupMusicStarted    = false;  // Track if pickup music has been triggered
    float pickupPostMusicDelay = 0.5f;   // Silence after music finishes
    float pickupPostMusicTimer = 0.0f;   // Timer for post-music silence
    bool returnToMapAfterPickup = false;  // If true, exit to world map after pickup sequence
    bool transitioningToMap     = false;  // Active fade-out from stage to world map
    bool endingStage      = false;   // True after touching end-of-area; shows scene and plays music
    bool endSceneShowing  = false;   // True once fade completes and scene is displayed
    float endFadeTimer    = 0.0f;    // Accumulated time for end fade
    float endFadeDuration = 2.5f;    // Fade duration in seconds (2-3s)
    float endFadeInTimer  = 0.0f;    // Accumulated time for scene fade-in
    float endFadeInDuration = 1.5f;  // Scene fade-in duration

    // World map transition fade
    bool wmFadingOut     = false;  // Fading to black before entering a stage
    bool wmFadingIn      = false;  // Fading from black after stage loads
    float wmFadeTimer    = 0.0f;
    float wmFadeDuration = 0.8f;  // Duration for fade out/in

    // Stage-to-stage transition fade (fast transitions between areas)
    bool stageFadingOut     = false;  // Fading to black before loading new stage
    bool stageFadingIn      = false;  // Fading from black after new stage loads
    float stageFadeTimer    = 0.0f;
    float stageFadeDuration = 0.5f;    // Fast fade: 0.5s out + 0.5s in = 1s total
    std::string nextStageName;         // Stage to load after fade-out completes
};
