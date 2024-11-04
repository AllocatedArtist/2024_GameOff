cmake_minimum_required(VERSION 3.24)

project(PlayGround)

add_library(Glad STATIC vendor/glad/src/glad.c)
target_include_directories(Glad PUBLIC vendor/glad/include)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(
  PlayGround 
  main.cc 
  App.cc
  Graphics.cc
)

target_include_directories(PlayGround PUBLIC vendor/glfw/include vendor/linalg)

target_link_libraries(PlayGround Glad glfw3)
target_link_directories(PlayGround PUBLIC lib/src)

target_compile_features(PlayGround PRIVATE cxx_std_17)
target_compile_options(PlayGround PRIVATE -Wall -Wpedantic -Werror)
