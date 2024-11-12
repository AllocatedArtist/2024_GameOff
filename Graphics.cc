#include "Graphics.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <cstddef>

#include <stb_image.h>

void Shader::Enable() const {
  glUseProgram(program_);
}

void Shader::Disable() const {
  glUseProgram(0);
}

void Shader::UnloadShader() {
  glDeleteShader(vertex_shader_);
  glDeleteShader(fragment_shader_);
  glDeleteProgram(program_);
}


int32_t Shader::GetUniformLocation(const char* uniform) const {
  return glGetUniformLocation(program_, uniform);
}

void Shader::SetUniformInt(int32_t location, int32_t value) const {
  glUniform1i(location, value);
}

void Shader::SetUniformFloat(int32_t location, float value) const {
  glUniform1f(location, value);
}

void Shader::SetUniformVec3(int32_t location, glm::vec3 value) const {
  glUniform3f(location, value.x, value.y, value.z);
}

void Shader::SetUniformVec4(int32_t location, glm::vec4 value) const {
  glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Shader::SetUniformMatrix(int32_t location, const glm::mat4& value) const {
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

static void CheckShaderError(uint32_t shader) {
  int32_t success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success == GL_FALSE) {
    char log[512];
    glGetShaderInfoLog(shader, 512, nullptr, log);
    std::cout << "SHADER: " << log << std::endl;
  }
}

void Shader::LoadShader(const std::string& filename) {
  std::fstream file(filename);
  std::string file_contents = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

  const std::string identifier = "#SPLIT";
  
  size_t split_location = file_contents.find(identifier);
  
  std::string vertex_str = file_contents.substr(0, split_location);
  std::string fragment_str = file_contents.substr(split_location + identifier.size());

  program_ = glCreateProgram();
  vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
  fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);

  const char* vertex_c_str = vertex_str.c_str();
  const char* fragment_c_str = fragment_str.c_str();

  glShaderSource(vertex_shader_, 1, &vertex_c_str, nullptr);
  glShaderSource(fragment_shader_, 1, &fragment_c_str, nullptr);

  glCompileShader(vertex_shader_);
  CheckShaderError(vertex_shader_);
  
  glCompileShader(fragment_shader_);
  CheckShaderError(fragment_shader_);

  glAttachShader(program_, vertex_shader_);
  glAttachShader(program_, fragment_shader_);

  glLinkProgram(program_);
}

void Graphics::ClearColor(Color color) {
  float red = static_cast<float>(color.r) / 255.f;
  float green = static_cast<float>(color.g) / 255.f;
  float blue = static_cast<float>(color.b) / 255.f;
  float alpha = static_cast<float>(color.a) / 255.f;

  glClearColor(red, green, blue, alpha);
}

Primitive Graphics::CreatePrimitive(
  const std::vector<Vertex>& vertices, 
  uint32_t index_count,
  uint32_t index_type,
  uint32_t joint_type,
  const std::vector<uint8_t>& indices
) {
  Primitive primitive;
  primitive.vertices_ = vertices;
  primitive.index_count_ = index_count;
  primitive.index_type_ = index_type;
  primitive.joint_type_ = joint_type; 

  glGenVertexArrays(1, &primitive.vao_);
  
  glGenBuffers(1, &primitive.vbo_);
  glGenBuffers(1, &primitive.ebo_);

  glBindVertexArray(primitive.vao_);

  glBindBuffer(GL_ARRAY_BUFFER, primitive.vbo_);
  glBufferData(GL_ARRAY_BUFFER, primitive.vertices_.size() * sizeof(Vertex), &primitive.vertices_[0], GL_STATIC_DRAW);
  
  if (index_count > 0) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size(), &indices[0], GL_STATIC_DRAW);
  }

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos_));
  glEnableVertexAttribArray(0);
  
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords_));
  glEnableVertexAttribArray(1);
  
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal_));
  glEnableVertexAttribArray(2);
  
  glVertexAttribIPointer(3, 4, primitive.joint_type_, sizeof(Vertex), (void*)offsetof(Vertex, joints_));
  glEnableVertexAttribArray(3);
  
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights_));
  glEnableVertexAttribArray(4);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  return primitive;
}


