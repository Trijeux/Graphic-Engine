#ifndef GPR924_COMPGRAPH_CUBE_H
#define GPR924_COMPGRAPH_CUBE_H
#include <GL/glew.h>
#include <glm/vec3.hpp>

#include "engine/renderer.h"
#include "engine/system.h"
#include "renderer/pipeline.h"

namespace graphics
{
	class Cube final : public common::SystemInterface, public common::DrawInterface
	{
		glm::vec3 cubePositions[7] = {
			glm::vec3(0.0f, 0.0f, -10.0f),
			glm::vec3(-2.0f, 4.0f, -10.0f),
			glm::vec3(2.0f, 4.0f, -10.0f),
			glm::vec3(-2.0f, -4.0f, -10.0f),
			glm::vec3(2.0f, -4.0f, -10.0f),
			glm::vec3(4.0f, 0.0f, -10.0f),
			glm::vec3(-4.0f, 0.0f, -10.0f),
		};

		std::string path_texture = "data/textures/";
		float  time = 0.f;
		common::Pipeline pipeline;
		GLuint textures[7] =
		{
			0,0,0,0,0,0,0
		};
		GLuint vao = 0;
		GLuint vbo = 0;
		GLuint ebo = 0;

		std::string path_files[7] =
		{
			path_texture + "boule 1.jpg",
			path_texture + "boule 2.jpg",
			path_texture + "boule 3.jpg",
			path_texture + "boule 4.jpg",
			path_texture + "boule 5.jpg",
			path_texture + "boule 6.jpg",
			path_texture + "boule 7.jpg",
		};

	public:
		void Begin() override;
		void End() override;
		void Update(float dt) override;
		void FixedUpdate() override;
		void Draw() override;
	};
}
#endif //GPR924_COMPGRAPH_CUBE_H
