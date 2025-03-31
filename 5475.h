#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <vector>
#include <SDL.h>
#include <SDL_mixer.h>
#include <ctime>
#include <SDL_image.h>
#include <cstdlib>
#include "enemyTank.h"
#include "Bullet.h"
#include "Wall.h"
#include "Constant.h"
#include "PlayerTank.h"

using namespace std;

class Game {
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* wallTexture;
    SDL_Texture* mapTexture;
    SDL_Texture* gameOverTexture;
    SDL_Texture* winTexture;
    Mix_Music* backgroundMusic;
    bool running;
    bool isGameOver;
    bool isGameWon;
    int enemiesKilled;
    vector<bullet> Bullets;
    vector<enemyTank> Enemies;
    vector<Wall> walls;
    PlayerTank* player;
    vector<SDL_Texture*> enemyUpFrames;
    vector<SDL_Texture*> enemyDownFrames;
    vector<SDL_Texture*> enemyLeftFrames;
    vector<SDL_Texture*> enemyRightFrames;
    SDL_Texture* enemyDeathFrame;

    Game();
    ~Game();

    void generateEnemies();
    void respawnEnemies();
    void generateWalls();
    void handleEvents();
    void render();
    void updateEnemies();
    void run();
};

#endif
