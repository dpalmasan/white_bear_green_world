#pragma once

#include <SDL2/SDL_rect.h>

namespace Collision
{
// Tighten a rectangle by an inset fraction on each side (clamped to non-negative size).
SDL_Rect shrinkRect(const SDL_Rect &r, float insetFrac);

// Simple AABB intersection test.
bool intersects(const SDL_Rect &a, const SDL_Rect &b);
}
