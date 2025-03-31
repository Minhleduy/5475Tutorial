#include "Bullet.h"
#include "PlayerTank.h"
#include "enemyTank.h"
#include "5475.h"

extern Game* game;

bullet::bullet(int startX, int startY, int dx, int dy, int sped, bool typ) {
    x = startX;
    y = startY;
    dirX = dx;
    dirY = dy;
    speed = sped;
    active = true;
    type = typ;
    rect = {x, y, 5, 5};
}

void bullet::move(vector<Wall>& walls, vector<enemyTank>& Enemies, PlayerTank& player, bool& isGameOver) {
    if (!active) return;

    int oldX = x;
    int oldY = y;
    x += dirX * speed;
    y += dirY * speed;
    rect.x = x;
    rect.y = y;

    for (int i = 0; i < walls.size(); i++) {
        if (walls[i].active && SDL_HasIntersection(&rect, &walls[i].rect)) {
            walls[i].active = false;
            active = false;
            return;
        }
    }
    if (type == false) {
        SDL_Rect playerRect = {player.x, player.y, TILE_SIZE, TILE_SIZE};
        if (SDL_HasIntersection(&rect, &playerRect)) {
             player.isDead = true;
            game->isGameOver = true;
            active = false;
            return;
        }
    }
    if (type == true) {
        for (auto& enemy : Enemies) {
            if (!enemy.active) continue;
            SDL_Rect enemyRect = {enemy.x, enemy.y, TILE_SIZE, TILE_SIZE};
            if (SDL_HasIntersection(&rect, &enemyRect)) {
                enemy.active = false;
                active = false;
                return;
            }
        }
    }

    if (x <= TILE_SIZE - 10 || x >= SCREEN_WIDTH - TILE_SIZE || y <= TILE_SIZE - 10 || y >= SCREEN_HEIGHT - TILE_SIZE) {
        active = false;
    }
}

void bullet::render(SDL_Renderer* renderer) {
    if (active) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}

bool bullet::checkCollision(SDL_Rect& target) {
    return SDL_HasIntersection(&rect, &target);
}
