#ifndef APP_H_
#define APP_H_

#include <string>
#include <cstdint>

#include "InputManager.h"

class App {
public:
  App(uint32_t width, uint32_t height, const char* title);
  ~App();
  
  bool Update();
  void BeginFrame();
  void EndFrame();

  double GetTime() const;

  void CloseWindow() const;

  InputManager& GetInputManager();
private:
  InputManager input_manager_;
private:
  uint32_t width_;
  uint32_t height_;
  std::string title_;
  
  struct GLFWwindow* window_;
};



#endif
