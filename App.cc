#include "App.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void WindowResize(GLFWwindow* window, int32_t width, int32_t height) {
  glViewport(0, 0, width, height);
}

App::App(uint32_t width, uint32_t height, const char* title) : width_(width), height_(height), title_(title) {  
  if (glfwInit() == GLFW_FALSE) {
    std::cerr << "GLFW UNABLE TO INITIALIZE!" << std::endl;
    exit(-1);
  }
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (window_ == nullptr) {
    std::cerr << "UNABLE TO CREATE WINDOW" << std::endl;
    glfwTerminate();
    exit(-1);
  }

  glfwMakeContextCurrent(window_);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "UNABLE TO ESTABLISH GLAD CONTEXT" << std::endl;
    glfwDestroyWindow(window_);
    glfwTerminate();
    exit(-1);
  }

  glViewport(0, 0, width_, height_);

  glfwSetWindowSizeCallback(window_, WindowResize);
}

App::~App() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

bool App::IsWindowOpen() {
  return !glfwWindowShouldClose(window_);
}

void App::BeginFrame() {
  glfwPollEvents();
  glClear(GL_COLOR_BUFFER_BIT);

}

void App::EndFrame() {
  glfwSwapBuffers(window_);
}
