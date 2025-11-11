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
static constexpr int64_t seekNextBalanced(const std::span<Token>& tokens, TokenType open, TokenType close) {
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

/**
 * @brief will search through `tokens` and will find the closing end to the opening block at
 * index 0 of `tokens`. If, While, For, While and Func are considered opening blocks, while
 * end closes all of them
 */
static constexpr int64_t seekNextBlockEnd(const std::span<Token>& tokens) {
    int count = 0;
    int64_t endPos = 0;
    for (const Token& t : tokens) {
        if (
            (t.type == TokenType::Func) || 
            (t.type == TokenType::If) || 
            (t.type == TokenType::While) || 
            (t.type == TokenType::For)
        ) {
            count++;
        } else if (t.type == TokenType::End) {
            count--;
        }

        if (count == 0) {
            return endPos;
        }
        endPos++;
    }
    return -1;
}

/**
 * @brief will seek through every element in `tokens` and return the index inside of tokens
 * where the first instance of `search` is found
 * @returns the index into `tokens` where the next element is, -1 if elem doesnt exist
 */
static constexpr int64_t seekNext(const std::span<Token>& tokens, TokenType search) {
    int64_t endPos = 0;
    for (const Token& t : tokens) {
        if (t.type == search) {
            return endPos;
        }
        endPos++;
    }
    return -1;
}

std::optional<ASTNodePtr> parseFunc(std::span<Token> tokens) {

}

/**
 * @brief parseGlobal is meant to be called on the global scope of a file
 */
std::optional<ASTNodePtr> parseGlobal(std::span<Token> tokens) {

    for (int i = 0; i < tokens.size(); i++) {
        const Token& token = tokens[i];
        if (token.type == TokenType::Func) {
            int64_t end = seekNextBlockEnd(tokens);
            if (end == -1) { return std::nullopt; }
            auto funcTree = parseFunc(tokens.subspan(i, end));
            if (funcTree.has_value())

            i = end; //Advance past this section
        }
    }
}
