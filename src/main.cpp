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
#define CELL_SIZE 60
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

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

    std::tuple<float, float> getDimensions() {
      return std::make_tuple(rect.width, rect.height);
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
  int frameUsed = -1;
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

bool checkCellCollision(bapeObj* obj) {
  int count = 0;

  float objX = std::get<0>(obj->getPos());
  float objY = std::get<1>(obj->getPos());
  float objMaxX = objX + std::get<0>(obj->getDimensions());
  float objMaxY = objY + std::get<1>(obj->getDimensions());
  
  int cellX = (int)floor(objX / CELL_SIZE);
  int cellY = (int)floor(objY / CELL_SIZE);

  int cellMaxX = (int)floor(objMaxX / CELL_SIZE);
  int cellMaxY = (int)floor(objMaxY / CELL_SIZE);
  
  for (int tempcx = cellX; tempcx <= cellMaxX; tempcx++) {
    for (int tempcy = cellY; tempcy <= cellMaxY; tempcy++) {
      for (int i = 0; i < grid[tempcx][tempcy].cellObjects.size(); i++) {
        if (obj != grid[tempcx][tempcy].cellObjects[i]) {
          bool col = isCollision(obj->getPos(), grid[tempcx][tempcy].cellObjects[i]->getPos());
          if (col) { count++; }
        }
      }
    }
  }
  
  return count > 0;
}

int createRandomObj(bapeObj* plr, int count) { // returns the new amount of random objects created
  std::tuple<float, float> cntrPos = std::get<4>(plr->getPos());
  bapeObj* obj = new bapeObj(std::get<0>(cntrPos), std::get<1>(cntrPos), 200, 200, 
      raylib::RAYWHITE, std::to_string(count));
  return ++count;
}

void propagateGrid(int &currentFrame) {
    currentFrame++;

    for (int i = 0; i < bapeObj::objectList.size(); i++) {
      float objX = std::get<0>(bapeObj::objectList[i]->getPos());
      float objY = std::get<1>(bapeObj::objectList[i]->getPos());
      float objMaxX = objX + std::get<0>(bapeObj::objectList[i]->getDimensions());
      float objMaxY = objY + std::get<1>(bapeObj::objectList[i]->getDimensions());
    
      int cellX = (int)floor(objX / CELL_SIZE);
      int cellY = (int)floor(objY / CELL_SIZE);

      int cellMaxX = (int)floor(objMaxX / CELL_SIZE);
      int cellMaxY = (int)floor(objMaxY / CELL_SIZE);

      for (int cx = cellX; cx <= cellMaxX; cx++) {
        for (int cy = cellY; cy <= cellMaxY; cy++) {
          Cell &c = grid[cx][cy];
          if (c.frameUsed != currentFrame) { // only clears a cell once per frame
            c.cellObjects.clear();
            c.frameUsed = currentFrame;
          }
          c.cellObjects.push_back(bapeObj::objectList[i]);
          //grid[cx][cy].cellObjects.push_back(bapeObj::objectList[i]);
        }
      }
    }
}

int main(void) {
  int currentFrame = 0;
  int randObjCreated = 0;
  int showCellGrid = 1;
  raylib::Window window(SCREEN_WIDTH, SCREEN_HEIGHT, "bape");


  grid.resize((SCREEN_WIDTH / CELL_SIZE) + 2); 
  for (int i = 0; i < (SCREEN_WIDTH / CELL_SIZE); i++) {
    grid[i].resize((SCREEN_HEIGHT / CELL_SIZE) + 2);
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

    propagateGrid(currentFrame);
    for (auto obj : bapeObj::objectList) {
      if (checkCellCollision(obj)) {
        obj->setColor(raylib::PURPLE);
      } else {
        obj->setColor(raylib::RAYWHITE);
      }
    }

    if (IsKeyDown(KEY_RIGHT)) {
      player.moveXInc();
    }
    if (IsKeyDown(KEY_LEFT)) {
      player.moveXDec();
    }
    if (IsKeyDown(KEY_UP)) {
      player.moveYInc();
    }
    if (IsKeyDown(KEY_DOWN)) {
      player.moveYDec();
    }
    if (IsKeyReleased(KEY_SPACE)) {
      std::cout << randObjCreated << std::endl;
      showCellGrid++;
    }
    if (IsKeyReleased(KEY_C)) {
      for (int i = 0; i < 1; i++) {
        randObjCreated = createRandomObj(&player, randObjCreated);
      }
    }

    window.BeginDrawing();
      window.ClearBackground(raylib::GREEN);
      
      BeginMode2D(camera);

      for (int i = 0; i < bapeObj::objectList.size(); i++) {
        bapeObj::objectList[i]->drawObj();
      }

      if ((showCellGrid % 2) == 0) {
        for (int i = 0; i < 1920; i += 60) {
           DrawLine(i, 0, i, 1080, raylib::RED);
           DrawLine(0, i, 1920, i, raylib::RED);
        }
      }

     
      EndMode2D();
      DrawFPS(10, 10);
      
    window.EndDrawing();

    //for (int i = 0; i < grid.size(); i++) {
    //  for (int j = 0; j < grid[i].size(); j++) {
    //    grid[i][j].cellObjects.clear();
    //  }
    //}
  }


  return 0;
}
