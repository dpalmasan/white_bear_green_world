# White Bear Green World - Agent Context File

## Project Overview
**White Bear Green World** is a 2D action-platformer game built with C++20, SDL2, and CMake.
- **Player Character**: Polar bear with unlockable elemental armors and abilities
- **Genre**: Metroidvania-style with boss fights, upgrades, and exploration
- **Resolution**: 320x240 base resolution (scaled to window size)
- **Platform**: Linux (tested on Ubuntu/Debian-based systems)

## Build System
- **CMake 3.16+** with C++20 standard
- **Dependencies**: SDL2, SDL2_image, SDL2_mixer, SDL2_ttf, nlohmann_json
- **Build Commands**:
  ```bash
  mkdir -p build && cd build
  cmake ..
  cmake --build . -j4
  ./bin/polar_bear [options]
  ```

## Project Structure
```
src/
├── main.cpp                    # Entry point
├── Game.{h,cpp}               # Main game loop, rendering, state management
├── GameState.{h,cpp}          # Persistent state (binary save format with encryption)
├── Config.{h,cpp}             # Command-line argument parsing
├── TileMap.{h,cpp}            # Level loading from JSON
├── AssetManager.{h,cpp}       # Resource management
├── Cutscene.{h,cpp}           # Intro cutscenes
├── TitleScreen.{h,cpp}        # Main menu
├── WorldMap.{h,cpp}           # Level selection screen
├── Explosion.h                # Particle effects
├── core/
│   ├── Camera.{h,cpp}         # Viewport tracking
│   ├── Collision.{h,cpp}      # Physics and collision detection
│   ├── Input.{h,cpp}          # Keyboard input handling (SDL_SCANCODE)
│   └── Time.{h,cpp}           # Delta time management
├── entities/
│   ├── PolarBear.{h,cpp}      # Player character with elemental armors
│   ├── Boss.h                 # Boss interface
│   ├── SnowRobotBoss.{h,cpp}  # First boss implementation
│   ├── Enemy.h                # Enemy base class
│   ├── RobotEnemy.{h,cpp}     # Basic ranged enemy
│   ├── FrenzyWolf.{h,cpp}     # Fast melee enemy
│   ├── Arachnoid.{h,cpp}      # Spider enemy
│   ├── IdleEnemy.h            # Stationary enemy
│   └── Fireball.h             # Projectile
├── actions/
│   └── Attack.{h,cpp}         # Slash attack system
├── levels/
│   └── StageRegistry.{h,cpp}  # Stage name constants
└── ui/
    └── Menu.{h,cpp}           # Tab menu for armor/skill selection

assets/
├── levels/                     # JSON map files (Tiled format)
├── images/                     # PNG sprites and backgrounds
├── music/                      # OGG music tracks
├── sfx/                        # WAV sound effects
└── fonts/                      # TTF fonts
```

## Code Conventions

### Naming
- **Private member variables**: End with underscore `_` (e.g., `isOpen_`, `armorCursor_`)
- **Public members**: No underscore (e.g., `running`, `paused`)
- **Classes**: PascalCase (e.g., `PolarBear`, `GameState`)
- **Functions/methods**: camelCase (e.g., `handleInput()`, `loadAssets()`)
- **Constants**: UPPER_SNAKE_CASE (e.g., `ASSET_PATH`)

### File Organization
- Header files (.h) contain class declarations
- Implementation files (.cpp) contain definitions
- Forward declarations preferred to reduce include dependencies
- Include guards use `#pragma once`

## Key Systems

### 1. Game State Management
**File**: `src/GameState.{h,cpp}`
- **Save Format**: Binary with XOR encryption + CRC32 checksum
- **Magic Header**: 0x57424757 ("WBGW")
- **Save Path**: `savegame.dat`
- **Stores**: Unlocked skills, armors, collectibles, progress
- **Methods**:
  - `unlockSlash()`, `unlockClimb()`, `unlockDash()`, `unlockIceBreath()`
  - `unlockEarthArmor()`, `unlockWindArmor()`, `unlockFireArmor()`, `unlockWaterArmor()`
  - `saveToFile()`, `loadFromFile()`

### 2. Input System
**File**: `src/core/Input.{h,cpp}`
- Uses SDL_Scancode (keyboard layout independent)
- **Key Mappings**:
  - Movement: A (left), D (right), W (up), S (down)
  - Actions: J (jump), K (attack/slash)
  - Menu: Tab (toggle menu)
- **Debouncing**: Single-press events tracked with `resetFrameEvents()`
- **Methods**: `isMovingLeft()`, `isJumping()`, `isAttacking()`, `isMenuPressed()`

### 3. Rendering System
**File**: `src/Game.cpp::render()`
- **Sub-pixel positioning**: Use `std::round()` to avoid visual jitter
  ```cpp
  int screenX = static_cast<int>(std::round(worldX - camera.x));
  ```
- **Rendering order**: Background → Tiles → Enemies → Player → Effects → HUD → Menu
- **Camera**: Follows player with configurable zoom

