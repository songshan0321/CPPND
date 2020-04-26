#ifndef FOOD_H
#define FOOD_H

#include <vector>
#include <mutex>
#include "SDL.h"

class Food {
 public:
  enum class Type { kNormal, kBoost, kCut };
  
  Food();
  void Count();
  void ResetCounter();
  void Update(const int& x, const int& y, const Type& t);
  
  Type type;
  SDL_Point pos;
  
 private:
  const int MAX_COUNT = 5 * 60; // 5 seconds
  int counter{MAX_COUNT};
  
};

class FoodManager {
 public:
  FoodManager();
  void PushBack(Food& food);
  void UpdateFood(const int& n, const int& x, const int& y, const Food::Type& type);
  int CheckFood(const int& x, const int& y);
  Food::Type GetType(const int& n);
  int GetSize();
  std::vector<Food> GetFoods();
  void Count();
  
  int target_food_number{2};
 
 private:
  std::vector<Food> foods;
  std::mutex mtx;

};

#endif