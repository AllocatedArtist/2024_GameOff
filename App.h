#ifndef APP_H_
#define APP_H_

#include <string>
#include <cstdint>

class App {
public:
  App(uint32_t width, uint32_t height, const char* title);
  ~App();
  
  bool IsWindowOpen();
  void BeginFrame();
  void EndFrame();
private:
  uint32_t width_;
  uint32_t height_;
  std::string title_;
  
  struct GLFWwindow* window_;
};






#endif
