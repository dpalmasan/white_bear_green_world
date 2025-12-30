# Component-Based Architecture for PolarBear

## Overview

The PolarBear class has been refactored to support a component-based architecture. This allows for modular, maintainable code that supports multiple armors and abilities without increasing complexity.

## Architecture

### Base Component: `BearComponent`

All bear components inherit from the base `BearComponent` class:

```cpp
class BearComponent {
public:
    virtual ~BearComponent() = default;
    virtual void update(PolarBear& bear, float dt, const TileMap& map) = 0;
};
```

### PolarBear Integration

PolarBear maintains a vector of components:

```cpp
std::vector<std::unique_ptr<BearComponent>> components;
```

During the update cycle, all components are called:

```cpp
void PolarBear::update(float dt, const TileMap& map) {
    // Update all components first
    for (auto& comp : components) {
        comp->update(*this, dt, map);
    }
    // ... rest of update logic
}
```

## Existing Components

### 1. MovementComponent

Handles basic physics, collision detection, and animation:
- Gravity application
- Horizontal movement (with slippery surface support)
- Collision detection (horizontal and vertical)
- Walk/idle animation
- Skips when climbing or swimming

**Location**: `src/entities/components/MovementComponent.h/cpp`

### 2. WindArmorComponent

Handles wind-specific mechanics:
- Detects when bear is in wind tiles
- Applies upward floating effect when Wind armor is equipped

**Location**: `src/entities/components/WindArmorComponent.h/cpp`

### 3. SwimmingComponent

Handles water detection and swimming mechanics:
- Detects water coverage
- Swimming physics (upward swim, sinking, resting on bottom)
- Swimming collision detection
- Swimming animation
- Water exit impulse

**Location**: `src/entities/components/SwimmingComponent.h/cpp`

### 4. ClimbingComponent

Handles climbing mechanics:
- Climbing velocity control
- Ledge mounting detection
- Climbing physics (no gravity)
- Climbing animation

**Location**: `src/entities/components/ClimbingComponent.h/cpp`

## How to Use

### Adding a Component

To add a component to PolarBear:

```cpp
polarBear.addComponent(std::make_unique<MovementComponent>());
polarBear.addComponent(std::make_unique<WindArmorComponent>());
```

### Creating a New Component

1. Create header file in `src/entities/components/`:

```cpp
#pragma once
#include "BearComponent.h"

class MyNewComponent : public BearComponent {
public:
    void update(PolarBear& bear, float dt, const TileMap& map) override;
};
```

2. Create implementation file:

```cpp
#include "MyNewComponent.h"
#include "entities/PolarBear.h"
#include "systems/TileMap.h"

void MyNewComponent::update(PolarBear& bear, float dt, const TileMap& map) {
    // Your component logic here
    // You have full access to bear's public members
}
```

3. Add component to PolarBear (typically in Game::loadAssets()):

```cpp
polarBear.addComponent(std::make_unique<MyNewComponent>());
```

## Migration Status

✅ **Migration Complete!** The component system has fully replaced the state machine:

- ✅ **MovementComponent**: Handles basic movement and physics
- ✅ **WindArmorComponent**: Handles wind floating
- ✅ **SwimmingComponent**: Handles water detection and swimming
- ✅ **ClimbingComponent**: Handles climbing mechanics
- ✅ **State Machine**: Completely removed from PolarBear

All components are instantiated in [`Game.cpp`](../../Game.cpp):

```cpp
// Initialize bear components (component-based architecture)
polarBear.addComponent(std::make_unique<MovementComponent>());
polarBear.addComponent(std::make_unique<WindArmorComponent>());
polarBear.addComponent(std::make_unique<SwimmingComponent>());
polarBear.addComponent(std::make_unique<ClimbingComponent>());
```

The update logic in `PolarBear::update()` now only calls components:

```cpp
void PolarBear::update(float dt, const TileMap& map)
{
    // Update all components (handles all movement, swimming, climbing, armor logic)
    for (auto& comp : components)
    {
        comp->update(*this, dt, map);
    }
    // ... rest of update logic (damage, attacks, bounds checking)
}
```

## Benefits

- **Modularity**: Each armor/ability is self-contained
- **Maintainability**: Easy to add/remove/modify abilities
- **Clarity**: Clear separation of concerns
- **Flexibility**: Components can be added/removed dynamically
- **Testability**: Each component can be tested independently

## Example: Creating a New Armor Component
