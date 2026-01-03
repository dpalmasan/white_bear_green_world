#pragma once

#include "Cutscene.h"
#include <memory>
#include <unordered_map>
#include <functional>

class CutsceneManager
{
public:
    using CutsceneFactory = std::function<std::unique_ptr<GameCutscene>()>;
    
    CutsceneManager() = default;
    
    // Register a cutscene factory by ID
    void registerCutscene(const std::string& id, CutsceneFactory factory);
    
    // Trigger a cutscene by ID (creates and starts it)
    bool trigger(const std::string& id);
    
    // Update active cutscene
    void update(float dt, Camera& camera, const TileMap& map);
    
    // Check if any cutscene is active
    bool isActive() const { return activeCutscene_ != nullptr; }
    
    // Get active cutscene (nullptr if none)
    GameCutscene* getActive() const { return activeCutscene_.get(); }
    
    // Force end current cutscene
    void endCurrent();
    
    // Get fade alpha for letterbox bars (0-255, increases over time)
    int getLetterboxAlpha() const { return letterboxAlpha_; }
    
    // Keep letterbox visible (for post-cutscene transitions)
    void keepLetterboxVisible() { keepLetterbox_ = true; }
    void hideLetterbox() { keepLetterbox_ = false; letterboxAlpha_ = 0; letterboxFadeTimer_ = 0.0f; }
    bool isLetterboxVisible() const { return keepLetterbox_ || isActive(); }
    
private:
    std::unordered_map<std::string, CutsceneFactory> factories_;
    std::unique_ptr<GameCutscene> activeCutscene_;
    int letterboxAlpha_ = 0;
    float letterboxFadeTimer_ = 0.0f;
    bool keepLetterbox_ = false;
    static constexpr float LETTERBOX_FADE_DURATION = 0.5f;
};
