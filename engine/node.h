#pragma once
#include "lib.h"
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
namespace eng {

    class ENG_API Node {
    protected:
        std::string name;
        glm::mat4 baseMatrix;
        std::vector<std::shared_ptr<Node>> children;
        Node* parent = nullptr;

    public:
        Node();
        virtual ~Node() = default;

        virtual void set_name(const std::string& n);
        virtual std::string get_name() const;

        virtual void set_base_matrix(const glm::mat4& m);
        glm::mat4 get_base_matrix() const;

        virtual void add_child(std::shared_ptr<Node> child);

         std::vector<std::shared_ptr<Node>>& get_children() ;
    };

}


