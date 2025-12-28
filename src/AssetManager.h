// Copyright 2025 Polar Bear Green World
// AssetManager.h
// Manages loading and unloading of game assets to optimize memory usage

#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <unordered_map>

class AssetManager
{
public:
    AssetManager() = default;
    ~AssetManager();

    // Prevent copying
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    // Texture management
    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& path);
    void unloadTexture(const std::string& path);
    void unloadAllTextures();

    // Music management
    Mix_Music* loadMusic(const std::string& path);
    void unloadMusic(const std::string& path);
    void unloadAllMusic();

    // Sound effect management
    Mix_Chunk* loadSound(const std::string& path);
    void unloadSound(const std::string& path);
    void unloadAllSounds();

    // Unload everything
    void clear();

private:
    std::unordered_map<std::string, SDL_Texture*> textures;
    std::unordered_map<std::string, Mix_Music*> music;
    std::unordered_map<std::string, Mix_Chunk*> sounds;
};
