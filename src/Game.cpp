// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// game.cpp
// Implementation of the main game class managing initialization, asset loading,
// input handling, game updates, and rendering.

#include "Game.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <algorithm>
#include <cmath>
#include <iostream>

// Initializes SDL, creates the window and renderer.
// Returns true on success, false if any initialization step fails.
bool Game::init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL init failed: " << SDL_GetError() << "\n";
        return false;
    }

    // Use nearest-neighbor to avoid seams between tiles when scaled
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

    // Create the main game window (logical base size)
    // Window is resizable to allow player to scale up
    window = SDL_CreateWindow("Polar Bear Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
        return false;
    }

    // Create an accelerated renderer with vsync for smooth scaling
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer)
    {
        // Enforce integer scaling to prevent subpixel seams when zoomed
        SDL_RenderSetIntegerScale(renderer, SDL_TRUE);
    }
    if (!renderer)
    {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << "\n";
        return false;
    }

    // Set logical render size to 640x640
    // This allows the renderer to scale to any window size while maintaining aspect ratio
    SDL_RenderSetLogicalSize(renderer, 640, 640);

    // Initialize SDL_image for PNG loading support.
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        std::cerr << "SDL_image init failed: " << IMG_GetError() << "\n";
        return false;
    }

    // Initialize SDL_mixer for audio playback.
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cerr << "SDL_mixer open audio failed: " << Mix_GetError() << "\n";
        return false;
    }
    // Optional: set music volume (0-128). Keep moderate volume.
    Mix_VolumeMusic(config.musicVolume);

    return true;
}

