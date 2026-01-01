#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <array>
#include "core/Camera.h"
#include "core/GameConstants.h"
#include "systems/GameState.h"

// SaveScreen mode: SAVE (from world map) or LOAD (from title screen)
enum class SaveScreenMode
{
    SAVE,  // Save current game state to selected slot
    LOAD   // Load game state from selected slot
};

// SaveScreen displays 3 save slots with game progress icons
// Accessible via ESC/Tab from world map (SAVE) or from title screen Continue option (LOAD)
class SaveScreen
{
public:
    SaveScreen() = default;
    explicit SaveScreen(SaveScreenMode mode) : mode_(mode) {}
    ~SaveScreen();

    // Load all textures and sounds from asset path
    bool loadAssets(SDL_Renderer* renderer, const std::string& assetPath);
    
    // Handle input (up/down to move cursor, ESC/Tab to close, J to confirm)
    // Returns true if screen should close
    bool handleInput(const class Input& input);
    
    // Check if user selected a slot to load (only in LOAD mode)
    bool shouldLoadGame() const { return shouldLoad_; }
    
    // Get the GameState from the selected slot (for loading)
    const GameState& getSelectedSlotState() const { return slotStates_[selectedSlot_]; }
    
    // Set the screen mode (SAVE or LOAD)
    void setMode(SaveScreenMode mode) { mode_ = mode; shouldLoad_ = false; }
    
    // Render the save screen (background, slots, cursor)
    void render(SDL_Renderer* renderer, const Camera& camera);
    
    // Get/set which slot is currently selected (0-2)
    int getSelectedSlot() const { return selectedSlot_; }
    void setSelectedSlot(int slot) { selectedSlot_ = slot; }
    
    // Save current game state to selected slot
    void saveToSlot(const GameState& currentState);
    
    // Load all save slots from disk
    void loadSlots();
    
private:
    // Textures
    SDL_Texture* backgroundTexture_ = nullptr;  // Background image
    SDL_Texture* hudTexture_ = nullptr;      // Background with 3 slots
    SDL_Texture* cursorTexture_ = nullptr;   // Selection cursor
    SDL_Texture* iconsTexture_ = nullptr;    // Spritesheet of icons
    
    // Sound effects
    Mix_Chunk* confirmSound_ = nullptr;       // Sound when saving/loading
    
    // Screen mode and state
    SaveScreenMode mode_ = SaveScreenMode::SAVE;
    bool shouldLoad_ = false;  // Set to true when user confirms load in LOAD mode
    
    // Cursor state
    int selectedSlot_ = 0;  // 0-2 for three slots
    
    // Save slot states (loaded from disk)
    std::array<GameState, 3> slotStates_;
    
    // Track which slots have valid save files
    std::array<bool, 3> slotHasSave_ = {false, false, false};
    
    // Asset path for save files
    std::string assetPath_;
    
    // Layout constants (using GameConstants where appropriate)
    static constexpr int ICON_SIZE = 32;
    // Icons use around 1/3 of the total surface// Icons use around 1/3 of the total surface
    static constexpr int ICON_EFFECTIVE_SIZE = ICON_SIZE / 3;
    static constexpr int CURSOR_HEIGHT = 76;
    static constexpr int SLOT_GAP = 4;
    static constexpr int HEART_START_X = 74;
    static constexpr int HEART_START_Y = 16;
    static constexpr int HEART_SPACING = 3;
    static constexpr int ARMOR_OFFSET_X = -48;
    static constexpr int ARMOR_OFFSET_Y = 24;
    static constexpr int ARMOR_SPACING = 12;
    static constexpr int SKILL_OFFSET_X = 70;
    static constexpr int SKILL_SPACING = -10;
    
    // Icon indices in spritesheet
    enum class IconType
    {
        EarthArmor = 0,
        WindArmor = 1,
        FireArmor = 2,
        WaterArmor = 3,
        Slash = 4,
        IceBreath = 5,
        Climb = 6,
        Dash = 7,
        FullHeart = 8,
        EmptyHeart = 9
    };
    
    // Render a single icon from the spritesheet
    void renderIcon(SDL_Renderer* renderer, IconType icon, int x, int y, int scaledWidth, int scaledHeight);
    
    // Render game state icons for a specific slot
    void renderSlotIcons(SDL_Renderer* renderer, int slotIndex, const GameState& state, int cameraWidth, int cameraHeight);
    
    // Calculate Y position for a given slot (0-2)
    int getSlotY(int slotIndex, int cameraHeight) const;
    
    // Get save file path for a slot (0-2)
    std::string getSaveFilePath(int slotIndex) const;
};
