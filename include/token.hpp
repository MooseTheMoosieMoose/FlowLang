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
    Undefined,
    Operator,
    Number,
    Prepocessor,
    EOL,
    Identifier,
    StringLit,
    OpenParen,
    CloseParen,
    OpenSquare,
    CloseSquare,
    OpenCurly,
    CloseCurly,
    If,
    Then,
    For,
    While,
    Do,
};

std::ostream& operator<<(std::ostream& os, const TokenType token);

struct Token {
    TokenType type;
    Utf8StringView text;
};

std::ostream& operator<<(std::ostream& os, const Token& token);