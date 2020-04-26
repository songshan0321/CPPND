#include "snake.h"
#include <cmath>
#include <iostream>

Snake::Snake(int grid_width, int grid_height, int init_x, int init_y, float speed)
      : grid_width(grid_width),
        grid_height(grid_height),
        head_x(init_x),
        head_y(init_y),
        speed(speed) {
  SDL_Point p = {static_cast<int>(head_x), static_cast<int>(head_y+1)};
  Snake::PushBack(p);
  p.y++;
  Snake::PushBack(p);       
}

void Snake::Update() {
  SDL_Point prev_cell{
      static_cast<int>(head_x),
      static_cast<int>(
          head_y)};  // We first capture the head's cell before updating.
  UpdateHead();
  SDL_Point current_cell{
      static_cast<int>(head_x),
      static_cast<int>(head_y)};  // Capture the head's cell after updating.

  // Update all of the body vector items if the snake head has moved to a new
  // cell.
  if (current_cell.x != prev_cell.x || current_cell.y != prev_cell.y) {
    UpdateBody(current_cell, prev_cell);
  }
}

void Snake::PushBack(SDL_Point& cell) {
  body.push_back(cell);
  size++;
}

void Snake::UpdateHead() {
  switch (direction) {
    case Direction::kUp:
      head_y -= speed;
      break;

    case Direction::kDown:
      head_y += speed;
      break;

    case Direction::kLeft:
      head_x -= speed;
      break;

    case Direction::kRight:
      head_x += speed;
      break;
  }

  // Wrap the Snake around to the beginning if going off of the screen.
  head_x = fmod(head_x + grid_width, grid_width);
  head_y = fmod(head_y + grid_height, grid_height);
}

void Snake::UpdateBody(SDL_Point &current_head_cell, SDL_Point &prev_head_cell) {
  // Add previous head location to vector
  body.push_back(prev_head_cell);

  if (!growing) {
    // Remove the tail from the vector.
    body.erase(body.begin());
  } else {
    growing = false;
    size++;
  }
  
  if (cutting) {
    int cut = std::min(3,size);
    body.erase(body.begin(), body.begin() + cut);
    size -= cut;
    cutting = false;
  }

  // Check if the snake has died.
  for (auto const &item : body) {
    if (current_head_cell.x == item.x && current_head_cell.y == item.y) {
      alive = false;
    }
  }
}

void Snake::GrowBody() { growing = true; }

void Snake::CutBody() { cutting = true; }

// Inefficient method to check if cell is occupied by snake.
bool Snake::SnakeCell(int x, int y) {
  if (x == static_cast<int>(head_x) && y == static_cast<int>(head_y)) {
    return true;
  }
  for (auto const &item : body) {
    if (x == item.x && y == item.y) {
      return true;
    }
  }
  return false;
}

void Snake::Navigate(std::vector<Food> foods) {
  // Find nearest food
  bool change = false;
  int x = static_cast<int>(head_x);
  int y = static_cast<int>(head_y);
  int index = 0;
  int min_distance = grid_width + grid_height;
  for (size_t i = 0; i < foods.size(); i++) {
    auto& food = foods[i];
    int distance = abs(food.pos.x - x) + abs(food.pos.y - y);
    if (distance < min_distance) { 
      min_distance = distance; 
      index = i;
    }
  }
  
  auto& food = foods[index];
  Direction dir = direction;
  // if food is behind (two steps needed)
  if ((direction == Direction::kUp && food.pos.y > y) ||
      (direction == Direction::kDown && food.pos.y < y)) {
    dir = food.pos.x < x ? Direction::kLeft : Direction::kRight;
    change = true;
  }
  else if ((direction == Direction::kLeft && food.pos.x > x) ||
           (direction == Direction::kRight && food.pos.x < x)) {
    dir = food.pos.y < y ? Direction::kUp : Direction::kDown;
    change = true;
  }
  else {
    if (food.pos.x == x && (direction == Direction::kLeft || direction == Direction::kRight)) {
      dir = food.pos.y > y ? Direction::kDown : Direction::kUp;
      change = true;
    }
    else if (food.pos.y == y && (direction == Direction::kDown || direction == Direction::kUp)) {
      dir = food.pos.x > x ? Direction::kRight : Direction::kLeft;
      change = true;
    }
  }
  
  // Avoid collision
  int next_x, next_y;
  for (size_t i = 0; i < 4; i++) {
    next_x = x;
    next_y = y;
    NextPos(dir, next_x, next_y);
    
    if (!SnakeCell(next_x, next_y)) { break; } 
    else { 
      NextDirection(dir); 
    }
  }
  
  direction = dir;
}

void Snake::NextDirection(Direction& dir) {
  switch(dir) {
    case Direction::kUp:
      dir = Direction::kRight;
    case Direction::kRight:
      dir = Direction::kDown;
    case Direction::kDown:
      dir = Direction::kLeft;
    case Direction::kLeft:
      dir = Direction::kRight;
  }
}

void Snake::NextPos(const Direction dir, int& next_x, int& next_y) {
  switch(dir) {
    case Direction::kUp:
      next_y--;
      break;
    case Direction::kDown:
      next_y++;
      break;
    case Direction::kLeft:
      next_x--;
      break;
    case Direction::kRight:
      next_x++;
      break;
  }
}




