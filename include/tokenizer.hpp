/*
 __                __                                           
|_| _    .   /\   (_  _ _. _ |_. _  _   |   _  _  _     _  _  _ 
| |(_)\)/.  /--\  __)(_| ||_)|_|| )(_)  |__(_|| )(_)|_|(_|(_)(- 
                          |        _/            _/       _/    

Copyright (c) 2025 Moose Abou-Harb All rights reserved.
This software is licensed under the BSD 3-Clause License, which can be found in the accompanying LICENSE file.
*/

#pragma once

#include "utf8string.hpp"
#include "token.hpp"
#include <vector>

/*======================================================================================================*/
/*                                       Tokenizer                                                      */
/*======================================================================================================*/

/**
 * @brief the tokenizer tokenizes using a state machine approach. This is currently implemented
 * as behavior within an objects constructor becuase originally I had a different idea of how to
 * handle errors and control flow for the pattern, however I plan to change this to a function later
 * @todo check for error handling, and refactor so that we dont need a distinct object type for this
 */
class Tokenizer {
public:
    Tokenizer(const Utf8String& text);
    const std::vector<Token>& getTokens() const;
private:
    std::vector<Token> tokens; //Actual tokens
};