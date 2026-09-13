#pragma once
#include <memory>
#include <string>
#include "node.h"
#include "mesh.h"


namespace eng {

	class SceneManager
	{

	private:
		std::shared_ptr<Node> sceneRoot;

		// Funzioni helper private 
		std::shared_ptr<Node> findNodeRecursive(std::shared_ptr<Node> current, const std::string& name);
		std::shared_ptr<Node> findParentOf(std::shared_ptr<Node> current, const std::string& childName);

	public:
		SceneManager() = default;
		~SceneManager() = default;

		bool loadScene(const std::string& filepath);
		std::shared_ptr<Node> getRoot() const;
		std::shared_ptr<Node> getNode(const std::string& name);
		void setParent(const std::string& childName, const std::string& parentName);
		float getMeshHeight(const std::string& nodeName);


	};

}


