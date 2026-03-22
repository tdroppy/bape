#include "BapeObjects.h"

int createRandomObj(bapeObj *plr, int count, int size) { // returns the new amount of random objects created
  Color randColor = {rand() % 256, rand() % 256, rand() % 256, 255};

  Vector2 mousePos = GetMousePosition();
  bapeObj *obj = new bapeObj(mousePos.x, mousePos.y, size, size, randColor,
                             std::to_string(count), size);
  return ++count;
}