// Loads all game assets (sprites, textures) and initializes the player and enemies.
void Game::loadAssets()
{
    // If starting on world map, load only world map assets and return
    if (config.showWorldMap)
    {
        if (!worldMap.load(renderer, config.assetPath))
        {
            std::cerr << "Failed to load world map assets (world-map.png, map-cursor.png) from '"
                      << config.assetPath << "'\n";
        }
        worldMap.debug = config.worldMapDebug;
        // Camera view is already set in init; nothing else to load
        return;
    }

    // Build stage path dynamically (e.g., "stage1" or "dev_stage")
    const std::string stagePath = stageName + "/";

    // Load the map from JSON
    if (!map.loadFromJSON(config.assetPath + stagePath + "map.json"))
    {
        std::cerr << "Failed to load map.json from stage '" << stageName << "'\n";
        return;
    }

    // Load the spritesheet for the tilemap
    if (!map.loadSpritesheet(renderer, config.assetPath + stagePath + "spritesheet.png"))
    {
        std::cerr << "Failed to load spritesheet.png\n";
        return;
    }

    // Load shared enemy/attack textures
    robotAttackTexture = IMG_LoadTexture(renderer, (config.assetPath + "robot-attack.png").c_str());
    if (!robotAttackTexture)
        std::cerr << "Failed to load robot-attack.png: " << IMG_GetError() << "\n";
    roboFireballTexture = IMG_LoadTexture(renderer, (config.assetPath + "robo-cannon.png").c_str());
    if (!roboFireballTexture)
        std::cerr << "Failed to load robo-cannon.png: " << IMG_GetError() << "\n";

    frenzyWolfIdleTex =
        IMG_LoadTexture(renderer, (config.assetPath + "frenzy_wolf-idle.png").c_str());
    if (!frenzyWolfIdleTex)
        std::cerr << "Failed to load frenzy_wolf-idle.png: " << IMG_GetError() << "\n";
    frenzyWolfRunTex =
        IMG_LoadTexture(renderer, (config.assetPath + "frenzy_wolf-attack.png").c_str());
    if (!frenzyWolfRunTex)
        std::cerr << "Failed to load frenzy_wolf-attack.png: " << IMG_GetError() << "\n";

    bossSnowRobotTex =
        IMG_LoadTexture(renderer, (config.assetPath + "boss-robot-deactivated.png").c_str());
    if (!bossSnowRobotTex)
        std::cerr << "Failed to load boss-robot-deactivated.png: " << IMG_GetError() << "\n";
    bossRobotAttackTex =
        IMG_LoadTexture(renderer, (config.assetPath + "boss-robot-attack.png").c_str());
    if (!bossRobotAttackTex)
        std::cerr << "Failed to load boss-robot-attack.png: " << IMG_GetError() << "\n";
    bossRobotDashTex =
        IMG_LoadTexture(renderer, (config.assetPath + "boss-robot-dash.png").c_str());
    if (!bossRobotDashTex)
        std::cerr << "Failed to load boss-robot-dash.png: " << IMG_GetError() << "\n";
    bossRobotVulnerableTex =
        IMG_LoadTexture(renderer, (config.assetPath + "boss-robot-vulnerable.png").c_str());
    if (!bossRobotVulnerableTex)
        std::cerr << "Failed to load boss-robot-vulnerable.png: " << IMG_GetError() << "\n";

    // Load sound effects
    slashSound = Mix_LoadWAV((config.assetPath + "sfx/slash.ogg").c_str());
    if (!slashSound)
        std::cerr << "Failed to load slash.ogg: " << Mix_GetError() << "\n";
    explosionSound = Mix_LoadWAV((config.assetPath + "sfx/explosion.ogg").c_str());
    if (!explosionSound)
        std::cerr << "Failed to load explosion.ogg: " << Mix_GetError() << "\n";

    // Apply zoom by scaling the renderer; camera view is window size divided by zoom
    SDL_RenderSetScale(renderer, cameraZoom, cameraZoom);

    // Initialize camera view based on zoomed logical size
    camera.width  = static_cast<int>(windowWidth / cameraZoom);
    camera.height = static_cast<int>(windowHeight / cameraZoom);

    // Set the world size for camera bounds to the full map dimensions.
    // Map coordinates are 0-based, so actual pixel size is (width) * tileSize
    camera.setWorldSize(map.width * map.tileSize, map.height * map.tileSize);

    // Load and configure polar bear sprite sheet.
    polarBear.spriteWidth  = 51;
    polarBear.spriteHeight = 40;
    polarBear.numFrames    = 4;
    polarBear.frameTime    = 0.15f;
    polarBear.loadTexture(renderer, config.assetPath + "bear.png");
    polarBear.loadJumpTexture(renderer, config.assetPath + "bear-jump.png");
    polarBear.loadAttackTexture(renderer, config.assetPath + "bear-attack.png");
    polarBear.loadSlashTexture(renderer, config.assetPath + "slash.png");
    // Enable climb skill and texture if configured
    polarBear.canClimb = config.enableClimbSkill;
    if (polarBear.canClimb)
    {
        polarBear.loadClimbTexture(renderer, config.assetPath + std::string("bear-climbing.png"));
    }

    // Set initial player position and state. If a polar_bear_spawn marker exists, use it.
    polarBear.x        = 100;
    polarBear.y        = 50;
    polarBear.onGround = false;

    if (const Tile *spawn = map.getPolarBearSpawnTile())
    {
        polarBear.x        = spawn->x * map.tileSize;
        polarBear.y        = spawn->y * map.tileSize - polarBear.spriteHeight;
        polarBear.onGround = false;
    }

    // Cache boss spawn location and instantiate boss
    {
        auto bossTiles = map.getBossTiles();
        if (!bossTiles.empty())
        {
            const Tile *bt = bossTiles.front();
            float spawnX   = bt->x * map.tileSize;
            float spawnY   = bt->y * map.tileSize - 48;  // account for render offset

            // Instantiate boss based on type
            if (bt->boss == "snow-robot")
            {
                boss = std::make_unique<SnowRobotBoss>();
                boss->loadAssets(renderer, config.assetPath);
                boss->setPosition(spawnX, spawnY);
                bossHasSpawn = true;
            }
        }
    }

    // Load HUD heart texture (single-frame). We'll darken it for empty hearts.
    {
        SDL_Surface *hbSurf = IMG_Load((config.assetPath + "health_bar.png").c_str());
        if (hbSurf)
        {
            heartTexture = SDL_CreateTextureFromSurface(renderer, hbSurf);
            int texW = 0, texH = 0;
            if (heartTexture)
            {
                SDL_QueryTexture(heartTexture, nullptr, nullptr, &texW, &texH);
                if (texW > 0)
                {
                    heartFrameW = texW;  // single frame
                    heartFrameH = texH;
                }
            }
            SDL_FreeSurface(hbSurf);
            if (!heartTexture)
            {
                std::cerr << "Failed to create heart texture: " << SDL_GetError() << "\n";
            }
        }
        else
        {
            std::cerr << "Failed to load health_bar.png: " << IMG_GetError() << "\n";
        }
    }

    // Load background texture for the level
    SDL_Surface *bgSurface = IMG_Load((config.assetPath + "background.png").c_str());
    if (bgSurface)
    {
        backgroundTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
        SDL_FreeSurface(bgSurface);
        if (!backgroundTexture)
        {
            std::cerr << "Failed to create background texture: " << SDL_GetError() << "\n";
        }
    }
    else
    {
        std::cerr << "Failed to load background.png: " << IMG_GetError() << "\n";
    }

    // Load menu texture for pause screen
    SDL_Surface *menuSurface = IMG_Load((config.assetPath + "menu.png").c_str());
    if (menuSurface)
    {
        menuTexture = SDL_CreateTextureFromSurface(renderer, menuSurface);
        SDL_FreeSurface(menuSurface);
        if (!menuTexture)
        {
            std::cerr << "Failed to create menu texture: " << SDL_GetError() << "\n";
        }
    }
    else
    {
        std::cerr << "Failed to load menu.png: " << IMG_GetError() << "\n";
    }

    // Spawn enemies from map tiles marked with "enemy" attribute
    auto spawnTiles = map.getEnemySpawnTiles();
    for (const auto *tile : spawnTiles)
    {
        // Convert tile coordinates to world coordinates
        float worldX      = tile->x * map.tileSize;
        float spawnOffset = 1.0f;  // Spawn 1 pixel above for proper collision detection

        if (tile->enemyType == "robot")
        {
            auto robo = std::make_unique<RobotEnemy>();
            // Position: top of tile below (y+1) minus sprite height minus offset
            robo->x        = worldX;
            robo->y        = (tile->y + 1) * map.tileSize - robo->height - spawnOffset;
            robo->vy       = 0.0f;
            robo->onGround = false;  // Let physics handle landing
            robo->loadTexture(renderer, config.assetPath + "robot-idle.png");

            // Configure attack animation (6 frames, ~0.1s each)
            if (robotAttackTexture)
            {
                int texW = 0, texH = 0;
                SDL_QueryTexture(robotAttackTexture, nullptr, nullptr, &texW, &texH);
                int frames = 6;
                float ft   = 0.1f;
                // Provide texture and frame timing to robot enemy
                robo->setAttackTexture(robotAttackTexture, frames, ft);
            }

            enemies.push_back(std::move(robo));
        }
        else if (tile->enemyType == "wolf")
        {
            auto wolf = std::make_unique<FrenzyWolf>();
            // Assign shared textures
            if (frenzyWolfIdleTex)
                wolf->setIdleTexture(frenzyWolfIdleTex);
            if (frenzyWolfRunTex)
                wolf->setRunTexture(frenzyWolfRunTex, 0.08f);
            // Position just above the tile below
            wolf->x        = worldX;
            wolf->y        = (tile->y + 1) * map.tileSize - wolf->height - spawnOffset;
            wolf->vy       = 0.0f;
            wolf->onGround = false;

            enemies.push_back(std::move(wolf));
        }
    }
    // Cache end-of-area triggers as rectangles
    {
        endAreas.clear();
        auto endTiles = map.getEndOfAreaTiles();
        for (const auto *tile : endTiles)
        {
            SDL_Rect r{tile->x * map.tileSize, tile->y * map.tileSize, map.tileSize, map.tileSize};
            endAreas.push_back(r);
        }
    }

    // Load power-up cue music
    {
        const std::string puPath = config.assetPath + std::string("music/power_up.ogg");
        powerUpMusic             = Mix_LoadMUS(puPath.c_str());
        if (!powerUpMusic)
        {
            std::cerr << "Failed to load power_up.ogg: " << Mix_GetError() << "\n";
        }
    }

    // Load end scene assets for stage1
    if (stageName == "snowy-cliffs")
    {
        // End scene texture
        SDL_Surface *endSurf = IMG_Load((config.assetPath + "end_stage1_scene.png").c_str());
        if (endSurf)
        {
            endSceneTexture = SDL_CreateTextureFromSurface(renderer, endSurf);
            SDL_FreeSurface(endSurf);
            if (!endSceneTexture)
            {
                std::cerr << "Failed to create end scene texture: " << SDL_GetError() << "\n";
            }
        }
        else
        {
            std::cerr << "Failed to load end_stage1_scene.png: " << IMG_GetError() << "\n";
        }
        // End scene music
        const std::string endMusicPath = config.assetPath + std::string("music/is_there_hope.ogg");
        endSceneMusic                  = Mix_LoadMUS(endMusicPath.c_str());
        if (!endSceneMusic)
        {
            std::cerr << "Failed to load end scene music: " << Mix_GetError() << "\n";
        }
    }

    // Load and play background music for the stage (looping).
    // Music is located in assets/music/ folder (shared across stages)
    // Skip autoplay for boss maps; boss music will start with intro
    if (stageName != "snowy-cliffs-boss")
    {
        std::string musicPath;
        if (stageName == "snowy-cliffs-boss")
        {
            musicPath = config.assetPath + std::string("music/snowy_cliffs_boss.ogg");
        }
        else
        {
            musicPath = config.assetPath + std::string("music/snowy_cliffs.ogg");
        }
        backgroundMusic = Mix_LoadMUS(musicPath.c_str());
        if (!backgroundMusic)
        {
            std::cerr << "Failed to load music '" << musicPath << "': " << Mix_GetError() << "\n";
        }
        else
        {
            if (Mix_PlayMusic(backgroundMusic, -1) < 0)
            {
                std::cerr << "Failed to play music: " << Mix_GetError() << "\n";
            }
        }
    }
    else
    {
        // Preload boss theme; do not play until intro starts
        const std::string bossMusicPath = config.assetPath + std::string("music/boss_theme.ogg");
        bossMusic                       = Mix_LoadMUS(bossMusicPath.c_str());
        if (!bossMusic)
        {
            std::cerr << "Failed to load boss music '" << bossMusicPath << "': " << Mix_GetError()
                      << "\n";
        }
    }
}

