// Copyright 2025 Polar Bear Green World
// Author: Game Development Team
//
// Input.cpp
// Implementation of centralized input handling.

#include "Input.h"

void Input::handleEvents(bool& running)
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
        {
            running = false;
            quitPressed = true;
            return;
        }

        if (e.type == SDL_KEYDOWN)
        {
            const SDL_Keycode key = e.key.keysym.sym;
            
            // Tab and Escape work in both gameplay and world map
            if (key == SDLK_ESCAPE && !pauseButtonHeld)
            {
                pausePressed = true;
                pauseButtonHeld = true;
            }
            if (key == SDLK_TAB && !menuButtonHeld)
            {
                menuPressed = true;
                menuButtonHeld = true;
            }
            
            if (worldMapActive)
            {
                // World map specific input
                if (key == SDLK_RETURN || key == SDLK_j)
                    selectPressed = true;
            }
        }
        else if (e.type == SDL_KEYUP)
        {
            const SDL_Keycode key = e.key.keysym.sym;
            if (key == SDLK_ESCAPE)
                pauseButtonHeld = false;
            if (key == SDLK_TAB)
                menuButtonHeld = false;
        }
    }

    // Continuous key state
    const Uint8* keystate = SDL_GetKeyboardState(nullptr);
    
    moveLeft = keystate[SDL_SCANCODE_A] != 0;
    moveRight = keystate[SDL_SCANCODE_D] != 0;
    climbUp = keystate[SDL_SCANCODE_W] != 0;
    climbDown = keystate[SDL_SCANCODE_S] != 0;
    
    // Single-press navigation for UI (W/S keys)
    bool upDown = keystate[SDL_SCANCODE_W] != 0;
    if (upDown && !upButtonHeld)
    {
        upPressed = true;
        upButtonHeld = true;
    }
    else if (!upDown)
    {
        upButtonHeld = false;
    }
    
    bool downDown = keystate[SDL_SCANCODE_S] != 0;
    if (downDown && !downButtonHeld)
    {
        downPressed = true;
        downButtonHeld = true;
    }
    else if (!downDown)
    {
        downButtonHeld = false;
    }

    // Single-press buttons with debouncing
    bool jumpDown = keystate[SDL_SCANCODE_J] != 0;
    if (jumpDown && !jumpButtonHeld)
    {
        jumpPressed = true;
        jumpButtonHeld = true;
    }
    else if (!jumpDown)
    {
        jumpButtonHeld = false;
    }

    bool attackDown = keystate[SDL_SCANCODE_K] != 0;
    if (attackDown && !attackButtonHeld)
    {
        attackPressed = true;
        attackButtonHeld = true;
    }
    else if (!attackDown)
    {
        attackButtonHeld = false;
    }
}

void Input::resetFrameEvents()
{
    // Reset single-frame events after processing
    jumpPressed = false;
    attackPressed = false;
    pausePressed = false;
    selectPressed = false;
    menuPressed = false;
    upPressed = false;
    downPressed = false;
}
