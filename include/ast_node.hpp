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
    std::vector<size_t> children;

    /**
     * @brief a nice shortcut to add a child to a given node
     */
    constexpr void addChild(size_t childIndx) noexcept {
        std::cout << "Added child to token: " << body << ", child lives in index: " << childIndx << std::endl;
        children.push_back(childIndx);
    }
};

} //end namespace fl