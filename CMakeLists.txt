cmake_minimum_required(VERSION 3.15)
project(bape LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
FetchContent_Declare(
  raylib-cpp
  GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git 
  GIT_TAG master
)

FetchContent_MakeAvailable(raylib-cpp)

add_executable(bape src/main.cpp)

target_link_libraries(bape PRIVATE raylib)

# Only needed if RAYLIB_INCLUDE_DIRS exists (optional)
target_include_directories(bape PRIVATE ${raylib-cpp_SOURCE_DIR}/include)
