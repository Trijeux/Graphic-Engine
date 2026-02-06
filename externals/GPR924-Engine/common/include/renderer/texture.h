#ifndef GPR924_ENGINE_TEXTURE_H
#define GPR924_ENGINE_TEXTURE_H

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

#include <span>
#include <string_view>
#include "resource/resource.h"
#include "third_party/gl_include.h"

namespace common {


struct TextureInfo {
  GLuint texture_name;
  GLenum texture_target;
};

class TextureDestructor {
public:
  explicit TextureDestructor(TextureInfo& texture_info)
      : texture_info_(texture_info) {}
  void operator()() const {
    glDeleteTextures(1, &texture_info_.texture_name);
    texture_info_.texture_name = 0;
  }
private:
  TextureInfo& texture_info_;
};

enum class WrappingMode : uint8_t {
  REPEAT,
  CLAMP_TO_EDGE,
};
enum class FilteringMode : uint8_t {
  NEAREST,
  LINEAR,
};
struct TextureLoadInfo {
  WrappingMode wrapping_mode = WrappingMode::REPEAT;
  FilteringMode filtering_mode = FilteringMode::LINEAR;
  uint8_t required_channels = 0;
  bool generate_mipmaps = true;
};

class Texture : public core::Resource<TextureInfo, TextureDestructor> {
public:
  void Load(std::string_view path, TextureLoadInfo texture_load_info = {});
  void Load(void* data, int width, int height, int channels, TextureLoadInfo texture_load_info = {});
  void LoadCubeMap(std::span<const std::string_view> faces);
  void Bind() const;
  void Create(GLenum target, GLsizei  width, GLsizei height, GLint internal_format, void* data = nullptr, GLint level = 0);
};

/**
 * Returns the expected base format for a given internal format.
 * This can be used to automatically select the correct format.
 *
 * @param internalformat The internal format
 * @return The recommended base format, or 0 if unknown
 */
constexpr GLenum get_expected_base_format(GLenum internal_format) {
    switch (internal_format) {
        // Depth formats
        case GL_DEPTH_COMPONENT:
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32F:
            return GL_DEPTH_COMPONENT;

        case GL_DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;

        // RED base (non-integer)
        case GL_RED:
        case GL_R8:
        case GL_R8_SNORM:
        case GL_R16:
        case GL_R16_SNORM:
        case GL_R16F:
        case GL_R32F:
        case GL_COMPRESSED_RED:
        case GL_COMPRESSED_RED_RGTC1:
        case GL_COMPRESSED_SIGNED_RED_RGTC1:
            return GL_RED;

        // RED base (integer)
        case GL_R8I:
        case GL_R8UI:
        case GL_R16I:
        case GL_R16UI:
        case GL_R32I:
        case GL_R32UI:
            return GL_RED;

        // RG base (non-integer)
        case GL_RG:
        case GL_RG8:
        case GL_RG8_SNORM:
        case GL_RG16:
        case GL_RG16_SNORM:
        case GL_RG16F:
        case GL_RG32F:
        case GL_COMPRESSED_RG:
        case GL_COMPRESSED_RG_RGTC2:
        case GL_COMPRESSED_SIGNED_RG_RGTC2:
            return GL_RG;

        // RG base (integer)
        case GL_RG8I:
        case GL_RG8UI:
        case GL_RG16I:
        case GL_RG16UI:
        case GL_RG32I:
        case GL_RG32UI:
            return GL_RG;

        // RGB base (non-integer)
        case GL_RGB:
        case GL_R3_G3_B2:
        case GL_RGB4:
        case GL_RGB5:
        case GL_RGB8:
        case GL_RGB8_SNORM:
        case GL_RGB10:
        case GL_RGB12:
        case GL_RGB16_SNORM:
        case GL_RGB16F:
        case GL_RGB32F:
        case GL_R11F_G11F_B10F:
        case GL_RGB9_E5:
        case GL_SRGB8:
        case GL_COMPRESSED_RGB:
        case GL_COMPRESSED_SRGB:
        case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
        case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
            return GL_RGB;

        // RGB base (integer)
        case GL_RGB8I:
        case GL_RGB8UI:
        case GL_RGB16I:
        case GL_RGB16UI:
        case GL_RGB32I:
        case GL_RGB32UI:
            return GL_RGB;

        // RGBA base (non-integer)
        case GL_RGBA:
        case GL_RGBA2:
        case GL_RGBA4:
        case GL_RGB5_A1:
        case GL_RGBA8:
        case GL_RGBA8_SNORM:
        case GL_RGB10_A2:
        case GL_RGBA12:
        case GL_RGBA16:
        case GL_RGBA16F:
        case GL_RGBA32F:
        case GL_SRGB8_ALPHA8:
        case GL_COMPRESSED_RGBA:
        case GL_COMPRESSED_SRGB_ALPHA:
        case GL_COMPRESSED_RGBA_BPTC_UNORM:
        case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
            return GL_RGBA;

        // RGBA base (integer)
        case GL_RGB10_A2UI:
        case GL_RGBA8I:
        case GL_RGBA8UI:
        case GL_RGBA16I:
        case GL_RGBA16UI:
        case GL_RGBA32I:
        case GL_RGBA32UI:
            return GL_RGBA;

        default:
            return 0;
    }
}
/**
 * Returns the expected/recommended type for a given internal format.
 * This returns the most appropriate GL type based on the bit depth and
 * data type (normalized, float, signed/unsigned integer) of the internal format.
 *
 * Note: For many formats, multiple types are valid. This function returns
 * the most commonly used or most efficient type for each format.
 *
 * @param internalformat The internal format
 * @return The recommended GL type, or 0 if unknown
 */
constexpr GLenum get_expected_type(GLenum internalformat) {
    switch (internalformat) {
        // ============================================================
        // Table 1: Base Internal Formats
        // (flexible - return common defaults)
        // ============================================================
        case GL_RED:
        case GL_RG:
        case GL_RGB:
        case GL_RGBA:
            return GL_UNSIGNED_BYTE;

        case GL_DEPTH_COMPONENT:
            return GL_FLOAT;

        case GL_DEPTH_STENCIL:
            return GL_UNSIGNED_INT_24_8;

        // ============================================================
        // Table 2: Sized Internal Formats - 8-bit normalized
        // ============================================================
        case GL_R8:
        case GL_RG8:
        case GL_RGB8:
        case GL_RGBA8:
        case GL_SRGB8:
        case GL_SRGB8_ALPHA8:
            return GL_UNSIGNED_BYTE;

        // 8-bit signed normalized
        case GL_R8_SNORM:
        case GL_RG8_SNORM:
        case GL_RGB8_SNORM:
        case GL_RGBA8_SNORM:
            return GL_BYTE;

        // ============================================================
        // Table 2: Sized Internal Formats - 16-bit normalized
        // ============================================================
        case GL_R16:
        case GL_RG16:
        case GL_RGBA16:
            return GL_UNSIGNED_SHORT;

        // 16-bit signed normalized
        case GL_R16_SNORM:
        case GL_RG16_SNORM:
        case GL_RGB16_SNORM:
            return GL_SHORT;

        // ============================================================
        // Table 2: Sized Internal Formats - Special packed RGB formats
        // ============================================================
        case GL_R3_G3_B2:
            return GL_UNSIGNED_BYTE_3_3_2;

        case GL_RGB4:
        case GL_RGB5:
            return GL_UNSIGNED_SHORT_5_6_5;

        case GL_RGB10:
        case GL_RGB12:
            return GL_UNSIGNED_SHORT;

        // ============================================================
        // Table 2: Sized Internal Formats - Special packed RGBA formats
        // ============================================================
        case GL_RGBA2:
        case GL_RGBA4:
            return GL_UNSIGNED_SHORT_4_4_4_4;

        case GL_RGB5_A1:
            return GL_UNSIGNED_SHORT_5_5_5_1;

        case GL_RGB10_A2:
            return GL_UNSIGNED_INT_10_10_10_2;

        case GL_RGB10_A2UI:
            return GL_UNSIGNED_INT_10_10_10_2;

        case GL_RGBA12:
            return GL_UNSIGNED_SHORT;

        // ============================================================
        // Table 2: Sized Internal Formats - 16-bit float
        // ============================================================
        case GL_R16F:
        case GL_RG16F:
        case GL_RGB16F:
        case GL_RGBA16F:
            return GL_HALF_FLOAT;

        // ============================================================
        // Table 2: Sized Internal Formats - 32-bit float
        // ============================================================
        case GL_R32F:
        case GL_RG32F:
        case GL_RGB32F:
        case GL_RGBA32F:
            return GL_FLOAT;

        // ============================================================
        // Table 2: Sized Internal Formats - Special float formats
        // ============================================================
        case GL_R11F_G11F_B10F:
            return GL_UNSIGNED_INT_10F_11F_11F_REV;

        case GL_RGB9_E5:
            return GL_UNSIGNED_INT_5_9_9_9_REV;

        // ============================================================
        // Table 2: Sized Internal Formats - 8-bit signed integer
        // ============================================================
        case GL_R8I:
        case GL_RG8I:
        case GL_RGB8I:
        case GL_RGBA8I:
            return GL_BYTE;

        // ============================================================
        // Table 2: Sized Internal Formats - 8-bit unsigned integer
        // ============================================================
        case GL_R8UI:
        case GL_RG8UI:
        case GL_RGB8UI:
        case GL_RGBA8UI:
            return GL_UNSIGNED_BYTE;

        // ============================================================
        // Table 2: Sized Internal Formats - 16-bit signed integer
        // ============================================================
        case GL_R16I:
        case GL_RG16I:
        case GL_RGB16I:
        case GL_RGBA16I:
            return GL_SHORT;

        // ============================================================
        // Table 2: Sized Internal Formats - 16-bit unsigned integer
        // ============================================================
        case GL_R16UI:
        case GL_RG16UI:
        case GL_RGB16UI:
        case GL_RGBA16UI:
            return GL_UNSIGNED_SHORT;

        // ============================================================
        // Table 2: Sized Internal Formats - 32-bit signed integer
        // ============================================================
        case GL_R32I:
        case GL_RG32I:
        case GL_RGB32I:
        case GL_RGBA32I:
            return GL_INT;

        // ============================================================
        // Table 2: Sized Internal Formats - 32-bit unsigned integer
        // ============================================================
        case GL_R32UI:
        case GL_RG32UI:
        case GL_RGB32UI:
        case GL_RGBA32UI:
            return GL_UNSIGNED_INT;

        // ============================================================
        // Table 2: Sized Internal Formats - Depth formats
        // ============================================================
        case GL_DEPTH_COMPONENT16:
            return GL_UNSIGNED_SHORT;

        case GL_DEPTH_COMPONENT24:
            return GL_UNSIGNED_INT;

        case GL_DEPTH_COMPONENT32F:
            return GL_FLOAT;

        // ============================================================
        // Table 3: Compressed Internal Formats
        // (Use GL_UNSIGNED_BYTE as default for upload data)
        // ============================================================
        case GL_COMPRESSED_RED:
        case GL_COMPRESSED_RG:
        case GL_COMPRESSED_RGB:
        case GL_COMPRESSED_RGBA:
        case GL_COMPRESSED_SRGB:
        case GL_COMPRESSED_SRGB_ALPHA:
        case GL_COMPRESSED_RED_RGTC1:
        case GL_COMPRESSED_SIGNED_RED_RGTC1:
        case GL_COMPRESSED_RG_RGTC2:
        case GL_COMPRESSED_SIGNED_RG_RGTC2:
        case GL_COMPRESSED_RGBA_BPTC_UNORM:
        case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
            return GL_UNSIGNED_BYTE;

        case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
        case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
            return GL_FLOAT;

        default:
            return 0;
    }
}
/**
 * Checks if the internal format is a depth-only format.
 *
 * @param internalformat The internal format to check
 * @return true if it's a depth format (not depth-stencil), false otherwise
 */
constexpr bool is_depth_format(GLenum internalformat) {
  switch (internalformat) {
    case GL_DEPTH_COMPONENT:
    case GL_DEPTH_COMPONENT16:
    case GL_DEPTH_COMPONENT24:
    case GL_DEPTH_COMPONENT32:
    case GL_DEPTH_COMPONENT32F:
      return true;
    default:
      return false;
  }
}

/**
 * Checks if the internal format is a depth-stencil format.
 *
 * @param internalformat The internal format to check
 * @return true if it's a depth-stencil format, false otherwise
 */
constexpr bool is_depth_stencil(GLenum internalformat) {
  switch (internalformat) {
    case GL_DEPTH_STENCIL:
    case GL_DEPTH24_STENCIL8:
    case GL_DEPTH32F_STENCIL8:
      return true;
    default:
      return false;
  }
}

/**
 * Checks if the internal format is any depth or depth-stencil format.
 *
 * @param internalformat The internal format to check
 * @return true if it's a depth or depth-stencil format, false otherwise
 */
constexpr bool is_depth_or_depth_stencil_format(GLenum internalformat) {
  return is_depth_format(internalformat) || is_depth_stencil(internalformat);
}
}  // namespace common
#endif  // GPR924_ENGINE_TEXTURE_H
