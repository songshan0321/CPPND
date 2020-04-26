#include "food.h"

Food::Food() {
  pos.x = -1;
  pos.y = -1;
  type = Type::kNormal;
  ResetCounter();
}

void Food::Count() {
  counter--;
  if (counter <= 0){
    type = Type::kNormal; 
  }
}

void Food::ResetCounter() {
  counter = MAX_COUNT;
}

void Food::Update(const int& x, const int& y, const Type& t) {
  pos.x = x;
  pos.y = y;
  type = t;
  ResetCounter();
}

// Food Manager

FoodManager::FoodManager() {
  Food food1, food2;
  PushBack(food1);
  PushBack(food2);
}

void FoodManager::PushBack(Food& food) {
  std::lock_guard<std::mutex> lock(mtx);
  foods.push_back(food);
}

void FoodManager::UpdateFood(const int& n, const int& x, const int& y, const Food::Type& type) {
  std::lock_guard<std::mutex> lock(mtx);
  foods[n].Update(x, y, type);
}

int FoodManager::CheckFood(const int& x, const int& y) {
  std::lock_guard<std::mutex> lock(mtx);
  for (size_t i = 0; i < foods.size(); i++) {
    auto& food = foods[i]; 
    if (food.pos.x == x && food.pos.y == y) {
      return i;
    }
  }
  return -1;
}

Food::Type FoodManager::GetType(const int& n) {
  std::lock_guard<std::mutex> lock(mtx);
  return foods[n].type;
}

std::vector<Food> FoodManager::GetFoods() {
  std::lock_guard<std::mutex> lock(mtx);
  return foods;
}

int FoodManager::GetSize() {
  std::lock_guard<std::mutex> lock(mtx);
  return foods.size();
}

void FoodManager::Count() {
  std::lock_guard<std::mutex> lock(mtx);
  for (auto& food : foods) {
    food.Count();
  }
}