void Graphics::DestroyPrimitive(Primitive& primitive) {
  if (primitive.index_count_ > 0) {
    glDeleteBuffers(1, &primitive.ebo_);
  }
  
  glDeleteBuffers(1, &primitive.vbo_);
  glDeleteVertexArrays(1, &primitive.vao_);
}

void Graphics::RenderPrimitive(const Primitive& primitive) {
  glBindVertexArray(primitive.vao_);
  glDrawArrays(GL_TRIANGLES, 0, primitive.vertices_.size());
  glBindVertexArray(0);
}

void Graphics::RenderPrimitiveIndexed(const Primitive& primitive) {
  glBindVertexArray(primitive.vao_);
  glDrawElements(GL_TRIANGLES, primitive.index_count_, primitive.index_type_, nullptr);
  glBindVertexArray(0);
}

void Texture::LoadFromFile(const std::string& file, bool flip) {
  int32_t width = 0;
  int32_t height = 0;
  int32_t channel = 0;

  stbi_set_flip_vertically_on_load(flip);
  
  stbi_uc* image_data = stbi_load(file.c_str(), &width, &height, &channel, 0);

  assert(image_data != nullptr && "Unable to load image");

  glGenTextures(1, &id_);
  glBindTexture(GL_TEXTURE_2D, id_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);

  GLenum format = GL_RGBA;
  if (channel == 1) {
    format = GL_RED;
  } else if (channel == 3) {
    format = GL_RGB;
  }
  
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(image_data);
  
  stbi_set_flip_vertically_on_load(false);
}

Texture::Texture(uint32_t id) : id_(id) {}

void Texture::UnloadTexture() {
  glDeleteTextures(1, &id_);
}

void Texture::Bind(int32_t slot) {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, id_);
}

void Texture::Unbind() {
  glBindTexture(GL_TEXTURE_2D, 0);
}

uint32_t Texture::GetTextureID() const {
  return id_;
}

Model::~Model() {
  if (!is_loaded_) {
    return;
  }
  
  for (Mesh& mesh : meshes_) {
    for (MeshPrimitive& primitive : mesh.mesh_primitives_) {
      Graphics::DestroyPrimitive(primitive.primitive_);
    }
  }
  
  for (auto& [_, texture] : texture_cache_) {
    texture.UnloadTexture();
  }
}

const std::vector<Mesh>& Model::GetMeshes() const {
  return meshes_;
}

uint32_t LoadGLTF_Texture(const tinygltf::Sampler& sampler, const tinygltf::Image& image) {
  uint32_t id = 0;

  std::vector<uint8_t> data = image.image;

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler.wrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler.wrapT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler.minFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler.magFilter);

  GLenum format = GL_RGBA;
  if (image.component == 1) {
    format = GL_RED;
  } else if (image.component == 2) {
    format = GL_RG;
  } else if (image.component == 3) {
    format = GL_RGB;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, format, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, data.data());
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);
  
  return id;
}

glm::ivec4 LoadAttributeIVec4(uint8_t* slice) {
  return glm::make_vec4(reinterpret_cast<const uint32_t*>(slice));
}

glm::vec4 LoadAttributeVec4(uint8_t* slice) {
  return glm::make_vec4(reinterpret_cast<const float*>(slice));
}

glm::vec3 LoadAttributeVec3(uint8_t* slice) {
  return glm::make_vec3(reinterpret_cast<const float*>(slice));
}

glm::vec2 LoadAttributeVec2(uint8_t* slice) {
  return glm::make_vec2(reinterpret_cast<const float*>(slice));
}

