#include "SaveScreen.h"

#include <SDL2/SDL_image.h>
#include <iostream>

#include "core/Input.h"

SaveScreen::~SaveScreen()
{
    if (backgroundTexture_)
        SDL_DestroyTexture(backgroundTexture_);
    if (hudTexture_)
        SDL_DestroyTexture(hudTexture_);
    if (cursorTexture_)
        SDL_DestroyTexture(cursorTexture_);
    if (iconsTexture_)
        SDL_DestroyTexture(iconsTexture_);
    if (confirmSound_)
        Mix_FreeChunk(confirmSound_);
}

bool SaveScreen::loadAssets(SDL_Renderer* renderer, const std::string& assetPath)
{
    assetPath_ = assetPath;
    
    // Load background image
    std::string backgroundPath = assetPath + "images/menu/saving-game-background.png";
    SDL_Surface* backgroundSurface = IMG_Load(backgroundPath.c_str());
    if (!backgroundSurface)
    {
        std::cerr << "Failed to load " << backgroundPath << ": " << IMG_GetError() << "\n";
        return false;
    }
    backgroundTexture_ = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);
    if (!backgroundTexture_)
    {
        std::cerr << "Failed to create background texture: " << SDL_GetError() << "\n";
        return false;
    }
    
    // Load HUD background (320x240)
    std::string hudPath = assetPath + "images/menu/saving-game-hud.png";
    SDL_Surface* hudSurface = IMG_Load(hudPath.c_str());
    if (!hudSurface)
    {
        std::cerr << "Failed to load " << hudPath << ": " << IMG_GetError() << "\n";
        return false;
    }
    hudTexture_ = SDL_CreateTextureFromSurface(renderer, hudSurface);
    SDL_FreeSurface(hudSurface);
    if (!hudTexture_)
    {
        std::cerr << "Failed to create HUD texture: " << SDL_GetError() << "\n";
        return false;
    }

    // Load cursor (320x76)
    std::string cursorPath = assetPath + "images/menu/saving-game-cursor.png";
    SDL_Surface* cursorSurface = IMG_Load(cursorPath.c_str());
    if (!cursorSurface)
    {
        std::cerr << "Failed to load " << cursorPath << ": " << IMG_GetError() << "\n";
        return false;
    }
    cursorTexture_ = SDL_CreateTextureFromSurface(renderer, cursorSurface);
    SDL_FreeSurface(cursorSurface);
    if (!cursorTexture_)
    {
        std::cerr << "Failed to create cursor texture: " << SDL_GetError() << "\n";
        return false;
    }

    // Load icons spritesheet (32x32 frames)
    std::string iconsPath = assetPath + "images/menu/saving-game-icons.png";
    SDL_Surface* iconsSurface = IMG_Load(iconsPath.c_str());
    if (!iconsSurface)
    {
        std::cerr << "Failed to load " << iconsPath << ": " << IMG_GetError() << "\n";
        return false;
    }
    iconsTexture_ = SDL_CreateTextureFromSurface(renderer, iconsSurface);
    SDL_FreeSurface(iconsSurface);
    if (!iconsTexture_)
    {
        std::cerr << "Failed to create icons texture: " << SDL_GetError() << "\n";
        return false;
    }

    // Load confirm sound
    confirmSound_ = Mix_LoadWAV((assetPath + "sfx/confirm.wav").c_str());
    if (!confirmSound_)
    {
        std::cerr << "Warning: Failed to load confirm.wav: " << Mix_GetError() << "\n";
        // Not fatal - continue without sound
    }
    
    // Load all save slots from disk
    loadSlots();

    return true;
}

bool SaveScreen::handleInput(const Input& input)
{
    // ESC or Tab closes the save screen
    if (input.isMenuPressed())
    {
        return true;  // Close screen
    }

    // Up/Down to move cursor
    if (input.isClimbingUp())
    {
        selectedSlot_--;
        if (selectedSlot_ < 0)
            selectedSlot_ = 2;  // Wrap to bottom
    }
    else if (input.isClimbingDown())
    {
        selectedSlot_++;
        if (selectedSlot_ > 2)
            selectedSlot_ = 0;  // Wrap to top
    }

    return false;  // Keep screen open
}

