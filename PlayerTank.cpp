#include "PlayerTank.h"
#include "enemyTank.h"

const char* rightFiles[3] = {"Pright.png", "Prightmove1.png", "Prightmove2.png"};
const char* leftFiles[3] = {"Pleft.png", "Pleftmove1.png", "Pleftmove2.png"};
const char* upFiles[3] = {"Pup.png", "Pupmove1.png", "Pupmove2.png"};
const char* downFiles[3] = {"Pdown.png", "Pdownmove1.png", "Pdownmove2.png"};
const char* deathFile = "Pdeath.png";

PlayerTank::PlayerTank(int startX, int startY, SDL_Renderer* renderer) {
    x = startX;
    y = startY;
    rect = {x, y, TILE_SIZE, TILE_SIZE};
    dirX = 0;
    dirY = -1;
    lastShotTime = 0;
    frame = 0;
    isDead = false;
    currentDirection = UP;
    for (int i = 0; i < 3; i++) {
        rightFrames.push_back(IMG_LoadTexture(renderer, rightFiles[i]));
        leftFrames.push_back(IMG_LoadTexture(renderer, leftFiles[i]));
        upFrames.push_back(IMG_LoadTexture(renderer, upFiles[i]));
        downFrames.push_back(IMG_LoadTexture(renderer, downFiles[i]));
    }
    deathFrame = IMG_LoadTexture(renderer, deathFile);
}

void PlayerTank::move(int dx, int dy, const vector<Wall>& walls, const vector<enemyTank>& enemies) {
    int newX = x + dx;
    int newY = y + dy;
    SDL_Rect newRect = {newX, newY, TILE_SIZE, TILE_SIZE};

    for (const auto& enemy : enemies) {
        if (!enemy.active) continue;
        SDL_Rect enemyRect = {enemy.x, enemy.y, TILE_SIZE, TILE_SIZE};
        if (SDL_HasIntersection(&newRect, &enemyRect))
            return;
    }

    for (const auto& wall : walls) {
        if (wall.active && SDL_HasIntersection(&newRect, &wall.rect))
            return;
    }

    x = newX;
    y = newY;
    rect.x = x;
    rect.y = y;
    if (x < TILE_SIZE) x = TILE_SIZE;
    if (x > SCREEN_WIDTH - TILE_SIZE * 2) x = SCREEN_WIDTH - TILE_SIZE * 2;
    if (y < TILE_SIZE) y = TILE_SIZE;
    if (y > SCREEN_HEIGHT - TILE_SIZE * 2) y = SCREEN_HEIGHT - TILE_SIZE * 2;
    rect.x = x;
    rect.y = y;

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
void PlayerTank::shoot() {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastShotTime >= 1000) {
        bullets.emplace_back(x + TILE_SIZE / 2 - 5, y + TILE_SIZE / 2 - 5, dirX, dirY, 1, true);
        lastShotTime = currentTime;
    }
}

void PlayerTank::updatebullets(vector<Wall>& walls, vector<enemyTank>& Enemies, PlayerTank& player) {
    bool Gameisover = false;
    for (int i = 0; i < bullets.size();) {
        bullets[i].move(walls, Enemies, player, Gameisover);
        if (!bullets[i].active)
            bullets.erase(bullets.begin() + i);
        else i++;
    }
}

void PlayerTank::render(SDL_Renderer* renderer) {
    if (isDead) {
        currentFrame = deathFrame;
    } else if (currentDirection == UP) {
        currentFrame = upFrames[frame];
    } else if (currentDirection == DOWN) {
        currentFrame = downFrames[frame];
    } else if (currentDirection == LEFT) {
        currentFrame = leftFrames[frame];
    } else if (currentDirection == RIGHT) {
        currentFrame = rightFrames[frame];
    }

    if (currentFrame) {
        SDL_RenderCopy(renderer, currentFrame, nullptr, &rect);
    }
    for (int i = 0; i < bullets.size(); i++) {
        bullets[i].render(renderer);
    }
}

PlayerTank::~PlayerTank() {
    for(int i = 0; i < upFrames.size(); i++) {
        SDL_DestroyTexture(upFrames[i]);
    }
    for(int i = 0; i < downFrames.size(); i++) {
        SDL_DestroyTexture(downFrames[i]);
    }
    for(int i = 0; i < leftFrames.size(); i++) {
        SDL_DestroyTexture(leftFrames[i]);
    }
    for(int i = 0; i < rightFrames.size(); i++) {
        SDL_DestroyTexture(rightFrames[i]);
    }
    SDL_DestroyTexture(deathFrame);
}
