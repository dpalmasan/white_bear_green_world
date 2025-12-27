#include "Collision.h"

#include <algorithm>

namespace Collision
{
SDL_Rect shrinkRect(const SDL_Rect &r, float insetFrac)
{
    SDL_Rect s = r;
    if (insetFrac <= 0.0f)
        return s;

    int insetX = static_cast<int>(r.w * insetFrac);
    int insetY = static_cast<int>(r.h * insetFrac);

    s.x += insetX;
    s.y += insetY;
    s.w = std::max(0, r.w - 2 * insetX);
    s.h = std::max(0, r.h - 2 * insetY);
    return s;
}

bool intersects(const SDL_Rect &a, const SDL_Rect &b)
{
    return !(a.x + a.w < b.x || b.x + b.w < a.x || a.y + a.h < b.y || b.y + b.h < a.y);
}
}  // namespace Collision
