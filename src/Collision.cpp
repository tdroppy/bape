#include "Collision.h"
#include <algorithm>
#include <math.h>
#include <unistd.h>
#include <vector>

CollisionDirection isCollision(
    const objBounds obj1,
    const objBounds obj2) { // TODO: maybe handle collision handling in here?

  bool collision =
      (obj1.leftSide < obj2.rightSide && obj1.rightSide > obj2.leftSide &&
       obj1.topSide < obj2.bottomSide && obj1.bottomSide > obj2.topSide);

  if (!collision) {
    return NONE;
  }

  float tcol = obj2.bottomSide - obj1.topSide;
  float bcol = obj1.bottomSide - obj2.topSide;
  float lcol = obj1.rightSide - obj2.leftSide;
  float rcol = obj2.rightSide - obj1.leftSide;

  float mpen = std::min({tcol, bcol, lcol, rcol});
  if (mpen == tcol)
    return TOP;
  if (mpen == bcol)
    return BOTTOM;
  if (mpen == lcol)
    return LEFT;
  if (mpen == rcol)
    return RIGHT;

  return NONE;
}

std::vector<CollisionDirection> checkCellCollision(bapeObj *obj) {
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
          CollisionDirection col = isCollision(
              obj->getPos(), grid[tempcx][tempcy].cellObjects[i]->getPos());
          if (col != NONE) {
            dlist.push_back(col);
          }
        }
      }
    }
  }

  return dlist;
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
        // grid[cx][cy].cellObjects.push_back(bapeObj::objectList[i]);
      }
    }
  }
}

// TODO: accelerate inverted velocity when object is INSIDE another
void handleObjectReactions() {
  for (auto obj : bapeObj::objectList) {
    std::vector<CollisionDirection> col = checkCellCollision(obj);
    int weight = obj->getWeight();
    if (std::find(col.begin(), col.end(), LEFT) != col.end()) {
      if (weight != 0) {
        obj->horizontalVelocity = (0 - (obj->horizontalVelocity + 3));
        // obj->moveHorizontally(0 - (obj->horizontalVelocity));
      }
    }
    if (std::find(col.begin(), col.end(), RIGHT) != col.end()) {
      if (weight != 0) {
        // obj->moveHorizontally(0 - (obj->horizontalVelocity));
        obj->horizontalVelocity = (0 - (obj->horizontalVelocity - 3));
      }
    }
    if (std::find(col.begin(), col.end(), TOP) != col.end()) {
      if (weight != 0) {
        obj->verticleVelocity = (0 - (obj->verticleVelocity + 3));
      }
    }
    if (std::find(col.begin(), col.end(), BOTTOM) != col.end()) {
      if (weight != 0) {
        obj->verticleVelocity = (0 - (obj->verticleVelocity - 3));
      }
    }
  }
}
