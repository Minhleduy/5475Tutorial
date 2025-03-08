#include <iostream>
#include <vector>
#include <SDL.h>
#include <ctime>
#include <cstdlib>
using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TILE_SIZE = 20;
const int MAP_WIDTH = SCREEN_WIDTH / TILE_SIZE;
const int MAP_HEIGHT = SCREEN_HEIGHT / TILE_SIZE;
class Wall {
public:
    int x, y;
    SDL_Rect rect;
    bool active;
    Wall(int startX, int startY) {
        x = startX;
        y = startY;
        active = true;
        rect = {x, y, TILE_SIZE, TILE_SIZE};
    };
    void render(SDL_Renderer* renderer) {
    if(active)  {
        SDL_SetRenderDrawColor(renderer, 150, 75, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
    }
};
class bullet {
public:
    int x,y;
    int speed = 1;
    int dirX, dirY;
    SDL_Rect rect;
    bool active;

    bullet(int startX, int startY, int dx, int dy) {
        x = startX;
        y = startY;
        dirX = dx;
        dirY = dy;
        active = true;
        rect = {x, y, 5, 5};
    }

    void move(vector<Wall>& walls) {
        if(!active) return;

        x += dirX * speed;
        y += dirY * speed;
        rect.x = x;
        rect.y = y;
    // kiểm tra va chạm tường
    for( int i = 0; i < walls.size();i++) {
        if (walls[i].active && SDL_HasIntersection(&rect, &walls[i].rect)) {
            walls[i].active = false;
            active = false;
            return;
        }
    }
    // đạn ra màn hình thì biến mất
    if (x <= TILE_SIZE - 10 || x >= SCREEN_WIDTH - TILE_SIZE || y <= TILE_SIZE - 10 || y >= SCREEN_HEIGHT - TILE_SIZE) {
            active = false;
        }
}
    void render(SDL_Renderer* renderer) {
        if (active) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
};
class PlayerTank {
public:
    int x, y;
    int dirX, dirY;
    vector<bullet> bullets;
    SDL_Rect rect;
    Uint32 lastShotTime;

    PlayerTank (int startX, int startY) {
        x = startX;
        y = startY;
        rect = {x, y, TILE_SIZE, TILE_SIZE};
        dirX = 0;
        dirY = -1;
    }

    void move(int dx, int dy, const vector<Wall>& walls) {
        int newX = x + dx;
        int newY = y + dy;
        this->dirX = dx;
        this->dirY = dy;

        SDL_Rect newRect = {newX, newY, TILE_SIZE, TILE_SIZE};
        // kiểm tra va chạm tường
        for( int i = 0; i < walls.size();i++) {
            if (walls[i].active && SDL_HasIntersection(&newRect, &walls[i].rect)) {
                return;
            }
        }
        //kiểm tra nó có ra ngoài phạm vi không
        if(newX >= TILE_SIZE && newX <= SCREEN_WIDTH - TILE_SIZE * 2 && newY >= TILE_SIZE && newY <= SCREEN_HEIGHT - TILE_SIZE * 2) {
            x = newX;
            y = newY;
            rect.x = x;
            rect.y = y;
        }
    }

    void shoot() {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastShotTime >= 1000) {
            bullets.emplace_back(x + TILE_SIZE / 2 - 5, y + TILE_SIZE / 2 - 5, dirX, dirY);
            lastShotTime = currentTime;
        }
    }

    void updatebullets(vector<Wall> &walls) {
        for( int i = 0; i < bullets.size();) {
            bullets[i].move(walls);
            if(!bullets[i].active)
                bullets.erase(bullets.begin() + i);
            else i++;
        }
    }
    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
        for( int i = 0; i < bullets.size();i++) {
                bullets[i].render(renderer);
        }
    }
};
class enemyTank {
public:
    int x, y;
    int dirX, dirY;
    vector<bullet> bullets;
    SDL_Rect rect;
    Uint32 lastShotTime;
    enemyTank (int startX, int startY) {
        x = startX;
        y = startY;
        rect = {x, y, TILE_SIZE, TILE_SIZE};
        dirX = 0;
        dirY = 1;
    }

