#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <glm/glm.hpp>

#include <vector>
#include <cstdint>

#include <tiny_gltf.h>

struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

struct Vertex {
  glm::vec3 pos_;
  glm::vec2 tex_coords_;
};

struct Primitive {
  std::vector<Vertex> vertices_;
  std::vector<uint16_t> indices_;

  uint32_t vbo_;
  uint32_t ebo_;
  uint32_t vao_;
};

class Shader {
public:
  void LoadShader(const std::string& filename);
  void UnloadShader();
  
  void Enable() const;
  void Disable() const;  

  int32_t GetUniformLocation(const char* uniform) const;

  void SetUniformInt(int32_t location, int32_t value) const;
  void SetUniformFloat(int32_t location, float value) const;
  void SetUniformVec3(int32_t location, glm::vec3 value) const;
  void SetUniformMatrix(int32_t location, glm::mat4 value) const;
private:  
  uint32_t program_;  
  uint32_t vertex_shader_;
  uint32_t fragment_shader_;
};

class Texture {
public:
  void LoadFromFile(const std::string& file, bool flip = false);
  void UnloadTexture();
  
  void Bind(int32_t slot);
  void Unbind();
private:
  uint32_t id_;
};

class Graphics {
public:  
  static void ClearColor(Color color);
  
  static void RenderPrimitive(const Primitive& primitive);
  static void RenderPrimitiveIndexed(const Primitive& primitive);
  
  static Primitive CreatePrimitive(
    const std::vector<Vertex>& vertices, 
    const std::vector<uint16_t>& indices = {}
  );

  static void DestroyPrimitive(Primitive& primitive);
  
public:
  constexpr static Color kRed { 255, 0, 0, 255 };
  constexpr static Color kGreen { 0, 255, 0, 255 };
  constexpr static Color kBlue { 0, 0, 255, 255 };
  
  constexpr static Color kWhite { 255, 255, 255, 255 };
  constexpr static Color kBlack { 0, 0, 0, 255 };
};



#endif
