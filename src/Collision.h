#ifndef COLLISION_H
#define COLLISION_H

#define CELL_SIZE 60

#include "BapeObjects.h"
#include <math.h>
#include <unistd.h>
#include <vector>

class bapeObj;
struct objBounds;

struct Cell {
  std::vector<bapeObj *> cellObjects;
  int frameUsed = -1;
};

enum CollisionDirection { NONE, TOP, BOTTOM, LEFT, RIGHT };

struct CollisionEvent {
  CollisionDirection objCollisionDirection;
  bapeObj *collidingObject;
};

extern std::vector<std::vector<Cell>> grid;

CollisionDirection isCollision(bapeObj *bapeObj1, bapeObj *bapeObj2);
std::vector<CollisionEvent> checkCellCollision(bapeObj *obj);
void propagateGrid(int &currentFrame);
void handleObjectReactions();

#endif
