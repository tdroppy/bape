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

struct objBounds {
  float leftSide;
  float bottomSide;
  float rightSide;
  float topSide;
};

class bapeObj {
  private:
    raylib::Rectangle rect;
    objBounds rectord;
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

    std::tuple<float, float> getCenter() {
      return std::make_tuple(rect.x + (rect.width / 2), rect.y + (rect.height / 2));
    }

    //std::tuple<float, float, float, float, std::tuple<float, float>> getPos() {
    //  std::tuple<float, float> center(rect.x + (rect.width / 2.0f), rect.y + (rect.height) / 2.0f);
    //  return std::make_tuple(rect.x, rect.y, (rect.x + rect.width), (rect.y + rect.height), center);
    //}
    
    objBounds getPos() {
      objBounds bounds = {rect.x, (rect.y + rect.height), (rect.x + rect.width), rect.y};
      return bounds;
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

enum CollisionDirection { NONE, TOP, BOTTOM, LEFT, RIGHT };

CollisionDirection isCollision(const objBounds obj1, const objBounds obj2) {
   
  bool collision = (
      obj1.leftSide < obj2.rightSide &&
      obj1.rightSide > obj2.leftSide &&
      obj1.topSide < obj2.bottomSide &&
      obj1.bottomSide > obj2.topSide
      );

  if (!collision) {
    return NONE;
  }

  float tcol = obj2.bottomSide - obj1.topSide;
  float bcol = obj1.bottomSide - obj2.topSide;
  float lcol = obj1.rightSide - obj2.leftSide;
  float rcol = obj2.rightSide - obj1.leftSide;

  float mpen = std::min({tcol, bcol, lcol, rcol});
  if (mpen == tcol) return TOP;
  if (mpen == bcol) return BOTTOM;
  if (mpen == lcol) return LEFT;
  if (mpen == rcol) return RIGHT;

  return NONE;
}

//bool isCollision(std::tuple<float, float, float, float, std::tuple<float, float>> obj1, //AABB 
//    std::tuple<float, float, float, float, std::tuple<float, float>> obj2) {
//  // 0: Left / 1: Up / 2: Right / 3: Down
//  return (std::get<0>(obj1) < std::get<2>(obj2)
//          && std::get<2>(obj1) > std::get<0>(obj2)
//          && std::get<1>(obj1) < std::get<3>(obj2)
//          && std::get<3>(obj1) > std::get<1>(obj2)
//      );
//}
bool checkCellCollision(bapeObj* obj) {
  int count = 0;

  objBounds bounds = obj->getPos(); 

  float objX = bounds.leftSide; 
  float objY = bounds.topSide;
  float objMaxX = bounds.rightSide;
  float objMaxY = bounds.bottomSide; 
  
  int cellX = (int)floor(objX / CELL_SIZE);
  int cellY = (int)floor(objY / CELL_SIZE);

  int cellMaxX = (int)floor(objMaxX / CELL_SIZE);
  int cellMaxY = (int)floor(objMaxY / CELL_SIZE);

  
  for (int tempcx = cellX; tempcx <= cellMaxX; tempcx++) {
    for (int tempcy = cellY; tempcy <= cellMaxY; tempcy++) {
      for (int i = 0; i < grid[tempcx][tempcy].cellObjects.size(); i++) {
        if (obj != grid[tempcx][tempcy].cellObjects[i]) {
          CollisionDirection col = isCollision(obj->getPos(), grid[tempcx][tempcy].cellObjects[i]->getPos());
          if (col != NONE) { count++; }
        }
      }
    }
  }
  
  return count > 0;
}

int createRandomObj(bapeObj* plr, int count) { // returns the new amount of random objects created
  std::tuple<float, float> cntrPos = plr->getCenter();
  bapeObj* obj = new bapeObj(std::get<0>(cntrPos), std::get<1>(cntrPos), 80, 80, 
      raylib::RAYWHITE, std::to_string(count));
  return ++count;
}

void propagateGrid(int &currentFrame) {
    currentFrame++;

    for (int i = 0; i < bapeObj::objectList.size(); i++) {
      objBounds bounds = bapeObj::objectList[i]->getPos();
      float objX = bounds.leftSide; 
      float objY = bounds.topSide;
      float objMaxX = bounds.rightSide; 
      float objMaxY = bounds.bottomSide; 
    
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
  camera.target = raylib::Vector2( SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 );
  camera.offset = raylib::Vector2( SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f );
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
  while(!WindowShouldClose()) {
    // before drawing, need to handle updates
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
        for (int i = 0; i < 1920; i += CELL_SIZE) {
           DrawLine(i, 0, i, 1080, raylib::RED);
           DrawLine(0, i, 1920, i, raylib::RED);
        }
      }

     
      EndMode2D();
      DrawFPS(10, 10);
      
    window.EndDrawing();

  }


  return 0;
}
