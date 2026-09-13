/**
 * @file    node.h
 * @brief   Defines the base Node class for the scene graph.
 *
 * This file contains the definition of the fundamental Node class, which represents
 * an entity in the 3D world (mesh, light, camera, group) and forms the hierarchical
 * structure of the scene graph.
 */

#pragma once
#include "lib.h"
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace eng {

    /**
     * @class Node
     * @brief The basic building block of the scene graph hierarchy.
     *
     * A Node represents a generic entity (object, light, group) in the scene
     * and holds a local transformation matrix and references to its children.
     * The hierarchy allows relative positioning and transformation inheritance.
     *
     */
    class ENG_API Node {
    protected:
        /** @brief The unique or descriptive name of the node. */
        std::string name;

        /** @brief The local 4x4 transformation matrix (translation, rotation, scale). */
        glm::mat4 baseMatrix;

        /** @brief List of child nodes attached to this node. */
        std::vector<std::shared_ptr<Node>> children;

        /** @brief Pointer to the parent node in the hierarchy (raw pointer to avoid cyclic references). */
        Node* parent = nullptr;

    public:
        /** @brief Default constructor. */
        Node();

        /** @brief Virtual destructor to ensure correct cleanup of derived classes. */
        virtual ~Node() = default;

        // --- Name Management ---

        /**
         * @brief Sets the name of the node.
         * @param n The new name string.
         */
        virtual void set_name(const std::string& n);

        /**
         * @brief Gets the name of the node.
         * @return The name string.
         */
        virtual std::string get_name() const;

        // --- Transformation Management ---

        /**
         * @brief Sets the local transformation matrix (Model Matrix) of the node.
         * * This matrix defines the node's position, rotation, and scale relative to its parent.
         * @param m The new 4x4 transformation matrix.
         */
        virtual void set_base_matrix(const glm::mat4& m);

        /**
         * @brief Gets the local transformation matrix of the node.
         * @return The 4x4 matrix.
         */
        glm::mat4 get_base_matrix() const;

        // --- Hierarchy Management ---

        /**
         * @brief Adds a child node to this node.
         * * The child will inherit the transformations of this node.
         * @param child Shared pointer to the new child node.
         */
        virtual void add_child(std::shared_ptr<Node> child);

        /**
         * @brief Gets a mutable reference to the children vector.
         * @return std::vector<std::shared_ptr<Node>>& Reference to the list of children.
         */
        std::vector<std::shared_ptr<Node>>& get_children();
    };

}