### 4. Physics & Collision
**File**: `src/core/Collision.{h,cpp}`
- **Tile-based collision**: Query `TileMap` for solid tiles
- **Entity collision**: AABB (axis-aligned bounding box)
- **Gravity**: Applied in `PolarBear::update()` and enemy update methods
- **Special tiles**: Water, wind, lava, climbable, damage

### 5. Menu System (NEW - Refactored Dec 2025)
**Files**: `src/ui/Menu.{h,cpp}`
- **Purpose**: In-game armor/skill selection screen
- **Trigger**: Tab key (pauses game, lowers music volume)
- **Navigation**: A/D keys (left/right through armor cursor)
- **Actions**:
  - J key: Equip selected armor (plays `confirm.wav` if available, `cancel.wav` if locked)
  - K key: Unequip current armor (plays `cancel.wav`)
- **Display**:
  - Skills shown if unlocked (slash always visible)
  - Only **equipped** armor shown (not all unlocked armors)
  - Cursor overlay shows current selection
- **Assets**: `assets/images/menu/*.png`

### 6. Elemental Armor System
**File**: `src/entities/PolarBear.{h,cpp}`
- **Elements**: None, Earth, Wind, Fire, Water
- **Wind Armor Effects**:
  - 1.25x movement speed (65 → 81.25 px/s)
  - 1.25x jump height (-318 → -398)
  - Jump from wind tiles (extra height boost)
- **Element Selection**: Via menu (Tab) or command-line `--element`

## Command-Line Options
```bash
./bin/polar_bear [OPTIONS]

Stage & Assets:
  --stage NAME            Load specific stage (snowy-cliffs, wind-peaks, snowy-cliffs-boss)
  --asset-path PATH       Override asset directory (default: ../../assets/)
  --worldmap              Start at world map selection screen

Window & Display:
  --window-width N        Set window width (default: 320)
  --window-height N       Set window height (default: 240)
  --zoom F                Camera zoom factor (default: 1.0)

Audio:
  --music-volume N        Background music volume 0-128 (default: 96)
  --pause-volume N        Paused music volume 0-128 (default: 32)

Development Options:
  --skills LIST           Unlock skills (comma-separated: slash,climb,dash,ice_breath)
  --armors LIST           Unlock armors (comma-separated: earth,wind,fire,water)
  --enable-climb          Legacy flag to enable climbing (same as --skills climb)
  --element NAME          Start with element equipped (water, wind, fire, earth)
```

**Examples**:
```bash
# Start with wind armor and climbing
./bin/polar_bear --armors wind --skills slash,climb

# Test all abilities
./bin/polar_bear --skills slash,climb,dash,ice_breath --armors earth,wind,fire,water

# World map with specific armors
./bin/polar_bear --worldmap --armors water,wind
```

## Known Issues & Gotchas

### Rendering
- **Always use `std::round()`** for sub-pixel world-to-screen conversion
- Without rounding, fractional positions cause 1-pixel jitter
- Fixed in: PolarBear, Attack, all enemies, power-ups (Dec 2025)

### Input Handling
- Menu must process input **before** game inputs to prevent action bleed-through
- Order: `handleEvents()` → `menu.handleInput()` → game inputs
- Menu returns `true` if open to block game input processing

### Save System
- Binary format prevents casual cheating
- CRC32 detects tampering (doesn't prevent determined hackers)
- XOR encryption key: `"WhiteBearGreenWorld2025"`

### Assets
- Menu images have **built-in positioning** (render as full-screen overlays)
- Don't manually position menu sprites - use `SDL_Rect{0, 0, camera.width, camera.height}`

## Testing Workflow
```bash
# Compile
cmake --build build -j4

# Test menu system
./build/bin/polar_bear --stage snowy-cliffs --armors earth,wind,fire --skills slash,climb

# Test specific boss
./build/bin/polar_bear --stage snowy-cliffs-boss --armors wind --skills slash,climb,dash

# Test world map
./build/bin/polar_bear --worldmap --armors water,wind
```

## Recent Changes (December 2025)

### Menu System Refactor
- Extracted 200+ lines of menu code into `Menu` class
- Private members now use `_` suffix convention
- Reduced `Game.h` from 15 menu variables to single `Menu menu;`

### Save System Overhaul
- Replaced JSON with encrypted binary format
- Prevents save editing in text editors
- Added version field for future compatibility

### Rendering Fixes
- Fixed visual jitter with wind armor
- Applied `std::round()` to all entity rendering
- Jump velocity now rounded to avoid fractional accumulation

### Input System
- Added Tab key menu toggle
- Menu input isolated from game input (no action bleed-through)
- Removed ESC pause (only Tab menu remains)

## Future Work
- Implement fire/water armor mechanics (currently only wind has special behavior)
- Add dash skill functionality
- Add ice breath attack
- Implement spirit NPCs for ability unlocking
- Save/load integration with menu (currently uses dev command-line options)
- Sound effects for menu navigation (only confirm/cancel implemented)

## Debugging Tips
- Use `std::cerr` for debug output (not `std::cout`)
- Check `grep_search` for existing implementations before adding new features
- Validate asset paths with `ls assets/...`
- Test build after structural changes: `cmake .. && cmake --build . -j4`

---
**Last Updated**: December 29, 2025
**Agent**: GitHub Copilot (Claude Sonnet 4.5)
