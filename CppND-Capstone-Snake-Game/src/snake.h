#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include "food.h"
#include "SDL.h"

class Snake {
 public:
  enum class Direction { kUp, kDown, kLeft, kRight };
  
  Snake(int grid_width, int grid_height, int init_x, int init_y, float speed);
    
  void Update();
  
  void PushBack(SDL_Point& cell);

  void GrowBody();
  void CutBody();
  bool SnakeCell(int x, int y);
  
  void Navigate(std::vector<Food> foods);

  Direction direction = Direction::kUp;

  float speed{0.0};
  int size{1};
  bool alive{true};
  float head_x;
  float head_y;
  const float MAX_SPEED{0.3};
  std::vector<SDL_Point> body;

 private:
  void UpdateHead();
  void UpdateBody(SDL_Point &current_cell, SDL_Point &prev_cell);

  bool growing{false};
  bool cutting{false};
  
  int grid_width;
  int grid_height;
  
  void NextDirection(Direction& dir);
  void NextPos(const Direction dir, int& next_x, int& next_y);
};

#endif