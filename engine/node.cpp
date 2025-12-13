#include "node.h"

namespace eng {

    ENG_API Node::Node() : baseMatrix(1.0f) {}

    void ENG_API Node::set_name(const std::string& n) {
        name = n;
    }

    std::string ENG_API Node::get_name() const {
        return name;
    }

    void ENG_API Node::set_base_matrix(const glm::mat4& m) {
        baseMatrix = m;
    }

    glm::mat4 ENG_API Node::get_base_matrix() const {
        return baseMatrix;
    }

    void ENG_API Node::add_child(std::shared_ptr<Node> child) {
        if (child) {
            children.push_back(child);
            child->parent = this;
        }
    }

    const std::vector<std::shared_ptr<Node>>& Node::get_children() const {
        return children;
    }

}
