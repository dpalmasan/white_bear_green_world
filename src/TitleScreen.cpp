#include "TitleScreen.h"

#include <SDL2/SDL_image.h>

#include <iostream>

TitleScreen::TitleScreen() {}

TitleScreen::~TitleScreen()
{
    if (backgroundTexture)
    {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    if (titleLettersTexture)
    {
        SDL_DestroyTexture(titleLettersTexture);
        titleLettersTexture = nullptr;
    }
    if (titleMusic)
    {
        Mix_FreeMusic(titleMusic);
        titleMusic = nullptr;
    }
}

bool TitleScreen::load(SDL_Renderer* renderer, const std::string& assetPath)
{
    // Load background image
    backgroundTexture = IMG_LoadTexture(renderer, (assetPath + "title-screen.png").c_str());
    if (!backgroundTexture)
    {
        std::cerr << "Failed to load title-screen.png: " << IMG_GetError() << "\n";
        return false;
    }

    // Load title letters image
    titleLettersTexture = IMG_LoadTexture(renderer, (assetPath + "title-letters.png").c_str());
    if (!titleLettersTexture)
    {
        std::cerr << "Failed to load title-letters.png: " << IMG_GetError() << "\n";
        return false;
    }

    // Load title screen music
    titleMusic = Mix_LoadMUS((assetPath + "music/title_screen.ogg").c_str());
    if (!titleMusic)
    {
        std::cerr << "Failed to load title_screen.ogg: " << Mix_GetError() << "\n";
        return false;
    }

    return true;
}

void TitleScreen::update(float dt)
{
    elapsedTime += dt;
}

void TitleScreen::render(SDL_Renderer* renderer)
{
    if (!backgroundTexture)
        return;

    // Draw background to fill screen
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

    // Draw title letters at top-center with 70% opacity
    if (titleLettersTexture)
    {
        int winWidth  = 0;
        int winHeight = 0;
        SDL_GetRendererOutputSize(renderer, &winWidth, &winHeight);

        int texWidth  = 0;
        int texHeight = 0;
        SDL_QueryTexture(titleLettersTexture, nullptr, nullptr, &texWidth, &texHeight);

        // Scale to 36% of window width (1.2x larger), maintain aspect ratio
        int displayWidth = (int)(winWidth * 0.36f);
        int displayHeight = (texHeight > 0) ? (int)(displayWidth * texHeight / (float)texWidth) : texHeight;

        // Position at top-left area (20 pixels from top, positioned more to the left)
        int destX = (winWidth - displayWidth) / 8 - (int)(winWidth * 0.02f);
        int destY = 20;

        SDL_Rect destRect = {destX, destY, displayWidth, displayHeight};

        // Set 70% opacity (255 * 0.7 ≈ 179)
        SDL_SetTextureAlphaMod(titleLettersTexture, 179);
        SDL_RenderCopy(renderer, titleLettersTexture, nullptr, &destRect);
        SDL_SetTextureAlphaMod(titleLettersTexture, 255);  // Reset to full opacity
    }
}

void TitleScreen::start()
{
    if (!musicStarted && titleMusic)
    {
        Mix_PlayMusic(titleMusic, -1);  // Loop the title music
        musicStarted = true;
    }
}

void TitleScreen::reset()
{
    elapsedTime   = 0.0f;
    startGame     = false;
    musicStarted  = false;
    if (titleMusic)
        Mix_HaltMusic();
}
