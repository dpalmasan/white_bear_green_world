#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <string>
#include <vector>

// Generic cutscene that displays a sequence of images with fade transitions
class Cutscene
{
   public:
    Cutscene();
    ~Cutscene();

    // Load scene images from a folder with pattern scene-1.png through scene-N.png
    // musicPath: path to music file (optional, can be empty)
    // totalScenes: number of scenes to load
    // skippable: whether pressing 'j' key skips the cutscene
    bool load(SDL_Renderer* renderer, const std::string& imageFolderPath, int totalScenes,
              const std::string& musicPath = "", bool skippable = true);

    // Update scene state and transitions
    void update(float dt);

    // Render the current scene
    void render(SDL_Renderer* renderer);

    // Start the cutscene sequence
    void start();

    // Check if cutscene is complete
    bool isComplete() const { return complete; }

    // Reset the cutscene
    void reset();

    // Check if this cutscene can be skipped
    bool canBeSkipped() const { return skippable; }

   private:
    enum State
    {
        FadeIn,
        Show,
        FadeOut
    };

    std::vector<SDL_Texture*> sceneTextures;
    Mix_Music* cutsceneMusic = nullptr;

    int currentScene      = 0;
    int totalScenes       = 0;
    State currentState    = FadeIn;
    float stateTime       = 0.0f;
    float alpha           = 0.0f;
    bool complete         = false;
    bool musicStarted     = false;
    bool skippable        = true;

    static constexpr float FADE_DURATION = 3.0f;   // 3 seconds fade
    static constexpr float SHOW_DURATION = 5.0f;   // 5 seconds display
};