// Processes keyboard input and updates player control state.
// Processes keyboard input and updates player control state.
void Game::handleInput()
{
    // Set world map mode for input manager
    input.setWorldMapActive(config.showWorldMap);
    
    // Handle all SDL events and keyboard state
    input.handleEvents(running);

    if (config.showWorldMap)
    {
        // Handle world map selection input
        if (input.isSelectPressed())
        {
            if (worldMap.currentIndex >= 0 &&
                worldMap.currentIndex < (int)worldMap.locations.size())
            {
                const auto &loc = worldMap.locations[worldMap.currentIndex];
                if (loc.name == "Snowy Cliffs")
                {
                    // Begin fade-out transition; actual load happens after fade completes
                    wmFadingOut = true;
                    wmFadingIn  = false;
                    wmFadeTimer = 0.0f;
                }
            }
        }
        // Reset frame events and return
        input.resetFrameEvents();
        return;
    }

    // Disable input during boss intro or while boss explicitly disables inputs
    if (boss && (boss->isIntroActive() || boss->shouldDisableInputs()))
    {
        input.resetFrameEvents();
        return;
    }

    // Disable all inputs during power-up pickup sequence
    if (pauseForPickup)
    {
        input.resetFrameEvents();
        return;
    }

    // Disable all input (movement and attack) during knockback
    if (polarBear.isKnockedBack)
    {
        input.resetFrameEvents();
        return;
    }

    // Reset intent each frame when not knocked back
    polarBear.moveIntent  = 0.0f;
    polarBear.climbIntent = 0.0f;

    // Left movement (A key)
    if (input.isMovingLeft())
    {
        polarBear.moveIntent = -1.0f;
        if (!polarBear.isAttacking)
        {
            polarBear.facingRight = false;
        }
    }

    // Right movement (D key)
    if (input.isMovingRight())
    {
        polarBear.moveIntent = 1.0f;
        if (!polarBear.isAttacking)
        {
            polarBear.facingRight = true;
        }
    }

    // Climbing input: W/S when adjacent to climbable tile; cling when adjacent
    bool adjacentClimbable = false;
    if (polarBear.canClimb)
    {
        float topY   = polarBear.y + 4.0f;
        float midY   = polarBear.y + polarBear.spriteHeight / 2.0f;
        float botY   = polarBear.y + polarBear.spriteHeight - 4.0f;
        float leftX  = polarBear.x - 1.0f;
        float rightX = polarBear.x + polarBear.spriteWidth + 1;

        bool leftAdjAny = map.isClimbableAtWorld(leftX, topY) ||
                          map.isClimbableAtWorld(leftX, midY) ||
                          map.isClimbableAtWorld(leftX, botY);
        bool rightAdjAny = map.isClimbableAtWorld(rightX, topY) ||
                           map.isClimbableAtWorld(rightX, midY) ||
                           map.isClimbableAtWorld(rightX, botY);
        adjacentClimbable = leftAdjAny || rightAdjAny;

        if (adjacentClimbable)
        {
            if (!polarBear.isClimbing)
            {
                if (rightAdjAny && !leftAdjAny)
                    polarBear.climbOnRightWall = true;
                else if (leftAdjAny && !rightAdjAny)
                    polarBear.climbOnRightWall = false;
            }
            if (input.isClimbingUp())
                polarBear.climbIntent = -1.0f;
            else if (input.isClimbingDown())
                polarBear.climbIntent = 1.0f;
            else
                polarBear.climbIntent = 0.0f;

            if (!polarBear.isClimbing)
                polarBear.isClimbing = (polarBear.climbIntent != 0.0f);
            else
                polarBear.isClimbing = true;

            polarBear.facingRight = polarBear.climbOnRightWall;
        }
        else
        {
            polarBear.isClimbing  = false;
            polarBear.climbIntent = 0.0f;
        }
    }

    // Jump (J key)
    if (input.isJumping())
    {
        if (polarBear.onGround)
        {
            polarBear.vy       = -336.0f;
            polarBear.onGround = false;
        }
        else if (polarBear.isClimbing)
        {
            polarBear.isClimbing  = false;
            polarBear.climbIntent = 0.0f;
            if (polarBear.vy < 40.0f)
                polarBear.vy = 40.0f;
        }
    }

    // Attack (K key)
    if (input.isAttacking())
    {
        polarBear.startAttack();
        bossSlashHit = false;  // Reset boss hit flag for new slash
        if (slashSound)
            Mix_PlayChannel(-1, slashSound, 0);
    }

    // Pause (ESC key) - toggle pause state (disabled during ending scene)
    if (input.isPausePressed() && !endingStage)
    {
        paused = !paused;
        if (paused)
        {
            Mix_VolumeMusic(config.pauseMusicVolume);
        }
        else
        {
            Mix_VolumeMusic(config.musicVolume);
        }
    }

    // Reset single-frame events
    input.resetFrameEvents();
}


