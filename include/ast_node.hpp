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

struct ASTNode;

using ASTNodePtr = std::shared_ptr<ASTNode>;

struct ASTNode {
public:
    Token body;
    std::vector<ASTNodePtr> children;

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