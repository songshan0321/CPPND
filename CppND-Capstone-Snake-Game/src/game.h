#ifndef GAME_H
#define GAME_H

#include <random>
#include <thread>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "food.h"

class Game {
 public:
  Game(std::size_t screen_width, std::size_t screen_height, std::size_t grid_width, std::size_t grid_height);
  ~Game();
  void Run(Controller const &controller, std::size_t target_frame_duration);
           
  std::vector<int> GetScore();
  bool running{true};

 private:
  std::thread sdl_thread;
  Snake snake;
  Snake enemy;
  FoodManager food_manager;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;
  std::uniform_int_distribution<int> random_n;
  
  const std::size_t screen_width_;
  const std::size_t screen_height_;
  const std::size_t grid_width_;
  const std::size_t grid_height_;

  std::vector<std::shared_ptr<int>> scores;
  std::mutex score_mtx;

  void PlaceFood(const int& index);
  void Update();
};

#endif