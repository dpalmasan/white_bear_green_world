// Copyright 2025 Polar Bear Green World
// WindArmorComponent - Handles wind-specific mechanics
// Wind armor: float upward in wind tiles

#pragma once

#include "BearComponent.h"

class WindArmorComponent : public BearComponent {
public:
    void update(PolarBear& bear, float dt, const TileMap& map) override;
};
