// Copyright 2025 Polar Bear Green World
// AssetManager.cpp
// Implementation of asset manager for optimized memory usage

#include "AssetManager.h"
#include <SDL2/SDL_image.h>
#include <iostream>

AssetManager::~AssetManager()
{
    clear();
}

SDL_Texture* AssetManager::loadTexture(SDL_Renderer* renderer, const std::string& path)
{
    // Check if already loaded
    auto it = textures.find(path);
    if (it != textures.end())
    {
        return it->second;
    }

    // Load new texture
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture)
    {
        std::cerr << "Failed to load texture '" << path << "': " << IMG_GetError() << "\n";
        return nullptr;
    }

    textures[path] = texture;
    return texture;
}

void AssetManager::unloadTexture(const std::string& path)
{
    auto it = textures.find(path);
    if (it != textures.end())
    {
        if (it->second)
        {
            SDL_DestroyTexture(it->second);
        }
        textures.erase(it);
    }
}

void AssetManager::unloadAllTextures()
{
    for (auto& pair : textures)
    {
        if (pair.second)
        {
            SDL_DestroyTexture(pair.second);
        }
    }
    textures.clear();
}

Mix_Music* AssetManager::loadMusic(const std::string& path)
{
    // Check if already loaded
    auto it = music.find(path);
    if (it != music.end())
    {
        return it->second;
    }

    // Load new music
    Mix_Music* mus = Mix_LoadMUS(path.c_str());
    if (!mus)
    {
        std::cerr << "Failed to load music '" << path << "': " << Mix_GetError() << "\n";
        return nullptr;
    }

    music[path] = mus;
    return mus;
}

void AssetManager::unloadMusic(const std::string& path)
{
    auto it = music.find(path);
    if (it != music.end())
    {
        if (it->second)
        {
            Mix_FreeMusic(it->second);
        }
        music.erase(it);
    }
}

void AssetManager::unloadAllMusic()
{
    // Stop any playing music first
    if (Mix_PlayingMusic())
    {
        Mix_HaltMusic();
    }

    for (auto& pair : music)
    {
        if (pair.second)
        {
            Mix_FreeMusic(pair.second);
        }
    }
    music.clear();
}

Mix_Chunk* AssetManager::loadSound(const std::string& path)
{
    // Check if already loaded
    auto it = sounds.find(path);
    if (it != sounds.end())
    {
        return it->second;
    }

    // Load new sound
    Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
    if (!sound)
    {
        std::cerr << "Failed to load sound '" << path << "': " << Mix_GetError() << "\n";
        return nullptr;
    }

    sounds[path] = sound;
    return sound;
}

void AssetManager::unloadSound(const std::string& path)
{
    auto it = sounds.find(path);
    if (it != sounds.end())
    {
        if (it->second)
        {
            Mix_FreeChunk(it->second);
        }
        sounds.erase(it);
    }
}

void AssetManager::unloadAllSounds()
{
    for (auto& pair : sounds)
    {
        if (pair.second)
        {
            Mix_FreeChunk(pair.second);
        }
    }
    sounds.clear();
}

void AssetManager::clear()
{
    unloadAllTextures();
    unloadAllMusic();
    unloadAllSounds();
}
