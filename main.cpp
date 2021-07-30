#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <utility>
#include <queue>
#include <stdlib.h>
#include <time.h>
#include <list>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

const int TILE_HEIGHT = 16;
const int TILE_WIDTH = 16;

const int SCREEN_TILE_HEIGHT = SCREEN_HEIGHT / TILE_HEIGHT;
const int SCREEN_TILE_WIDTH = SCREEN_WIDTH / TILE_WIDTH;

const uint32_t FRAME_DELAY = 1000 / 8;

SDL_Window* g_window = NULL;
SDL_Renderer* g_renderer = NULL;

bool init() {
  bool success = true;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
    success = false;
  } else {
    g_window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (g_window == NULL) {
      cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
      success = false;
    } else {
      g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
      if(g_renderer == NULL) {
        cout << SDL_GetError() << endl;
        success = false;
      }
    }
  }
  return success;
}

void close() {
  SDL_DestroyWindow(g_window);
  g_window = NULL;

  SDL_Quit();
}

class Snake {
  public:
    vector<pair<int, int>> tail;
    pair<int, int> direction;
    queue<pair<int, int>> belly;
    
    Snake() {
      tail.push_back(make_pair(SCREEN_TILE_WIDTH / 4, SCREEN_TILE_HEIGHT / 2));
      direction = make_pair(0, 0);
    }

    void set_direction(pair<int, int> dir) {
      // prevent turning in the opposite direction
      // TODO: prevent turning into second tail element
      if (direction.first != 0  && direction.first == dir.first * -1)
        return;
      if (direction.second != 0 && direction.second == dir.second * -1)
        return;
      direction = dir;
    }

    void update(bool eat) {
      bool extend_tail = false;
      if (belly.size() > 0) {
        bool tail_on_fruit = false;
        for (auto x : tail) {
          if (x == belly.front())
            tail_on_fruit = true;
        }
        extend_tail = !tail_on_fruit;
      }
      // update tail positions
      for (int i = tail.size() - 1; i > 0; i--)
        tail[i] = tail[i-1];
      if (extend_tail) {
        tail.push_back(belly.front());
        belly.pop();
      }
      if (eat) 
        belly.push(tail[0]);

      tail[0].first += direction.first;
      tail[0].second += direction.second;
    }
    
};

void draw_tile(pair<int, int> pos) {
    SDL_Rect fill_rect = { pos.first * TILE_WIDTH, pos.second * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT };
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(g_renderer, &fill_rect);

}

void game_loop() {

  Snake snake;
  pair<int, int> fruit = make_pair(SCREEN_TILE_WIDTH * 0.75, SCREEN_TILE_HEIGHT / 2);
  list<pair<int, int>> spawns;
  for (int x = 0; x < SCREEN_TILE_WIDTH; x++)
    for (int y = 0; y < SCREEN_TILE_HEIGHT; y++)
      spawns.push_back(make_pair(x, y));
  spawns.remove(fruit);


  uint32_t start_time = SDL_GetTicks();
  uint32_t stop_time = SDL_GetTicks();

  srand(time(0));
  

  SDL_Event e;
  bool quit = false;
  while (!quit) {
    // handle events
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT)
        quit = true;
      if (e.type == SDL_KEYDOWN) {
        switch(e.key.keysym.sym) {
          
          case SDLK_UP:
            snake.set_direction(make_pair(0, -1));
            break;

          case SDLK_DOWN:
            snake.set_direction(make_pair(0, 1));
            break;

          case SDLK_LEFT:
            snake.set_direction(make_pair(-1, 0));
            break;

          case SDLK_RIGHT:
            snake.set_direction(make_pair(1, 0));
            break;
        }
      }
    }


    stop_time = SDL_GetTicks();
    if (stop_time - start_time > FRAME_DELAY) {
      if (snake.tail[0] == fruit) {
        snake.update(true);
        fruit = *next(spawns.begin(), (rand() % spawns.size()) - 1);
        spawns.remove(fruit);
        
      }
      else {
        snake.update(false);
      }
      // draw frame
      
      // clear screen
      SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 0);
      SDL_RenderClear(g_renderer);

      for (auto x : snake.tail) {
        draw_tile(x);
      }
      draw_tile(fruit);

      SDL_RenderPresent(g_renderer);
      start_time = SDL_GetTicks();
    }
    
  }
}

int main(int argc, char* args[]) {
  if (!init())
    cout << "Failed to initialize" << endl;
  else 
    game_loop();

  close();
  return 0;
}
