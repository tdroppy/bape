#ifndef BAPE_OBJECTS_H
#define BAPE_OBJECTS_H

#include "raylib-cpp.hpp"
#include <algorithm>
#include <math.h>
#include <string>
#include <tuple>
#include <unistd.h>
#include <vector>

#define PLAYER_SPEED 350

struct objBounds {
  float leftSide;
  float bottomSide;
  float rightSide;
  float topSide;
};

class bapeObj {
private:
  raylib::Rectangle rect; // holds position
  objBounds rectord;
  raylib::Color color;
  raylib::Color defColor;
  std::string objectName;
  int weight;

public:
  float horizontalVelocity = 0;
  float verticleVelocity = 0;
  static std::vector<bapeObj *> objectList;

  bapeObj(float x, float y, float width, float height, Color colorInput,
          std::string objectName, int weight = 1)
      : rect(x, y, width, height), color(colorInput) {
    rectord = {x, y, x + width, y + height};
    objectList.push_back(this);
    this->objectName = objectName;
    this->weight = weight;
    defColor = color;
  }

  void drawObj() { rect.Draw(color); }

  void setColor(Color colorInput) { color = colorInput; }

  void resetColorToDefault() { color = defColor; }

  void moveHorizontally(float vel) {
    rect.x += vel * raylib::Window::GetFrameTime();
  }
  void moveHorizontally() {
    rect.x += horizontalVelocity * raylib::Window::GetFrameTime();
  }

  void moveVertically(float vel) {
    rect.y += ((0 - vel) * raylib::Window::GetFrameTime());
  }
  void moveVertically() {
    rect.y += ((0 - this->verticleVelocity) * raylib::Window::GetFrameTime());
  }

  std::string getName() { return objectName; }

  std::tuple<float, float> getDimensions() {
    return std::make_tuple(rect.width, rect.height);
  }

  std::tuple<float, float> getCenter() {
    return std::make_tuple(rect.x + (rect.width / 2),
                           rect.y + (rect.height / 2));
  }

  int getWeight() { return weight; }

  objBounds getPos() {
    objBounds bounds = {rect.x, (rect.y + rect.height), (rect.x + rect.width),
                        rect.y};
    return bounds;
  }

  ~bapeObj() {
    auto it = std::remove(objectList.begin(), objectList.end(), this);
    objectList.erase(it, objectList.end());
  }
};

int createRandomObj(bapeObj *plr, int count);

#endif
