#include "traverse.hpp"
#include "../node/GeometryNode.hpp"
#include <string>
#include <iostream>
#include <queue>
#include <unordered_set>

using namespace std;

// applies f to every node until we get true from f
SceneNode *apply_to_node(
    std::function<bool(SceneNode *)> f,
    SceneNode *root
) {
    if (root == nullptr) return nullptr;

    unordered_set<SceneNode *> visited;
    queue<SceneNode *> q;

    q.push(root);
    visited.insert(root);

    while (!q.empty()) {
        // get and remove the first child
        SceneNode *next = q.front();
        q.pop();

        if (next == nullptr) continue;
        if (f(next)) return next;

        // haven't found it yet, go over all the children
        for (auto &child : next->children) {
            if (visited.count(child) == 0) {
                q.push(child);
                visited.insert(child);
            }
        }
    }
    
    return nullptr;
}

// prints all possible materials
void print_material(SceneNode *root) {
    auto print_node_mat = [](SceneNode *node) {
        if (node->m_nodeType == NodeType::GeometryNode) {
            GeometryNode *geom_node = static_cast<GeometryNode *>(node);
            geom_node->printMaterial();
        }
        return false;
    };

    apply_to_node(print_node_mat, root);
}

void print_vec(const glm::vec3 &vec) {
    std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
}
