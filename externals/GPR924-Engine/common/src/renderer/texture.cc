/*
Copyright 2025 SAE Institute Switzerland SA

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Contributors: Elias Farhan, Jemoel Ablay
*/

#include "renderer/texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdexcept>

namespace common {

void Texture::Load(std::string_view path, TextureLoadInfo texture_load_info) {
  int w, h, channels;
  auto* img_data = stbi_load(path.data(), &w, &h, &channels,
                             texture_load_info.required_channels);
  if (img_data == nullptr) {
    throw std::runtime_error("Failed to load image data");
  }
  Load(img_data, w, h, channels, texture_load_info);
}
void Texture::Load(void* data, int width, int height, int channels,
                   TextureLoadInfo texture_load_info) {
  get().texture_target = GL_TEXTURE_2D;

  glGenTextures(1, &get().texture_name);
  glBindTexture(GL_TEXTURE_2D, get().texture_name);
  GLint internal_format;
  switch (channels) {
    case 3:
      internal_format = GL_RGB8;
      break;
    case 4:
      internal_format = GL_RGBA8;
      break;
    default:
      throw std::runtime_error("Unsupported texture channels");
  }
  Create(get().texture_target, width, height, internal_format, data);

  GLint wrapping_mode;
  switch (texture_load_info.wrapping_mode) {
    case WrappingMode::REPEAT:
      wrapping_mode = GL_REPEAT;
      break;
    case WrappingMode::CLAMP_TO_EDGE:
      wrapping_mode = GL_CLAMP_TO_EDGE;
      break;
    default:
      throw std::runtime_error("Unsupported wrapping mode");
  }
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping_mode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping_mode);
  GLint filtering_mode;
  switch (texture_load_info.filtering_mode) {
    case FilteringMode::LINEAR:
      filtering_mode = GL_LINEAR;
      break;
    case FilteringMode::NEAREST:
      filtering_mode = GL_NEAREST;
      break;
    default:
      throw std::runtime_error("Unsupported filtering mode");
  }
  glTexParameteri(get().texture_target, GL_TEXTURE_MAG_FILTER, filtering_mode);
  glTexParameteri(
      get().texture_target, GL_TEXTURE_MIN_FILTER,
      filtering_mode == GL_LINEAR && texture_load_info.generate_mipmaps
          ? GL_LINEAR_MIPMAP_LINEAR
          : filtering_mode);
  if (texture_load_info.generate_mipmaps) {
    glGenerateMipmap(get().texture_target);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::LoadCubeMap(const std::span<const std::string_view> faces) {
  if (faces.size() != 6) throw std::runtime_error("Cube map requires 6 faces");

  glGenTextures(1, &get().texture_name);
  glBindTexture(GL_TEXTURE_CUBE_MAP, get().texture_name);

  for (size_t i = 0; i < faces.size(); i++) {
    int width, height, channels;
    unsigned char* data =
        stbi_load(std::string(faces[i]).c_str(), &width, &height, &channels, 0);
    if (!data)
      throw std::runtime_error("Failed to load cube map texture: " +
                               std::string(faces[i]));

    const GLenum format = (channels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i), 0,
                 format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  get().texture_target = GL_TEXTURE_CUBE_MAP;
}

void Texture::Bind() const {
  glBindTexture(get().texture_target, get().texture_name);
}

void Texture::Create(GLenum target, GLsizei width, GLsizei height,
                     GLint internal_format, void* data, GLint level) {
  const GLint format = get_expected_base_format(internal_format);
  const GLenum type = get_expected_type(internal_format);
  glGenTextures(1, &get().texture_name);
  get().texture_target = target;
  glBindTexture(get().texture_target, get().texture_name);
  glTexImage2D(get().texture_target, level, internal_format, width, height, 0,
               format, type, data);
}

}  // namespace common