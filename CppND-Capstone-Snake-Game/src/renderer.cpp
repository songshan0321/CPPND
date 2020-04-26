#include "renderer.h"
#include <iostream>
#include <string>

Renderer::Renderer(const std::size_t screen_width, const std::size_t screen_height,
                   const std::size_t grid_width, const std::size_t grid_height,
                   std::vector<std::shared_ptr<int>> scores)
    : screen_width(screen_width),
      screen_height(screen_height),
      grid_width(grid_width),
      grid_height(grid_height),
      scores(scores) {
        
}

// Overload () operator 
void Renderer::operator()(Snake& snake, Snake& enemy, FoodManager& food_manager) 
{ 
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create Window
  sdl_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, screen_width,
                                screen_height, SDL_WINDOW_SHOWN);

  if (nullptr == sdl_window) {
    std::cerr << "Window could not be created.\n";
    std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create renderer
  sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_SOFTWARE);
  if (nullptr == sdl_renderer) {
    std::cerr << "Renderer could not be created.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }
  
  std::cout<<"Start Running Renderer thread"<<std::endl;
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  while(true) {
    frame_start = SDL_GetTicks();
    
    Renderer::Render(snake, enemy, food_manager);
    
    frame_end = SDL_GetTicks();
    frame_duration = frame_end - frame_start;

    // After every half a second, update the window title.
    if (frame_end - title_timestamp >= 500) {
      UpdateWindowTitle(*scores[0], *scores[1]);
      title_timestamp = SDL_GetTicks();
    }

    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
} 


Renderer::~Renderer() {
  SDL_DestroyWindow(sdl_window);
  SDL_Quit();
}

void Renderer::Render(Snake& snake, Snake& enemy, FoodManager& food_manager) {
  SDL_Rect block;
  block.w = screen_width / grid_width;
  block.h = screen_height / grid_height;

  // Clear screen
  SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(sdl_renderer);

  // Render food
  foods = food_manager.GetFoods();
  for (auto& food : foods) {
    switch (food.type) {
      case Food::Type::kNormal:
        SDL_SetRenderDrawColor(sdl_renderer, 0xCC, 0xCC, 0xCC, 0xFF);
        break;
      case Food::Type::kBoost:
        SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0xFF, 0x00, 0xFF);
        break;
      case Food::Type::kCut:
        SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xCC, 0x00, 0xFF);
        break;
    }
    block.x = food.pos.x * block.w;
    block.y = food.pos.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }
  
  // Render snake's body
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  for (SDL_Point const &point : snake.body) {
    block.x = point.x * block.w;
    block.y = point.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Render snake's head
  block.x = static_cast<int>(snake.head_x) * block.w;
  block.y = static_cast<int>(snake.head_y) * block.h;
  if (snake.alive) {
    SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0x7A, 0xCC, 0xFF);
  } else {
    SDL_SetRenderDrawColor(sdl_renderer, 0xCC, 0xCC, 0xCC, 0xFF);
  }
  SDL_RenderFillRect(sdl_renderer, &block);

  // Update Screen
  SDL_RenderPresent(sdl_renderer);
  
  // Render enemy's body
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  for (SDL_Point const &point : enemy.body) {
    block.x = point.x * block.w;
    block.y = point.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Render enemy's head
  block.x = static_cast<int>(enemy.head_x) * block.w;
  block.y = static_cast<int>(enemy.head_y) * block.h;
  if (enemy.alive) {
    SDL_SetRenderDrawColor(sdl_renderer, 0xCC, 0x00, 0x00, 0xFF);
  } else {
    SDL_SetRenderDrawColor(sdl_renderer, 0xCC, 0xCC, 0xCC, 0xFF);
  }
  SDL_RenderFillRect(sdl_renderer, &block);

  // Update Screen
  SDL_RenderPresent(sdl_renderer);
}

void Renderer::UpdateWindowTitle(int score_snake, int score_enemy) {
  std::string title{"Your Score: " + std::to_string(score_snake) + " | Enemy Score: " + std::to_string(score_enemy)};
  SDL_SetWindowTitle(sdl_window, title.c_str());
}
