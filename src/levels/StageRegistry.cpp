// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// StageRegistry.cpp

#include "StageRegistry.h"

namespace
{
const StageInfo kSnowyCliffs{
    StageNames::SnowyCliffs,
    "levels/snowy-cliffs",
    "map.json",
    "spritesheet.png",
    "",                        // backgroundImage (defaults to snowy-cliffs.png)
    "music/snowy_cliffs.ogg",  // backgroundMusic
    "",                        // bossMusic
    "",                       // endSceneTexture (asset removed in cleaned layout)
    "music/is_there_hope.ogg", // endSceneMusic
    false                       // isBoss
};

const StageInfo kSnowyCliffsBoss{
    StageNames::SnowyCliffsBoss,
    "levels/snowy-cliffs-boss",
    "map.json",
    "spritesheet.png",
    "",                       // backgroundImage (defaults to snowy-cliffs-boss.png)
    "",                       // backgroundMusic (handled by boss theme)
    "music/boss_theme.ogg",   // bossMusic
    "",                       // endSceneTexture
    "",                       // endSceneMusic
    true                       // isBoss
};

const StageInfo kWindPeaks{
    StageNames::WindPeaks,
    "levels/wind-peaks",
    "map.json",
    "spritesheet.png",
    "",                       // backgroundImage (defaults to wind-peaks.png)
    "music/wind_peaks.ogg",
    "",                       // bossMusic
    "",                       // endSceneTexture
    "",                       // endSceneMusic
    false                      // isBoss
};

const StageInfo kWindPeaksCaveBoss{
    StageNames::WindPeaksCaveBoss,
    "levels/wind-peaks-cave-boss",
    "map.json",
    "spritesheet.png",
    "wind-peaks-cave.png",    // backgroundImage (use cave background)
    "music/wind_peaks_cave.ogg",   // backgroundMusic (plays until boss intro)
    "music/wind_peaks_cave.ogg",   // bossMusic (same track continues)
    "",                       // endSceneTexture
    "",                       // endSceneMusic
    true                       // isBoss
};

const StageInfo kDevStage{
    StageNames::DevStage,
    StageNames::DevStage,
    "map.json",
    "spritesheet.png",
    "",                       // backgroundImage (defaults to dev_stage.png)
    "music/snowy_cliffs.ogg", // reuse snowy cliffs music for dev stage
    "",                       // bossMusic
    "",                       // endSceneTexture
    "",                       // endSceneMusic
    false
};

const StageInfo* resolveDefault()
{
    return &kSnowyCliffs;
}
}

const StageInfo* StageRegistry::find(const std::string& name)
{
    if (name == StageNames::SnowyCliffs)
        return &kSnowyCliffs;
    if (name == StageNames::SnowyCliffsBoss)
        return &kSnowyCliffsBoss;
    if (name == StageNames::WindPeaks)
        return &kWindPeaks;
    if (name == StageNames::WindPeaksCaveBoss)
        return &kWindPeaksCaveBoss;
    if (name == StageNames::DevStage)
        return &kDevStage;
    return nullptr;
}

const StageInfo& StageRegistry::defaultStage()
{
    return *resolveDefault();
}
