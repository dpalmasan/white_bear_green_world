#include "WorldMap.h"

#include <SDL2/SDL_image.h>

#include <algorithm>
#include <cmath>

static float dot2(int ax, int ay, int bx, int by)
{
    return static_cast<float>(ax * bx + ay * by);
}

bool WorldMap::load(SDL_Renderer* renderer, const std::string& assetPath)
{
    // Load images
    SDL_Surface* bgSurf =
        IMG_Load((assetPath + "images/backgrounds/world-map.png").c_str());
    if (!bgSurf)
        return false;
    background = SDL_CreateTextureFromSurface(renderer, bgSurf);
    SDL_FreeSurface(bgSurf);
    if (!background)
        return false;

    SDL_Surface* curSurf = IMG_Load((assetPath + "images/icons/map-cursor.png").c_str());
    if (!curSurf)
        return false;
    cursorTex = SDL_CreateTextureFromSurface(renderer, curSurf);
    SDL_FreeSurface(curSurf);
    if (!cursorTex)
        return false;

    // Define locations in 256x256 map space
    locations.clear();
    locations.push_back({"Core Glacier", 97, 23});
    locations.push_back({"Polluted Waters", 21, 141});
    locations.push_back({"Snowy Cliffs", 113, 206});
    locations.push_back({"Wind Peaks", 204, 128});
    locations.push_back({"Machine Factory", 116, 135});

    // Initialize neighbor graph according to the provided structure:
    //
    //                         Core Glacier
    //                             |
    //                             |
    //   Polluted waters -- Machine Factory -- Wind Peaks
    //                             |
    //                             |
    //                         Snowy Cliffs
    neighbors.assign(locations.size(), Neighbors{});
    auto findIndex = [&](const char* name)
    {
        for (int i = 0; i < (int)locations.size(); ++i)
            if (locations[i].name == name)
                return i;
        return -1;
    };
    int iCore     = findIndex("Core Glacier");
    int iPolluted = findIndex("Polluted Waters");
    int iSnowy    = findIndex("Snowy Cliffs");
    int iWind     = findIndex("Wind Peaks");
    int iMachine  = findIndex("Machine Factory");

    if (iMachine >= 0)
    {
        if (iCore >= 0)
        {
            neighbors[iMachine].up = iCore;
            neighbors[iCore].down  = iMachine;
        }
        if (iSnowy >= 0)
        {
            neighbors[iMachine].down = iSnowy;
            neighbors[iSnowy].up     = iMachine;
        }
        if (iPolluted >= 0)
        {
            neighbors[iMachine].left   = iPolluted;
            neighbors[iPolluted].right = iMachine;
        }
        if (iWind >= 0)
        {
            neighbors[iMachine].right = iWind;
            neighbors[iWind].left     = iMachine;
        }
    }

    // Choose a starting node: pick the one closest to center as default
    const int cx = 128, cy = 128;
    int bestIdx   = 0;
    int bestDist2 = 999999;
    for (int i = 0; i < (int)locations.size(); ++i)
    {
        int dx = locations[i].x - cx;
        int dy = locations[i].y - cy;
        int d2 = dx * dx + dy * dy;
        if (d2 < bestDist2)
        {
            bestDist2 = d2;
            bestIdx   = i;
        }
    }
    currentIndex = bestIdx;
    cursorX      = static_cast<float>(locations[currentIndex].x);
    cursorY      = static_cast<float>(locations[currentIndex].y);
    targetX      = cursorX;
    targetY      = cursorY;

    return true;
}

void WorldMap::handleEvent(const SDL_Event& e)
{
    if (e.type != SDL_KEYDOWN)
        return;
    const SDL_Keycode key = e.key.keysym.sym;
    int dirX = 0, dirY = 0;
    // WASD navigation
    if (key == SDLK_d)
        dirX = 1;
    else if (key == SDLK_a)
        dirX = -1;
    else if (key == SDLK_s)
        dirY = 1;
    else if (key == SDLK_w)
        dirY = -1;
    else
        return;

    int next = nextIndexInDirection(dirX, dirY);
    if (next != currentIndex)
    {
        currentIndex = next;
        targetX      = static_cast<float>(locations[currentIndex].x);
        targetY      = static_cast<float>(locations[currentIndex].y);
    }
}

void WorldMap::update(float dt)
{
    // Smoothly approach target at constant speed
    float dx   = targetX - cursorX;
    float dy   = targetY - cursorY;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist > 0.001f)
    {
        float step = moveSpeed * dt;
        if (step >= dist)
        {
            cursorX = targetX;
            cursorY = targetY;
        }
        else
        {
            cursorX += dx / dist * step;
            cursorY += dy / dist * step;
        }
    }
}

void WorldMap::render(SDL_Renderer* renderer, int viewW, int viewH)
{
    // Render background scaled to fit view preserving aspect ratio
    int texW = 0, texH = 0;
    SDL_QueryTexture(background, nullptr, nullptr, &texW, &texH);
    if (texW == 0 || texH == 0)
        return;

    // Stretch to fill entire view (fit screen)
    float scaleX = static_cast<float>(viewW) / texW;
    float scaleY = static_cast<float>(viewH) / texH;
    int offX     = 0;
    int offY     = 0;

    SDL_Rect dst{offX, offY, viewW, viewH};
    SDL_RenderCopy(renderer, background, nullptr, &dst);

    // Cursor: map space (256x256) -> stretched screen space
    const float sx = scaleX;
    const float sy = scaleY;

    int cursorTexW = 32, cursorTexH = 32;  // expected size
    SDL_QueryTexture(cursorTex, nullptr, nullptr, &cursorTexW, &cursorTexH);

    float screenX = offX + cursorX * sx - (cursorTexW * 0.5f) * sx - (cursorOffsetX * sx);
    float screenY = offY + cursorY * sy - (cursorTexH * 0.5f) * sy - (cursorOffsetY * sy);

    SDL_Rect cDst{static_cast<int>(screenX), static_cast<int>(screenY),
                  static_cast<int>(cursorTexW * sx), static_cast<int>(cursorTexH * sy)};
    SDL_RenderCopy(renderer, cursorTex, nullptr, &cDst);

    // Debug: draw small squares at each location's exact center to verify anchoring
    if (debug)
    {
        for (int i = 0; i < (int)locations.size(); ++i)
        {
            int lx = static_cast<int>(locations[i].x * sx);
            int ly = static_cast<int>(locations[i].y * sy);
            int px = offX + lx - 2;  // center a 4x4 marker
            int py = offY + ly - 2;
            SDL_Rect mr{px, py, 4, 4};
            if (i == currentIndex)
                SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
            else
                SDL_SetRenderDrawColor(renderer, 80, 255, 80, 255);
            SDL_RenderFillRect(renderer, &mr);
        }
    }
}

void WorldMap::clean()
{
    if (cursorTex)
    {
        SDL_DestroyTexture(cursorTex);
        cursorTex = nullptr;
    }
    if (background)
    {
        SDL_DestroyTexture(background);
        background = nullptr;
    }
}

int WorldMap::nextIndexInDirection(int dirX, int dirY) const
{
    if (currentIndex < 0 || currentIndex >= (int)neighbors.size())
        return currentIndex;
    const auto& nb = neighbors[currentIndex];
    if (dirX > 0 && nb.right != -1)
        return nb.right;
    if (dirX < 0 && nb.left != -1)
        return nb.left;
    if (dirY > 0 && nb.down != -1)
        return nb.down;
    if (dirY < 0 && nb.up != -1)
        return nb.up;
    return currentIndex;
}
