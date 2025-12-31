// Copyright 2025 Polar Bear Green World
// GameConstants.h
// Centralized game constants to eliminate magic numbers

#pragma once

namespace GameConstants
{
    // Window and Display
    namespace Display
    {
        constexpr int DEFAULT_WINDOW_WIDTH = 320;
        constexpr int DEFAULT_WINDOW_HEIGHT = 240;
        constexpr int LOGICAL_WIDTH = 320;
        constexpr int LOGICAL_HEIGHT = 240;
        constexpr float DEFAULT_ZOOM = 1.0f;
    }

    // Audio
    namespace Audio
    {
        constexpr int MAX_VOLUME = 128;
        constexpr int DEFAULT_MUSIC_VOLUME = 96;
        constexpr int DEFAULT_PAUSE_VOLUME = 32;
        constexpr int MIN_VOLUME = 0;
    }

    // Physics
    namespace Physics
    {
        constexpr float GRAVITY = 980.0f;              // pixels/s²
        constexpr float DEFAULT_JUMP_IMPULSE = -318.0f; // pixels/s (negative = up)
        constexpr float TERMINAL_VELOCITY = 600.0f;    // max falling speed
        constexpr float GROUND_FRICTION = 0.85f;       // horizontal slowdown
    }

    // Player Constants
    namespace Player
    {
        // Movement
        constexpr float RUN_SPEED = 60.0f;             // ground speed
        constexpr float CLIMB_SPEED = 60.0f;           // climbing speed
        constexpr float SWIM_UP_SPEED = 140.0f;        // swimming upward
        constexpr float SWIM_SINK_SPEED = 80.0f;       // sinking in water
        constexpr float SWIM_RUN_SPEED = 70.0f;        // horizontal swim speed
        
        // Jump physics
        constexpr float MAX_UP_VELOCITY = 320.0f;      // for jump animation
        constexpr float MAX_DOWN_VELOCITY = 600.0f;    // for fall animation
        constexpr float PEAK_THRESHOLD = 40.0f;        // velocity near peak
        
        // Wind armor
        constexpr float WIND_SPEED_MULTIPLIER = 1.25f; // movement boost
        constexpr float WIND_JUMP_MULTIPLIER = 1.25f;  // jump boost
        constexpr float WIND_GLIDE_GRAVITY = 0.3f;     // reduced gravity when gliding
        
        // Damage and invulnerability
        constexpr float DAMAGE_FRAME_DURATION = 0.08f;     // seconds per damage frame
        constexpr float INVULNERABILITY_DURATION = 3.0f;   // seconds of immunity
        constexpr float KNOCKBACK_SPEED = 150.0f;          // horizontal knockback
        constexpr float KNOCKBACK_JUMP = -250.0f;          // upward knockback
        constexpr float DAMAGE_ROTATION_ANGLE = 25.0;      // knockback spin angle (degrees)
        constexpr float BLINK_PERIOD = 0.15f;              // invulnerability blink cycle
        
        // Jump animation velocity mapping
        constexpr float JUMP_ANIMATION_MAX_UP = 320.0f;
        constexpr float JUMP_ANIMATION_PEAK_THRESHOLD = 40.0f;
        
        // Animation
        constexpr float WALK_FRAME_TIME = 0.15f;
        constexpr float CLIMB_FRAME_TIME = 0.15f;
        constexpr float SWIM_FRAME_TIME = 0.1f;
        
        // Sprite dimensions
        constexpr int DEFAULT_WIDTH = 64;
        constexpr int DEFAULT_HEIGHT = 64;
        constexpr int CLIMB_WIDTH = 37;
        constexpr int CLIMB_HEIGHT = 47;
        constexpr int ATTACK_WIDTH = 54;
        constexpr int ATTACK_HEIGHT = 37;
        
        // Water sprites
        constexpr int WATER_WALK_WIDTH = 54;
        constexpr int WATER_WALK_HEIGHT = 35;
        constexpr int WATER_JUMP_WIDTH = 57;
        constexpr int WATER_JUMP_HEIGHT = 37;
        constexpr int WATER_SWIM_WIDTH = 54;
        constexpr int WATER_SWIM_HEIGHT = 36;
        constexpr int WATER_WALK_FRAMES = 4;
        constexpr int WATER_JUMP_FRAMES = 4;
        constexpr int WATER_SWIM_FRAMES = 11;
        
        // Wind sprites
        constexpr int WIND_WALK_WIDTH = 56;
        constexpr int WIND_WALK_HEIGHT = 36;
        constexpr int WIND_JUMP_WIDTH = 62;
        constexpr int WIND_JUMP_HEIGHT = 41;
        constexpr int WIND_WALK_FRAMES = 4;
        constexpr int WIND_JUMP_FRAMES = 6;
        
