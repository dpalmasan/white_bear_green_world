#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>

// Forward declarations
class PolarBear;
class GameState;
class Input;
struct Camera;

// Menu system for armor selection and skill display.
// Handles Tab key menu with armor equipment/unequipment.
class Menu
{
public:
    // Armor menu indices
    enum class ArmorType : int
    {
        Earth = 0,
        Wind = 1,
        Fire = 2,
        Water = 3,
        None = -1
    };

    // Menu constants
    static constexpr int ARMOR_COUNT = 4;
    static constexpr int HEART_SIZE = 32;
    static constexpr int HEART_SPACING = 29;
    static constexpr int HEART_START_X = 21;
    static constexpr int HEART_START_Y = 20;
    static constexpr int HEART_FRAME_FULL = 0;
    static constexpr int HEART_FRAME_EMPTY = 1;

    Menu() = default;
    ~Menu() = default;

    // Load all menu assets (textures and sounds).
    void loadAssets(SDL_Renderer* renderer, const std::string& assetPath);

    // Handle menu input (Tab to open/close, A/D to navigate, J to equip, K to unequip).
    // Returns true if menu consumed the input (menu is open).
    bool handleInput(Input& input, PolarBear& bear, GameState& state, 
                     bool& paused, int musicVolume, int pauseVolume, bool endingStage);

    // Render the menu if open.
    void render(SDL_Renderer* renderer, const Camera& camera, const GameState& state);

    // Check if menu is currently open.
    bool isOpen() const { return isOpen_; }

    // Clean up resources.
    void cleanup();

private:
    // Menu state
    bool isOpen_ = false;
    ArmorType armorCursor_ = ArmorType::Earth;
    ArmorType equippedArmor_ = ArmorType::None;

    // Textures
    SDL_Texture* backgroundTex_ = nullptr;
    SDL_Texture* slashIconTex_ = nullptr;
    SDL_Texture* climbIconTex_ = nullptr;
    SDL_Texture* iceBreathIconTex_ = nullptr;
    SDL_Texture* dashIconTex_ = nullptr;
    SDL_Texture* earthArmorTex_ = nullptr;
    SDL_Texture* windArmorTex_ = nullptr;
    SDL_Texture* fireArmorTex_ = nullptr;
    SDL_Texture* waterArmorTex_ = nullptr;
    SDL_Texture* earthArmorCursorTex_ = nullptr;
    SDL_Texture* windArmorCursorTex_ = nullptr;
    SDL_Texture* fireArmorCursorTex_ = nullptr;
    SDL_Texture* waterArmorCursorTex_ = nullptr;
    SDL_Texture* heartTex_ = nullptr;  // 32x32 sprite with 2 frames (full/empty)

    // Sound effects
    Mix_Chunk* confirmSound_ = nullptr;
    Mix_Chunk* cancelSound_ = nullptr;

    // Input debouncing
    bool aHeld_ = false;
    bool dHeld_ = false;
    bool jHeld_ = false;
    bool kHeld_ = false;

    // Helper to load a single texture
    void loadTexture(SDL_Texture*& tex, SDL_Renderer* renderer, 
                     const std::string& path, const std::string& filename);
};
