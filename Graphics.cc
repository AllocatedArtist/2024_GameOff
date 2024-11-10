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
  Enable();
  glUniform1i(location, value);
  Disable();
}

void Shader::SetUniformFloat(int32_t location, float value) const {
  Enable();
  glUniform1f(location, value);
  Disable();
}

void Shader::SetUniformVec3(int32_t location, glm::vec3 value) const {
  Enable();
  glUniform3f(location, value.x, value.y, value.z);
  Disable();
}

void Shader::SetUniformMatrix(int32_t location, glm::mat4 value) const {
  Enable();
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
  Disable();
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
  const std::vector<uint16_t>& indices
) {
  Primitive primitive;
  primitive.vertices_ = vertices;
  primitive.indices_ = indices;

  glGenVertexArrays(1, &primitive.vao_);
  
  glGenBuffers(1, &primitive.vbo_);
  glGenBuffers(1, &primitive.ebo_);

  glBindVertexArray(primitive.vao_);

  glBindBuffer(GL_ARRAY_BUFFER, primitive.vbo_);
  glBufferData(GL_ARRAY_BUFFER, primitive.vertices_.size() * sizeof(Vertex), &primitive.vertices_[0], GL_STATIC_DRAW);
  
  if (primitive.indices_.size() > 0) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, primitive.indices_.size() * sizeof(uint16_t), &primitive.indices_[0], GL_STATIC_DRAW);
  }

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos_));
  glEnableVertexAttribArray(0);
  
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords_));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  return primitive;
}

void Graphics::DestroyPrimitive(Primitive& primitive) {
  if (primitive.indices_.size() > 0) {
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
  glDrawElements(GL_TRIANGLES, primitive.indices_.size(), GL_UNSIGNED_SHORT, nullptr);
  glBindVertexArray(0);
}


void Texture::LoadFromFile(const std::string& file, bool flip) {
  int32_t width = 0;
  int32_t height = 0;
  int32_t channel = 0;

  stbi_set_flip_vertically_on_load(flip);
  
  stbi_uc* image_data = stbi_load(file.c_str(), &width, &height, &channel, 0);

  std::string error = "Unable to load image: " + file;
  assert(image_data != nullptr && error.c_str());

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