        // Frame counts
        constexpr int WALK_FRAMES = 4;
        constexpr int JUMP_FRAMES = 7;
        constexpr int ATTACK_FRAMES = 7;
        constexpr int CLIMB_FRAMES = 4;
        
        // Climb offsets
        constexpr int CLIMB_RIGHT_DRAW_OFFSET = 5;
        constexpr int CLIMB_LEFT_DRAW_OFFSET = 0;
    }

    // Collision
    namespace Collision
    {
        constexpr int COLLISION_SAMPLES = 10;          // vertical/horizontal collision checks
        constexpr float COLLISION_THRESHOLD = 0.2f;    // 20% of samples needed
        constexpr float TIGHT_COLLISION_THRESHOLD = 0.3f; // 30% for tighter checks
        constexpr float SHRINK_INSET_DEFAULT = 0.10f;  // 10% shrink for collision boxes
        constexpr float WATER_SAMPLE_INSET = 2.0f;     // pixel inset for water detection
    }

    // Camera
    namespace Camera
    {
        constexpr int VIEWPORT_MARGIN = 32;  // extra pixels for culling
    }

    // Enemies
    namespace Enemies
    {
        // Generic
        constexpr float DEFAULT_FRAME_TIME = 0.12f;  // ~8 FPS animation
        constexpr int DEFAULT_WIDTH = 24;
        constexpr int DEFAULT_HEIGHT = 44;
        constexpr float GROUND_ALIGN_INSET = 0.15f;
        
        // Frenzy Wolf
        namespace Wolf
        {
            constexpr int IDLE_WIDTH = 34;
            constexpr int RUN_WIDTH = 38;
            constexpr int HEIGHT = 25;
            constexpr int IDLE_FRAMES = 6;
            constexpr int RUN_FRAMES = 6;
            constexpr float FRAME_TIME = 0.1f;
            constexpr float GROUND_INSET = 0.15f;
            constexpr float TARGET_SPEED = 260.0f;
            constexpr float ACCELERATION = 1200.0f;
            constexpr float VISION_DISTANCE = 10.0f;  // tiles
            constexpr float CHASE_BEHIND_DISTANCE = 3.0f;  // tiles
            constexpr float VERTICAL_BAND = 48.0f;
            constexpr float MIN_MOVEMENT_THRESHOLD = 4.0f;
            constexpr float MIN_RUNNING_SPEED = 5.0f;
            constexpr float DECEL_MULTIPLIER = 1.2f;
        }
        
        // Arachnoid
        namespace Arachnoid
        {
            constexpr int WIDTH = 32;
            constexpr int HEIGHT = 32;
            constexpr float SPEED = 50.0f;
            constexpr int EDGE_CHECKS = 3;
            constexpr int RENDER_Y_OFFSET = 6;
        }
        
        // Robot
        namespace Robot
        {
            constexpr int WIDTH = 24;
            constexpr int HEIGHT = 46;
            constexpr int ATTACK_WIDTH = 37;
            constexpr int ATTACK_HEIGHT = 44;
            constexpr int ATTACK_FRAMES = 6;
            constexpr float ATTACK_FRAME_TIME = 0.1f;
            constexpr float FIRE_COOLDOWN = 3.0f;  // seconds between shots
            constexpr float DETECTION_RANGE = 8.0f;  // tiles
            constexpr float VERTICAL_BAND = 48.0f;
        }
    }

    // Projectiles
    namespace Projectiles
    {
        constexpr float FIREBALL_SCALE = 0.35f;
        constexpr int FIREBALL_FRAMES = 5;
        constexpr float FIREBALL_FRAME_TIME = 0.1f;
    }

    // World Map
    namespace WorldMap
    {
        constexpr int MAP_SIZE = 256;           // 256x256 map space
        constexpr int CURSOR_SIZE = 32;         // 32x32 cursor
        constexpr float CURSOR_SPEED = 300.0f;  // pixels/s in map space
    }

    // Timing
    namespace Timing
    {
        constexpr int TARGET_FPS = 60;
        constexpr int FRAME_DELAY_MS = 16;  // ~60 FPS
    }

    // Tile
    namespace Tile
    {
        constexpr int DEFAULT_SIZE = 16;  // 16x16 tiles
    }

    // Save System
    namespace SaveSystem
    {
        constexpr int MIN_FILE_SIZE = 16;  // header + version + checksum
        constexpr uint32_t MAGIC_HEADER = 0x57424757;  // "WBGW"
    }
}
