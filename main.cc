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
  
  while (app.IsWindowOpen()) {
    app.BeginFrame();

    float scale = cosf(app.GetTime());

    float time_f = static_cast<float>(app.GetTime());

    rotation = glm::rotate(rotation, glm::radians(time_f * 0.01f), glm::vec3(0.0, 0.0, 1.0));

    shader.SetUniformFloat(uniform_size_loc, scale);
    shader.SetUniformMatrix(uniform_rot_loc, rotation);
    
    Graphics::ClearColor(better_white);
    Graphics::RenderPrimitiveIndexed(triangle, shader);
    
    app.EndFrame();
  }

  Graphics::DestroyPrimitive(triangle);
  shader.UnloadShader();
  
  return 0;
}
