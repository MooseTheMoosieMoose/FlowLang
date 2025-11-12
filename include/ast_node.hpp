/*
 __                __                                           
|_| _    .   /\   (_  _ _. _ |_. _  _   |   _  _  _     _  _  _ 
| |(_)\)/.  /--\  __)(_| ||_)|_|| )(_)  |__(_|| )(_)|_|(_|(_)(- 
                          |        _/            _/       _/    

Copyright (c) 2025 Moose Abou-Harb All rights reserved.
This software is licensed under the BSD 3-Clause License, which can be found in the accompanying LICENSE file.
*/

#pragma once

#include "token.hpp"
#include <vector>
#include <memory>
#include <iostream>

namespace fl {

/*======================================================================================================*/
/*                                          ASTNode                                                     */
/*======================================================================================================*/

//Forward declaration
struct ASTNode;

/**
 * @brief a type define for a shared pointer wrapping an ASTNode. This approach will be cleaned
 * up in later iterations so that we can build a more defined ASTTree that has a destructor to clean
 * things up but for now this makes sure that our testing is at least leak free
 */
using ASTNodePtr = std::shared_ptr<ASTNode>;

/**
 * @brief the ASTNode represents one node in our abstract syntax tree, it consists of
 * a token defining the node itself, as well as a vector of pointers to child nodes
 */
struct ASTNode {
public:
    Token body;
    std::vector<ASTNodePtr> children;

    /**
     * @brief a simple logging function that pretty (ish) prints the contents
     * of a node and all its children recursivly
     */
    void log(int depth = 0) {
        if (depth > 0) {
            for (int i = 0; i < depth; i++) {
                std::cout << "-";
            }
            std::cout << "> " << body.text << std::endl;
            for (const auto& c : children) {
                c->log(depth + 1);
            }
        }
    }
};

} //end namespace fl