// Copyright 2025 Polar Bear Green World
// MusicManager.h
// Manages music playback with loop points, fading, and volume control

#pragma once

#include <SDL2/SDL_mixer.h>
#include <string>
#include <unordered_map>
#include <memory>

// Metadata for a single music track
struct MusicTrack
{
    Mix_Music* music = nullptr;
    std::string path;
    
    // Loop point timing (in seconds)
    double loopStartSec = 0.0;      // Where to jump back to when looping
    double loopEndSec = -1.0;       // Where loop ends (-1 = end of file)
    double introEndSec = 0.0;       // Duration of non-looping intro
    bool hasIntro = false;          // Whether track has separate intro section
    
    // Default volume for this track (0-128)
    int defaultVolume = 96;
};

// Music playback channels for volume management
enum class MusicChannel
{
    Background,    // Normal stage music
    Boss,          // Boss battle music
    Cutscene,      // Story cutscene music
    Menu,          // Menu/UI music
    PowerUp        // Short stingers/jingles
};

class MusicManager
{
public:
    MusicManager() = default;
    ~MusicManager();
    
    // Prevent copying
    MusicManager(const MusicManager&) = delete;
    MusicManager& operator=(const MusicManager&) = delete;
    
    // === Loading ===
    
    // Load a music track from file
    Mix_Music* loadTrack(const std::string& path);
    
    // Load track with loop point metadata
    Mix_Music* loadTrack(const std::string& path, 
                        double loopStartSec, 
                        double loopEndSec = -1.0,
                        bool hasIntro = false,
                        double introEndSec = 0.0);
    
    // Unload a specific track
    void unloadTrack(const std::string& path);
    
    // Unload all tracks
    void unloadAll();
    
    // === Playback Control ===
    
    // Play a track immediately (loops = -1 for infinite)
    bool play(const std::string& path, int loops = -1, MusicChannel channel = MusicChannel::Background);
    
    // Fade in a track over specified duration (ms)
    bool fadeIn(const std::string& path, int fadeMs, int loops = -1, MusicChannel channel = MusicChannel::Background);
    
    // Fade out current music over specified duration (ms)
    void fadeOut(int fadeMs);
    
    // Stop music immediately
    void stop();
    
    // Pause/resume
    void pause();
    void resume();
    bool isPaused() const;
    bool isPlaying() const;
    
    // === Volume Control ===
    
    // Set volume for current track (0-128)
    void setVolume(int volume);
    
    // Get current volume
    int getVolume() const;
    
    // Set channel-specific volume multipliers (0.0 - 1.0)
    void setChannelVolume(MusicChannel channel, float volume);
    float getChannelVolume(MusicChannel channel) const;
    
    // === Loop Point Management ===
    
    // Set loop points for a track (must be loaded first)
    void setLoopPoints(const std::string& path, double loopStartSec, double loopEndSec = -1.0);
    
    // Set intro section (plays once before looping)
    void setIntro(const std::string& path, double introEndSec);
    
    // Get track metadata
    const MusicTrack* getTrackInfo(const std::string& path) const;
    
    // === Advanced ===
    
    // Crossfade to new track (fadeOut current, fadeIn new)
    bool crossfade(const std::string& newPath, int fadeDurationMs, int loops = -1, MusicChannel channel = MusicChannel::Background);
    
    // Update loop logic (call each frame if using manual loop points)
    void update();
    
private:
    // Track storage
    std::unordered_map<std::string, MusicTrack> tracks_;
    
    // Currently playing track
    std::string currentTrack_;
    MusicChannel currentChannel_ = MusicChannel::Background;
    
    // Channel volume multipliers
    float channelVolumes_[5] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    
    // Current base volume
    int baseVolume_ = 96;
    
    // Helper to apply channel volume
    void applyVolume();
};