// Updates game state: player physics, enemy behavior, collisions, and effects.
void Game::update(float dt)
{
    if (config.showWorldMap)
    {
        worldMap.update(dt);
        if (wmFadingOut)
        {
            wmFadeTimer += dt;
            if (wmFadeTimer >= wmFadeDuration)
            {
                // Execute transition: leave map, load stage, start fade-in
                worldMap.clean();
                config.showWorldMap = false;
                stageName           = "snowy-cliffs";

                // Reset gameplay state
                enemies.clear();
                fireballs.clear();
                explosions.clear();
                powerUps.clear();
                endAreas.clear();

                loadAssets();
                // switch fades
                wmFadingOut = false;
                wmFadingIn  = true;
                wmFadeTimer = 0.0f;
                return;
            }
        }
        return;
    }
    // Skip gameplay physics when paused; during special pauses, update sequence timers
    if (paused)
    {
        if (pauseForPickup)
        {
            // State 1: Wait for initial delay before playing music
            if (!pickupMusicStarted)
            {
                pickupMusicTimer += dt;
                if (pickupMusicTimer >= pickupMusicDelay)
                {
                    // Play pickup music after delay
                    pickupMusicStarted = true;
                    if (powerUpMusic)
                    {
                        if (Mix_PlayMusic(powerUpMusic, 1) < 0)
                            std::cerr << "Failed to play power_up music: " << Mix_GetError()
                                      << "\n";
                    }
                }
            }
            // State 2 & 3: Music is playing or finished, wait for post-music silence
            else
            {
                // Check if music finished
                if (Mix_PlayingMusic() == 0)
                {
                    pickupPostMusicTimer += dt;
                    // After post-music silence, resume game
                    if (pickupPostMusicTimer >= pickupPostMusicDelay)
                    {
                        pauseForPickup       = false;
                        paused               = false;
                        pickupMusicStarted   = false;
                        pickupPostMusicTimer = 0.0f;
                        // Resume background music
                        if (backgroundMusic)
                        {
                            if (Mix_PlayMusic(backgroundMusic, -1) < 0)
                                std::cerr << "Failed to resume background music: " << Mix_GetError()
                                          << "\n";
                            // Restore volume
                            Mix_VolumeMusic(96);
                        }
                    }
                }
            }
        }
        else if (boss && boss->isIntroActive())
        {
            // Advance boss intro animation frames while paused
            boss->updateIntro(dt);
        }
        else if (endingStage)
        {
            // Advance fade timer; when complete, show scene and play end music
            if (!endSceneShowing)
            {
                endFadeTimer += dt;
                if (endFadeTimer >= endFadeDuration)
                {
                    endSceneShowing = true;
                    endFadeInTimer  = 0.0f;
                    // Ensure previous music is stopped; then play end scene music
                    Mix_HaltMusic();
                    if (endSceneMusic)
                    {
                        if (Mix_PlayMusic(endSceneMusic, 1) < 0)
                            std::cerr << "Failed to play end scene music: " << Mix_GetError()
                                      << "\n";
                    }
                }
            }
            else
            {
                // Advance fade-in timer for scene
                endFadeInTimer += dt;
                if (endFadeInTimer >= endFadeInDuration)
                {
                    endFadeInTimer = endFadeInDuration;
                }
            }
        }
        // Boss death handling during pause (none currently)
        else if (boss && boss->isDead())
        {
            // No progression while paused
        }
        return;
    }

    // Helper to shrink AABB to approximate opaque pixels (reduces false hits on transparent areas)
    auto shrinkRect = [](const SDL_Rect &r, float insetFrac)
    {
        SDL_Rect s = r;
        int insetX = static_cast<int>(r.w * insetFrac);
        int insetY = static_cast<int>(r.h * insetFrac);
        s.x += insetX;
        s.y += insetY;
        s.w = std::max(0, r.w - 2 * insetX);
        s.h = std::max(0, r.h - 2 * insetY);
        return s;
    };

    // Update player only if not in boss intro; keep updating during death to let animations finish
    bool bossFreezePlayer = boss && boss->isIntroActive();

    // If boss disables inputs, clear movement intent but keep updating for animation completion
    if (boss && boss->shouldDisableInputs())
    {
        polarBear.moveIntent = 0.0f;
        polarBear.vx         = 0.0f;
    }

    if (!bossFreezePlayer)
    {
        polarBear.update(dt, map);
    }

    // Update camera: handle transition, lock, or follow player
    if (cameraTransitioning)
    {
        // Move camera toward target at bear speed (75 px/s)
        float speed = 75.0f;
        float dx    = targetCamX - camera.x;
        float dy    = targetCamY - camera.y;
        float dist  = std::sqrt(dx * dx + dy * dy);

        if (dist < speed * dt || dist < 5.0f)
        {
            // Reached target - if boss intro pending, lock and start intro
            if (boss && bossAlive && !boss->isIntroActive() && !boss->isIntroDone())
            {
                camera.x            = targetCamX;
                camera.y            = targetCamY;
                lockCamX            = targetCamX;
                lockCamY            = targetCamY;
                cameraTransitioning = false;
                cameraLocked        = true;

                // Now start boss intro
                boss->startIntro();
                // Cancel any ongoing player attack and freeze motion intents
                polarBear.currentAttack.reset();
                polarBear.isAttacking = false;
                polarBear.moveIntent  = 0.0f;
                polarBear.vx          = 0.0f;
                polarBear.vy          = 0.0f;
                // Start boss music only if boss is still alive
                if (bossAlive)
                {
                    Mix_HaltMusic();
                    if (bossMusic)
                    {
                        if (Mix_PlayMusic(bossMusic, 0) < 0)
                            std::cerr << "Failed to play boss music: " << Mix_GetError() << "\n";
                        else
                        {
                            bossMusicStarted = true;
                            bossMusicLooped  = false;
                        }
                    }
                }
            }
            else
            {
                // Transition complete
                if (cameraUnlocking)
                {
                    // Unlock complete, resume normal follow and re-enable inputs
                    cameraLocked    = false;
                    cameraUnlocking = false;
                    if (boss)
                        boss->enableInputs();
                }
                cameraTransitioning = false;
            }
        }
        else
        {
            // Move toward target
            float moveX = (dx / dist) * speed * dt;
            float moveY = (dy / dist) * speed * dt;
            camera.x += static_cast<int>(moveX);
            camera.y += static_cast<int>(moveY);
        }
    }
    else if (cameraLocked)
    {
        camera.x = lockCamX;
        camera.y = lockCamY;
    }
    else
    {
        camera.follow(polarBear.x + polarBear.spriteWidth / 2.0f,
                      polarBear.y + polarBear.spriteHeight / 2.0f);
    }

    // Trigger boss intro when within 8 tiles of the boss
    if (boss && bossHasSpawn && !boss->isIntroActive() && !boss->isIntroDone() &&
        !cameraTransitioning)
    {
        float bearCX = polarBear.x + polarBear.spriteWidth * 0.5f;
        float bearCY = polarBear.y + polarBear.spriteHeight * 0.5f;
        float bossX  = boss->getX() + 32.0f;
        float bossY  = boss->getY() + 32.0f;
        float dx     = bearCX - bossX;
        float dy     = bearCY - bossY;
        float dist   = std::sqrt(dx * dx + dy * dy);
        if (dist <= 8.0f * map.tileSize)
        {
            // Mark boss alive and initiate smooth camera transition
            bossAlive           = true;
            cameraTransitioning = true;
            targetCamX = std::max(0, std::min(camera.x + 16, camera.worldWidth - camera.width));
            targetCamY = camera.y;
        }
    }

    // Advance boss intro animation frames while active
    if (boss && boss->isIntroActive())
    {
        boss->updateIntro(dt);
    }

    // Boss AI: runs after intro completes (keep running during death states for fade effects)
    if (boss && boss->isIntroDone())
    {
        // Update boss AI (handles all state logic internally)
        boss->updateAI(dt, map, polarBear);

        // Spawn any projectiles created by the boss during updateAI
        // Cast to SnowRobotBoss to access spawn methods (in future, could be virtual in Boss interface)
        if (dynamic_cast<SnowRobotBoss*>(boss.get()))
        {
            SnowRobotBoss* snowBoss = dynamic_cast<SnowRobotBoss*>(boss.get());
            if (snowBoss)
            {
                snowBoss->spawnProjectiles(fireballs);
                snowBoss->spawnExplosions(explosions);
            }
        }

        // Check if boss requested music stop (when taking last hit)
        if (boss->shouldStopMusic() && bossMusicStarted)
        {
            Mix_PauseMusic();
            bossMusicStarted = false;
            bossMusicLooped  = false;
        }

        // Check if boss died and handle camera unlock (only check when fade is done)
        if (boss->isDead() && boss->getFadeAlpha() == 0 && bossAlive)
        {
            // Boss just died and fade is complete, trigger camera unlock and cleanup
            bossAlive    = false;
            bossHasSpawn = false;

            // Spawn heart power-up at boss location
            PowerUp heart;
            heart.x         = boss->getX();
            heart.y         = boss->getY();
            heart.vy        = 0.0f;
            heart.type      = "heart";
            heart.collected = false;
            heart.glowPhase = 0.0f;
            powerUps.push_back(heart);

            // Start smooth camera unlock transition to player position
            cameraUnlocking     = true;
            cameraTransitioning = true;
            float playerCenterX = polarBear.x + polarBear.spriteWidth / 2.0f;
            float playerCenterY = polarBear.y + polarBear.spriteHeight / 2.0f;
            targetCamX          = static_cast<int>(playerCenterX) - camera.width / 2;
            targetCamY          = static_cast<int>(playerCenterY) - camera.height / 2;
            // Clamp to world bounds
            if (targetCamX < 0)
                targetCamX = 0;
            if (targetCamY < 0)
                targetCamY = 0;
            if (targetCamX + camera.width > camera.worldWidth)
                targetCamX = camera.worldWidth - camera.width;
            if (targetCamY + camera.height > camera.worldHeight)
                targetCamY = camera.worldHeight - camera.height;
        }
    }

    // Once boss music finishes the initial play, switch to looping without restarting
    if (bossAlive && bossMusicStarted && !bossMusicLooped && Mix_PlayingMusic() == 0)
    {
        if (bossMusic)
        {
            if (Mix_PlayMusic(bossMusic, -1) < 0)
                std::cerr << "Failed to loop boss music: " << Mix_GetError() << "\n";
            else
                bossMusicLooped = true;
        }
    }

    // Update all active enemies.
    for (auto &e : enemies)
    {
        if (!e || !e->alive)
            continue;

        // Only update enemies that are in the camera viewport
        if (!camera.isInViewport(e->x, e->y, e->width, e->height))
            continue;

        if (auto robo = dynamic_cast<RobotEnemy *>(e.get()))
        {
            robo->tickAI(dt, map, polarBear, fireballs, roboFireballTexture);
        }
        else if (auto wolf = dynamic_cast<FrenzyWolf *>(e.get()))
        {
            wolf->tickAI(dt, map, polarBear);
        }

        e->update(dt, map);
    }

    // Update explosion animations.
    for (auto &ex : explosions)
        ex.update(dt);
    // Remove finished explosions.
    explosions.erase(std::remove_if(explosions.begin(), explosions.end(),
                                    [](const Explosion &e) { return e.done(); }),
                     explosions.end());

    // Slash collision detection: check if the slash hits any enemies.
    SDL_Rect slashRect;
    if (polarBear.getAttackWorldRect(slashRect))
    {
        for (auto &e : enemies)
        {
            if (!e->alive)
                continue;
            SDL_Rect er      = e->getAABB();
            SDL_Rect erTight = shrinkRect(er, 0.10f);
            // AABB intersection test: check if slash and enemy rectangles overlap.
            if (!(slashRect.x + slashRect.w < erTight.x || erTight.x + erTight.w < slashRect.x ||
                  slashRect.y + slashRect.h < erTight.y || erTight.y + erTight.h < slashRect.y))
            {
                // Hit detected: mark enemy as dead and create explosion effect.
                e->alive = false;
                Explosion ex;
                ex.x     = e->x + e->width / 2;
                ex.y     = e->y + e->height / 2;
                ex.timer = 0.0f;
                explosions.push_back(ex);
                if (explosionSound)
                    Mix_PlayChannel(-1, explosionSound, 0);
            }
        }
        // Remove dead enemies from the active list.
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                                     [](const std::unique_ptr<Enemy> &e) { return !e->alive; }),
                      enemies.end());

        // Check slash collision with boss (regardless of vulnerability) - only once per slash
        if (boss && bossHasSpawn && !boss->isDead() && !bossSlashHit)
        {
            SDL_Rect bossRect;
            boss->getCollisionRect(bossRect);
            SDL_Rect bossTight = shrinkRect(bossRect, 0.10f);
            if (!(slashRect.x + slashRect.w < bossTight.x ||
                  bossTight.x + bossTight.w < slashRect.x ||
                  slashRect.y + slashRect.h < bossTight.y ||
                  bossTight.y + bossTight.h < slashRect.y))
            {
                bossSlashHit = true;  // Mark this slash as having hit
                // Hit detected - let boss decide if damage applies (plays metal clash if not
                // vulnerable)
                bool wasVulnerable = boss->isVulnerable();
                boss->takeDamage(1);

                // Only create explosion if boss was vulnerable
                if (wasVulnerable)
                {
                    Explosion ex;
                    ex.x     = boss->getX() + 32.0f;
                    ex.y     = boss->getY() + 32.0f;
                    ex.timer = 0.0f;
                    explosions.push_back(ex);
                    if (explosionSound)
                        Mix_PlayChannel(-1, explosionSound, 0);
                }

                // Freeze player if boss died
                if (boss->isDead())
                {
                    polarBear.currentAttack.reset();
                    polarBear.isAttacking = false;
                    polarBear.moveIntent  = 0.0f;
                    polarBear.vx          = 0.0f;
                    polarBear.vy          = 0.0f;
                }
            }
        }
    }

    // Enemy-bear collision detection: check if any enemy touches the bear.
    SDL_Rect bearRect{static_cast<int>(polarBear.x), static_cast<int>(polarBear.y),
                      polarBear.spriteWidth, polarBear.spriteHeight};
    SDL_Rect bearTight = shrinkRect(bearRect, 0.10f);

    // Boss collision: if robot touches the bear, damage him (skip during intro or death phases)
    if (boss && bossHasSpawn && !boss->isIntroActive() && boss->canDamagePlayer())
    {
        SDL_Rect bossRect;
        boss->getCollisionRect(bossRect);
        SDL_Rect bossTight = shrinkRect(bossRect, 0.10f);
        bool bossHit =
            !(bearTight.x + bearTight.w < bossTight.x || bossTight.x + bossTight.w < bearTight.x ||
              bearTight.y + bearTight.h < bossTight.y || bossTight.y + bossTight.h < bearTight.y);
        if (bossHit)
        {
            polarBear.takeDamage();
        }
    }

    // End-of-area trigger collision: touching ends the stage
    if (!endingStage)
    {
        for (const auto &r : endAreas)
        {
            bool endHit = !(bearTight.x + bearTight.w < r.x || r.x + r.w < bearTight.x ||
                            bearTight.y + bearTight.h < r.y || r.y + r.h < bearTight.y);
            if (endHit)
            {
                endingStage     = true;
                endSceneShowing = false;
                endFadeTimer    = 0.0f;
                paused          = true;
                // Fade out current music over the fade duration (2-3 seconds)
                int ms = static_cast<int>(endFadeDuration * 1000.0f);
                if (Mix_PlayingMusic())
                    Mix_FadeOutMusic(ms);
                break;
            }
        }
    }
    for (auto &e : enemies)
    {
        if (!e->alive)
            continue;
        SDL_Rect er      = e->getAABB();
        SDL_Rect erTight = shrinkRect(er, 0.10f);
        // AABB intersection test: check if bear and enemy rectangles overlap.
        if (!(bearTight.x + bearTight.w < erTight.x || erTight.x + erTight.w < bearTight.x ||
              bearTight.y + bearTight.h < erTight.y || erTight.y + erTight.h < bearTight.y))
        {
            // Collision detected: damage the bear.
            polarBear.takeDamage();
        }
    }

    // Update fireballs and check collisions with player
    // Remove boss-origin fireballs once boss cannot damage the player
    if (boss && !boss->canDamagePlayer())
    {
        for (auto &fbc : fireballs)
        {
            if (fbc.fromBoss)
                fbc.alive = false;
        }
    }
    for (auto &fb : fireballs)
    {
        fb.update(dt, map);
        if (!fb.alive)
            continue;

        SDL_Rect fbRect{static_cast<int>(fb.x), static_cast<int>(fb.y), fb.width, fb.height};
        SDL_Rect fbTight = shrinkRect(fbRect, 0.25f);
        bool hit = !(fbTight.x + fbTight.w < bearTight.x || bearTight.x + bearTight.w < fbTight.x ||
                     fbTight.y + fbTight.h < bearTight.y || bearTight.y + bearTight.h < fbTight.y);
        if (hit)
        {
            fb.alive = false;
            // Suppress boss-origin damage when boss is in death phases
            bool suppressBossDamage = boss && !boss->canDamagePlayer() && fb.fromBoss;
            if (!suppressBossDamage)
                polarBear.takeDamage();
        }
    }
    fireballs.erase(std::remove_if(fireballs.begin(), fireballs.end(),
                                   [](const Fireball &f) { return !f.alive; }),
                    fireballs.end());

    // Update power-up glow animation and handle collision (hearts)
    for (auto &p : powerUps)
    {
        if (!p.collected)
        {
            p.glowPhase += dt * 2.0f;  // faster pulse
            if (p.glowPhase > 1000.0f)
                p.glowPhase = 0.0f;

            // Apply gravity
            p.vy += 840.0f * dt;  // Same gravity as player
            p.y += p.vy * dt;

            // Ground collision
            float bottom = p.y + map.tileSize;
            int tileY    = static_cast<int>(bottom / map.tileSize);
            int tileX    = static_cast<int>((p.x + map.tileSize / 2) / map.tileSize);
            if (tileY >= 0 && tileY < map.height && tileX >= 0 && tileX < map.width)
            {
                if (map.isSolidAtWorld(p.x + map.tileSize / 2, bottom))
                {
                    p.y  = tileY * map.tileSize - map.tileSize;
                    p.vy = 0.0f;
                }
            }
        }
        if (p.collected)
            continue;
        if (p.type == "heart")
        {
            SDL_Rect pr{static_cast<int>(p.x), static_cast<int>(p.y), map.tileSize, map.tileSize};
            bool hit = !(bearTight.x + bearTight.w < pr.x || pr.x + pr.w < bearTight.x ||
                         bearTight.y + bearTight.h < pr.y || pr.y + pr.h < bearTight.y);
            if (hit)
            {
                p.collected = true;
                polarBear.maxHearts += 1;
                polarBear.hearts = polarBear.maxHearts;
                // Pause game and prepare to play pickup cue after delay
                pauseForPickup       = true;
                pickupMusicTimer     = 0.0f;
                pickupMusicStarted   = false;
                pickupPostMusicTimer = 0.0f;
                paused               = true;
                // Stop current music; new music will play after delay
                Mix_HaltMusic();
            }
        }
    }
}

