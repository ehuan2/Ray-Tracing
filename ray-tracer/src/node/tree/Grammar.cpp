#include "Grammar.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>

using namespace std;

// AST ---------------------------------------------------------------------------------------------
void AST::print() {
    if (root.get() == nullptr) {
        cout << "empty AST" << endl;
    }
    cout << "Root node" << endl;
    print_helper(root.get(), 0);
}

void AST::print_helper(Node *n_ptr, int num_tabs) {
    if (n_ptr == nullptr) return;

    for (int i = 0; i < num_tabs; ++i) {
        cout << "\t";
    }

    auto print_vec = [](const glm::vec3 &vec) {
        cout << vec.x << " " << vec.y << " " << vec.z;
    };

    // now we'll print it
    if (n_ptr->trunk.has_value()) {
        cout << "Trunk: ";
        cout << n_ptr->symbol << " ";
        print_vec(n_ptr->trunk->scaling);
        cout << endl;
    }

    if (n_ptr->branching.has_value()) {
        Branching branching = n_ptr->branching.value();
        cout << "Branch: " << branching.left_paren << branching.right_paren << " ";
        print_vec(branching.rotation);
        cout << endl;
    }

    print_helper(n_ptr->left_ptr.get(), num_tabs);
    
    for (int i = 0; i < n_ptr->branches.size(); ++i) {
        Node *node_ptr = n_ptr->branches[i].get();
        print_helper(node_ptr, num_tabs + 1);
    }
}

// Grammar -----------------------------------------------------------------------------------------
void Grammar::parse_rule(
    const std::string &non_terminal, istringstream &istr_stream
) {
    double prob;

    if (!(istr_stream >> prob)) {
        cout << "misformed grammar rule, no probability..." << endl;
        exit(1);
    }

    // now we go through the rest of the grammar rule
    // ensure that there is at least one
    bool expands_one = false;
    std::string symbol;
    std::vector <string> symbols; 

    while (istr_stream >> symbol) {
        expands_one = true;
        symbols.push_back(symbol);
    }

    if (!expands_one) {
        cout << "misformed grammar rule, nothing to expand to" << endl;
    }

    // add the next symbols to our mapping!
    Expansion next_expansion{prob, symbols};
    if (rules.count(non_terminal) == 0) {
        // initialize a vector here for it
        rules[non_terminal] = vector<Expansion>();
    }

    // grab the non-terminal symbol as the key
    rules[non_terminal].push_back(next_expansion);
}

void Grammar::parse_geom_interpretation(
    const std::string &interp, istringstream &istr_stream
) {
    // we have two types of trunks, a trunk or a leaf
    if (interp == "trunk" || interp == "leaf") {
        string symbol;
        double x_scale, y_scale, z_scale;
        istr_stream >> symbol >> x_scale >> y_scale >> z_scale;
        
        // each one should be new (only one geometric interpretation)
        if (trunk_map.count(symbol) != 0) {
            cout << "too many geometric interpretations for " << symbol << endl;
            exit(1);
        }

        trunk_map[symbol] = Trunk {
            (interp == "leaf"),
            glm::vec3(x_scale, y_scale, z_scale)
        };
        return;
    }

    // then it's a branch
    string left_paren, right_paren;
    double x_rot, y_rot, z_rot;
    istr_stream >> left_paren >> right_paren;
    istr_stream >> x_rot >> y_rot >> z_rot;

    // take the left parenthesis as our key
    if (branch_map.count(left_paren) != 0) {
        cout << "too many geometric interpretations for branching ";
        cout << left_paren << " " << right_paren << endl;
        exit(1);
    }

    branch_map[left_paren] = Branching { 
        left_paren,
        right_paren,
        glm::vec3(x_rot, y_rot, z_rot)
    };
}

Grammar::Grammar(const std::string &fname) {
    // now we open the file and do fun stuff :)
    ifstream ifs( fname.c_str() );
    if (!ifs.good()) {
        cout << "Improper file name provided for tree grammar" << endl;
        exit(1);
    }

    std::string grammar_rule;

    bool is_start = true;
    while( getline(ifs, grammar_rule) ) {
        // skip whitespace
        if (grammar_rule == "") continue;

        if (is_start) {
            // set the start symbol
            start_symbol = grammar_rule;
            cur_grammar.push_back(start_symbol);
            is_start = false;
            continue;
        }

        // now we check whether it's a rule or an interpretation 
        std::string start;

        // now we define the grammars
        std::istringstream istr_stream (grammar_rule);

        if (!(istr_stream >> start)) {
            cout << "misformed interpretation or grammar rule, no start" << endl;
            exit(1);
        }

        if (start == "branch" || start == "trunk" || start == "leaf") {
            parse_geom_interpretation(start, istr_stream);
            continue;
        }

        parse_rule(start, istr_stream);
    }

    //print();
}

