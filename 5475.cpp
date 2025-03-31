#include "5475.h"

Game* game = nullptr;

void Game::generateEnemies() {
    Enemies.emplace_back(TILE_SIZE * 2, TILE_SIZE * 2, renderer, enemyUpFrames, enemyDownFrames, enemyLeftFrames, enemyRightFrames, enemyDeathFrame);
    Enemies.emplace_back(SCREEN_WIDTH - TILE_SIZE * 3, TILE_SIZE * 2, renderer, enemyUpFrames, enemyDownFrames, enemyLeftFrames, enemyRightFrames, enemyDeathFrame);
    Enemies.emplace_back(SCREEN_WIDTH / 2, TILE_SIZE * 2, renderer, enemyUpFrames, enemyDownFrames, enemyLeftFrames, enemyRightFrames, enemyDeathFrame);
}

void Game::respawnEnemies() {
    Uint32 currentTime = SDL_GetTicks();
    for (int i = 0; i < Enemies.size(); i++) {
        if (!Enemies[i].active && Enemies[i].deathTime != 0) {
            Uint32 timeSinceDeath = currentTime - Enemies[i].deathTime;
            if (timeSinceDeath >= 5000) {
                Enemies[i].x = Enemies[i].startX;
                Enemies[i].y = Enemies[i].startY;
                Enemies[i].rect.x = Enemies[i].x;
                Enemies[i].rect.y = Enemies[i].y;
                Enemies[i].active = true;
                Enemies[i].deathTime = 0;
                Enemies[i].lastMoveTime = SDL_GetTicks();
                Enemies[i].lastShotTime = SDL_GetTicks();
                Enemies[i].bullets.clear();
                Enemies[i].frame = 0;
                Enemies[i].currentDirection = DOWN;
            }
        }
    }
}

void Game::generateWalls() {
    srand(time(0));
    int numwalls = (MAP_WIDTH * MAP_HEIGHT) / 10;
    for (int i = 0; i < numwalls;) {
        int x = (rand() % (MAP_WIDTH - 2) + 1) * TILE_SIZE;
        int y = (rand() % (MAP_HEIGHT - 2) + 1) * TILE_SIZE;
        bool validPosition = true;
        if (abs(x - player->x) < TILE_SIZE * 2 && abs(y - player->y) < TILE_SIZE * 2)
            validPosition = false;
        for (auto& enemy : Enemies)
            if (abs(x - enemy.x) < TILE_SIZE * 2 && abs(y - enemy.y) < TILE_SIZE * 2)
                validPosition = false;
        for (int j = 0; j < walls.size(); j++) {
            if (walls[j].x == x && walls[j].y == y) {
                validPosition = false;
                break;
            }
        }
        if (validPosition) {
            walls.emplace_back(x, y, wallTexture);
            i++;
        }
    }
}

