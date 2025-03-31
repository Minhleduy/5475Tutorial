#ifndef WALL_H
#define WALL_H

#include <SDL.h>
#include <SDL_image.h>

class Wall {
public:
    int x, y;
    bool active;
    SDL_Rect rect;
    SDL_Texture* wallTexture;
    Wall(int startX, int startY, SDL_Texture* texture);
    void render(SDL_Renderer* renderer);
    ~Wall();
};

#endif
