#include "Cutscene.h"

#include <SDL2/SDL_image.h>

#include <iostream>

Cutscene::Cutscene() {}

Cutscene::~Cutscene()
{
    for (auto* texture : sceneTextures)
    {
        if (texture)
        {
            SDL_DestroyTexture(texture);
        }
    }
    sceneTextures.clear();

    if (cutsceneMusic)
    {
        Mix_FreeMusic(cutsceneMusic);
        cutsceneMusic = nullptr;
    }
}

bool Cutscene::load(SDL_Renderer* renderer, const std::string& imageFolderPath, int totalScenesCount,
                    const std::string& musicPath, bool isSkippable)
{
    totalScenes = totalScenesCount;
    skippable   = isSkippable;

    // Load all scene images
    for (int i = 1; i <= totalScenes; ++i)
    {
        std::string filename = imageFolderPath + "scene-" + std::to_string(i) + ".png";
        SDL_Texture* texture = IMG_LoadTexture(renderer, filename.c_str());
        if (!texture)
        {
            std::cerr << "Failed to load " << filename << ": " << IMG_GetError() << "\n";
            return false;
        }
        sceneTextures.push_back(texture);
    }

    // Load music if provided
    if (!musicPath.empty())
    {
        cutsceneMusic = Mix_LoadMUS(musicPath.c_str());
        if (!cutsceneMusic)
        {
            std::cerr << "Failed to load music: " << musicPath << ": " << Mix_GetError() << "\n";
            // Don't fail if music can't be loaded, just continue without it
        }
    }

    return true;
}

void Cutscene::update(float dt)
{
    if (complete)
        return;

    stateTime += dt;

    // Start music on first scene fade-in
    if (!musicStarted && currentScene == 0 && currentState == FadeIn && cutsceneMusic)
    {
        Mix_PlayMusic(cutsceneMusic, -1);  // Loop music
        musicStarted = true;
    }

    switch (currentState)
    {
        case FadeIn:
            alpha = stateTime / FADE_DURATION;
            if (alpha >= 1.0f)
            {
                alpha        = 1.0f;
                currentState = Show;
                stateTime    = 0.0f;
            }
            break;

        case Show:
            if (stateTime >= SHOW_DURATION)
            {
                currentState = FadeOut;
                stateTime    = 0.0f;
            }
            break;

        case FadeOut:
            alpha = 1.0f - (stateTime / FADE_DURATION);
            if (alpha <= 0.0f)
            {
                alpha = 0.0f;
                // Move to next scene
                currentScene++;
                if (currentScene >= totalScenes)
                {
                    complete = true;
                    // Don't stop music - let it continue playing
                }
                else
                {
                    currentState = FadeIn;
                    stateTime    = 0.0f;
                }
            }
            break;
    }
}

void Cutscene::render(SDL_Renderer* renderer)
{
    // Clear to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (currentScene < totalScenes && currentScene < (int)sceneTextures.size() && sceneTextures[currentScene])
    {
        // Set texture alpha for fade effect
        SDL_SetTextureAlphaMod(sceneTextures[currentScene], (Uint8)(alpha * 255));

        // Render scene image to fill screen
        SDL_RenderCopy(renderer, sceneTextures[currentScene], nullptr, nullptr);
    }
}

void Cutscene::start()
{
    currentScene  = 0;
    currentState  = FadeIn;
    stateTime     = 0.0f;
    alpha         = 0.0f;
    complete      = false;
    musicStarted  = false;
}

void Cutscene::reset()
{
    currentScene  = 0;
    currentState  = FadeIn;
    stateTime     = 0.0f;
    alpha         = 0.0f;
    complete      = false;
    musicStarted  = false;
    if (cutsceneMusic)
        Mix_HaltMusic();
}
