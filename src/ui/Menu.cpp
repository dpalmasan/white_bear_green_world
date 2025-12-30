#include "Menu.h"
#include "entities/PolarBear.h"
#include "systems/GameState.h"
#include "core/Input.h"
#include "core/Camera.h"

#include <SDL2/SDL_image.h>
#include <iostream>

void Menu::loadTexture(SDL_Texture*& tex, SDL_Renderer* renderer,
                       const std::string& path, const std::string& filename)
{
    SDL_Surface* surf = IMG_Load((path + filename).c_str());
    if (surf)
    {
        tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
        if (!tex)
            std::cerr << "Failed to create texture from " << filename << ": " << SDL_GetError() << "\n";
    }
    else
    {
        std::cerr << "Failed to load " << filename << ": " << IMG_GetError() << "\n";
    }
}

void Menu::loadAssets(SDL_Renderer* renderer, const std::string& assetPath)
{
    std::string menuPath = assetPath + "images/menu/";

    loadTexture(backgroundTex_, renderer, menuPath, "menu_background.png");
    loadTexture(slashIconTex_, renderer, menuPath, "slash.png");
    loadTexture(climbIconTex_, renderer, menuPath, "climb.png");
    loadTexture(iceBreathIconTex_, renderer, menuPath, "ice_breath.png");
    loadTexture(dashIconTex_, renderer, menuPath, "dash.png");
    loadTexture(earthArmorTex_, renderer, menuPath, "earth_armor.png");
    loadTexture(windArmorTex_, renderer, menuPath, "wind_armor.png");
    loadTexture(fireArmorTex_, renderer, menuPath, "fire_armor.png");
    loadTexture(waterArmorTex_, renderer, menuPath, "water_armor.png");
    loadTexture(earthArmorCursorTex_, renderer, menuPath, "earth_armor_cursor.png");
    loadTexture(windArmorCursorTex_, renderer, menuPath, "wind_armor_cursor.png");
    loadTexture(fireArmorCursorTex_, renderer, menuPath, "fire_armor_cursor.png");
    loadTexture(waterArmorCursorTex_, renderer, menuPath, "water_armor_cursor.png");

    // Load sound effects
    confirmSound_ = Mix_LoadWAV((assetPath + "sfx/confirm.wav").c_str());
    if (!confirmSound_)
        std::cerr << "Failed to load confirm.wav: " << Mix_GetError() << "\n";
    cancelSound_ = Mix_LoadWAV((assetPath + "sfx/cancel.wav").c_str());
    if (!cancelSound_)
        std::cerr << "Failed to load cancel.wav: " << Mix_GetError() << "\n";
}

bool Menu::handleInput(Input& input, PolarBear& bear, GameState& state,
                       bool& paused, int musicVolume, int pauseVolume, bool endingStage)
{
    // Menu (Tab key) - toggle menu (disabled during ending scene, but allow closing if already open)
    if (input.isMenuPressed() && !endingStage && (!paused || isOpen_))
    {
        isOpen_ = !isOpen_;
        if (isOpen_)
        {
            // Pause game and lower music volume
            paused = true;
            Mix_VolumeMusic(pauseVolume);

            // Set cursor and equipped armor based on current armor
            if (bear.element == PolarBear::Element::Earth)
            {
                armorCursor_ = 0;
                equippedArmor_ = 0;
            }
            else if (bear.element == PolarBear::Element::Wind)
            {
                armorCursor_ = 1;
                equippedArmor_ = 1;
            }
            else if (bear.element == PolarBear::Element::Fire)
            {
                armorCursor_ = 2;
                equippedArmor_ = 2;
            }
            else if (bear.element == PolarBear::Element::Water)
            {
                armorCursor_ = 3;
                equippedArmor_ = 3;
            }
            else
            {
                armorCursor_ = 0;
                equippedArmor_ = -1; // No armor equipped
            }
        }
        else
        {
            // Unpause game and restore music volume
            paused = false;
            Mix_VolumeMusic(musicVolume);
        }
    }

    // Process menu inputs if menu is open
    if (isOpen_)
    {
        // Navigate armor cursor with A (left) and D (right)
        if (input.isMovingLeft() && !aHeld_)
        {
            aHeld_ = true;
            armorCursor_ = (armorCursor_ + 3) % 4; // Move left (wrap around)
        }
        else if (!input.isMovingLeft())
        {
            aHeld_ = false;
        }

        if (input.isMovingRight() && !dHeld_)
        {
            dHeld_ = true;
            armorCursor_ = (armorCursor_ + 1) % 4; // Move right (wrap around)
        }
        else if (!input.isMovingRight())
        {
            dHeld_ = false;
        }

        // J key: Equip selected armor
        if (input.isJumping() && !jHeld_)
        {
            jHeld_ = true;
            bool armorAvailable = false;

            // Check if armor is available
            switch (armorCursor_)
            {
                case 0: armorAvailable = state.hasEarthArmor(); break;
                case 1: armorAvailable = state.hasWindArmor(); break;
                case 2: armorAvailable = state.hasFireArmor(); break;
                case 3: armorAvailable = state.hasWaterArmor(); break;
            }

            if (armorAvailable)
            {
                // Equip armor
                switch (armorCursor_)
                {
                    case 0: bear.setElement(PolarBear::Element::Earth); break;
                    case 1: bear.setElement(PolarBear::Element::Wind); break;
                    case 2: bear.setElement(PolarBear::Element::Fire); break;
                    case 3: bear.setElement(PolarBear::Element::Water); break;
                }
                equippedArmor_ = armorCursor_;
                if (confirmSound_)
                    Mix_PlayChannel(-1, confirmSound_, 0);
            }
            else
            {
                // Armor not available
                if (cancelSound_)
                    Mix_PlayChannel(-1, cancelSound_, 0);
            }
        }
        else if (!input.isJumping())
        {
            jHeld_ = false;
        }

        // K key: Unequip armor
        if (input.isAttacking() && !kHeld_)
        {
            kHeld_ = true;
            if (equippedArmor_ != -1)
            {
                // Unequip armor
                bear.setElement(PolarBear::Element::None);
                equippedArmor_ = -1;
                if (cancelSound_)
                    Mix_PlayChannel(-1, cancelSound_, 0);
            }
        }
        else if (!input.isAttacking())
        {
            kHeld_ = false;
        }

        return true; // Menu consumed input
    }

    return false; // Menu not open
}