// Renders all game objects: tilemap, player, slash, enemies, and effects.
void Game::render()
{
    // Clear the screen with a dark blue background.
    SDL_SetRenderDrawColor(renderer, 50, 50, 100, 255);
    SDL_RenderClear(renderer);

    if (config.showWorldMap)
    {
        // Render world map scaled to the renderer's logical size (fit full screen)
        int lw = 0, lh = 0;
        SDL_RenderGetLogicalSize(renderer, &lw, &lh);
        if (lw <= 0 || lh <= 0)
        {
            // Fallback to camera view if logical size not set
            lw = camera.width;
            lh = camera.height;
        }
        worldMap.render(renderer, lw, lh);
        // Draw fade-out overlay while transitioning off world map
        if (wmFadingOut)
        {
            float t     = std::min(wmFadeTimer / wmFadeDuration, 1.0f);
            Uint8 alpha = static_cast<Uint8>(255 * t);  // clear -> black
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
            SDL_Rect full{0, 0, lw, lh};
            SDL_RenderFillRect(renderer, &full);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
        SDL_RenderPresent(renderer);
        return;
    }

    // Render the background image if available
    if (backgroundTexture)
    {
        // Query texture dimensions
        int texW = 0, texH = 0;
        SDL_QueryTexture(backgroundTexture, nullptr, nullptr, &texW, &texH);

        // Calculate scaling to fill the camera view
        float scaleX = static_cast<float>(camera.width) / texW;
        float scaleY = static_cast<float>(camera.height) / texH;
        float scale  = std::max(scaleX, scaleY);

        int scaledW = static_cast<int>(texW * scale);
        int scaledH = static_cast<int>(texH * scale);

        // Center the scaled background in camera view
        int offsetX = (camera.width - scaledW) / 2;
        int offsetY = (camera.height - scaledH) / 2;

        SDL_Rect dest{offsetX, offsetY, scaledW, scaledH};
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, &dest);
    }

    // Render the tilemap layers using the spritesheet
    map.render(renderer, camera.x, camera.y);

    // Render bosses
    if (boss && bossHasSpawn && !boss->isDead())
    {
        boss->render(renderer, camera);
    }

    // Render the player bear with appropriate flipping.
    SDL_RendererFlip flip = polarBear.facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    polarBear.render(renderer, camera.x, camera.y, flip);

    // Render the attack effect during attacks.
    polarBear.renderAttack(renderer, camera.x, camera.y);

    // Render all enemies.
    for (auto &e : enemies)
        e->render(renderer, camera.x, camera.y);

    // Render boss death fade overlay (if active)
    if (boss && boss->getFadeAlpha() > 0)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, boss->getFadeAlpha());
        SDL_Rect screenRect = {0, 0, windowWidth, windowHeight};
        SDL_RenderFillRect(renderer, &screenRect);
    }

    // Render power-ups (only hearts for now)
    for (auto &p : powerUps)
    {
        if (p.collected)
            continue;
        if (p.type == "heart" && heartTexture)
        {
            SDL_Rect src{0, 0, heartFrameW, heartFrameH};
            // Base heart
            SDL_Rect dst{static_cast<int>(p.x) - camera.x, static_cast<int>(p.y) - camera.y,
                         map.tileSize, map.tileSize};
            // Glow pulse: scaled copy with alpha
            float pulse     = 0.5f + 0.5f * std::sin(p.glowPhase);
            float glowScale = 1.0f + 0.25f * pulse;
            int glowW       = static_cast<int>(dst.w * glowScale);
            int glowH       = static_cast<int>(dst.h * glowScale);
            SDL_Rect glowDst;
            glowDst.w = glowW;
            glowDst.h = glowH;
            // Center glow around base dst
            glowDst.x = dst.x + (dst.w - glowW) / 2;
            glowDst.y = dst.y + (dst.h - glowH) / 2;

            Uint8 oldR = 255, oldG = 255, oldB = 255, oldA = 255;
            // Modulate color slightly and alpha for glow
            SDL_SetTextureColorMod(heartTexture, 255, 200, 200);
            SDL_SetTextureAlphaMod(heartTexture, static_cast<Uint8>(180 + 75 * pulse));
            SDL_RenderCopy(renderer, heartTexture, &src, &glowDst);
            // Reset mods
            SDL_SetTextureColorMod(heartTexture, oldR, oldG, oldB);
            SDL_SetTextureAlphaMod(heartTexture, oldA);

            // Draw base heart
            SDL_RenderCopy(renderer, heartTexture, &src, &dst);
        }
    }

    // Render fireballs.
    for (auto &fb : fireballs)
        fb.render(renderer, camera.x, camera.y);

    // Render explosion effects on top.
    for (auto &ex : explosions)
        ex.render(renderer, camera.x, camera.y);

    // HUD: Render hearts at top-left
    if (heartTexture && heartFrameW > 0 && heartFrameH > 0)
    {
        // Compute scaled heart size
        float scale = static_cast<float>(heartPixelHeight) / static_cast<float>(heartFrameH);
        int dstW    = static_cast<int>(heartFrameW * scale);
        int dstH    = static_cast<int>(heartFrameH * scale);
        // Ensure HUD can display all hearts gained from power-ups
        int totalHUD = std::max(heartRows * heartCols, polarBear.maxHearts);
        int maxDraw  = polarBear.maxHearts;

        for (int idx = 0; idx < maxDraw; ++idx)
        {
            int row = idx / heartCols;
            int col = idx % heartCols;
            SDL_Rect src;
            bool full = (idx < polarBear.hearts);
            src.x     = 0;  // single-frame source
            src.y     = 0;
            src.w     = heartFrameW;
            src.h     = heartFrameH;

            SDL_Rect dst;
            dst.x = heartMargin + col * (dstW + heartSpacing);
            dst.y = heartMargin + row * (dstH + heartSpacing);
            dst.w = dstW;
            dst.h = dstH;

            // For empty hearts, draw the same sprite but darker and slightly transparent
            if (!full)
            {
                // Darken via color modulation and reduce alpha
                SDL_SetTextureColorMod(heartTexture, 80, 80, 80);
                SDL_SetTextureAlphaMod(heartTexture, 170);
                SDL_RenderCopy(renderer, heartTexture, &src, &dst);
                // Reset mods for subsequent draws
                SDL_SetTextureColorMod(heartTexture, 255, 255, 255);
                SDL_SetTextureAlphaMod(heartTexture, 255);
            }
            else
            {
                SDL_RenderCopy(renderer, heartTexture, &src, &dst);
            }
        }
    }

    // Render pause menu when paused (not during pickup pause or ending)
    if (paused && !pauseForPickup && !endingStage && menuTexture)
    {
        // Render menu image scaled to fill the camera view
        int texW = 0, texH = 0;
        SDL_QueryTexture(menuTexture, nullptr, nullptr, &texW, &texH);

        float scaleX = static_cast<float>(camera.width) / texW;
        float scaleY = static_cast<float>(camera.height) / texH;
        float scale  = std::max(scaleX, scaleY);

        int scaledW = static_cast<int>(texW * scale);
        int scaledH = static_cast<int>(texH * scale);

        int offsetX = (camera.width - scaledW) / 2;
        int offsetY = (camera.height - scaledH) / 2;

        SDL_Rect dest{offsetX, offsetY, scaledW, scaledH};
        SDL_RenderCopy(renderer, menuTexture, nullptr, &dest);
    }

    // Render end scene overlay when in ending stage
    if (endingStage && endSceneTexture)
    {
        if (endSceneShowing)
        {
            // Draw solid black background first to prevent seeing game during fade-in
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_Rect full{0, 0, camera.width, camera.height};
            SDL_RenderFillRect(renderer, &full);

            int texW = 0, texH = 0;
            SDL_QueryTexture(endSceneTexture, nullptr, nullptr, &texW, &texH);
            float scaleX = static_cast<float>(camera.width) / texW;
            float scaleY = static_cast<float>(camera.height) / texH;
            float scale  = std::max(scaleX, scaleY);
            int scaledW  = static_cast<int>(texW * scale);
            int scaledH  = static_cast<int>(texH * scale);
            int offsetX  = (camera.width - scaledW) / 2;
            int offsetY  = (camera.height - scaledH) / 2;
            SDL_Rect dest{offsetX, offsetY, scaledW, scaledH};
            // Apply fade-in alpha modulation
            float fadeInProgress = std::min(endFadeInTimer / endFadeInDuration, 1.0f);
            Uint8 alpha          = static_cast<Uint8>(255 * fadeInProgress);
            SDL_SetTextureAlphaMod(endSceneTexture, alpha);
            SDL_RenderCopy(renderer, endSceneTexture, nullptr, &dest);
            SDL_SetTextureAlphaMod(endSceneTexture, 255);  // Reset for next frame
        }
    }

    // Fade-to-black overlay during ending stage before scene shows
    if (endingStage && !endSceneShowing)
    {
        float t     = std::min(endFadeTimer / endFadeDuration, 1.0f);
        Uint8 alpha = static_cast<Uint8>(255 * t);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_Rect full{0, 0, camera.width, camera.height};
        SDL_RenderFillRect(renderer, &full);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    // World map: when active, handled earlier with early return

    // Fade-in overlay after world map transition
    if (wmFadingIn)
    {
        float t     = std::min(wmFadeTimer / wmFadeDuration, 1.0f);
        Uint8 alpha = static_cast<Uint8>(255 * (1.0f - t));  // start black -> clear
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_Rect full{0, 0, camera.width, camera.height};
        SDL_RenderFillRect(renderer, &full);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        wmFadeTimer += (1.0f / 60.0f);  // approximate; actual dt already consumed earlier
        if (wmFadeTimer >= wmFadeDuration)
        {
            wmFadingIn  = false;
            wmFadeTimer = 0.0f;
        }
    }

    // White composite overlay during boss death temporarily disabled
    /* if (bossState == BossDeath && bossDeathStage == 2) {
        // Fade disabled
    } */

    // Present the rendered frame to the screen.
    SDL_RenderPresent(renderer);
}

