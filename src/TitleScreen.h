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

    // Check if player wants to start the game
    bool shouldStartGame() const { return startGame; }

    // Reset the title screen state
    void reset();

   private:
    SDL_Texture* backgroundTexture = nullptr;
    SDL_Texture* titleLettersTexture = nullptr;
    Mix_Music* titleMusic          = nullptr;

    float elapsedTime = 0.0f;
    bool startGame    = false;
    bool musicStarted = false;
};
