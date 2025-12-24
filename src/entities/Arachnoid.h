#pragma once
#include "Enemy.h"

// Arachnoid enemy: walks left/right and reverses at edges or walls.
class Arachnoid : public Enemy
{
   public:
    explicit Arachnoid(int w = 32, int h = 32)
    {
        width  = w;
        height = h;
        vx     = 50.0f;  // initial speed to the right
    }

    void updateBehavior(float dt, const TileMap& map) override;
    void render(SDL_Renderer* renderer, int camX, int camY) override;
};
