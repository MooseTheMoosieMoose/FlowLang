/*
 __                __                                           
|_| _    .   /\   (_  _ _. _ |_. _  _   |   _  _  _     _  _  _ 
| |(_)\)/.  /--\  __)(_| ||_)|_|| )(_)  |__(_|| )(_)|_|(_|(_)(- 
                          |        _/            _/       _/    

Copyright (c) 2025 Moose Abou-Harb All rights reserved.
This software is licensed under the BSD 3-Clause License, which can be found in the accompanying LICENSE file.
*/

#include "parser.hpp"
#include "fl_util.hpp"
#include <stdint.h>
#include <iostream>

namespace fl {

/*======================================================================================================*/
/*                                        Seekers                                                       */
/*======================================================================================================*/

/**
 * @brief will seek through a span to find the next balanced token based on an open and close
 * Balanced token refers to some set with a defined way to open and close, i.e every func must
 * match with an end, and every { must match with a }
 * @returns an int64_t with the offset from the begining of `tokens` where the matching token is, -1 if err
 */
static constexpr int64_t seekNextBalanced(const Span<Token>& tokens, TokenType open, TokenType close) {
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
 * @returns an int64_t representing the position in the given span to search for
 */
static constexpr int64_t seekNextBlockEnd(const Span<Token>& tokens) {
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
static constexpr int64_t seekNext(const Span<Token>& tokens, TokenType search) {
    int64_t endPos = 0;
    for (const Token& t : tokens) {
        if (t.type == search) {
            return endPos;
        }
        endPos++;
    }
    return -1;
}

/*======================================================================================================*/
/*                                          Parsers                                                     */
/*======================================================================================================*/

Result<ASTNodePtr, Utf8String> parseFunc(const Span<Token>& tokens) {
    return Result<ASTNodePtr, Utf8String>::Ok(ASTNodePtr());
}

Result<ASTNodePtr, Utf8String> parseGlobal(const Span<Token>& tokens) {

    for (int i = 0; i < tokens.size(); i++) {
        const Token& token = tokens[i];
        if (token.type == TokenType::Func) {
            //Seek the matching end to this function block
            int64_t end = seekNextBlockEnd(tokens);

            //Err if not found
            if (end == -1) {
                return Result<ASTNodePtr, Utf8String>::Err("Function block opened but improperly closed, are you missing an end token?"_utf8);
            }

            //Otherwise, parse the function block
            auto funcTree = parseFunc(tokens.subspan(i, end));

            //Check to ensure that the function parsing went good
            if (!funcTree.isOk()) {
                return Result<ASTNodePtr, Utf8String>::Err("Function body failed to parse!"_utf8);
            }

            //Advance to the next token type
            i = end; //Advance past this section
        }
    }

    return Result<ASTNodePtr, Utf8String>::Ok(ASTNodePtr());
}

} //end namespace fl