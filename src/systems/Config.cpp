#include "Config.h"

#include <cstdlib>
#include <string>

static int toInt(const char* s, int fallback)
{
    if (!s)
        return fallback;
    try
    {
        return std::stoi(s);
    }
    catch (...)
    {
        return fallback;
    }
}

static float toFloat(const char* s, float fallback)
{
    if (!s)
        return fallback;
    try
    {
        return std::stof(s);
    }
    catch (...)
    {
        return fallback;
    }
}

Config parseArgs(int argc, char* argv[])
{
    Config cfg;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--stage" && i + 1 < argc)
        {
            cfg.stageName      = argv[++i];
            cfg.stageSpecified = true;
        }
        else if (arg == "--asset-path" && i + 1 < argc)
        {
            cfg.assetPath = argv[++i];
            // Ensure trailing slash for consistency
            if (!cfg.assetPath.empty() && cfg.assetPath.back() != '/')
                cfg.assetPath += '/';
        }
        else if (arg == "--map" && i + 1 < argc)
        {
            cfg.mapPath = argv[++i];
        }
        else if (arg == "--window-width" && i + 1 < argc)
        {
            cfg.windowWidth = toInt(argv[++i], cfg.windowWidth);
        }
        else if (arg == "--window-height" && i + 1 < argc)
        {
            cfg.windowHeight = toInt(argv[++i], cfg.windowHeight);
        }
        else if (arg == "--zoom" && i + 1 < argc)
        {
            cfg.cameraZoom = toFloat(argv[++i], cfg.cameraZoom);
        }
        else if (arg == "--music-volume" && i + 1 < argc)
        {
            cfg.musicVolume = toInt(argv[++i], cfg.musicVolume);
            if (cfg.musicVolume < 0)
                cfg.musicVolume = 0;
            if (cfg.musicVolume > 128)
                cfg.musicVolume = 128;
        }
        else if (arg == "--pause-volume" && i + 1 < argc)
        {
            cfg.pauseMusicVolume = toInt(argv[++i], cfg.pauseMusicVolume);
            if (cfg.pauseMusicVolume < 0)
                cfg.pauseMusicVolume = 0;
            if (cfg.pauseMusicVolume > 128)
                cfg.pauseMusicVolume = 128;
        }
        else if (arg == "--dev-mode")
        {
            // Maintain original behavior: dev-mode selects dev_stage
            cfg.stageName = "dev_stage";
        }
        else if (arg == "--worldmap")
        {
            cfg.showWorldMap = true;
        }
        else if (arg == "--worldmap-debug")
        {
            cfg.worldMapDebug = true;
        }
        else if (arg == "--armors" && i + 1 < argc)
        {
            cfg.devArmors = argv[++i];
        }
        else if (arg == "--skills" && i + 1 < argc)
        {
            cfg.devSkills = argv[++i];
        }
        else if (arg == "--bosses" && i + 1 < argc)
        {
            cfg.devBosses = argv[++i];
        }
    }

    return cfg;
}
