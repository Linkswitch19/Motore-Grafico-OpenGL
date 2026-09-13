#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace eng {
	struct OrbData
	{
		glm::vec3 position;
		int lightID;
	};

	struct ShadowRequest {
		std::string nodeName;
		glm::vec3 lightPosition;
	};

	class LightManager {
		private:
			std::vector<OrbData> orbsList;
			std::vector<ShadowRequest> shadowList;
			int activeLightCount = 0;
		public:
			void createOrb(float x, float y, float z);
			void addLight(float x, float y, float z, float r, float g, float b, float a);
			void addShadow(const std::string& nodeName, float x, float y, float z);
			void renderOrbs(float rotationAngleY, float totalTime);

			int getOrbCount() const;
			const std::vector<ShadowRequest>& getShadowRequests() const;

	};
}
