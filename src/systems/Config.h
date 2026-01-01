#pragma once
#include <string>
#include "core/GameConstants.h"

// Simple game configuration structure and CLI parser.
// Centralizes settings like stage selection, asset paths, window size, zoom, and volumes.
struct Config
{
    // Stage selection and assets
    std::string stageName = "snowy-cliffs";   // default stage folder
    bool stageSpecified   = false;             // true when --stage is provided
    std::string assetPath = "../../assets/";  // base path to assets
    std::string mapPath;                      // optional explicit map.json path

    // Window and camera
    int windowWidth  = GameConstants::Display::DEFAULT_WINDOW_WIDTH;
    int windowHeight = GameConstants::Display::DEFAULT_WINDOW_HEIGHT;
    float cameraZoom = GameConstants::Display::DEFAULT_ZOOM;

    // Audio
    int musicVolume      = GameConstants::Audio::DEFAULT_MUSIC_VOLUME;
    int pauseMusicVolume = GameConstants::Audio::DEFAULT_PAUSE_VOLUME;

    // Dev toggles
    bool showWorldMap     = false;  // start on world map screen
    bool worldMapDebug    = false;  // draw target markers on world map

    // Dev: comma-separated armor/skill/boss lists
    std::string devArmors = "";  // e.g., "earth,wind,fire,water"
    std::string devSkills = "";  // e.g., "slash,ice_breath,climb,dash"
    std::string devBosses = "";  // e.g., "snow-robot" (marks bosses as defeated)
};

// Parse command-line arguments into a Config.
// Supported flags:
//   --stage NAME
//   --asset-path PATH
//   --map PATH
//   --window-width N
//   --window-height N
//   --zoom F
//   --music-volume N
//   --pause-volume N
//   --armors LIST (dev: comma-separated armors, e.g., "earth,wind,fire,water")
//   --skills LIST (dev: comma-separated skills, e.g., "slash,ice_breath,climb,dash")
//   --bosses LIST (dev: comma-separated bosses marked as defeated, e.g., "snow-robot" or "snowy-cliffs-boss:snow-robot")
Config parseArgs(int argc, char* argv[]);
