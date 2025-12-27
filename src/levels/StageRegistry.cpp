// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// StageRegistry.cpp

#include "StageRegistry.h"

namespace
{
const StageInfo kSnowyCliffs{
    StageNames::SnowyCliffs,
    StageNames::SnowyCliffs,
    "map.json",
    "spritesheet.png",
    "music/snowy_cliffs.ogg",  // backgroundMusic
    "",                        // bossMusic
    "end_stage1_scene.png",    // endSceneTexture
    "music/is_there_hope.ogg", // endSceneMusic
    false                       // isBoss
};

const StageInfo kSnowyCliffsBoss{
    StageNames::SnowyCliffsBoss,
    StageNames::SnowyCliffsBoss,
    "map.json",
    "spritesheet.png",
    "",                       // backgroundMusic (handled by boss theme)
    "music/boss_theme.ogg",   // bossMusic
    "",                       // endSceneTexture
    "",                       // endSceneMusic
    true                       // isBoss
};

const StageInfo kDevStage{
    StageNames::DevStage,
    StageNames::DevStage,
    "map.json",
    "spritesheet.png",
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
    if (name == StageNames::DevStage)
        return &kDevStage;
    return nullptr;
}

const StageInfo& StageRegistry::defaultStage()
{
    return *resolveDefault();
}
