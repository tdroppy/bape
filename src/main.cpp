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
    raylib::Color defColor;
    std::string objectName;
    int weight;
  public:
    static std::vector<bapeObj*> objectList;

    bapeObj(float x, float y, float width, float height, Color colorInput, std::string objectName, int weight = 1) 
     : rect(x, y, width, height), color(colorInput)
    {
      rectord = {x, y, x + width, y + height};
      objectList.push_back(this);
      this->objectName = objectName;
      this->weight = weight;
      defColor = color;
    } 

    void drawObj() {
      rect.Draw(color);
    }

    void setColor(Color colorInput) {
      color = colorInput;
    }

    void resetColorToDefault() {
      color = defColor;
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

    int getWeight() {
      return weight;
    }

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

CollisionDirection isCollision(const objBounds obj1, const objBounds obj2) { // TODO: maybe handle collision handling in here?
   
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
std::vector<CollisionDirection> checkCellCollision(bapeObj* obj) {
  int count = 0;
  std::vector<CollisionDirection> dlist;
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
          if (col != NONE) { dlist.push_back(col); }
        }
      }
    }
  }
  
  return dlist;
}

int createRandomObj(bapeObj* plr, int count) { // returns the new amount of random objects created
  Vector2 mousePos = GetMousePosition();
  bapeObj* obj = new bapeObj(mousePos.x, mousePos.y, 80, 80, 
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

  int columns = ((SCREEN_WIDTH / CELL_SIZE) + 2);
  int rows = ((SCREEN_HEIGHT / CELL_SIZE) + 2);

  grid.resize(columns); 
  for (int i = 0; i < columns; i++) {
    grid[i].resize(rows);
  }

  bapeObj testBlock1 = {240, 250, 80, 80, raylib::BROWN, "testBlock1", -1};
  bapeObj testBlock2 = {500, 250, 80, 80, raylib::PINK, "testBlock2", -1};
  bapeObj player = {400, 280, 40, 40, raylib::RED, "Player"};

  // borders
  bapeObj borderLeft = {1, 1, 40, SCREEN_HEIGHT - 2, raylib::Color {40, 40, 40, 255}, "borderLeft", -1}; 
  bapeObj borderRight = {SCREEN_WIDTH - 41, 1, 40, SCREEN_HEIGHT - 2, raylib::Color {40, 40, 40, 255}, "borderRight", -1}; 
  bapeObj borderTop = {41, 1, SCREEN_WIDTH - 82, 40, raylib::Color {40, 40, 40, 255}, "borderTop", -1}; 
  bapeObj borderBottom = {41, SCREEN_HEIGHT - 41, SCREEN_WIDTH - 82, 40, raylib::Color {40, 40, 40, 255}, "borderBottom", -1}; 


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
      std::vector<CollisionDirection> col = checkCellCollision(obj);
      int weight = obj->getWeight();
      if (std::find(col.begin(), col.end(), LEFT) != col.end()) {
        if (weight != -1) { obj->moveXDec(); }
      }
      if (std::find(col.begin(), col.end(), RIGHT) != col.end()) {
        if (weight != -1) { obj->moveXInc(); }
      }
      if (std::find(col.begin(), col.end(), TOP) != col.end()) {
        if (weight != -1) { obj->moveYDec(); }
      }
      if (std::find(col.begin(), col.end(), BOTTOM) != col.end()) {
        if (weight != -1) { obj->moveYInc(); }
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
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      for (int i = 0; i < 1; i++) {
        randObjCreated = createRandomObj(&player, randObjCreated);
      }
    }

    window.BeginDrawing();
      window.ClearBackground(raylib::Color {70, 70, 70, 255});
      
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
