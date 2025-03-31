#ifndef BULLET_H
#define BULLET_H

#include <SDL.h>
#include <vector>
#include <iostream>
#include "Wall.h"
#include "Constant.h"

using namespace std;

class PlayerTank;
class enemyTank;

class bullet {
public:
    int x, y;
    int speed;
    int dirX, dirY;
    SDL_Rect rect;
    bool active;
    bool type;
    bullet(int startX, int startY, int dx, int dy, int sped, bool typ);
    void move(vector<Wall>& walls, vector<enemyTank>& Enemies, PlayerTank& player, bool& isGameOver);
    void render(SDL_Renderer* renderer);
    bool checkCollision(SDL_Rect& target);
};

#endif
