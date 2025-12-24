# Polar Bear Green World

A 2D platformer game built with C++ and SDL2 where you play as a polar bear navigating snowy environments and battling a robotic boss.

## Features

- **Platformer Mechanics**: Jump, climb, and navigate through tiered levels
- **Combat System**: Slash attacks with collision detection
- **Boss Battle**: Dynamic snow robot boss with multiple attack patterns, health-based scaling, and vulnerability phases
- **Audio**: Background music, sound effects, and dynamic audio feedback
- **HUD**: Heart-based health system with power-up collection
- **Camera System**: Smooth camera transitions and boss-focused locking during encounters

## Compilation

### Prerequisites

- CMake 3.10+
- C++17 compiler (GCC, Clang, or MSVC)
- SDL2, SDL2_image, and SDL2_mixer development libraries

### Build Instructions

```shell
cd /path/to/white_bear_green_world
mkdir build && cd build
cmake ..
cmake --build . -j4
cd bin
```

The compiled executable is `./polar_bear`.

## Running

### Default Run

```shell
./polar_bear
```

Starts at the Snowy Cliffs stage with default settings (640x640 window, 2.0x zoom).

### Run Modes with Command-Line Arguments

#### Select a Stage
```shell
./polar_bear --stage <stage_name>
```

Available stages:
- `snowy-cliffs` (default) - Main level with boss
- `snowy-cliffs-boss` - Boss arena (requires map file)
- `dev_stage` - Developer test level

Examples:
```shell
./polar_bear --stage dev_stage
./polar_bear --stage snowy-cliffs-boss --enable-climb
```

#### Enable Climbing Skill (Dev)
```shell
./polar_bear --enable-climb
./polar_bear --climb
```

Allows the player to climb on climbable wall tiles.

#### World Map Mode
```shell
./polar_bear --worldmap
```

Starts on the world map screen instead of a stage.

Debug mode for world map:
```shell
./polar_bear --worldmap --worldmap-debug
```

#### Graphics & Zoom Options
```shell
./polar_bear --window-width 1280 --window-height 720 --zoom 1.5
```

- `--window-width N` - Window width in pixels (default: 640)
- `--window-height N` - Window height in pixels (default: 640)
- `--zoom F` - Camera zoom level (default: 2.0)

#### Audio Volume
```shell
./polar_bear --music-volume 80 --pause-volume 40
```

- `--music-volume N` - Music volume (0-128, default: 96)
- `--pause-volume N` - Pause menu music volume (0-128, default: 32)

#### Custom Asset Path
```shell
./polar_bear --asset-path /custom/assets/path/
```

- `--asset-path PATH` - Base path to game assets (default: `../../assets/`)

### Example Commands

```shell
# Boss battle with climbing enabled
./polar_bear --stage snowy-cliffs-boss --enable-climb

# Large window with lower zoom
./polar_bear --window-width 1280 --window-height 720 --zoom 1.0

# Dev stage with full volume
./polar_bear --stage dev_stage --music-volume 128

# World map with debug markers
./polar_bear --worldmap --worldmap-debug
```

## Controls

- **A / D** - Move left/right
- **W / S** - Climb up/down (when adjacent to climbable walls, requires `--enable-climb`)
- **J** - Jump (or release from climb)
- **K** - Attack / Slash
- **ESC** - Pause/Resume

## Gameplay

### Snowy Cliffs Stage

Navigate the snowy terrain and reach the end-of-stage marker. Defeat regular enemies (robots, wolves) to progress.

### Boss Battle

When approaching the snow robot boss:

1. **Intro Sequence**: Camera locks, boss intro animation plays with music
2. **Attack Patterns**: 
   - Dash attack (moves across arena)
   - Fireball projectiles (speed and size increase as boss health decreases)
3. **Vulnerable Phases**: After each dash, the boss enters a vulnerable state where hits are possible (frames 10-12)
   - Metal clash sound plays when hitting during non-vulnerable frames
4. **Death Sequence**: 
   - Boss explodes and fades to white
   - Heart power-up spawns at boss location and falls with gravity
   - Player inputs re-enable after fade completes

### Power-Ups

- **Heart**: Increases max health by 1 and restores to full. Collects with a special pickup animation.

## Project Structure

```
white_bear_green_world/
├── src/
│   ├── main.cpp              # Entry point
│   ├── Config.cpp/h          # Command-line argument parsing
│   ├── Game.cpp/h            # Main game loop and update
│   ├── TileMap.cpp/h         # Level data and collision
│   ├── WorldMap.cpp/h        # World map screen
│   ├── Camera.cpp/h          # Camera system
│   ├── Explosion.h           # Explosion effect
│   ├── Fireball.h            # Projectile struct
│   ├── entities/
│   │   ├── PolarBear.cpp/h   # Player character
│   │   ├── Enemy.h           # Enemy interface
│   │   ├── Boss.h            # Boss interface
│   │   ├── SnowRobotBoss.cpp/h  # Snow robot boss implementation
│   │   ├── RobotEnemy.cpp/h  # Regular robot enemy
│   │   ├── Arachnoid.cpp/h   # Spider enemy
│   │   ├── FrenzyWolf.cpp/h  # Wolf enemy
│   ├── actions/
│   │   ├── Attack.cpp/h      # Slash attack implementation
│   └── core/
│       ├── Input.cpp/h       # Input handling
│       ├── Time.cpp/h        # Timing utilities
│
├── assets/               # Game sprites, maps, music, sfx
│   ├── *.png            # Sprite sheets
│   ├── music/           # Background and boss music
│   ├── sfx/             # Sound effects (wav, ogg)
│   └── */map.json       # Level layouts
├── CMakeLists.txt       # Build configuration
└── README.md            # This file
```

## Game Development

The codebase uses an object-oriented design with clear separation of concerns:

- **Game loop**: Handles input, update, and render phases
- **Entity system**: PolarBear, Enemy (base), and Boss (base) classes
- **State machines**: Boss uses explicit state machine for attack patterns
- **Collision detection**: AABB with shrinking for accurate hits
- **Audio system**: SDL_mixer for music and sound effects
- **Camera system**: Smooth following with lock/unlock transitions

## Notes

- The game runs at ~60 FPS with a fixed timestep
- Integer scaling is used to avoid seams between tiles
- Boss difficulty scales with health (projectiles get faster and bigger)
- All audio is optional and won't crash the game if missing
