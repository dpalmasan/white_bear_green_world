#pragma once
#include "Enemy.h"
#include "core/GameConstants.h"

// Arachnoid enemy: walks left/right and reverses at edges or walls.
class Arachnoid : public Enemy
{
   public:
    explicit Arachnoid(int w = GameConstants::Enemies::Arachnoid::WIDTH,
                       int h = GameConstants::Enemies::Arachnoid::HEIGHT)
    {
        width  = w;
        height = h;
        vx     = GameConstants::Enemies::Arachnoid::SPEED;
    }

    void updateBehavior(float dt, const TileMap& map) override;
    void render(SDL_Renderer* renderer, int camX, int camY) override;
};
