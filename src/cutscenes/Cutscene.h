#pragma once

#include <SDL2/SDL.h>
#include <string>

class Camera;
class TileMap;
class PolarBear;
class Boss;

// Base class for in-game cutscenes (different from screens/Cutscene which is for intro sequences)
// Each cutscene manages its own camera, entities, and logic
class GameCutscene
{
public:
    virtual ~GameCutscene() = default;
    
    // Called once when cutscene starts
    virtual void start() = 0;
    
    // Update cutscene logic and camera (called every frame)
    virtual void update(float dt, Camera& camera, const TileMap& map) = 0;
    
    // Check if cutscene is complete
    virtual bool isComplete() const = 0;
    
    // Get cutscene ID for identification
    virtual std::string getId() const = 0;
    
    // Check if cutscene should block player input
    virtual bool blocksInput() const { return true; }
};
