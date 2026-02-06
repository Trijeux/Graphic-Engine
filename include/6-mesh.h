#ifndef MESH_H
#define MESH_H
#include <GL/glew.h>
#include <SDL3/SDL_video.h>

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "renderer/pipeline.h"

namespace graphics {

    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    struct Texture {
        GLuint id;
        std::string type; // "texture_diffuse" or "texture_specular"
        std::string path;
    };

    class Mesh {
    public:
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture>      textures;

        Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<Texture> &textures) {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;
            setupMesh();
        }

      void SetupInstancingAttributes(GLuint instanceVBO) {
          // Utilise le type de GLEW pour être sûr de la convention d'appel (stdcall sur Windows)
          typedef void (GLAPIENTRY * PFN_AttribDivisor)(GLuint index, GLuint divisor);

          static PFN_AttribDivisor MyAttribDivisor = nullptr;
          static bool searched = false;

           if (!searched) {
             // On cherche sous tous les noms possibles
             MyAttribDivisor = (PFN_AttribDivisor)SDL_GL_GetProcAddress("glVertexAttribDivisor");
             if (!MyAttribDivisor) MyAttribDivisor = (PFN_AttribDivisor)SDL_GL_GetProcAddress("glVertexAttribDivisorARB");
             if (!MyAttribDivisor) MyAttribDivisor = (PFN_AttribDivisor)SDL_GL_GetProcAddress("glVertexAttribDivisorEXT");
             searched = true;
           }

           // SI LE POINTEUR EST TOUJOURS NUL :
           // On ne doit PAS appeler la fonction, sinon -> Crash 0xc0000005
           if (!MyAttribDivisor) {
             // Optionnel : affiche une erreur une seule fois pour savoir pourquoi ça ne marche pas
             return;
           }

          glBindVertexArray(VAO);
          glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

          std::size_t vec4Size = sizeof(glm::vec4);
          for (int i = 0; i < 4; ++i) {
            glEnableVertexAttribArray(3 + i);
            // Configuration du pointeur pour chaque colonne de la matrice
            glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(i * vec4Size));

            // --- UTILISATION DU POINTEUR RÉCUPÉRÉ ---
            MyAttribDivisor(3 + i, 1);
            //glVertexAttribDivisor(3 + i, 1);
          }

          glBindVertexArray(0);
          glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

      void Draw(common::Pipeline &pipeline, int instanceCount) const {
          // 1. Définition du type et récupération du pointeur (statique pour l'efficacité)
          typedef void (GLAPIENTRY * PFN_DrawElementsInstanced)(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei instancecount);
          static PFN_DrawElementsInstanced MyDrawInstanced = nullptr;
          static bool searched = false;

          if (!searched) {
            MyDrawInstanced = (PFN_DrawElementsInstanced)SDL_GL_GetProcAddress("glDrawElementsInstanced");
            if (!MyDrawInstanced) MyDrawInstanced = (PFN_DrawElementsInstanced)SDL_GL_GetProcAddress("glDrawElementsInstancedARB");
            searched = true;
          }

          // 2. Gestion des textures (ton code actuel)
          for(unsigned int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            std::string name = textures[i].type;
            if(name == "texture_diffuse")
              pipeline.SetInt("material_diffuse", i);
            else if(name == "texture_specular")
              pipeline.SetInt("material_specular", i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
          }

          // 3. Dessin sécurisé
          glBindVertexArray(VAO);

          if (MyDrawInstanced) {
            // ON APPELLE LE POINTEUR RÉCUPÉRÉ, PAS LA FONCTION GLOBALE
            MyDrawInstanced(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0, instanceCount);
          } else {
            // Fallback : Si l'instanciation n'est pas supportée, on dessine normalement (optionnel)
            // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
          }

          glBindVertexArray(0);
          glActiveTexture(GL_TEXTURE0);
        }

    private:
        GLuint VAO{}, VBO{}, EBO{};

        void setupMesh() {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

            // Vertex Positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

            // Vertex Normals
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

            // Vertex Texture Coords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

            glBindVertexArray(0);
        }
    };
}
#endif //MESH_H
