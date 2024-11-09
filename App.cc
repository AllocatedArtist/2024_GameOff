#include "App.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


struct {
  std::vector<KeyInt> updated_keys_;
  bool keys_[GLFW_KEY_MENU];
} Global;

void WindowResize(GLFWwindow* window, int32_t width, int32_t height) {
  glViewport(0, 0, width, height);
}

void KeyCallback(GLFWwindow* window, KeyInt key, int32_t scancode, int32_t action, int32_t mods) {
  bool state = action != GLFW_RELEASE;
  if (Global.keys_[key] != state) {
    Global.keys_[key] = state;
    Global.updated_keys_.push_back(key);
  }
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
  glfwSetKeyCallback(window_, KeyCallback);
}

App::~App() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

bool App::Update() {
  glfwPollEvents();

  for (KeyInt updated_key: Global.updated_keys_) {
    std::optional<std::string> action = input_manager_.GetAction(Key(updated_key));
    if (action) {
      std::string action_str = action.value();
      ActionType action_type = Global.keys_[updated_key] ? ActionType::kPress: ActionType::kRelease;
      Action new_action = Action(action_str, action_type);
      input_manager_.actions_.insert_or_assign(action_str, new_action);
    }
  }
  Global.updated_keys_.clear();
  
  return !glfwWindowShouldClose(window_);
}

double App::GetTime() const {
  return glfwGetTime();
}

void App::BeginFrame() {
  glClear(GL_COLOR_BUFFER_BIT);
}

void App::EndFrame() {
  glfwSwapBuffers(window_);
}

void App::CloseWindow() const {
  glfwSetWindowShouldClose(window_, GLFW_TRUE);
}


InputManager& App::GetInputManager() {
  return input_manager_;
}
