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
#include <iostream>

namespace fl {

/*======================================================================================================*/
/*                                          ASTNode                                                     */
/*======================================================================================================*/

/**
 * @brief the ASTNode represents one node in our abstract syntax tree, it consists of
 * a token defining the node itself, as well as a vector of pointers to child nodes
 */
struct ASTNode {
public:
    Token body;
    std::vector<ASTNode&> children;

    /**
     * @brief a nice shortcut to add a child to a given node
     */
    constexpr void addChild(ASTNode& newChildRef) noexcept {
        children.push_back(newChildRef);
    }

    /**
     * @brief a simple logging function that pretty (ish) prints the contents
     * of a node and all its children recursivly
     */
    void log(int depth = 0) {
        if (!body.text.isEmpty()) {
            for (int i = 0; i < depth; i++) {
                std::cout << "-";
            }
            std::cout << "> " << body.text << std::endl;
        }
        
        for (auto& c : children) {
            c.log(depth + 1);
        }
    }
};

} //end namespace fl