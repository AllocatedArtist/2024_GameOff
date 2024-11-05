#include "Graphics.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>

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

void Shader::SetUniformMatrix(int32_t location, glm::mat4 value) const {
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
  const std::vector<glm::vec3>& vertices, 
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
  glBufferData(GL_ARRAY_BUFFER, primitive.vertices_.size() * sizeof(glm::vec3), &primitive.vertices_[0], GL_STATIC_DRAW);
  
  if (primitive.indices_.size() > 0) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, primitive.indices_.size() * sizeof(uint16_t), &primitive.indices_[0], GL_STATIC_DRAW);
  }

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
  glEnableVertexAttribArray(0);

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

void Graphics::RenderPrimitive(const Primitive& primitive, const Shader& shader) {
  shader.Enable();
  glBindVertexArray(primitive.vao_);
  glDrawArrays(GL_TRIANGLES, 0, primitive.vertices_.size());
  glBindVertexArray(0);
  shader.Disable();
}

void Graphics::RenderPrimitiveIndexed(const Primitive& primitive, const Shader& shader) {
  shader.Enable();
  glBindVertexArray(primitive.vao_);
  glDrawElements(GL_TRIANGLES, primitive.indices_.size(), GL_UNSIGNED_SHORT, nullptr);
  glBindVertexArray(0);
  shader.Disable();
}
