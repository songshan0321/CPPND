#include "game.h"
#include <iostream>
#include "SDL.h"

Game::Game(std::size_t screen_width, std::size_t screen_height, std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height, grid_width/2 - 5, grid_height/2, 0.15f),
      enemy(grid_width, grid_height, grid_width/2 + 5, grid_height/2 + 5, 0.05f),
      engine(dev()),
      screen_width_(screen_width), 
      screen_height_(screen_height), 
      grid_width_(grid_width), 
      grid_height_(grid_height),
      random_w(0, static_cast<int>(grid_width-1)),
      random_h(0, static_cast<int>(grid_height-1)),
      random_n(0, 99){
        
  scores.push_back(std::make_shared<int>(0));
  scores.push_back(std::make_shared<int>(1));
        
  for (size_t i = 0; i < food_manager.target_food_number; i++) {
    PlaceFood(i);
  }
}

Game::~Game() {
  sdl_thread.detach();
}

void Game::Run(Controller const &controller,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;
  
  // Run render thread
  sdl_thread = std::thread(Renderer(screen_width_, screen_height_, grid_width_, grid_height_, scores), 
                              std::ref(snake), std::ref(enemy), std::ref(food_manager));
  
  // Main thread running
  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    
    // Enemy AI
    enemy.Navigate(food_manager.GetFoods());
    
    Update();

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_duration = frame_end - frame_start;

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood(const int& index) {
  int x, y, n;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    Food::Type type;
    
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x, y) && !enemy.SnakeCell(x, y)) {
      n = random_n(engine);
      if (n >=0 && n <15) {  type = Food::Type::kBoost; }
      else if (n >= 15 && n < 30) { type = Food::Type::kCut; }
      else { type = Food::Type::kNormal; }
      
      food_manager.UpdateFood(index, x, y, type);
      break;
    }
  }
}

void Game::Update() {
  if (!snake.alive || !enemy.alive) {
    return;
  }

  snake.Update();
  enemy.Update();
  
  food_manager.Count();

  int new_x_snake = static_cast<int>(snake.head_x);
  int new_y_snake = static_cast<int>(snake.head_y);
  
  int new_x_enemy = static_cast<int>(enemy.head_x);
  int new_y_enemy = static_cast<int>(enemy.head_y);

  int index;
  // Check if snake eating a food
  index = food_manager.CheckFood(new_x_snake, new_y_snake);
  if (index != -1) {
    std::lock_guard<std::mutex> lock(score_mtx);
    auto type = food_manager.GetType(index);
    if (type == Food::Type::kNormal) {
      snake.GrowBody();
      if (snake.speed < snake.MAX_SPEED) snake.speed += 0.005;
      *(scores[0]) += 1;
    }
    else if (type == Food::Type::kBoost) {
      snake.GrowBody();
      snake.speed += 0.05;
      *(scores[0]) += 3;
    }
    else if (type == Food::Type::kCut) {
      snake.GrowBody();
      snake.CutBody();
      *(scores[0]) += 3;
    }
    PlaceFood(index);
  }
  
  // Check if enemy eating a food
  index = food_manager.CheckFood(new_x_enemy, new_y_enemy);
  if (index != -1) {
    std::lock_guard<std::mutex> lock(score_mtx);
    
    auto type = food_manager.GetType(index);
    if (type == Food::Type::kNormal) {
      enemy.GrowBody();
      if (enemy.speed < enemy.MAX_SPEED) enemy.speed += 0.005;
      *(scores[1]) += 1;
    }
    else if (type == Food::Type::kBoost) {
      enemy.GrowBody();
      enemy.speed += 0.05;
      *(scores[1]) += 3;
    }
    else if (type == Food::Type::kCut) {
      enemy.GrowBody();
      enemy.CutBody();
      *(scores[1]) += 3;
    }
    PlaceFood(index);
  }
}

std::vector<int> Game::GetScore() { 
  std::lock_guard<std::mutex> lock(score_mtx);
  std::vector<int> s;
  s.push_back(*scores[0]);
  s.push_back(*scores[1]);
  return s; 
}