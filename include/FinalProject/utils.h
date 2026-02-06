#ifndef GPR924_COMPGRAPH_UTILS_H
#define GPR924_COMPGRAPH_UTILS_H
#include <stb_image.h>
#include <GL/glew.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace Graphics {
    inline std::string LoadFile(const char *path) {
        const std::ifstream file(path);
        if (!file.is_open()) return "";
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    inline void CheckShader(const GLuint shader, const char *name) {
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[512];
            glGetShaderInfoLog(shader, 512, nullptr, log);
            printf("Shader compile error (%s):\n%s\n", name, log);
        }
    }

    inline void CheckProgram(const GLuint program) {
        GLint success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char log[512];
            glGetProgramInfoLog(program, 512, nullptr, log);
            printf("Program link error:\n%s\n", log);
        }
    }

    // Helper function to load texture from file
inline unsigned int TextureFromFile(const char *path, const std::string &directory) {
      std::string filename = std::string(path);
      filename = directory + '/' + filename;

      unsigned int textureID;
      glGenTextures(1, &textureID);

      int w, h, c;
      // On laisse 0 ici pour que stbi nous dise le vrai nombre de canaux du fichier
      unsigned char *data = stbi_load(filename.c_str(), &w, &h, &c, 0);

      if (data) {
        GLenum format;
        if (c == 1)
          format = GL_RED;
        else if (c == 3)
          format = GL_RGB;
        else if (c == 4)
          format = GL_RGBA;
        else
          format = GL_RGB; // Valeur par défaut par sécurité

        glBindTexture(GL_TEXTURE_2D, textureID);

        // --- CORRECTION ICI ---
        // On dit à OpenGL de lire les pixels un par un sans chercher d'alignement sur 4 octets
        // C'est indispensable pour ta texture de 4094px !
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
      } else {
        std::cerr << "Texture failed to load at path: " << filename << std::endl;
        // Pas besoin de stbi_image_free(data) ici car data est NULL
      }

      return textureID;
    }
}

#endif //GPR924_COMPGRAPH_UTILS_H
