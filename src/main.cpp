#include "raylib-cpp.hpp"
#include <math.h>
#include <stdio.h>
#include <tuple>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <iostream>
#include <string>

#define PLAYER_SPEED 350
#define CELL_SIZE 120

struct objCoord {
  float minX;
  float minY;
  float maxX;
  float maxY;
};


class bapeObj {
  private:
    raylib::Rectangle rect;
    objCoord rectord;
    raylib::Color color;
    std::string objectName;
  public:
    static std::vector<bapeObj*> objectList;

    bapeObj(float x, float y, float width, float height, Color colorInput, std::string objectName) 
     : rect(x, y, width, height), color(colorInput)
    {
      rectord = {x, y, x + width, y + height};
      objectList.push_back(this);
      this->objectName = objectName;
    } 

    void drawObj() {
      rect.Draw(color);
    }

    void setColor(Color colorInput) {
      color = colorInput;
    }

    void moveXInc() {rect.x += PLAYER_SPEED * raylib::Window::GetFrameTime();}

    void moveYInc() {rect.y -= PLAYER_SPEED * raylib::Window::GetFrameTime();}

    void moveXDec() {rect.x -= PLAYER_SPEED * raylib::Window::GetFrameTime();}

    void moveYDec() {rect.y += PLAYER_SPEED * raylib::Window::GetFrameTime();}

    std::string getName() {
      return objectName;
    }

    std::tuple<float, float, float, float, std::tuple<float, float>> getPos() {
      std::tuple<float, float> center(rect.x + (rect.width / 2.0f), rect.y + (rect.height) / 2.0f);
      return std::make_tuple(rect.x, rect.y, (rect.x + rect.width), (rect.y + rect.height), center);
    }

    ~bapeObj() {
      auto it = std::remove(objectList.begin(), objectList.end(), this);
      objectList.erase(it, objectList.end());
    }

};
std::vector<bapeObj*> bapeObj::objectList;

struct Cell {
  std::vector<bapeObj*> cellObjects;
};
std::vector<std::vector<Cell>> grid;

bool isCollision(std::tuple<float, float, float, float, std::tuple<float, float>> obj1, //AABB 
    std::tuple<float, float, float, float, std::tuple<float, float>> obj2) {
  return (std::get<0>(obj1) < std::get<2>(obj2)
          && std::get<2>(obj1) > std::get<0>(obj2)
          && std::get<1>(obj1) < std::get<3>(obj2)
          && std::get<3>(obj1) > std::get<1>(obj2)
      );
}

// this will not check for nearby objects in other cells, so this is pretty inaccurate
// adding a full implementation of this later
bool checkSameCellCollision(bapeObj* obj) {
  int gx = std::floor((std::get<0>(std::get<4>(obj->getPos()))) / CELL_SIZE);
  int gy = std::floor((std::get<1>(std::get<4>(obj->getPos()))) / CELL_SIZE);
  int count = 0;
  gx = std::clamp(gx, 0, int(grid.size() - 1));
  gy = std::clamp(gy, 0, int(grid[0].size() - 1));

  for (int i = 0; i < grid[gx][gy].cellObjects.size(); i++) {
    if (obj != grid[gx][gy].cellObjects[i]) {
      //std::cout << "Checking: " << obj->getName() << " -> " << grid[gx][gy].cellObjects[i]->getName() << std::endl;
      bool col = isCollision(obj->getPos(), grid[gx][gy].cellObjects[i]->getPos());
      if (col) { count++; }
    }
  }

  return count > 0;
}

int createRandomObj(bapeObj* plr, int count) { // returns the new amount of random objects created
  std::tuple<float, float> cntrPos = std::get<4>(plr->getPos());
  bapeObj* obj = new bapeObj(std::get<0>(cntrPos), std::get<1>(cntrPos), 40, 40, 
      raylib::RAYWHITE, std::to_string(count));
  return ++count;
}

int main(void) {
  const int SCREEN_WIDTH = 1920;
  const int SCREEN_HEIGHT = 1080;
  int randObjCreated = 0;
  raylib::Window window(SCREEN_WIDTH, SCREEN_HEIGHT, "bape");


  grid.resize((SCREEN_WIDTH / CELL_SIZE) + 1); 
  for (int i = 0; i < (SCREEN_WIDTH / CELL_SIZE); i++) {
    grid[i].resize((SCREEN_HEIGHT / CELL_SIZE) + 1);
  }

  bapeObj brownShit{240, 250, 80, 80, raylib::BROWN, "BrownSHIT"};
  bapeObj idk = {500, 250, 80, 80, raylib::PINK, "yuh"};
  bapeObj player = {400, 280, 40, 40, raylib::RED, "Player"};


  raylib::Camera2D camera;
  std::tuple pos = player.getPos();
  camera.target = raylib::Vector2( SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 );
  camera.offset = raylib::Vector2( SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f );
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
  while(!WindowShouldClose()) {
    // before drawing, need to handle updates
    std::tuple pos = player.getPos();
    float deltaTime = GetFrameTime();

    for (int i = 0; i < bapeObj::objectList.size(); i++) {
      int gx = std::floor((std::get<0>(std::get<4>(bapeObj::objectList[i]->getPos()))) / CELL_SIZE);
      int gy = std::floor((std::get<1>(std::get<4>(bapeObj::objectList[i]->getPos()))) / CELL_SIZE);
      grid[gx][gy].cellObjects.push_back(bapeObj::objectList[i]);

    }

    if (IsKeyDown(KEY_RIGHT)) {
      player.moveXInc();
      if (checkSameCellCollision(&player)) {
        player.setColor(raylib::YELLOW);
      } else { player.setColor(raylib::RED); }
    }
    if (IsKeyDown(KEY_LEFT)) {
      player.moveXDec();
      if (checkSameCellCollision(&player)) {
        player.setColor(raylib::YELLOW);
      }else { player.setColor(raylib::RED); } 
    }
    if (IsKeyDown(KEY_UP)) {
      player.moveYInc();
      if (checkSameCellCollision(&player)) {
        player.setColor(raylib::YELLOW);
      }else { player.setColor(raylib::RED); } 
    }
    if (IsKeyDown(KEY_DOWN)) {
      player.moveYDec();
      if (checkSameCellCollision(&player)) {
        player.setColor(raylib::YELLOW);
      }else { player.setColor(raylib::RED); } 
    }
    if (IsKeyReleased(KEY_SPACE)) {
      std::cout << randObjCreated << std::endl;
    }
    if (IsKeyReleased(KEY_C)) {
      randObjCreated = createRandomObj(&player, randObjCreated);
    }

    window.BeginDrawing();
      window.ClearBackground(raylib::GREEN);
      
      BeginMode2D(camera);

      for (int i = 0; i < bapeObj::objectList.size(); i++) {
        bapeObj::objectList[i]->drawObj();
      }

      EndMode2D();
      DrawFPS(10, 10);
      
    window.EndDrawing();

    for (int i = 0; i < grid.size(); i++) {
      for (int j = 0; j < grid[i].size(); j++) {
        grid[i][j].cellObjects.clear();
      }
    }
  }


  return 0;
}
