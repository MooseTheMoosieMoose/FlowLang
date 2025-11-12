/*
 __                __                                           
|_| _    .   /\   (_  _ _. _ |_. _  _   |   _  _  _     _  _  _ 
| |(_)\)/.  /--\  __)(_| ||_)|_|| )(_)  |__(_|| )(_)|_|(_|(_)(- 
                          |        _/            _/       _/    

Copyright (c) 2025 Moose Abou-Harb All rights reserved.
This software is licensed under the BSD 3-Clause License, which can be found in the accompanying LICENSE file.
*/

#include "token.hpp"

namespace fl {

/*======================================================================================================*/
/*                                           Token                                                      */
/*======================================================================================================*/

/**
 * @brief a quick and dirty temporary override for the print stream
 * to make debugging with tokens easier
 */
std::ostream& operator<<(std::ostream& os, const TokenType token) {
    switch (token) {
        case TokenType::Undefined : { os << "Undefined"; return os; }
        case TokenType::Number : { os << "Number"; return os; }
        case TokenType::Operator : { os << "Operator"; return os; }
        case TokenType::EOL : { os << "End of Line"; return os; }
        case TokenType::Prepocessor: { os << "Preprocessor"; return os; }
        case TokenType::Identifier: { os << "Identifier"; return os; }
        case TokenType::StringLit: { os << "String Literal"; return os; }
        case TokenType::OpenParen: { os << "Open Parenthesis"; return os; }
        case TokenType::CloseParen: { os << "Close Parenthesis"; return os; }
        case TokenType::OpenSquare: { os << "Open Square Bracket"; return os; }
        case TokenType::CloseSquare: { os << "Close Parenthesis"; return os; }
        case TokenType::OpenCurly: { os << "Open Curly Bracket"; return os; }
        case TokenType::CloseCurly: { os << "Close Curly Bracket"; return os; }
        default: { os << "Keyword"; return os; }
    }
}

/*======================================================================================================*/
/*                                       Token Type                                                     */
/*======================================================================================================*/

/**
 * @brief a quick and dirty temporary override for the print stream
 * to make debugging with tokens easier
 */
std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Token: [" << token.type << "]";
    if (token.type != TokenType::EOL) {
        os << " With text: [" << token.text << "]";
    }
    return os;
}

} //end namespace fl