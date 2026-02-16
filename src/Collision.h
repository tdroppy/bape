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
extern std::vector<std::vector<Cell>> grid;

CollisionDirection isCollision(const objBounds obj1, const objBounds obj2);
std::vector<CollisionDirection> checkCellCollision(bapeObj *obj);
void propagateGrid(int &currentFrame);
void handleObjectReactions();

#endif
