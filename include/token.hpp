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

enum class TokenType {
    //The undefined error type
    Undefined,

    //General Operators
    Operator,

    //Reserved Words
    Func,
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

std::ostream& operator<<(std::ostream& os, const TokenType token);

struct Token {
    TokenType type;
    Utf8StringView text;
};

std::ostream& operator<<(std::ostream& os, const Token& token);