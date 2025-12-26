#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <string>

// Title screen that displays before the main game
class TitleScreen
{
   public:
    TitleScreen();
    ~TitleScreen();

    // Load assets (background image and music)
    bool load(SDL_Renderer* renderer, const std::string& assetPath);

    // Update title screen state
    void update(float dt);

    // Render the title screen
    void render(SDL_Renderer* renderer);

    // Start the title screen (play music, show screen)
    void start();

    // Handle input events for menu navigation
    void handleInput(SDL_Event& event);

    // Check if player wants to start the game
    bool shouldStartGame() const { return startGame; }

    // Check if player selected continue
    bool shouldContinue() const { return selectedIndex == 0; }

    // Reset the title screen state
    void reset();

    // Reset fade-in state (for transitioning from cutscene)
    void resetFadeIn();

   private:
    SDL_Texture* backgroundTexture = nullptr;
    SDL_Texture* titleLettersTexture = nullptr;
    SDL_Texture* newGameTexture = nullptr;
    SDL_Texture* continueTexture = nullptr;
    Mix_Music* titleMusic          = nullptr;

    float elapsedTime = 0.0f;
    float alpha       = 0.0f;
    bool startGame    = false;
    bool musicStarted = false;
    bool fadedIn      = false;

    // Menu selection: 0 = continue, 1 = new game
    int selectedIndex = 1;  // Start at new_game

    static constexpr float FADE_IN_DURATION = 3.0f;  // 3 seconds fade-in
};