Mesh GetMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh, std::unordered_map<std::string, Texture>& texture_cache) {
  Mesh result;

  for (const tinygltf::Primitive& primitive : mesh.primitives) {    
    
    const tinygltf::Accessor& indices_accessor = model.accessors[primitive.indices];
    const tinygltf::BufferView& indices_bv = model.bufferViews[indices_accessor.bufferView];
    const tinygltf::Buffer& indices_b = model.buffers[indices_bv.buffer];

    std::vector<uint8_t> indices_data(      
      indices_b.data.cbegin() + indices_bv.byteOffset, 
      indices_b.data.cbegin() + indices_bv.byteOffset + indices_bv.byteLength
    );
    
    uint32_t index_type = indices_accessor.componentType;
    uint32_t index_count = indices_accessor.count;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normals;
    std::vector<glm::ivec4> joints; 
    std::vector<glm::vec4> weights;

    uint32_t joint_type = 0;

    for (auto& [name, index] : primitive.attributes) {
      const tinygltf::Accessor& accessor = model.accessors[index];
      const tinygltf::BufferView& accessor_bv = model.bufferViews[accessor.bufferView];
      const tinygltf::Buffer& accessor_b = model.buffers[accessor_bv.buffer];      

      constexpr size_t kVec3Size = 12;
      constexpr size_t kVec2Size = 8;
      constexpr size_t kVec4Size = 16;
      constexpr size_t kIVec4Size = 4;
      
      std::vector<uint8_t> data(          
        accessor_b.data.cbegin() + accessor_bv.byteOffset, 
        accessor_b.data.cbegin() + accessor_bv.byteOffset + accessor_bv.byteLength
      );
      
      if (name == "NORMAL") {
        for (int32_t i = 0; i < data.size() / kVec3Size; ++i) {
          normals.emplace_back(LoadAttributeVec3(data.data() + accessor.ByteStride(accessor_bv) * i));
        }
      }
      if (name == "POSITION") {
        for (int32_t i = 0; i < data.size() / kVec3Size; ++i) {
          positions.emplace_back(LoadAttributeVec3(data.data() + accessor.ByteStride(accessor_bv) * i));
        }
      }
      if (name == "TEXCOORD_0") {
        for (int32_t i = 0; i < data.size() / kVec2Size; ++i) {
          texcoords.emplace_back(LoadAttributeVec2(data.data() + accessor.ByteStride(accessor_bv) * i));
        }
      }
      if (name == "JOINTS_0") {
        joint_type = accessor.componentType;
        for (int32_t i = 0; i < data.size() / kIVec4Size; ++i) {
          joints.emplace_back(LoadAttributeIVec4(data.data() + accessor.ByteStride(accessor_bv) * i));
        }
      }
      if (name == "WEIGHTS_0") {        
        for (int32_t i = 0; i < data.size() / kVec4Size; ++i) {
          weights.emplace_back(LoadAttributeVec4(data.data() + accessor.ByteStride(accessor_bv) * i));
        }
      }
    }
        
    assert(positions.size() == normals.size() && positions.size() > 0);

    std::vector<Vertex> vertices;
    for (int32_t i = 0; i < positions.size(); ++i) {
      Vertex vertex;
      vertex.pos_ = positions[i];
      vertex.normal_ = normals[i];
      if (texcoords.size() > 0) {
        vertex.tex_coords_ = texcoords[i];
      } else {
        vertex.tex_coords_ = glm::vec2(0.0);
      }
      vertices.push_back(vertex);
    }  
    
    if (joints.size() > 0 && weights.size() > 0) {
      assert(joints.size() == weights.size() && weights.size() == positions.size());
      for (int32_t i = 0; i < joints.size(); ++i) {
        vertices[i].joints_ = joints[i];
        vertices[i].weights_ = weights[i];
      }
    }

    MeshPrimitive mesh_p;
    
    // TODO (HANDLE COLOR)
    if (primitive.material >= 0) {      
      Material p_material;
      const tinygltf::Material& material = model.materials[primitive.material];
      const tinygltf::TextureInfo& texture_info = material.pbrMetallicRoughness.baseColorTexture;
      if (texture_info.index >= 0) {
        const tinygltf::Texture& texture = model.textures[texture_info.index];
        const tinygltf::Image& image = model.images[texture.source];
        
        if (texture_cache.find(image.name) != texture_cache.cend()) {
          p_material = Material { texture_cache.at(image.name).GetTextureID(), true };
        } else {
          texture_cache[image.name] = LoadGLTF_Texture(model.samplers[texture.sampler], image);
          p_material = Material { texture_cache.at(image.name).GetTextureID(), true };
        }
      }

      std::vector<double> color = material.pbrMetallicRoughness.baseColorFactor;
      if (!color.empty()) {
        p_material.color_ = glm::vec4(color[0], color[1], color[2], color[3]);
      }

      mesh_p.material_ = p_material;
    }
    
    mesh_p.primitive_ = Graphics::CreatePrimitive(
      vertices, 
      index_count, 
      index_type, 
      joint_type, 
      indices_data
    );
    result.mesh_primitives_.push_back(mesh_p);
  }
  
  return result;
}

