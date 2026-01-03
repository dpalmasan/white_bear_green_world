#include "CutsceneManager.h"
#include <iostream>

void CutsceneManager::registerCutscene(const std::string& id, CutsceneFactory factory)
{
    factories_[id] = factory;
}

bool CutsceneManager::trigger(const std::string& id)
{
    // Don't start new cutscene if one is active
    if (activeCutscene_)
    {
        std::cerr << "[CutsceneManager] Cannot trigger '" << id 
                  << "' - cutscene already active\n";
        return false;
    }
    
    auto it = factories_.find(id);
    if (it == factories_.end())
    {
        std::cerr << "[CutsceneManager] Cutscene '" << id << "' not registered\n";
        return false;
    }
    
    // Create and start cutscene
    activeCutscene_ = it->second();
    activeCutscene_->start();
    
    std::cerr << "[CutsceneManager] Started cutscene: " << id << "\n";
    return true;
}

void CutsceneManager::update(float dt, Camera& camera, const TileMap& map)
{
    if (!activeCutscene_)
        return;
    
    // Fade in letterbox bars
    if (letterboxFadeTimer_ < LETTERBOX_FADE_DURATION)
    {
        letterboxFadeTimer_ += dt;
        if (letterboxFadeTimer_ > LETTERBOX_FADE_DURATION)
            letterboxFadeTimer_ = LETTERBOX_FADE_DURATION;
        letterboxAlpha_ = static_cast<int>((letterboxFadeTimer_ / LETTERBOX_FADE_DURATION) * 255.0f);
    }
    
    activeCutscene_->update(dt, camera, map);
    
    // Auto-end when complete
    if (activeCutscene_->isComplete())
    {
        std::cerr << "[CutsceneManager] Cutscene completed: " 
                  << activeCutscene_->getId() << "\n";
        activeCutscene_.reset();
        // Keep letterbox visible if flag is set (for camera transitions)
        if (!keepLetterbox_) {
            letterboxAlpha_ = 0;
            letterboxFadeTimer_ = 0.0f;
        }
    }
}

void CutsceneManager::endCurrent()
{
    if (activeCutscene_)
    {
        std::cerr << "[CutsceneManager] Force ending cutscene: " 
                  << activeCutscene_->getId() << "\n";
        activeCutscene_.reset();
    }
}
