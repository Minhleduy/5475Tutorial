#ifndef ENEMYTANK_H
#define ENEMYTANK_H

#include <vector>
#include <SDL.h>
#include "PlayerTank.h"
#include "Wall.h"
#include "Bullet.h"
#include "Constant.h"

class enemyTank {
public:
    int startX, startY;
    int x, y;
    SDL_Rect rect;
    bool active;
    int dirX, dirY;
    Uint32 lastMoveTime;
    Uint32 lastShotTime;
    Uint32 deathTime;
    std::vector<bullet> bullets;
    std::vector<SDL_Texture*> upFrames;
    std::vector<SDL_Texture*> downFrames;
    std::vector<SDL_Texture*> leftFrames;
    std::vector<SDL_Texture*> rightFrames;
    SDL_Texture* deathFrame;
    int frame;
    Direction currentDirection;
    SDL_Texture* currentFrame;

    enemyTank(int startX, int startY, SDL_Renderer* renderer,
              std::vector<SDL_Texture*> up, std::vector<SDL_Texture*> down,
              std::vector<SDL_Texture*> left, std::vector<SDL_Texture*> right,
              SDL_Texture* death);
    void moveToward(PlayerTank& player, const std::vector<Wall>& walls);
    void shoot(std::vector<Wall>& walls, PlayerTank& player);
    void updatebullets(std::vector<Wall>& walls, std::vector<enemyTank>& Enemies, PlayerTank& player);
    void render(SDL_Renderer* renderer);
    ~enemyTank();
};

#endif
