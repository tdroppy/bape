#include "BapeObjects.h"

int createRandomObj(
    bapeObj *plr,
    int count) { // returns the new amount of random objects created
  Vector2 mousePos = GetMousePosition();
  bapeObj *obj = new bapeObj(mousePos.x, mousePos.y, 80, 80, raylib::RAYWHITE,
                             std::to_string(count));
  return ++count;
}
