#pragma once
#include <string>

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
    int windowWidth  = 320;
    int windowHeight = 240;
    float cameraZoom = 1.0f;

    // Audio
    int musicVolume      = 96;  // 0-128
    int pauseMusicVolume = 32;  // 0-128

    // Dev toggles
    bool showWorldMap     = false;  // start on world map screen
    bool worldMapDebug    = false;  // draw target markers on world map

    // Dev: comma-separated armor/skill lists
    std::string devArmors = "";  // e.g., "earth,wind,fire,water"
    std::string devSkills = "";  // e.g., "slash,ice_breath,climb,dash"
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
Config parseArgs(int argc, char* argv[]);
