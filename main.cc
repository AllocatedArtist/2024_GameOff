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
      Vertex { glm::vec3(-1.0, 1.0, 0.0), glm::vec2(0.0, 1.0) }, 
      Vertex { glm::vec3(-1.0, -1.0, 0.0), glm::vec2(0.0, 0.0) }, 
      Vertex { glm::vec3(1.0, -1.0, 0.0), glm::vec2(1.0, 0.0) },
      Vertex { glm::vec3(1.0, 1.0, 0.0), glm::vec2(1.0, 1.0) }
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

  Texture shrek;
  shrek.LoadFromFile("../assets/shrek_map.png", true);

  int32_t u_texture0 = shader.GetUniformLocation("texture0");

  shader.Enable();
  shader.SetUniformInt(u_texture0, 0);
  shader.Disable();
    
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

    shader.Enable();
    shrek.Bind(0);
    
    Graphics::RenderPrimitiveIndexed(triangle);
    
    shrek.Unbind();
    shader.Disable();
    
    app.EndFrame();    
  }

  shrek.UnloadTexture();
  
  Graphics::DestroyPrimitive(triangle);
  shader.UnloadShader();
  
  return 0;
}
