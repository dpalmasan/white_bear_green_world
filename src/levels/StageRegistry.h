// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// StageRegistry.h
// Central place for stage identifiers and metadata so stage-specific logic
// does not rely on scattered string literals.

#pragma once

#include <string>
#include <unordered_map>

struct StageInfo
{
    std::string name;              // Unique id (e.g., "snowy-cliffs")
    std::string folder;            // Folder under assets/ (same as name by default)
    std::string mapFile    = "map.json";
    std::string spriteFile = "spritesheet.png";

    // Visuals
    std::string backgroundImage;   // Background PNG filename (defaults to name.png if empty)

    // Music
    std::string backgroundMusic;   // Path relative to assets/ (e.g., music/snowy_cliffs.ogg)
    std::string bossMusic;         // Boss theme (if boss level)

    // Optional ending scene assets
    std::string endSceneTexture;   // Relative path to ending scene texture
    std::string endSceneMusic;     // Relative path to ending scene music

    bool isBoss = false;
};

namespace StageNames
{
    inline constexpr const char* SnowyCliffs     = "snowy-cliffs";
    inline constexpr const char* SnowyCliffsBoss = "snowy-cliffs-boss";
    inline constexpr const char* WindPeaks       = "wind-peaks";
    inline constexpr const char* WindPeaksCaveBoss = "wind-peaks-cave-boss";
    inline constexpr const char* DevStage        = "dev_stage";
}

class StageRegistry
{
   public:
    // Returns metadata for the given stage name, or nullptr if not found.
    static const StageInfo* find(const std::string& name);

    // Returns the default stage metadata (snowy-cliffs).
    static const StageInfo& defaultStage();
};