void Menu::render(SDL_Renderer* renderer, const Camera& camera, const GameState& state)
{
    if (!isOpen_ || !backgroundTex_)
        return;

    // Render background
    SDL_Rect dest{0, 0, camera.width, camera.height};
    SDL_RenderCopy(renderer, backgroundTex_, nullptr, &dest);

    // Render skills (images have built-in positioning)
    SDL_Rect fullScreenDest{0, 0, camera.width, camera.height};

    // Slash (always shown)
    if (slashIconTex_)
        SDL_RenderCopy(renderer, slashIconTex_, nullptr, &fullScreenDest);

    // Climb (if learned)
    if (state.hasClimb() && climbIconTex_)
        SDL_RenderCopy(renderer, climbIconTex_, nullptr, &fullScreenDest);

    // Ice Breath (if learned)
    if (state.hasIceBreath() && iceBreathIconTex_)
        SDL_RenderCopy(renderer, iceBreathIconTex_, nullptr, &fullScreenDest);

    // Dash (if learned)
    if (state.hasDash() && dashIconTex_)
        SDL_RenderCopy(renderer, dashIconTex_, nullptr, &fullScreenDest);

    // Render equipped armor only (if any)
    if (equippedArmor_ == 0 && earthArmorTex_)
        SDL_RenderCopy(renderer, earthArmorTex_, nullptr, &fullScreenDest);
    else if (equippedArmor_ == 1 && windArmorTex_)
        SDL_RenderCopy(renderer, windArmorTex_, nullptr, &fullScreenDest);
    else if (equippedArmor_ == 2 && fireArmorTex_)
        SDL_RenderCopy(renderer, fireArmorTex_, nullptr, &fullScreenDest);
    else if (equippedArmor_ == 3 && waterArmorTex_)
        SDL_RenderCopy(renderer, waterArmorTex_, nullptr, &fullScreenDest);

    // Render armor cursor based on selection (cursor images have built-in offsets)
    SDL_Texture* cursorTex = nullptr;

    switch (armorCursor_)
    {
        case 0: // Earth
            cursorTex = earthArmorCursorTex_;
            break;
        case 1: // Wind
            cursorTex = windArmorCursorTex_;
            break;
        case 2: // Fire
            cursorTex = fireArmorCursorTex_;
            break;
        case 3: // Water
            cursorTex = waterArmorCursorTex_;
            break;
    }

    if (cursorTex)
    {
        // Render cursor at full screen with built-in positioning
        SDL_Rect cursorDest{0, 0, camera.width, camera.height};
        SDL_RenderCopy(renderer, cursorTex, nullptr, &cursorDest);
    }
}

void Menu::cleanup()
{
    if (backgroundTex_) SDL_DestroyTexture(backgroundTex_);
    if (slashIconTex_) SDL_DestroyTexture(slashIconTex_);
    if (climbIconTex_) SDL_DestroyTexture(climbIconTex_);
    if (iceBreathIconTex_) SDL_DestroyTexture(iceBreathIconTex_);
    if (dashIconTex_) SDL_DestroyTexture(dashIconTex_);
    if (earthArmorTex_) SDL_DestroyTexture(earthArmorTex_);
    if (windArmorTex_) SDL_DestroyTexture(windArmorTex_);
    if (fireArmorTex_) SDL_DestroyTexture(fireArmorTex_);
    if (waterArmorTex_) SDL_DestroyTexture(waterArmorTex_);
    if (earthArmorCursorTex_) SDL_DestroyTexture(earthArmorCursorTex_);
    if (windArmorCursorTex_) SDL_DestroyTexture(windArmorCursorTex_);
    if (fireArmorCursorTex_) SDL_DestroyTexture(fireArmorCursorTex_);
    if (waterArmorCursorTex_) SDL_DestroyTexture(waterArmorCursorTex_);

    if (confirmSound_) Mix_FreeChunk(confirmSound_);
    if (cancelSound_) Mix_FreeChunk(cancelSound_);

    backgroundTex_ = nullptr;
    slashIconTex_ = nullptr;
    climbIconTex_ = nullptr;
    iceBreathIconTex_ = nullptr;
    dashIconTex_ = nullptr;
    earthArmorTex_ = nullptr;
    windArmorTex_ = nullptr;
    fireArmorTex_ = nullptr;
    waterArmorTex_ = nullptr;
    earthArmorCursorTex_ = nullptr;
    windArmorCursorTex_ = nullptr;
    fireArmorCursorTex_ = nullptr;
    waterArmorCursorTex_ = nullptr;
    confirmSound_ = nullptr;
    cancelSound_ = nullptr;
}
