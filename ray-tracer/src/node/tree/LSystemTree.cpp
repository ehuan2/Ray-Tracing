#include "LSystemTree.hpp"
#include "../../tools/directives.hpp"
#include <iostream>
#include <random>
#include <string>

using namespace std;

LSystemTree::LSystemTree(
    const std::string &name,
    const std::string &grammar_fname,
    uint depth,
    GeometryNode *trunk,
    GeometryNode *leaf
):
    SceneNode(name),
    depth(depth),
    grammar(grammar_fname), new_id(0), 
    trunk(trunk), leaf(leaf)
{
	m_nodeType = NodeType::LSystemTree;
    init();
}

void LSystemTree::init() {
    // this function will take the grammar that we have and create a tree structure
    // the tree structure is made up of geometrynodes that we'll use for our purposes
    if (!leaf || !trunk || !leaf->m_primitive || !trunk->m_primitive) {
        cout << "there requires a primitive to be given" << endl;
        exit(1);
    }

    for (uint i = 0; i < depth; ++i) {
        grammar.expand();
    }
#ifdef PRINT_TREE_GRAMMAR
    grammar.print_grammar();
#endif

    // if there is no grammar, there is no tree
    if (grammar.is_empty()) {
        cout << "Grammar produced no tree" << endl;
        return;
    }

    // now we need to take this grammar and actually initialize a tree with it
    auto grammar_ast = grammar.getAST();

    // now, given our ast, we need to iterate through it
    // and build the scene nodes that we want
    buildLSystemTree(this, grammar_ast.root.get());

    // finally, we need to initialize the bounding box (at some point)
}

// a function that builds the LSystemTree for us
void LSystemTree::buildLSystemTree(SceneNode *scene, AST::Node *node) {
    // recursively build our nodes!
    // check our children, and build them up

    // TODO: for every new node, we want to also scale it down (force its children to scale down)

    if (node->left_ptr.get()) {
        // add an actual geometry node to scene
        // simply add it to the end of the parent (move it by y = 1?)

        // I need to actually create a scene node and a geometry node!
        // such that only the geometry node gets scaled
        SceneNode *new_scene = new SceneNode(
            m_name + node->left_ptr->symbol + std::to_string(new_id) + "_scene"
        );
        // we need to rotate, translate, translate for this
        double scale_value = node->left_ptr->trunk->scaling.y;
        new_scene->translate(
            glm::vec3(0, scale_value, 0)
        );

        // EDIT: Actually need three of them, I need to separate the translation
        // as well!
        SceneNode *translate = new SceneNode(
            m_name + node->left_ptr->symbol + std::to_string(new_id) + "_translate"
        );

        translate->translate(
            glm::vec3(0, scale_value, 0)
        );

        bool is_leaf = node->left_ptr->trunk->is_leaf;

        SceneNode *geom_node = new SceneNode(
            m_name + node->left_ptr->symbol + std::to_string(new_id)
        );
        geom_node->scale( node->left_ptr->trunk->scaling ); // scale it down appropriately
        geom_node->add_child(is_leaf ? leaf : trunk); // add the child primitive that we want

        new_id += 1;

        scene->add_child(new_scene);
        new_scene->add_child(geom_node);
        new_scene->add_child(translate);
        buildLSystemTree( translate, node->left_ptr.get() );
    }

    // iterate over the right children, and add their scene nodes
    for (int i = 0; i < node->branches.size(); ++i) {
        AST::Node *branch_node = node->branches[i].get();
        GeometryNode *branch = new GeometryNode(
            m_name + branch_node->symbol + std::to_string(new_id),
            nullptr, nullptr
        );

        branch->rotate('X', branch_node->branching->rotation.x);
        branch->rotate('Y', branch_node->branching->rotation.y);
        branch->rotate('Z', branch_node->branching->rotation.z);

        new_id += 1;

        scene->add_child(branch);
        buildLSystemTree( branch, branch_node );
    }
}

// a helper function that will implement a bounding box

