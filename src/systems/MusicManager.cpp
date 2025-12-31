// Copyright 2025 Polar Bear Green World
// MusicManager.cpp
// Implementation of music management system

#include "MusicManager.h"
#include <iostream>

MusicManager::~MusicManager()
{
    unloadAll();
}

Mix_Music* MusicManager::loadTrack(const std::string& path)
{
    // Check if already loaded
    auto it = tracks_.find(path);
    if (it != tracks_.end())
    {
        return it->second.music;
    }
    
    // Load new track
    Mix_Music* music = Mix_LoadMUS(path.c_str());
    if (!music)
    {
        std::cerr << "MusicManager: Failed to load '" << path << "': " << Mix_GetError() << "\n";
        return nullptr;
    }
    
    // Store track with default metadata
    MusicTrack track;
    track.music = music;
    track.path = path;
    tracks_[path] = track;
    
    return music;
}

Mix_Music* MusicManager::loadTrack(const std::string& path,
                                   double loopStartSec,
                                   double loopEndSec,
                                   bool hasIntro,
                                   double introEndSec)
{
    // Load the music file
    Mix_Music* music = loadTrack(path);
    if (!music)
        return nullptr;
    
    // Set loop point metadata
    auto it = tracks_.find(path);
    if (it != tracks_.end())
    {
        it->second.loopStartSec = loopStartSec;
        it->second.loopEndSec = loopEndSec;
        it->second.hasIntro = hasIntro;
        it->second.introEndSec = introEndSec;
    }
    
    return music;
}

void MusicManager::unloadTrack(const std::string& path)
{
    auto it = tracks_.find(path);
    if (it != tracks_.end())
    {
        if (it->second.music)
        {
            // Stop if currently playing
            if (currentTrack_ == path)
            {
                Mix_HaltMusic();
                currentTrack_.clear();
            }
            
            Mix_FreeMusic(it->second.music);
        }
        tracks_.erase(it);
    }
}

void MusicManager::unloadAll()
{
    Mix_HaltMusic();
    currentTrack_.clear();
    
    for (auto& pair : tracks_)
    {
        if (pair.second.music)
        {
            Mix_FreeMusic(pair.second.music);
        }
    }
    tracks_.clear();
}

bool MusicManager::play(const std::string& path, int loops, MusicChannel channel)
{
    auto it = tracks_.find(path);
    if (it == tracks_.end())
    {
        std::cerr << "MusicManager: Track '" << path << "' not loaded\n";
        return false;
    }
    
    // Note: SDL_mixer's Mix_PlayMusic doesn't support custom loop points natively
    // For now, we use standard looping. Custom loop points would require
    // additional implementation with Mix_SetMusicPosition or manual tracking
    
    if (Mix_PlayMusic(it->second.music, loops) < 0)
    {
        std::cerr << "MusicManager: Failed to play '" << path << "': " << Mix_GetError() << "\n";
        return false;
    }
    
    currentTrack_ = path;
    currentChannel_ = channel;
    applyVolume();
    
    return true;
}

bool MusicManager::fadeIn(const std::string& path, int fadeMs, int loops, MusicChannel channel)
{
    auto it = tracks_.find(path);
    if (it == tracks_.end())
    {
        std::cerr << "MusicManager: Track '" << path << "' not loaded\n";
        return false;
    }
    
    if (Mix_FadeInMusic(it->second.music, loops, fadeMs) < 0)
    {
        std::cerr << "MusicManager: Failed to fade in '" << path << "': " << Mix_GetError() << "\n";
        return false;
    }
    
    currentTrack_ = path;
    currentChannel_ = channel;
    applyVolume();
    
    return true;
}

void MusicManager::fadeOut(int fadeMs)
{
    if (!Mix_FadeOutMusic(fadeMs))
    {
        // FadeOut returns 0 if no music playing, which is fine
    }
}

void MusicManager::stop()
{
    Mix_HaltMusic();
    currentTrack_.clear();
}

void MusicManager::pause()
{
    Mix_PauseMusic();
}

void MusicManager::resume()
{
    Mix_ResumeMusic();
}

bool MusicManager::isPaused() const
{
    return Mix_PausedMusic() == 1;
}

bool MusicManager::isPlaying() const
{
    return Mix_PlayingMusic() == 1;
}

void MusicManager::setVolume(int volume)
{
    baseVolume_ = volume;
    applyVolume();
}

int MusicManager::getVolume() const
{
    return baseVolume_;
}

void MusicManager::setChannelVolume(MusicChannel channel, float volume)
{
    int idx = static_cast<int>(channel);
    if (idx >= 0 && idx < 5)
    {
        channelVolumes_[idx] = volume;
        applyVolume();
    }
}

float MusicManager::getChannelVolume(MusicChannel channel) const
{
    int idx = static_cast<int>(channel);
    if (idx >= 0 && idx < 5)
    {
        return channelVolumes_[idx];
    }
    return 1.0f;
}

void MusicManager::setLoopPoints(const std::string& path, double loopStartSec, double loopEndSec)
{
    auto it = tracks_.find(path);
    if (it != tracks_.end())
    {
        it->second.loopStartSec = loopStartSec;
        it->second.loopEndSec = loopEndSec;
    }
}

void MusicManager::setIntro(const std::string& path, double introEndSec)
{
    auto it = tracks_.find(path);
    if (it != tracks_.end())
    {
        it->second.hasIntro = true;
        it->second.introEndSec = introEndSec;
    }
}

const MusicTrack* MusicManager::getTrackInfo(const std::string& path) const
{
    auto it = tracks_.find(path);
    if (it != tracks_.end())
    {
        return &it->second;
    }
    return nullptr;
}

bool MusicManager::crossfade(const std::string& newPath, int fadeDurationMs, int loops, MusicChannel channel)
{
    // Fade out current music
    fadeOut(fadeDurationMs);
    
    // Wait a frame for fade to start, then fade in new track
    // Note: This is a simple implementation. For perfect crossfade,
    // you'd need to track fade progress and start new track mid-fade
    return fadeIn(newPath, fadeDurationMs, loops, channel);
}

void MusicManager::update()
{
    // This method can be used for manual loop point handling in the future
    // For example, using Mix_GetMusicPosition() to detect when to loop back
    // Currently, SDL_mixer handles basic looping automatically
    
    // Potential future implementation:
    // if (isPlaying() && !currentTrack_.empty()) {
    //     const MusicTrack* track = getTrackInfo(currentTrack_);
    //     if (track && track->loopEndSec > 0.0) {
    //         double pos = Mix_GetMusicPosition(track->music);
    //         if (pos >= track->loopEndSec) {
    //             Mix_SetMusicPosition(track->loopStartSec);
    //         }
    //     }
    // }
}

void MusicManager::applyVolume()
{
    // Calculate final volume: base * channel multiplier
    int channelIdx = static_cast<int>(currentChannel_);
    float multiplier = (channelIdx >= 0 && channelIdx < 5) ? channelVolumes_[channelIdx] : 1.0f;
    
    int finalVolume = static_cast<int>(baseVolume_ * multiplier);
    finalVolume = std::max(0, std::min(128, finalVolume));  // Clamp to valid range
    
    Mix_VolumeMusic(finalVolume);
}
