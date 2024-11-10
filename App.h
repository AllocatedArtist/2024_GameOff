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

  int32_t GetScreenWidth() const;
  int32_t GetScreenHeight() const;

  InputManager& GetInputManager();

  float GetDeltaTime() const;
private:
  InputManager input_manager_;

  float current_time_;
  float previous_time_;
  float delta_;
private:
  uint32_t width_;
  uint32_t height_;
  std::string title_;
  
  struct GLFWwindow* window_;
};



#endif
