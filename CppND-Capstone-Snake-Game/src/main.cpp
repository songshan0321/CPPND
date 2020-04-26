#include <iostream>
#include <thread>
#include "controller.h"
#include "game.h"
#include "renderer.h"

int main() {
  constexpr std::size_t kFramesPerSecond{60};
  constexpr std::size_t kMsPerFrame{1000 / kFramesPerSecond};
  constexpr std::size_t kScreenWidth{640};
  constexpr std::size_t kScreenHeight{640};
  constexpr std::size_t kGridWidth{32};
  constexpr std::size_t kGridHeight{32};

  Controller controller;
  Game game(kScreenWidth, kScreenHeight, kGridWidth, kGridHeight);
  game.Run(controller, kMsPerFrame);
  if (game.GetScore()[0] > game.GetScore()[1]) {
    std::cout << "Congratulation, you have won our god level AI snake!" << "\n";
  } 
  else if (game.GetScore()[0] == game.GetScore()[1]){
    std::cout << "You are as good as our AI snake!" << "\n";
  } else {
    std::cout << "Sorry, you have lost." << "\n";
  }
  std::cout << "Your Score: " << game.GetScore()[0] << "\n";
  std::cout << "Enemy Score: " << game.GetScore()[1] << "\n";
  
  
  return 0;
}