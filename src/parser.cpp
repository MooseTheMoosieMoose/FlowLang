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

/**
 * @todo REMOVE AFTER TESTING
 */
#include <iostream> 

namespace fl {

/*======================================================================================================*/
/*                                     General Parser Tools                                             */
/*======================================================================================================*/

size_t FlowParser::addAstNode(const Token* newNodeBody, int64_t newParent) {
    if (newNodeBody != nullptr) {
        ast.emplace_back(ASTNode{.body = *newNodeBody});
    } else {
        ast.emplace_back(ASTNode{.body = Token{}});
    }

    const size_t newChildIndx = ast.size() - 1;

    if (newParent != -1) {
        ast[newParent].children.push_back(newChildIndx);
    }
    
    return newChildIndx; //Get the last element index
}

/*======================================================================================================*/
/*                                        Seekers                                                       */
/*======================================================================================================*/

/**
 * @brief will seek through a span to find the next balanced token based on an open and close
 * Balanced token refers to some set with a defined way to open and close, i.e every func must
 * match with an end, and every { must match with a }
 * @note this should have an open token at span[0] and will return the index directly before the matching end
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
 * @note this is NOT inclusive of the matching end token to the one in index 0!
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

ParseResult FlowParser::parseFunc(const Span<Token>& tokens) {
    size_t tokenCount = tokens.size();
    //Tokens contains the entire contents of a function body, so the node we want to return is at the top level,
    //a func token, which is at 0, which we know exists becuase it came down from the top level parser
    size_t funcHead = addAstNode(&tokens[0]);

    //The first child must then be the function name, which is the next token
    if ((tokenCount < 2) || (tokens[1].type != TokenType::Identifier)) {
        return ParseResult::Err("Function declaration is missing a name!"_utf8);
    }
    size_t funcName = addAstNode(&tokens[1], funcHead);

    //Next we should expect a parenthesis
    if ((tokenCount < 3) || (tokens[2].type != TokenType::OpenParen)) {
        return ParseResult::Err("Function declaration expects a parenthetical parameter list, did you forget a `(`?"_utf8);
    }

    //Now we should expect to see an arg list until we hit a close paren, lets look for that
    int64_t closeParen = seekNextBalanced(tokens.subspan(2), TokenType::OpenParen, TokenType::CloseParen);
    if (closeParen == -1) {
        return ParseResult::Err("Function declaration parameter list is missing a closing parenthesis!"_utf8);
    }
    closeParen += 2; //Adjust to make it relative to the whole expression

    //Before we get our parameters, lets capture our return type
    bool retCheck = (tokenCount < closeParen + 2) || 
                    (tokens[closeParen + 1].type != TokenType::Returns);
    if (retCheck) {
        return ParseResult::Err("Function declaration is missing a return type!"_utf8);
    }
    size_t retType = addAstNode(&tokens[closeParen + 2], funcHead);

    //Now we can walk through the pairs of [type, identifier, comma?] in the parameter list and add them
    int curTokenIndx = 3;
    while (curTokenIndx < closeParen) {
        if (tokens[curTokenIndx].type != TokenType::Identifier) {
            return ParseResult::Err("Expected to see a parameter type!"_utf8);
        }
        addAstNode(&tokens[curTokenIndx], funcHead);
        curTokenIndx++;

        if (tokens[curTokenIndx].type != TokenType::Identifier) {
            return ParseResult::Err("Expected to see a parameter name!"_utf8);
        }
        addAstNode(&tokens[curTokenIndx], funcHead);
        curTokenIndx++;

        if (curTokenIndx != closeParen) {
            if (tokens[curTokenIndx].type != TokenType::Comma) {
                return ParseResult::Err("Expected to see a comma!"_utf8);
            }
            curTokenIndx++;
        }
    }

    //Now "all" thats left is the actual body of the function, which consists of expressions and blocks
    // int curTokenIndx = closeParen + 4;
    // while (curTokenIndx < tokenCount) {
    //     auto firstToken = 
    // }
    
    //Everything was good, return the new tree, and add this node to our top level registry of functions
    functionDecs.insert({ast[funcName].body.text, funcHead});
    return ParseResult::Ok(funcHead);
}

ParseResult FlowParser::parseGlobal(const Span<Token>& tokens) {
    //Create an initial empty head to put all top level compilation frags into
    size_t globalHead = addAstNode();

    int curTokenIndx = 0;
    while (curTokenIndx < tokens.size()) {
        if (tokens[curTokenIndx].type == TokenType::Func) {
            //Seek the matching end to this function block
            int64_t end = seekNextBlockEnd(tokens.subspan(curTokenIndx));

            //Err if not found
            if (end == -1) {
                return ParseResult::Err("Function block opened but improperly closed, are you missing an end token?"_utf8);
            }

            //Otherwise, parse the function block
            auto funcTree = parseFunc(tokens.subspan(curTokenIndx, curTokenIndx + end));

            //Check to ensure that the function parsing went good
            if (!funcTree.isOk()) {
                return ParseResult::Err(funcTree.errValue());
            }

            //Everything is valid, we have a func head node ready to get pushed up
            ast[globalHead].addChild(funcTree.okValue());

            //Advance to the next token type
            curTokenIndx = curTokenIndx + end + 1; //Advance past this section
            std::cout << "Parsed one block!" << std::endl;
        }
        else {
            std::cout << "Illegal top level token: " << tokens[curTokenIndx] << std::endl;
            curTokenIndx++;
        }
    }

    //All parsing is aOk, we can return
    return ParseResult::Ok(globalHead);
}

} //end namespace fl