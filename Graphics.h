#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

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
  glm::vec3 normal_;
  glm::ivec4 joints_;
  glm::vec4 weights_;
};

struct Primitive {
  std::vector<Vertex> vertices_;
  
  uint32_t index_count_;
  uint32_t index_type_;
  uint32_t joint_type_;

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
  void SetUniformVec4(int32_t location, glm::vec4 value) const;
  void SetUniformMatrix(int32_t location, const glm::mat4& value) const;
private:  
  uint32_t program_;  
  uint32_t vertex_shader_;
  uint32_t fragment_shader_;
};

class Texture {
public:
  Texture() = default;
  Texture(uint32_t id);
  
  void LoadFromFile(const std::string& file, bool flip = false);
  void UnloadTexture();
  
  void Bind(int32_t slot);
  void Unbind();

  uint32_t GetTextureID() const;
private:
  uint32_t id_;
};

struct Material {
  uint32_t texture_id_;
  bool has_texture_;

  glm::vec4 color_ = glm::vec4(0.0, 0.0, 0.0, 1.0);
};

struct MeshPrimitive {
  Primitive primitive_;
  std::optional<Material> material_;
};

struct Mesh {
  std::vector<MeshPrimitive> mesh_primitives_;
  glm::mat4 local_transform_;
};

struct Joint {
  glm::mat4 transform_;
  std::vector<Joint> children_;
};

struct Skin {
  Joint root_;
  std::vector<glm::mat4> inverse_bind_matrices_;
};

class Model {
public:
  Model() = default;
  ~Model();
  
  void Load(const std::string& filename);

  const std::vector<Mesh>& GetMeshes() const;
  const std::vector<Skin>& GetSkins() const;
private:    
  std::vector<Mesh> meshes_;
  std::vector<Skin> skins_;
  std::unordered_map<std::string, Texture> texture_cache_;
private:
  bool is_loaded_;
};

class Graphics {
public:  
  static void ClearColor(Color color);
  
  static void RenderPrimitive(const Primitive& primitive);
  static void RenderPrimitiveIndexed(const Primitive& primitive);
  
  static Primitive CreatePrimitive(
    const std::vector<Vertex>& vertices, 
    uint32_t index_count,
    uint32_t index_type,
    uint32_t joint_type,
    const std::vector<uint8_t>& raw_indices = {}
  );

  //Assumes that attributes have been set
  static void DestroyPrimitive(Primitive& primitive);
  
public:
  constexpr static Color kRed { 255, 0, 0, 255 };
  constexpr static Color kGreen { 0, 255, 0, 255 };
  constexpr static Color kBlue { 0, 0, 255, 255 };
  
  constexpr static Color kWhite { 255, 255, 255, 255 };
  constexpr static Color kBlack { 0, 0, 0, 255 };
};



#endif
