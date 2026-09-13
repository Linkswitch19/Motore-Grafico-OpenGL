
#pragma once
#include <vector>
#include <string>
#include "textHUD.h"
namespace eng {
	class HUDManager {
		private:
			std::vector<TextHUD> hudList;
			std::string currentMessage;
		public:
			HUDManager() = default;

			void addText(const TextHUD& textObject);
			void setMessage(const std::string& msg);
			void clearHUD();
			void renderHUD(int fps, int detail, int windowWidth, int windowHeight);

	};
}