// this grammar prints the actual rules that we have
void Grammar::print() {
    // here we'll print our grammar:
    cout << "--Starting Symbol " << start_symbol << endl;
    cout << "Geometric Interpretation: Trunks" << endl;
    for (auto& [key, trunk] : trunk_map) {
        cout << key << ": " << endl;
        cout << trunk.scaling.x << " ";
        cout << trunk.scaling.y << " ";
        cout << trunk.scaling.z << endl;
    }
    cout << "Geometric Interpretation: Branching" << endl;
    for (auto& [key, branches] : branch_map) {
        cout << key << " " << branches.right_paren << ": " << endl;
        cout << branches.rotation.x << " ";
        cout << branches.rotation.y << " ";
        cout << branches.rotation.z << endl;
    }

    cout << "Rules:" << endl;
    for (auto& [key, expansions] : rules) {
        cout << key << ":" << endl;
        for (auto &expansion : expansions) {
            string concat = "";
            for (auto symbol : expansion.symbols) {
                concat += symbol + " ";
            }
            cout << "\tProb: " << expansion.prob << ", Rule: " << concat << endl;
        }
    }
}

// this function prints the current grammar that we have
void Grammar::print_grammar() {
    cout << "Current Grammar: ";
    for (auto &str : cur_grammar) {
        cout << str << " ";
    }
    cout << endl;
}

// now we create functions that will expand on the grammar
// and be able to query it back for us
void Grammar::expand() {
    // for every string in get_grammar, we'll look at its expansion rule
    // (if it exists) and then take a stochastic approach
    // to see which expansion rule to go for
    vector <std::string> next_grammar;

    // we'll use a uniform distribution over 0 and 1
    mt19937 rng;
    rng.seed(13285);
    std::uniform_real_distribution<double> unif_dist(0, 1);

    for (auto &str : cur_grammar) {
        // it's a terminal symbol, add it and don't expand
        if (rules.count(str) == 0) {
            next_grammar.push_back(str);
            continue;
        }

        // to do the right branching, we'll have a uniform distribution
        // over which, if it's within the range of [lower_prob, lower_prob + prob)
        // then we branch over that and stop looking
        double lower_bound = 0.0;
        double rng_prob = unif_dist(rng);
        
        for (auto &expansion : rules[str]) {
            // check if we have the random probability within our range
            // if yes, then expand on it
            if (lower_bound <= rng_prob &&
                rng_prob < lower_bound + expansion.prob) {
                for (auto &symbol : expansion.symbols) {
                    next_grammar.push_back(symbol);
                }
                break;
            }
            lower_bound += expansion.prob;
        }
    }

    cur_grammar = next_grammar;
}

AST Grammar::getAST() {
    // iterate over the current grammar that we have
    AST ast;
    if (cur_grammar.size() == 0) return ast;

    // let's create a root node
    ast.root = make_unique<AST::Node>();
    
    int counter = 0;
    std::string stop_condition = "";
    buildSubtree(counter, ast.root.get(), stop_condition);
    return ast;
}

// a reference to idx that will increment the index along
// parent pointer, and the stopping condition of the right parenthesis
void Grammar::buildSubtree(int &idx, AST::Node *parent, const std::string &right_paren) {
    // base case: we reached the end! stop
    if (idx >= cur_grammar.size()) return;

    string symbol = cur_grammar[idx];

    // another base case: stop if the grammar's reached the right parenthesis match
    if (symbol == right_paren) {
        ++idx; // increment the counter, skip over the right parenthesis
        return;
    }

    // now, we check if it's a branching
    // if it's a branching, we build the subtree and add it as a right child
    if (branch_map.count(symbol) != 0) {
        // first check that it exists
        if (branch_map.count(symbol) == 0) {
            cout << "error, no geometric interpretation for " << symbol << endl;
            exit(1);
        }
 
        // don't forget to increment the pointer
        ++idx;

        // add to the branches, a node
        parent->branches.push_back(make_unique<AST::Node>());

        AST::Node *output = parent->branches[parent->branches.size() - 1].get();
        output->branching = branch_map[symbol];
        output->symbol = symbol;
        output->parent_ptr = parent;

        buildSubtree(idx, output, output->branching->right_paren);
        // re recurse to get all the children
        buildSubtree(idx, parent, right_paren);
        return;
    }

    // add the next thing as a left pointer, a continuation
    // check that it has a meaning (has to be a non branching)
    if (trunk_map.count(symbol) == 0) {
        cout << "error, no geometric interpretation for " << symbol << endl;
        exit(1);
    }

    parent->left_ptr = make_unique<AST::Node>();
    AST::Node *output = parent->left_ptr.get();
    output->trunk = trunk_map[symbol];
    output->symbol = symbol;
    output->parent_ptr = parent;

    ++idx;
    // recurse onto the next one
    buildSubtree(idx, output, right_paren);
}

bool Grammar::is_empty() {
    return cur_grammar.size() == 0;
}

