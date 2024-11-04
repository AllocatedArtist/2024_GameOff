#include <linalg/linalg.h>

#include "App.h"
#include "Graphics.h"


int main(void) {

  App app(1600, 1480, "Graphics");

  Color better_white = { 195, 195, 195, 255 };

  Shader shader;
  shader.LoadShader("../shaders/model.glsl");

  Primitive triangle = Graphics::CreatePrimitive({ float3(0.0, 0.5, 0.0 ), float3(-0.5, -0.5, 0.0 ), float3(0.5, -0.5, 0.0 ) });

  while (app.IsWindowOpen()) {
    app.BeginFrame();
    Graphics::ClearColor(better_white);

    Graphics::RenderPrimitive(triangle, shader);
    
    app.EndFrame();
  }

  Graphics::DestroyPrimitive(triangle);
  shader.UnloadShader();
  
  return 0;
}
