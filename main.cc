#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

#include <iostream>

#include "App.h"
#include "Graphics.h"


int main(void) {

  App app(1600, 1480, "Graphics");

  Color better_white = { 195, 195, 195, 255 };

  Shader shader;
  shader.LoadShader("../shaders/model.glsl");

  Primitive triangle = Graphics::CreatePrimitive(
    { 
      glm::vec3(-1.0, 1.0, 0.0), 
      glm::vec3(-1.0, -1.0, 0.0), 
      glm::vec3(1.0, -1.0, 0.0),
      glm::vec3(1.0, 1.0, 0.0)
    },
    {
      0, 1, 2, 
      2, 3, 0  
    }
  );

  InputManager& input = app.GetInputManager();
  input.AddAction(Key::kKeyEscape, "Quit");
  input.AddAction(Key::kKey6, "Quit");

  input.AddAction(Key::kKey0, "t0");
  input.AddAction(Key::kKey1, "t1");

  input.RegisterInputs();

  float dt = 1.f / 60.f;
  float accumulator = 0.f;
    
  while (app.Update()) {        
    float frame_time = app.GetDeltaTime();
    accumulator += frame_time;
    
    while (accumulator >= dt) {      
      if (input.IsActionDown("Quit")) {
        app.CloseWindow();
      }          

      accumulator -= dt;
    }

    app.BeginFrame();
    
    Graphics::ClearColor(better_white);
    Graphics::RenderPrimitiveIndexed(triangle, shader);
    
    app.EndFrame();    
  }

  Graphics::DestroyPrimitive(triangle);
  shader.UnloadShader();
  
  return 0;
}