Game::Game() {
    running = true;
    isGameOver = false;
    isGameWon = false;
    enemiesKilled = 0;
    player = nullptr;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        running = false;
        return;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cerr << "SDL_mixer could not initialize! Mix_Error: " << Mix_GetError() << endl;
        running = false;
        return;
    }
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << endl;
        running = false;
        return;
    }
    window = SDL_CreateWindow("Battle City", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        running = false;
        return;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        running = false;
        return;
    }
    wallTexture = IMG_LoadTexture(renderer, "Wall.png");
    if (!wallTexture) {
        cerr << "Failed to load wall texture! IMG_Error: " << IMG_GetError() << endl;
        running = false;
        return;
    }
    mapTexture = IMG_LoadTexture(renderer, "Map.png");
    if (!mapTexture) {
        cerr << "Failed to load ground texture! IMG_Error: " << IMG_GetError() << endl;
        running = false;
        return;
    }
    gameOverTexture = IMG_LoadTexture(renderer, "GameOver.png");
    if (!gameOverTexture) {
        cerr << "Failed to load game over texture! IMG_Error: " << IMG_GetError() << endl;
        running = false;
        return;
    }
    winTexture = IMG_LoadTexture(renderer, "Victory.png");
    if (!winTexture) {
        cerr << "Failed to load win texture! IMG_Error: " << IMG_GetError() << endl;
        running = false;
        return;
    }
    backgroundMusic = Mix_LoadMUS("Background.mp3");
    if (!backgroundMusic) {
        cerr << "Failed to load background music! Mix_Error: " << Mix_GetError() << endl;
        running = false;
        return;
    }
    if (Mix_PlayMusic(backgroundMusic, -1) == -1) {
        cerr << "Failed to play background music! Mix_Error: " << Mix_GetError() << endl;
        running = false;
        return;
    }
    const char* EupFiles[3] = {"Eup.png", "Eupmove1.png", "Eupmove2.png"};
    const char* EdownFiles[3] = {"Edown.png", "Edownmove1.png", "Edownmove2.png"};
    const char* EleftFiles[3] = {"Eleft.png", "Eleftmove1.png", "Eleftmove2.png"};
    const char* ErightFiles[3] = {"Eright.png", "Erightmove1.png", "Erightmove2.png"};
    for (int i = 0; i < 3; i++) {
        enemyUpFrames.push_back(IMG_LoadTexture(renderer, EupFiles[i]));
        enemyDownFrames.push_back(IMG_LoadTexture(renderer, EdownFiles[i]));
        enemyLeftFrames.push_back(IMG_LoadTexture(renderer, EleftFiles[i]));
        enemyRightFrames.push_back(IMG_LoadTexture(renderer, ErightFiles[i]));
        if (!enemyUpFrames[i] || !enemyDownFrames[i] || !enemyLeftFrames[i] || !enemyRightFrames[i]) {
            cerr << "Không tải được texture enemy " << EupFiles[i] << "! Lỗi: " << IMG_GetError() << endl;
            running = false;
            return;
        }
    }
    enemyDeathFrame = IMG_LoadTexture(renderer, "Edeath.png");
    if (!enemyDeathFrame) {
        cerr << "Không tải được texture death! Lỗi: " << IMG_GetError() << endl;
        running = false;
        return;
    }
    player = new PlayerTank(((MAP_WIDTH - 1) / 2) * TILE_SIZE, (MAP_HEIGHT - 2) * TILE_SIZE, renderer);
    generateEnemies();
    generateWalls();
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN && !isGameOver && !isGameWon) {
            switch (event.key.keysym.sym) {
                case SDLK_UP: player->move(0, -5, walls, Enemies); break;
                case SDLK_DOWN: player->move(0, 5, walls, Enemies); break;
                case SDLK_LEFT: player->move(-5, 0, walls, Enemies); break;
                case SDLK_RIGHT: player->move(5, 0, walls, Enemies); break;
                case SDLK_SPACE: player->shoot(); break;
            }
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderClear(renderer);

    if (isGameOver) {
        if (gameOverTexture) {
            SDL_Rect gameOverRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, gameOverTexture, nullptr, &gameOverRect);
        }
    } else if (isGameWon) {
        if (winTexture) {
            SDL_Rect winRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, winTexture, nullptr, &winRect);
        }
    } else {
        if (mapTexture) {
            SDL_Rect mapRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, mapTexture, nullptr, &mapRect);
        }
        for (int i = 0; i < walls.size(); i++) {
            walls[i].render(renderer);
        }
        for (int i = 0; i < Enemies.size(); i++) {
            Enemies[i].render(renderer);
        }
        player->render(renderer);
    }
    SDL_RenderPresent(renderer);
}

void Game::updateEnemies() {
    for (int i = 0; i < Enemies.size(); i++) {
        if (Enemies[i].active) {
            Enemies[i].moveToward(*player, walls);
            Enemies[i].shoot(walls, *player);
        }
        Enemies[i].updatebullets(walls, Enemies, *player);
        if (!Enemies[i].active && Enemies[i].deathTime == 0) {
            Enemies[i].deathTime = SDL_GetTicks();
            enemiesKilled++;
            cout << "Total killed: " << enemiesKilled << endl;
        }
    }
    if (enemiesKilled >= 6) {
        isGameWon = true;
    }
}

void Game::run() {
    while (running) {
        Uint32 startTime = SDL_GetTicks();
        handleEvents();
        if (!isGameOver && !isGameWon) {
            player->updatebullets(walls, Enemies, *player);
            updateEnemies();
            respawnEnemies();
        }
        render();
        Uint32 frameTime = SDL_GetTicks() - startTime;
        if (frameTime < 16) {
            SDL_Delay(16 - frameTime);
        }
    }
}

Game::~Game() {
    delete player;
    SDL_DestroyTexture(wallTexture);
    SDL_DestroyTexture(mapTexture);
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyTexture(winTexture);
    Mix_FreeMusic(backgroundMusic);
    Mix_CloseAudio();
    for (auto texture : enemyUpFrames) SDL_DestroyTexture(texture);
    for (auto texture : enemyDownFrames) SDL_DestroyTexture(texture);
    for (auto texture : enemyLeftFrames) SDL_DestroyTexture(texture);
    for (auto texture : enemyRightFrames) SDL_DestroyTexture(texture);
    SDL_DestroyTexture(enemyDeathFrame);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    game = new Game();
    if (game->running) {
        game->run();
    }
    delete game;
    return 0;
}
