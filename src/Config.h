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
    int windowWidth  = 640;
    int windowHeight = 640;
    float cameraZoom = 2.0f;

    // Audio
    int musicVolume      = 96;  // 0-128
    int pauseMusicVolume = 32;  // 0-128

    // Dev toggles
    bool showWorldMap     = false;  // start on world map screen
    bool worldMapDebug    = false;  // draw target markers on world map
    bool enableClimbSkill = false;  // enable player climbing ability (dev option)

    // Element selection (dev): "none" (default), "water"
    std::string startElement = "none";
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
//   --enable-climb (dev: enable climbing skill)
//   --element NAME (dev: start with element; supports "water")
Config parseArgs(int argc, char* argv[]);
