#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <thread>
#include "SDL.h"
#include "snake.h"
#include "food.h"

class Renderer {
 public:
  Renderer(const std::size_t screen_width, const std::size_t screen_height,
                   const std::size_t grid_width, const std::size_t grid_height,
                   std::vector<std::shared_ptr<int>> scores);
  ~Renderer();
  
  void operator()(Snake& snake, Snake& enemy, FoodManager& food_manager) ;
  void Render(Snake& snake, Snake& enemy, FoodManager& food_manager);
  void UpdateWindowTitle(int score_snake, int score_enemy);

 private:
  SDL_Window *sdl_window;
  SDL_Renderer *sdl_renderer;
  
  std::vector<Food> foods;

  const std::size_t screen_width;
  const std::size_t screen_height;
  const std::size_t grid_width;
  const std::size_t grid_height;
  const std::size_t target_frame_duration{1000/60};
  
  std::vector<std::shared_ptr<int>> scores;
};

#endif