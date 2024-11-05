#include <glm/glm.hpp>

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

  while (app.IsWindowOpen()) {
    app.BeginFrame();
    
    Graphics::ClearColor(better_white);
    Graphics::RenderPrimitiveIndexed(triangle, shader);
    
    app.EndFrame();
  }

  Graphics::DestroyPrimitive(triangle);
  shader.UnloadShader();
  
  return 0;
}
