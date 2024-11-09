#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "App.h"
#include "Graphics.h"


int main(void) {

  App app(1600, 1480, "Graphics");

  Color better_white = { 195, 195, 195, 255 };

  Shader shader;
  shader.LoadShader("../shaders/model.glsl");

  Primitive triangle = Graphics::CreatePrimitive(
    { 
      glm::vec3(-0.5, 0.5, 0.0), 
      glm::vec3(-0.5, -0.5, 0.0), 
      glm::vec3(0.5, -0.5, 0.0),
      glm::vec3(0.5, 0.5, 0.0)
    },
    {
      0, 1, 2, 
      2, 3, 0  
    }
  );

  int32_t uniform_size_loc = shader.GetUniformLocation("size");
  int32_t uniform_rot_loc = shader.GetUniformLocation("rotation");

  glm::mat4 rotation(1.0);

  InputManager& input = app.GetInputManager();
  input.AddAction(Key::kKeyEscape, "Quit");
  input.AddAction(Key::kKey6, "Quit");

  input.AddAction(Key::kKeyRight, "SpinRight");
  input.AddAction(Key::kKeyLeft, "SpinLeft");
  
  while (app.Update()) {    
    
    if (input.IsActionReleased("Quit")) {
      app.CloseWindow();
    }

    app.BeginFrame();

    if (input.IsActionPressed("SpinRight")) {
      rotation = glm::rotate(rotation, glm::radians(-0.01f), glm::vec3(0.0, 0.0, 1.0));
    } else if (input.IsActionPressed("SpinLeft")) {
      rotation = glm::rotate(rotation, glm::radians(0.01f), glm::vec3(0.0, 0.0, 1.0));
    }

    shader.SetUniformFloat(uniform_size_loc, 1);
    shader.SetUniformMatrix(uniform_rot_loc, rotation);
    
    Graphics::ClearColor(better_white);
    Graphics::RenderPrimitiveIndexed(triangle, shader);
    
    app.EndFrame();
  }

  Graphics::DestroyPrimitive(triangle);
  shader.UnloadShader();
  
  return 0;
}