void SaveScreen::render(SDL_Renderer* renderer, const Camera& camera)
{
    // Render background image (full screen)
    SDL_Rect backgroundRect{0, 0, camera.width, camera.height};
    SDL_RenderCopy(renderer, backgroundTexture_, nullptr, &backgroundRect);
    
    // Add semi-transparent black overlay (40% transparency = 60% opacity)
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 153);  // 153/255 ≈ 60% opacity
    SDL_RenderFillRect(renderer, &backgroundRect);
    
    // Render HUD (full screen, scaled to camera size)
    SDL_Rect hudRect{0, 0, camera.width, camera.height};
    SDL_RenderCopy(renderer, hudTexture_, nullptr, &hudRect);

    // Render icons for all 3 save slots (only if slot has save data)
    for (int i = 0; i < 3; ++i)
    {
        if (slotHasSave_[i])
        {
            renderSlotIcons(renderer, i, slotStates_[i], camera.width, camera.height);
        }
    }

    // Render cursor at selected slot position (scaled to camera dimensions)
    int cursorY = getSlotY(selectedSlot_, camera.height);
    int scaledCursorHeight = camera.height * CURSOR_HEIGHT / GameConstants::Display::LOGICAL_HEIGHT;
    SDL_Rect cursorRect{0, cursorY, camera.width, scaledCursorHeight};
    SDL_RenderCopy(renderer, cursorTexture_, nullptr, &cursorRect);
}

void SaveScreen::renderIcon(SDL_Renderer* renderer, IconType icon, int x, int y, int scaledWidth, int scaledHeight)
{
    int iconIndex = static_cast<int>(icon);
    SDL_Rect src{iconIndex * ICON_SIZE, 0, ICON_SIZE, ICON_SIZE};
    SDL_Rect dst{x, y, scaledWidth, scaledHeight};
    SDL_RenderCopy(renderer, iconsTexture_, &src, &dst);
}

void SaveScreen::renderSlotIcons(SDL_Renderer* renderer, int slotIndex, const GameState& state, int cameraWidth, int cameraHeight)
{
    // Calculate scale factors based on camera dimensions (same as Menu)
    float scaleX = static_cast<float>(cameraWidth) / GameConstants::Display::LOGICAL_WIDTH;
    float scaleY = static_cast<float>(cameraHeight) / GameConstants::Display::LOGICAL_HEIGHT;
    
    int slotY = getSlotY(slotIndex, cameraHeight);
    int scaledIconWidth = static_cast<int>(ICON_SIZE * scaleX);
    int scaledIconHeight = static_cast<int>(ICON_SIZE * scaleY);
    int scaledEffectiveSize = static_cast<int>(ICON_EFFECTIVE_SIZE * scaleX);

    // Render hearts (at relative position from slot top)
    int heartX = static_cast<int>(HEART_START_X * scaleX);
    int heartY = slotY + static_cast<int>(HEART_START_Y * scaleY);
    int scaledHeartSpacing = static_cast<int>(HEART_SPACING * scaleX);
    
    for (int i = 0; i < state.getMaxHearts(); ++i)
    {
        IconType heartIcon = (i < state.getCurrentHearts()) ? IconType::FullHeart : IconType::EmptyHeart;
        renderIcon(renderer, heartIcon, heartX, heartY, scaledIconWidth, scaledIconHeight);
        heartX += scaledEffectiveSize + 2 * scaledHeartSpacing;
    }

    // Render armors (offset below hearts)
    int armorY = heartY + static_cast<int>(ARMOR_OFFSET_Y * scaleY);
    int armorX = static_cast<int>(HEART_START_X * scaleX) + static_cast<int>(ARMOR_OFFSET_X * scaleX);
    int scaledArmorSpacing = static_cast<int>(ARMOR_SPACING * scaleX);
    if (state.hasEarthArmor())
    {
        int earthArmorX = armorX + scaledEffectiveSize + scaledArmorSpacing;
        renderIcon(renderer, IconType::EarthArmor, earthArmorX, armorY, scaledIconWidth, scaledIconHeight);
    }
    if (state.hasWindArmor())
    {
        int windArmorX = armorX  + 2 * (scaledEffectiveSize + scaledArmorSpacing);
        renderIcon(renderer, IconType::WindArmor, windArmorX, armorY, scaledIconWidth, scaledIconHeight);
    }
    if (state.hasFireArmor())
    {
        int fireArmorX = armorX + 3 * (scaledEffectiveSize + scaledArmorSpacing);
        renderIcon(renderer, IconType::FireArmor, fireArmorX, armorY, scaledIconWidth, scaledIconHeight);
    }
    if (state.hasWaterArmor())
    {
        int waterArmorX = armorX + 4 * (scaledEffectiveSize + scaledArmorSpacing);
        renderIcon(renderer, IconType::WaterArmor, waterArmorX, armorY, scaledIconWidth, scaledIconHeight);
    }

    // Render skills (same row as armors)
    int skillY = armorY;
    int skillX = static_cast<int>(HEART_START_X * scaleX) + static_cast<int>(SKILL_OFFSET_X * scaleX);
    int scaledSkillSpacing = static_cast<int>(SKILL_SPACING * scaleX);
    
    if (state.hasSlash())
    {
        int slashX = skillX + scaledIconWidth + scaledSkillSpacing;
        renderIcon(renderer, IconType::Slash, slashX, skillY, scaledIconWidth, scaledIconHeight);
    }
    if (state.hasIceBreath())
    {
        int iceBreathX = skillX + 2 * (scaledIconWidth + scaledSkillSpacing);
        renderIcon(renderer, IconType::IceBreath, iceBreathX, skillY, scaledIconWidth, scaledIconHeight);
    }
    if (state.hasClimb())
    {
        int climbX = skillX + 3 * (scaledIconWidth + scaledSkillSpacing);
        renderIcon(renderer, IconType::Climb, climbX, skillY, scaledIconWidth, scaledIconHeight);
    }
    if (state.hasDash())
    {
        int dashX = skillX + 4 * (scaledIconWidth + scaledSkillSpacing);
        renderIcon(renderer, IconType::Dash, dashX, skillY, scaledIconWidth, scaledIconHeight);
    }
}

