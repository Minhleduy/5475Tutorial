#include "Wall.h"
#include <cstdlib>
#include "Constant.h"
#include <iostream>

Wall::Wall(int startX, int startY, SDL_Texture* texture) {
    x = startX;
    y = startY;
    active = true;
    rect = {x, y, TILE_SIZE, TILE_SIZE};
    wallTexture = texture;
}
void Wall::render(SDL_Renderer* renderer) {
    if (active && wallTexture) {
        SDL_RenderCopy(renderer, wallTexture, nullptr, &rect);
    }
}
Wall::~Wall() {
}
