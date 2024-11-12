#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

#include <iostream>

#include "App.h"
#include "Graphics.h"

void ProcessRoot(std::vector<glm::mat4>& transforms, Joint root, glm::mat4 parent) {
  glm::mat4 global = parent * root.transform_;
  transforms.emplace_back(global);
  for (const Joint& child : root.children_) {
    ProcessRoot(transforms, child, global);
  }
}

int main(void) {

  App app(1600, 1480, "Graphics");

  Color better_white = { 195, 195, 195, 255 };

  Shader shader;
  shader.LoadShader("../shaders/model.glsl");

  Model cube;
  cube.Load("../assets/robot.glb");

  InputManager& input = app.GetInputManager();
  input.AddAction(Key::kKeyEscape, "Quit");
  input.AddAction(Key::kKey6, "Quit");

  input.RegisterInputs();

  float dt = 1.f / 60.f;
  float accumulator = 0.f;

  int32_t u_model = shader.GetUniformLocation("u_Model");
  int32_t u_vp = shader.GetUniformLocation("u_ViewProjection");  
  int32_t u_texture0 = shader.GetUniformLocation("texture0");
  int32_t u_base_color = shader.GetUniformLocation("u_baseColor");

  shader.Enable();
  shader.SetUniformInt(u_texture0, 0);
  shader.Disable();

  glm::mat4 model(1.0);
  glm::mat4 view(1.0);
  glm::mat4 projection(1.0);

  std::vector<int32_t> u_bind_pose;
  std::vector<glm::mat4> bind_poses;
  
  for (int i = 0; i < 100; ++i) {
    std::string uniform = "u_Joints[" + std::to_string(i) + "]";
    u_bind_pose.push_back(shader.GetUniformLocation(uniform.c_str()));
  }

      
  for (const Skin& skin : cube.GetSkins()) {
    std::vector<glm::mat4> bone_transforms;
    ProcessRoot(bone_transforms, skin.root_, glm::mat4(1.0));
    for (int i = 0; i < skin.inverse_bind_matrices_.size(); ++i) {            
      bind_poses.push_back(bone_transforms[i] * skin.inverse_bind_matrices_[i]);
    }
  }
    
  while (app.Update()) {        
    float frame_time = app.GetDeltaTime();
    accumulator += frame_time;
    
    while (accumulator >= dt) {      
      if (input.IsActionDown("Quit")) {
        app.CloseWindow();
      }          

      float t = app.GetTime();
      float x = cosf(t) * 1.5;
      float z = sinf(t) * 1.5;

      model = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -1, 0.0));
      // model = glm::rotate(model, glm::radians(90.f),glm::vec3(1.0,0.0,0.0));
      // model = glm::scale(model, glm::vec3(0.05));
      view = glm::lookAt(glm::vec3(x, 0.0, z), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

      int32_t width = app.GetScreenWidth();
      int32_t height = app.GetScreenHeight();

      if (height == 0) height = 1;
    
      projection = glm::perspective(glm::radians(90.f), float(width) / float(height), 0.01f, 100.f);
      
      accumulator -= dt;
    }

    app.BeginFrame();
    
    Graphics::ClearColor(better_white);

    shader.Enable();

    shader.SetUniformMatrix(u_vp, projection * view);

    for (int32_t i = 0; i < bind_poses.size(); ++i) {
      shader.SetUniformMatrix(u_bind_pose[i], bind_poses[i]);
    }

    for (const Mesh& mesh : cube.GetMeshes()) {
      shader.SetUniformMatrix(u_model, model * mesh.local_transform_);
      for (const MeshPrimitive& primitive : mesh.mesh_primitives_) {
        if (primitive.material_) {
          const Material& material = primitive.material_.value();
          if (material.has_texture_) {
            Texture(material.texture_id_).Bind(0);
          }
          shader.SetUniformVec4(u_base_color, material.color_);
        }        
        Graphics::RenderPrimitiveIndexed(primitive.primitive_);
        Texture(0).Unbind();
      }
    }
    
    shader.Disable();
    
    app.EndFrame();    
  }
  
  shader.UnloadShader();
  
  return 0;
}
