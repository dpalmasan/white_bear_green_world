// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// Input.h
// Centralized input handling for cleaner separation of concerns.

#pragma once

#include <SDL2/SDL.h>

// Encapsulates all keyboard and event state for the game
class Input
{
public:
    Input() = default;
    ~Input() = default;

    // Process all SDL events for this frame
    void handleEvents(bool& running);
    
    // Query current input state
    bool isMovingLeft() const { return moveLeft; }
    bool isMovingRight() const { return moveRight; }
    bool isClimbingUp() const { return climbUp; }
    bool isClimbingDown() const { return climbDown; }
    bool isJumping() const { return jumpPressed; }
    bool isJumpHeld() const { return jumpButtonHeld; }
    bool isAttacking() const { return attackPressed; }
    bool isPausePressed() const { return pausePressed; }
    bool isQuitPressed() const { return quitPressed; }
    bool isMenuPressed() const { return menuPressed; }
    
    // Single-press navigation (for menus/UI)
    bool isUpPressed() const { return upPressed; }
    bool isDownPressed() const { return downPressed; }
    
    // Reset single-frame events
    void resetFrameEvents();

    // World map specific
    bool isSelectPressed() const { return selectPressed; }
    bool isWorldMapActive() const { return worldMapActive; }
    void setWorldMapActive(bool active) { worldMapActive = active; }

private:
    // Continuous state
    bool moveLeft = false;
    bool moveRight = false;
    bool climbUp = false;
    bool climbDown = false;

    // Single-frame events (reset each frame)
    bool jumpPressed = false;
    bool attackPressed = false;
    bool pausePressed = false;
    bool selectPressed = false;
    bool quitPressed = false;
    bool menuPressed = false;
    bool upPressed = false;
    bool downPressed = false;

    // Mode tracking
    bool worldMapActive = false;
    bool jumpButtonHeld = false;
    bool attackButtonHeld = false;
    bool pauseButtonHeld = false;
    bool menuButtonHeld = false;
    bool upButtonHeld = false;
    bool downButtonHeld = false;
};
