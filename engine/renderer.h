#pragma once
#include "renderer.h"
#include "mesh.h"
#include "material.h"
#include "light.h"
#include "directional_light.h"
#include "spot_light.h"
#include "point_light.h"
#include "texture.h"
#include "node.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "light_manager.h" 
namespace eng {
	class Renderer {
		private:
			void renderOvoNode(std::shared_ptr<Node> node, glm::mat4 parentMatrix, int currentLOD);
			void renderShadowNode(std::shared_ptr<Node> node, glm::mat4 parentMatrix);
			glm::mat4 computeShadowMatrix(glm::vec4 groundPlane, glm::vec4 lightPos);
			void processLight(std::shared_ptr<eng::Light> light, const glm::mat4& globalMatrix);
			void renderMesh(std::shared_ptr<eng::Mesh> mesh, const glm::mat4& globalMatrix, int currentLOD);
			void setupMaterial(std::shared_ptr<eng::Material> mat);

		public:
			Renderer() = default;
			void renderScene(std::shared_ptr<Node> root, const glm::mat4& viewMatrix, int currentLOD);
			void renderShadows(std::shared_ptr<Node> root, const std::vector<ShadowRequest>& shadows, const glm::mat4& viewMatrix);

	};
}

