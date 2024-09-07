// Termm--Fall 2023
#pragma once

#include "../GeometryNode.hpp"
#include "Grammar.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>

// an l system tree is a type of GeometryNode - even though sometimes it might not hold any
class LSystemTree : public SceneNode {
public:
  // l system tree generation
  // exactly like FractalMountain except we also decide which grammar we want to use
  LSystemTree(
    const std::string &name,
    const std::string &grammar_fname,
    uint depth,
    GeometryNode *trunk,
    GeometryNode *leaf
  );

  // we need to first initialize
  void init();
private:

  // a function that initializes the grammar for us, and expands on it for us
  void init_grammar();

  // a function that takes the AST we built and generates the actual
  // structure we want for the scene nodes
  void buildLSystemTree(SceneNode *scene, AST::Node *node);

  // tells us which grammar to expand from
  std::string grammar_fname;

  // tells us how tall to build the tree up to
  uint depth;

  Grammar grammar;

  int new_id;

  GeometryNode *trunk;
  GeometryNode *leaf;
};