Joint ProcessJoint(const tinygltf::Model& model, const tinygltf::Node& joint) {
  Joint curr;
  curr.transform_ = glm::mat4(1.0);
  
  if (!joint.translation.empty()) {
    curr.transform_ = glm::translate(
      curr.transform_, 
      glm::vec3(joint.translation[0], joint.translation[1], joint.translation[2])
    );
  }
  if (!joint.rotation.empty()) {
    glm::quat rot(joint.rotation[3], joint.rotation[0], joint.rotation[1], joint.rotation[2]);
    
    float angle = glm::angle(rot);
    glm::vec3 axis = glm::axis(rot);
    
    curr.transform_ = glm::rotate(curr.transform_, angle, axis);
  }
  if (!joint.scale.empty()) {
    curr.transform_ = glm::scale(
      curr.transform_, glm::vec3(joint.scale[0], joint.scale[1], joint.scale[2])
    );
  }

  for (int32_t child_id : joint.children) {
    curr.children_.emplace_back(ProcessJoint(model, model.nodes[child_id]));
  }
  
  return curr;
}

void Model::Load(const std::string& filename) {
  tinygltf::TinyGLTF loader;

  tinygltf::Model model;
  std::string err;
  std::string warn;
  
  bool success = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
  if (!err.empty()) {
    std::cout << "[" << filename << "] ERROR: " << err << std::endl;
  }
  if (!warn.empty()) {
    std::cout << "[" << filename << "] WARN: " << warn << std::endl;
  }
  assert(success && "Failed to parse GLTF");

  for (const tinygltf::Node& node : model.nodes) {
    if (node.mesh < 0) {
      continue;
    }
    Mesh mesh = GetMesh(model, model.meshes[node.mesh], texture_cache_);

    glm::mat4 local(1.0);
    
    if (!node.translation.empty()) {
      local = glm::translate(local, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
    }
    if (!node.rotation.empty()) {
      glm::quat rot(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
      
      float angle = glm::angle(rot);
      glm::vec3 axis = glm::axis(rot);
      
      local = glm::rotate(local, angle, axis);
    }
    if (!node.scale.empty()) {
      local = glm::scale(local, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
    }

    mesh.local_transform_ = local;
    
    meshes_.push_back(mesh);
  }

  for (const tinygltf::Skin& skin : model.skins) {
      Skin loaded_skin;
      
      const tinygltf::Accessor& inverse_bind_matrices = model.accessors[skin.inverseBindMatrices];
      const tinygltf::BufferView& ibm_bv = model.bufferViews[inverse_bind_matrices.bufferView];
      const tinygltf::Buffer& ibm_b = model.buffers[ibm_bv.buffer];

      std::vector<uint8_t> data(
        ibm_b.data.cbegin() + ibm_bv.byteOffset,
        ibm_b.data.cbegin() + ibm_bv.byteOffset + ibm_bv.byteLength
      );

      constexpr size_t kMatrixSize = 64;

      loaded_skin.root_ = ProcessJoint(model, model.nodes[skin.joints[0]]);
      
      for (int32_t i = 0; i < data.size() / kMatrixSize; ++i) {  
        loaded_skin.inverse_bind_matrices_.emplace_back(
          glm::make_mat4(
            reinterpret_cast<float*>(data.data() + inverse_bind_matrices.ByteStride(ibm_bv) * i)
          )
        );
      }

      skins_.push_back(loaded_skin);
  }

  is_loaded_ = true;
}

const std::vector<Skin>& Model::GetSkins() const {
  return skins_;
}
