#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <linalg/linalg.h>

#include <vector>
#include <cstdint>

using namespace linalg::aliases;

struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

struct Primitive {
  std::vector<float3> vertices_;
  std::vector<uint16_t> indices_;

  uint32_t vbo_;
  uint32_t ebo_;
  uint32_t vao_;
};

class Shader {
public:
  void LoadShader(const std::string& filename);
  void UnloadShader();
  
  const void Enable() const;
  const void Disable() const;
  
private:
  uint32_t program_;  
  uint32_t vertex_shader_;
  uint32_t fragment_shader_;
};

class Graphics {
public:  
  static void ClearColor(Color color);
  
  static void RenderPrimitive(const Primitive& primitive, const Shader& shader);
  static void RenderPrimitiveIndexed(const Primitive& primitive, const Shader& shader);
  
  static Primitive CreatePrimitive(
    const std::vector<float3>& vertices, 
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
