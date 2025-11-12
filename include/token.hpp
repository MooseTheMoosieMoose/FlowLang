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

namespace fl {

/*======================================================================================================*/
/*                                       Token Type                                                     */
/*======================================================================================================*/

/**
 * @brief this enum class covers all the different type options for a token
 */
enum class TokenType {
    //The undefined error type
    Undefined,

    //General Operators
    Operator,

    //Reserved Words
    Func,
    End,
    Returns,
    Let,
    Import,
    If,
    Then,
    For,
    While,
    Do,

    //All other words
    Identifier,

    //Literals
    Number,
    StringLit,

    //Special Symbols
    Prepocessor,
    EOL,
    
    //Brackets and braces
    OpenParen,
    CloseParen,
    OpenSquare,
    CloseSquare,
    OpenCurly,
    CloseCurly
    
};

/**
 * @brief an override on the output stream to make debugging easier for tokens
 */
std::ostream& operator<<(std::ostream& os, const TokenType token);

/*======================================================================================================*/
/*                                           Token                                                      */
/*======================================================================================================*/

/**
 * @brief a Token is a simple collection of text and type that defines a single atomic
 * unit of lexical information. It should be noted that these tokens work over views,
 * not established strings, which means that if the underlying data changes there is
 * no garuntee to their validity
 * @todo see if a default empty constructor would improve anything, and see about going through
 * and making these owning strings after tokenization is complete 
 */
struct Token {
    TokenType type;
    Utf8StringView text;
};

/**
 * @brief 
 */
std::ostream& operator<<(std::ostream& os, const Token& token);

} //end namespace fl