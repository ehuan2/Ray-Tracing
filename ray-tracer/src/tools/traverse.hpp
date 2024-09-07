// Termm--Fall 2023
#pragma once

#include "../node/SceneNode.hpp"
#include <functional>
#include <glm/glm.hpp>

// these files here are meant as helper functions to traverse over
// the tree that we have as well as provide helper print functions

// a function that takes in a lambda function f
// and continues to apply it until bool returned is true
SceneNode *apply_to_node(
    std::function<bool(SceneNode*)> f,
    SceneNode *root
);

// prints all the materials of the given subtree SceneNode
void print_material(SceneNode *root);
void print_vec(const glm::vec3 &vec);

