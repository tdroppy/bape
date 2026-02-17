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
  bapeObj borderLeft = {
      1, 1, 40, SCREEN_HEIGHT - 2, raylib::Color{40, 40, 40, 255}, "borderLeft",
      0};
  bapeObj borderRight = {SCREEN_WIDTH - 41,
                         1,
                         40,
                         SCREEN_HEIGHT - 2,
                         raylib::Color{40, 40, 40, 255},
                         "borderRight",
                         0};
  bapeObj borderTop = {
      41,          1, SCREEN_WIDTH - 82, 40, raylib::Color{40, 40, 40, 255},
      "borderTop", 0};
  bapeObj borderBottom = {41,
                          SCREEN_HEIGHT - 41,
                          SCREEN_WIDTH - 82,
                          40,
                          raylib::Color{40, 40, 40, 255},
                          "borderBottom",
                          0};
  bapeObj player = {400, 280, 40, 40, raylib::RED, "Player"};
  raylib::Camera2D camera;
  camera.target = raylib::Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
  camera.offset = raylib::Vector2(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
  while (!WindowShouldClose()) {
    // before drawing, need to handle updates
    float deltaTime = GetFrameTime();

    propagateGrid(currentFrame);

    if (IsKeyDown(KEY_RIGHT)) {
      if (player.horizontalVelocity < 300) {
        player.horizontalVelocity += 3;
      }
    }
    if (IsKeyDown(KEY_LEFT)) {
      if (player.horizontalVelocity > -300) {
        player.horizontalVelocity -= 3;
      }
    }
    if (IsKeyDown(KEY_UP)) {
      if (player.verticleVelocity < 300) {
        player.verticleVelocity += 3;
      }
    }
    if (IsKeyDown(KEY_DOWN)) {
      if (player.verticleVelocity > -300) {
        player.verticleVelocity -= 3;
      }
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

    // test velocity case
    for (int i = 0; i < bapeObj::objectList.size(); i++) {
      bapeObj::objectList[i]->moveHorizontally();
      bapeObj::objectList[i]->horizontalVelocity *= 0.999;
      bapeObj::objectList[i]->moveVertically();
      bapeObj::objectList[i]->verticleVelocity *= 0.999;
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

    RLAPI::DrawText(velReading.c_str(), 1700, 10, 20, raylib::RED);
    EndMode2D();
    DrawFPS(10, 10);

    window.EndDrawing();
  }

  return 0;
}
