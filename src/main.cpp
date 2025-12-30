// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// main.cpp
// Entry point for the Polar Bear game.

#include <string>

#include "systems/Config.h"
#include "Game.h"

// Program entry point.
// Initializes the game, loads assets, runs the main loop, and cleans up resources.
int main(int argc, char* argv[])
{
    Game game;

    // Parse command-line arguments via Config and apply to game
    Config cfg        = parseArgs(argc, argv);
    game.config       = cfg;
    game.stageName    = cfg.stageName;  // maintains existing stage selection behavior
    game.windowWidth  = cfg.windowWidth;
    game.windowHeight = cfg.windowHeight;
    game.cameraZoom   = cfg.cameraZoom;

    // If a stage was specified explicitly, skip intro/title and load directly
    if (cfg.stageSpecified)
    {
        game.showIntroCutscene = false;
        game.showTitleScreen   = false;
    }

    // Initialize SDL, create window and renderer.
    if (!game.init())
    {
        return -1;
    }

    // Load game assets (sprites, textures, enemies).
    game.loadAssets();

    // Run the main game loop.
    game.run();

    // Clean up SDL resources.
    game.clean();

    return 0;
}
