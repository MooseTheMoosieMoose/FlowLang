/*
 __                __                                           
|_| _    .   /\   (_  _ _. _ |_. _  _   |   _  _  _     _  _  _ 
| |(_)\)/.  /--\  __)(_| ||_)|_|| )(_)  |__(_|| )(_)|_|(_|(_)(- 
                          |        _/            _/       _/    

Copyright (c) 2025 Moose Abou-Harb All rights reserved.
This software is licensed under the BSD 3-Clause License, which can be found in the accompanying LICENSE file.
*/

#include "parser.hpp"
#include <stdint.h>
#include <iostream>

/**
 * @brief will seek through a span to find the next balanced token based on an open and close
 * Balanced token refers to some set with a defined way to open and close, i.e every func must
 * match with an end, and every { must match with a }
 * @returns an int64_t with the offset from the begining of `tokens` where the matching token is, -1 if err
 */
constexpr int64_t seekNextBalanced(const std::span<Token>& tokens, TokenType open, TokenType close) {
    int count = 0;
    int64_t endPos = 0;
    for (const Token& t : tokens) {
        if (t.type == open) {
            count++;
        } else if (t.type == close) {
            count--;
        }

        if (count == 0) {
            return endPos;
        }
        endPos++;
    }
    return -1;
}

ASTNodePtr parseTokens(std::span<Token> tokens) {
    int64_t endPos = seekNextBalanced(tokens, TokenType::OpenParen, TokenType::CloseParen);
    std::cout << "End at: " << endPos << std::endl;
    return std::make_shared<ASTNode>();
}