    void move(PlayerTank &player, const vector<Wall>& walls) {
        int targetX = player.x;
        int targetY = player.y;
        int dx = (targetX > x) ? 1 : (targetX < x) ? -1 : 0;
        int dy = (targetY > y) ? 1 : (targetY < y) ? -1 : 0;
        if (rand() % 2 == 0) swap(dx, dy);
        int newX = x + dx;
        int newY = y + dy;
        SDL_Rect newRect = {newX, newY, TILE_SIZE, TILE_SIZE};
        //kiểm tra va chạm tường
        for( int i = 0; i < walls.size();i++) {
            if (walls[i].active && SDL_HasIntersection(&newRect, &walls[i].rect)) {
                return;
            }
        }

        x = newX;
        y = newY;
        rect.x = x;
        rect.y = y;
        dirX = dx;
        dirY = dy;
    }
    void shoot() {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastShotTime >= 1000) { // Mỗi giây bắn 1 lần
            bullets.emplace_back(x + TILE_SIZE / 2 - 5, y + TILE_SIZE / 2 - 5, dirX, dirY);
            lastShotTime = currentTime;
        }
    }
    void updatebullets(vector<Wall> &walls) {
        for( int i = 0; i < bullets.size();) {
            bullets[i].move(walls);
            if(!bullets[i].active)
                bullets.erase(bullets.begin() + i);
            else i++;
        }
    }
    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
        for( int i = 0; i < bullets.size();i++) {
                bullets[i].render(renderer);
        }
    }
};
class Game{
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    vector <Wall> walls;
    PlayerTank player;
    //tạo tường cho game, muốn sinh ngẫu nhiên
    void generateWalls() {
//        for (int i = 3; i < MAP_HEIGHT - 3; i += 2) {
//            for (int j = 3; j < MAP_WIDTH - 3; j += 2) {
//                Wall w = Wall(j * TILE_SIZE, i * TILE_SIZE);
//                walls.push_back(w);
//            }
//        }
        srand(time(0));
        int numwalls= (MAP_WIDTH * MAP_HEIGHT) / 10;
        for(int i = 0; i < numwalls;)
        {
            int x = (rand() % (MAP_WIDTH - 2) + 1) * TILE_SIZE;
            int y = (rand() % (MAP_HEIGHT - 2) + 1) * TILE_SIZE;
            // điều kiện
            bool validPosition = true;
            if( abs(x - player.x) < TILE_SIZE * 2 && abs(y - player.y) < TILE_SIZE * 2)
                validPosition = false;
            for( int j = 0; j < walls.size(); j++) {
                if (walls[j].x == x && walls[j].y == y) {
                    validPosition = false;
                    break;
                }
            }
            if (validPosition) {
            walls.emplace_back(x, y);
            i++;
            }
        }
    }
    Game() : player(((MAP_WIDTH-1) / 2) * TILE_SIZE, (MAP_HEIGHT - 2) * TILE_SIZE) {
    running = true;
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        running = false;
    }
    window = SDL_CreateWindow ("Battle City", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(!window)
    {
        cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        running = false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if  (!renderer) {
        cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        running = false;
    }
    generateWalls();

}
    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP: player.move(0, -5, walls); break;
                    case SDLK_DOWN: player.move(0, 5, walls); break;
                    case SDLK_LEFT: player.move(-5, 0, walls); break;
                    case SDLK_RIGHT: player.move(5, 0, walls); break;
                    case SDLK_SPACE: player.shoot(); break;
                }
            }
        }
    }
    void render() {
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for(int i = 1; i < MAP_HEIGHT - 1; i++) {
        for(int j = 1; j < MAP_WIDTH -1; j++) {
            SDL_Rect tile = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            SDL_RenderFillRect(renderer, &tile);
        }
    }

    for(int i = 0; i < walls.size(); i++) {
        walls[i].render(renderer);
    }

    player.render(renderer);
    SDL_RenderPresent(renderer);
}
    void run() {
        while (running) {
            Uint32 startTime = SDL_GetTicks();
            handleEvents();
            player.updatebullets(walls);
            render();
            Uint32 frameTime = SDL_GetTicks() - startTime;
            if (frameTime < 16) {
            SDL_Delay(16 - frameTime);
            }
        }
}
    ~Game() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
}
};
int main(int argc, char* argv[])
{
    Game game;
    if (game.running) {
        game.run();
    }
    return 0;
}
