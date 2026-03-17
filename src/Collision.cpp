#include "Collision.h"
#include <algorithm>
#include <math.h>
#include <unistd.h>
#include <vector>
#include <iostream>

CollisionDirection isCollision(bapeObj *bapeObj1, bapeObj *bapeObj2) {
  objBounds obj1 = bapeObj1->getPos();
  objBounds obj2 = bapeObj2->getPos();

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
  if (bapeObj1->getWeight() != 0) {
    if (mpen == tcol) {
      bapeObj1->forceMoveVertically(mpen);
      return TOP;
    }
    if (mpen == bcol) {
      bapeObj1->forceMoveVertically(-mpen);
      return BOTTOM;
    }
    if (mpen == lcol) {
      bapeObj1->forceMoveHorizontally(-mpen);
      return LEFT;
    }
    if (mpen == rcol) {
      bapeObj1->forceMoveHorizontally(mpen);
      return RIGHT;
    }
    
  }
  return NONE;
}

std::vector<CollisionEvent> checkCellCollision(bapeObj *obj) {
  int count = 0;
  std::vector<CollisionEvent> collisionList;
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
          CollisionDirection col = isCollision(obj, grid[tempcx][tempcy].cellObjects[i]);
          CollisionEvent pair = {col, grid[tempcx][tempcy].cellObjects[i]};
          if (col != NONE) {
            collisionList.push_back(pair);
          }
        }
      }
    }
  }

  return collisionList;
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

// TODO: fix left border phasing
void handleObjectReactions() {
  std::vector<bapeObj *> handledObjects;
  for (auto obj : bapeObj::objectList) {
    std::vector<CollisionEvent> colEvent = checkCellCollision(obj);
    
    if (std::find(handledObjects.begin(), handledObjects.end(), obj) == handledObjects.end()) {
      int weight = obj->getWeight();

      for (auto event : colEvent) {
        handledObjects.push_back(obj);
        handledObjects.push_back(event.collidingObject);
        
        bapeObj* v1 = obj;
        bapeObj* v2 = event.collidingObject;

        float origV1 = v1->horizontalVelocity;
        float origV2 = v2->horizontalVelocity;

        float vorigV1 = v1->verticleVelocity;
        float vorigV2 = v2->verticleVelocity;

        switch (event.objCollisionDirection) {
          case LEFT:
          case RIGHT:
            if (event.collidingObject->getWeight() != 0) {
              obj->horizontalVelocity = ((v1->getWeight() - v2->getWeight()) * origV1 + 2 * v2->getWeight() * origV2) / (v2->getWeight() + v1->getWeight());
              event.collidingObject->horizontalVelocity = ((v2->getWeight() - v1->getWeight()) * origV2 + 2 * v1->getWeight() * origV1) / (v1->getWeight() + v2->getWeight());
            }
            break;
          case TOP:
          case BOTTOM:
            if (event.collidingObject->getWeight() != 0){
              obj->verticleVelocity = ((v1->getWeight() - v2->getWeight()) * vorigV1 + 2 * v2->getWeight() * vorigV2) / (v2->getWeight() + v1->getWeight());
              event.collidingObject->verticleVelocity = ((v2->getWeight() - v1->getWeight()) * vorigV2 + 2 * v1->getWeight() * vorigV1) / (v1->getWeight() + v2->getWeight());
            }
            break;
        }
      }
    }
  }
}
