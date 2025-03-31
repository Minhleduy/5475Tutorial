#ifndef PLAYERTANK_H
#define PLAYERTANK_H

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include "Bullet.h"
#include "Wall.h"
#include "enemyTank.h"
#include "Constant.h"
using namespace std;


class PlayerTank {
public:
    int x, y;
    int dirX, dirY;
    SDL_Rect rect;
    vector<bullet> bullets;
    Uint32 lastShotTime;
    int frame;
    bool isDead;
    Direction currentDirection;

    vector<SDL_Texture*> upFrames;
    vector<SDL_Texture*> downFrames;
    vector<SDL_Texture*> leftFrames;
    vector<SDL_Texture*> rightFrames;
    SDL_Texture* deathFrame;
    SDL_Texture* currentFrame;

    PlayerTank(int startX, int startY, SDL_Renderer* renderer);
    void move(int dx, int dy, const vector<Wall>& walls, const vector<enemyTank>& enemies);
    void updateAnimation();
    void shoot();
    void updatebullets(vector<Wall>& walls, vector<enemyTank>& enemies, PlayerTank& player);
    void render(SDL_Renderer* renderer);
    ~PlayerTank();
};

#endif