int SaveScreen::getSlotY(int slotIndex, int cameraHeight) const
{
    // Scale all dimensions proportionally to camera height
    int scaledCursorHeight = cameraHeight * CURSOR_HEIGHT / GameConstants::Display::LOGICAL_HEIGHT;
    int scaledGap = cameraHeight * SLOT_GAP / GameConstants::Display::LOGICAL_HEIGHT;
    int scaledOffset = cameraHeight * 3 / GameConstants::Display::LOGICAL_HEIGHT;
    return slotIndex * scaledCursorHeight + scaledGap + scaledOffset;
}
void SaveScreen::saveToSlot(const GameState& currentState)
{
    // Debug: verify current state has armors
    std::cout << "Saving - Current state has wind armor: " << currentState.hasWindArmor() << "\n";
    std::cout << "Saving - Current state has climb: " << currentState.hasClimb() << "\n";
    
    std::string savePath = getSaveFilePath(selectedSlot_);
    if (currentState.saveToFile(savePath))
    {
        // Update the cached slot state
        slotStates_[selectedSlot_] = currentState;
        
        // Mark this slot as having a valid save
        slotHasSave_[selectedSlot_] = true;
        
        // Debug: verify cached state was updated
        std::cout << "After save - Slot " << selectedSlot_ << " has wind armor: " 
                  << slotStates_[selectedSlot_].hasWindArmor() << "\n";
        
        // Play confirm sound
        if (confirmSound_)
        {
            Mix_PlayChannel(-1, confirmSound_, 0);
        }
        
        std::cout << "Game saved to slot " << (selectedSlot_ + 1) << "\n";
    }
    else
    {
        std::cerr << "Failed to save game to slot " << (selectedSlot_ + 1) << "\n";
    }
}

void SaveScreen::loadSlots()
{
    for (int i = 0; i < 3; ++i)
    {
        std::string savePath = getSaveFilePath(i);
        // Try to load each slot; track if successful
        slotHasSave_[i] = slotStates_[i].loadFromFile(savePath);
    }
}

std::string SaveScreen::getSaveFilePath(int slotIndex) const
{
    return assetPath_ + "../savegame_slot" + std::to_string(slotIndex) + ".dat";
}