// Cleans up all SDL resources and textures.
void Game::clean()
{
    // World map cleanup
    if (config.showWorldMap)
    {
        worldMap.clean();
    }
    // Destroy textures.
    if (heartTexture)
        SDL_DestroyTexture(heartTexture);
    if (backgroundTexture)
        SDL_DestroyTexture(backgroundTexture);
    if (menuTexture)
        SDL_DestroyTexture(menuTexture);
    if (robotAttackTexture)
        SDL_DestroyTexture(robotAttackTexture);
    if (roboFireballTexture)
        SDL_DestroyTexture(roboFireballTexture);
    if (frenzyWolfIdleTex)
        SDL_DestroyTexture(frenzyWolfIdleTex);
    if (frenzyWolfRunTex)
        SDL_DestroyTexture(frenzyWolfRunTex);
    if (map.spritesheet)
        SDL_DestroyTexture(map.spritesheet);
    if (polarBear.texture)
        SDL_DestroyTexture(polarBear.texture);
    if (polarBear.jumpTexture)
        SDL_DestroyTexture(polarBear.jumpTexture);
    if (polarBear.attackTexture)
        SDL_DestroyTexture(polarBear.attackTexture);
    if (polarBear.slashTexture)
        SDL_DestroyTexture(polarBear.slashTexture);
    if (polarBear.climbTexture)
        SDL_DestroyTexture(polarBear.climbTexture);
    if (powerUpMusic)
        Mix_FreeMusic(powerUpMusic);

    // Stop and free music, close audio.
    if (Mix_PlayingMusic())
        Mix_HaltMusic();
    if (backgroundMusic)
    {
        Mix_FreeMusic(backgroundMusic);
        backgroundMusic = nullptr;
    }
    if (slashSound)
    {
        Mix_FreeChunk(slashSound);
        slashSound = nullptr;
    }
    if (explosionSound)
    {
        Mix_FreeChunk(explosionSound);
        explosionSound = nullptr;
    }
    Mix_CloseAudio();
    // Destroy renderer and window.
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    // Shut down SDL subsystems.
    IMG_Quit();
    SDL_Quit();
}

// Main game loop: runs at ~60 FPS until the game exits.
void Game::run()
{
    const float dt = 1.0f / 60.0f;  // Fixed timestep of 16.67 ms (~60 FPS).
    while (running)
    {
        handleInput();
        update(dt);
        render();
        SDL_Delay(16);  // Simple timing to maintain ~60 FPS.
    }
}
