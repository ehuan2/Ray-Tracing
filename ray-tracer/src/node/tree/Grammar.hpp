// Termm--Fall 2023
#pragma once
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>

// a structure that describes how we want to branch
struct Branching {
    std::string left_paren;
    std::string right_paren;
    glm::vec3 rotation;
};

struct Trunk {
    bool is_leaf;
    glm::vec3 scaling;
};

// an abstract syntax tree that is generated from a grammar
// and will be easier to parse through and generate the final graphics
// modelling that we want to
class AST {
public:
    // we also might actually care about memory, so in this case we'll
    // use unique pointers (since we want to delete AST almost right away)
    struct Node {
        // in this case, left means we don't deviate away
        std::unique_ptr<Node> left_ptr = nullptr;
        // in this case, right means we deviate away according to branching
        std::vector<std::unique_ptr<Node>> branches;
        Node *parent_ptr = nullptr;
        
        // we set the branching in the current, i.e. it describes
        // how right_ptr will branch
        std::string symbol;
        std::optional<Trunk> trunk; // every node except root should have this
        std::optional<Branching> branching;
    };

    // we'll always have the root node be empty s.t. we can branch right away
    std::unique_ptr<Node> root;
    void print();
private:
    void print_helper(Node *n_ptr, int num_tabs);
};

// a grammar class to use to expand on a grammar
// and eventually generate an AST that we will use
class Grammar {
public:
    Grammar(const std::string &fname);
    // expands on the existing grammar that we have
    void expand();
    AST getAST();
    void print_grammar();
    bool is_empty();
private:
    // a structure that holds the end part of a rule
    struct Expansion {
        double prob;
        std::vector<std::string> symbols;
    };
    std::vector<std::string> cur_grammar;
    std::string start_symbol;
    std::unordered_map<std::string, std::vector<Expansion>> rules;
    std::unordered_map<std::string, Trunk> trunk_map;
    std::unordered_map<std::string, Branching> branch_map;

    // a helper function that parses a line of rules
    void parse_rule(
        const std::string &non_terminal,
        std::istringstream &istr_stream
    );

    void parse_geom_interpretation(
        const std::string &interp,
        std::istringstream &istr_stream
    );

    void print();

    void buildSubtree(int &idx, AST::Node *parent, const std::string &right_paren);
};

