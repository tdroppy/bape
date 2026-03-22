#include "BapeObjects.h"
#include "Collision.h"
#include <cmath>
#include <iostream>
#include <math.h>
#include <string>
#include <unistd.h>
#include <vector>

#define CELL_SIZE 60
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define GRAVITY = 1920.0f

std::vector<bapeObj *> bapeObj::objectList;
std::vector<std::vector<Cell>> grid;

int main(void) {
  int columns = ((SCREEN_WIDTH / CELL_SIZE) + 2);
  int rows = ((SCREEN_HEIGHT / CELL_SIZE) + 2);

  grid.resize(columns);
  for (int i = 0; i < columns; i++) {
    grid[i].resize(rows);
  }

  int velocityCeiling = 500;
  int spawnPerClick = 1;
  int rObjectSize = 20;
  int randObjCreated = 0;
  int currentFrame = 0;
  int showCellGrid = 1;

  raylib::Window window(SCREEN_WIDTH, SCREEN_HEIGHT, "bape");

  std::cout << "BAPE: Starting Initialization.." << std::endl;
  std::cout << "BAPE: Spawning Test Objects!" << std::endl;

  bapeObj testBlock1 = {240, 250, 80, 80, raylib::BROWN, "testBlock1", 1};
  bapeObj testBlock2 = {500, 250, 80, 80, raylib::PINK, "testBlock2", 1};

  std::cout << "BAPE: Spawning Borders!" << std::endl;
  // borders
  bapeObj borderLeft =   {1,                  1,                40,                 SCREEN_HEIGHT - 2,   raylib::Color{40, 40, 40, 255}, "borderLeft",   0};
  bapeObj borderRight =  {SCREEN_WIDTH - 41,  1,                40,                 SCREEN_HEIGHT - 2,   raylib::Color{40, 40, 40, 255}, "borderRight",  0};
  bapeObj borderTop =    {41,                 1,                SCREEN_WIDTH - 82,  40,                  raylib::Color{40, 40, 40, 255}, "borderTop",    0};
  bapeObj borderBottom = {41,                 SCREEN_HEIGHT - 41, SCREEN_WIDTH - 82, 40,                 raylib::Color{40, 40, 40, 255}, "borderBottom", 0};
  bapeObj wallLeft =     {41,                 41,               300,                SCREEN_HEIGHT - 82,  raylib::Color{40, 40, 40, 255}, "wallLeft",     0};

  bapeObj player = {400, 280, 40, 40, raylib::RED, "Player", 10};
  raylib::Camera2D camera;
  camera.target = raylib::Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
  camera.offset = raylib::Vector2(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
  while (!WindowShouldClose()) {
    // before drawing, need to handle updates
    float deltaTime = GetFrameTime();

    propagateGrid(currentFrame);

    if (IsKeyDown(KEY_D)) {
      if (player.horizontalVelocity < 300) {
        player.horizontalVelocity += 3;
      }
    }
    if (IsKeyDown(KEY_A)) {
      if (player.horizontalVelocity > -300) {
        player.horizontalVelocity -= 3;
      }
    }
    if (IsKeyDown(KEY_W)) {
      if (player.verticleVelocity < 300) {
        player.verticleVelocity += 3;
      }
    }
    if (IsKeyDown(KEY_S)) {
      if (player.verticleVelocity > -300) {
        player.verticleVelocity -= 3;
      }
    }
    if (IsKeyReleased(KEY_R)) {
      for (auto o : bapeObj::objectList) {
        if (o->getWeight() != 0) {
          int rH = rand() % velocityCeiling;
          int rV = rand() % velocityCeiling;

          if (rand() % 2 == 1) {
            rH = (0 - rH);
          }
          if (rand() % 2 == 1) {
            rV = (0 - rV);
          }

          o->horizontalVelocity = rH;
          o->verticleVelocity = rV;
        }
      }
    }
    if (IsKeyReleased(KEY_SPACE)) {
      showCellGrid++;
    }
    if (IsKeyReleased(KEY_UP)) {
      spawnPerClick += 1;
    }
    if (IsKeyReleased(KEY_DOWN)) {
      if (spawnPerClick > 1) spawnPerClick -= 1;
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      for (int i = 0; i < spawnPerClick; i++) {
        randObjCreated = createRandomObj(&player, randObjCreated, rObjectSize);
      }
    }

    float wheel = GetMouseWheelMove();
    if (wheel > 0) {
      if (IsKeyDown(KEY_LEFT_SHIFT)) {
        if (velocityCeiling < 1500) velocityCeiling += 50;
      } else
      if (rObjectSize < 50) rObjectSize += 2;
    }
    if (wheel < 0) {
      if (IsKeyDown(KEY_LEFT_SHIFT)) {
        if (velocityCeiling > 50) velocityCeiling -= 50;
      } else
      if (rObjectSize > 2) rObjectSize -= 2;
    }

    // test velocity case
    for (int i = 0; i < bapeObj::objectList.size(); i++) {
      bapeObj::objectList[i]->moveHorizontally();
      bapeObj::objectList[i]->horizontalVelocity *= 0.9999;
      bapeObj::objectList[i]->moveVertically();
      bapeObj::objectList[i]->verticleVelocity *= 0.9999;
    }

    handleObjectReactions();
    window.BeginDrawing();
    window.ClearBackground(raylib::Color{70, 70, 70, 255});

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

    std::string velReading =
        "Current Velocity: \nH: " +
        std::to_string(std::fabs(player.horizontalVelocity)) +
        "\nV: " + std::to_string(std::fabs(player.verticleVelocity));

    
    EndMode2D();
    RLAPI::DrawText("BAPE - Debug Menu", 21, 45, 20, raylib::RED);
    RLAPI::DrawText(TextFormat("FPS: %d", GetFPS()), 21, 75, 20, raylib::RED);
    RLAPI::DrawText(velReading.c_str(), 21, 105, 20, raylib::RED);
    RLAPI::DrawText(TextFormat("R_Objects Loaded: %d", randObjCreated), 21, 180, 20, raylib::RED);
    RLAPI::DrawText(TextFormat("R_Objects Size: %d", rObjectSize), 21, 210, 20, raylib::RED);
    RLAPI::DrawText(TextFormat("R_Objects per click: %d", spawnPerClick), 21, 240, 20, raylib::RED);
    RLAPI::DrawText(TextFormat("R_Velocity ceiling: %d", velocityCeiling), 21, 270, 20, raylib::RED);
    RLAPI::DrawText(" - - CONTROLS - - ", 21, 330, 20, raylib::RED);
    RLAPI::DrawText("Left Click: Spawn R_Object", 21, 360, 20, raylib::RED);
    RLAPI::DrawText("Scroll: Object Size", 21, 390, 20, raylib::RED);
    RLAPI::DrawText("Shift + Scroll: Velocity Ceiling", 21, 420, 20, raylib::RED);
    RLAPI::DrawText("R: Add random velocity (All)", 21, 450, 20, raylib::RED);
    RLAPI::DrawText("Space: Show collision grid", 21, 480, 20, raylib::RED);
    RLAPI::DrawText("UP/DOWN: Spawn/click", 21, 510, 20, raylib::RED);

    window.EndDrawing();
  }

  return 0;
}
