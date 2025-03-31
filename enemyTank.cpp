#include "enemyTank.h"
#include <iostream>

enemyTank::enemyTank(int startX, int startY, SDL_Renderer* renderer,
                     std::vector<SDL_Texture*> up, std::vector<SDL_Texture*> down,
                     std::vector<SDL_Texture*> left, std::vector<SDL_Texture*> right,
                     SDL_Texture* death) {
    this->startX = startX;
    this->startY = startY;
    x = startX;
    y = startY;
    rect = {x, y, TILE_SIZE, TILE_SIZE};
    active = true;
    dirX = 0;
    dirY = 1;
    lastMoveTime = SDL_GetTicks();
    lastShotTime = 0;
    deathTime = 0;
    frame = 0;
    currentDirection = DOWN;
    currentFrame = nullptr;
    upFrames = up;
    downFrames = down;
    leftFrames = left;
    rightFrames = right;
    deathFrame = death;
}
void enemyTank::moveToward(PlayerTank& player, const std::vector<Wall>& walls) {
    if (!active) return;
    int targetX = player.x;
    int targetY = player.y;
    int dx = 0, dy = 0;
    if (rand() % 2 == 0) {
        dx = (targetX > x) ? 1 : (targetX < x) ? -1 : 0;
    } else {
        dy = (targetY > y) ? 1 : (targetY < y) ? -1 : 0;
    }
    int newX = x + dx;
    int newY = y + dy;

    SDL_Rect newRect = {newX, newY, TILE_SIZE, TILE_SIZE};
    SDL_Rect playerRect = {player.x, player.y, TILE_SIZE, TILE_SIZE};
    for (int i = 0; i < walls.size(); i++) {
        if (walls[i].active && SDL_HasIntersection(&newRect, &walls[i].rect)) {
            return;
        }
    }
    if (SDL_HasIntersection(&newRect, &playerRect))
        return;
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastMoveTime >= 500) {
        x = newX;
        y = newY;
        rect.x = x;
        rect.y = y;
        lastMoveTime = currentTime;

        Direction newDirection = (dx == 0 && dy < 0) ? UP :
                                (dx == 0 && dy > 0) ? DOWN :
                                (dx < 0) ? LEFT : RIGHT;
        if (newDirection != currentDirection) {
            currentDirection = newDirection;
            frame = 0;
        } else {
            frame = (frame + 1) % 3;
        }
        dirX = dx;
        dirY = dy;
    }
}
void enemyTank::shoot(std::vector<Wall>& walls, PlayerTank& player) {
    if (!active) return;
    Uint32 currentTime = SDL_GetTicks();
    bool canShoot = false;
    if (player.x + TILE_SIZE > x - TILE_SIZE && x + TILE_SIZE > player.x - TILE_SIZE) {
        if (player.y > y && dirY == 1)
            canShoot = true;
        if (player.y < y && dirY == -1)
            canShoot = true;
    } else if (y - TILE_SIZE < player.y && y + TILE_SIZE > player.y - TILE_SIZE) {
        if (player.x > x && dirX == 1)
            canShoot = true;
        if (player.x < x && dirX == -1)
            canShoot = true;
    }
    for (auto wall : walls) {
        if (wall.x == x) {
            if (wall.y > y && dirY == 1)
                canShoot = true;
            if (wall.y < y && dirY == -1)
                canShoot = true;
        } else if (wall.y == y) {
            if (wall.x > x && dirX == 1)
                canShoot = true;
            if (wall.x < x && dirX == -1)
                canShoot = true;
        }
    }
    if (currentTime - lastShotTime >= 1000 && canShoot) {
        bullets.emplace_back(x + TILE_SIZE / 2 - 5, y + TILE_SIZE / 2 - 5, dirX, dirY, 1, false);
        lastShotTime = currentTime;
    }
}

void enemyTank::updatebullets(std::vector<Wall>& walls, std::vector<enemyTank>& Enemies, PlayerTank& player) {
    bool Gameisover = false;
    for (int i = 0; i < bullets.size();) {
        bullets[i].move(walls, Enemies, player, Gameisover);
        if (!bullets[i].active)
            bullets.erase(bullets.begin() + i);
        else i++;
    }
}

void enemyTank::render(SDL_Renderer* renderer) {
    if (active) {
        if (currentDirection == UP) {
            currentFrame = upFrames[frame];
        } else if (currentDirection == DOWN) {
            currentFrame = downFrames[frame];
        } else if (currentDirection == LEFT) {
            currentFrame = leftFrames[frame];
        } else if (currentDirection == RIGHT) {
            currentFrame = rightFrames[frame];
        }
    } else {
        currentFrame = deathFrame;
    }

    if (currentFrame) {
        SDL_RenderCopy(renderer, currentFrame, nullptr, &rect);
    }
    for (int i = 0; i < bullets.size(); i++) {
        bullets[i].render(renderer);
    }
}
enemyTank::~enemyTank() {
}
