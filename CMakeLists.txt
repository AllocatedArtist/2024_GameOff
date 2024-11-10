cmake_minimum_required(VERSION 3.24)

project(PlayGround)

add_library(Glad STATIC vendor/glad/src/glad.c)
target_include_directories(Glad PUBLIC vendor/glad/include)

add_library(TinyGLTF STATIC vendor/tinygltf/tiny_gltf.cc)
target_include_directories(TinyGLTF PUBLIC vendor/tinygltf)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(
  PlayGround   
  main.cc 
  App.cc
  Graphics.cc
  InputManager.cc  
)

target_include_directories(PlayGround PUBLIC vendor/glfw/include vendor/glm)

target_link_libraries(PlayGround Glad TinyGLTF glfw3)
target_link_directories(PlayGround PUBLIC lib/src)

target_compile_features(PlayGround PRIVATE cxx_std_17)
target_compile_options(PlayGround PRIVATE -Wall -Wpedantic -Werror)
