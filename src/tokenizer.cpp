/*
 __                __                                           
|_| _    .   /\   (_  _ _. _ |_. _  _   |   _  _  _     _  _  _ 
| |(_)\)/.  /--\  __)(_| ||_)|_|| )(_)  |__(_|| )(_)|_|(_|(_)(- 
                          |        _/            _/       _/    

Copyright (c) 2025 Moose Abou-Harb All rights reserved.
This software is licensed under the BSD 3-Clause License, which can be found in the accompanying LICENSE file.
*/

#include "tokenizer.hpp"
#include "utf8string.hpp"
#include <map>
#include <algorithm>

namespace fl {

/*======================================================================================================*/
/*                                       Take Macros                                                    */
/*======================================================================================================*/

/**
 * @brief a quick macro that checks if a uChar is part of an operator
 */
static constexpr bool isOperatorChar(uChar c) {
    switch (c.n) {
        case "+"_u.n:
        case "-"_u.n:
        case "/"_u.n:
        case "*"_u.n:
        case ">"_u.n:
        case "<"_u.n:
        case "!"_u.n:
        case "="_u.n:
        case "%"_u.n:
        case "."_u.n: {
            return true;
        }
        default: {
            return false;
        }
    }
}

/**
 * @brief a macro to determine if a uChar is a number
 * @note if the internal representation of a uChar changes, this
 * is not garunteed to remain valid!
 */
static constexpr bool isNumber(uChar c) {
    return ((c.n >= "0"_u.n) && (c.n <= "9"_u.n));
}

/**
 * @brief checks if a given uChar is a whitespace, which is either an ascii space,
 * ascii newline, ascii tab or ascii linefeed
 */
static constexpr bool isWhitespace(uChar c) {
    switch (c.n) {
        case " "_u.n:
        case "\n"_u.n:
        case "\t"_u.n:
        case "\r"_u.n: {
            return true;
        }
        default: {
            return false;
        }
    }
}

/**
 * @brief checks if a given uchar is NOT double quotes
 * @todo explore making this just a lambda
 */
static constexpr bool isntDoubleQuotes(uChar c) {
    return (c != "\""_u);
}

/**
 * @brief checks if a uChar is anything but a comment close sign
 * @todo explore making this just a lambda in the actual tokenizer call
 */
static constexpr bool isntTag(uChar c) {
    return (c != "#"_u);
}

/**
 * @brief a macro to detect if a given uChar is part of the ascii subset of reserved
 * single chars that form their own tokens
 */
static constexpr bool isReservedChar(uChar c) {
    switch (c.n) {
        case "@"_u.n:
        case ";"_u.n:
        case "("_u.n:
        case ")"_u.n:
        case "["_u.n:
        case "]"_u.n: 
        case "{"_u.n:
        case "}"_u.n:
        case "."_u.n:
        case ","_u.n:
        case "\""_u.n:
        {
            return true;
        }
        default: {
            return false;
        }
    }
}

/**
 * @brief a macro that sees if its any of the non-special or non reserved charachters,
 * these charachters are all valid to be part of an identifier
 */
static constexpr bool isIdentifier(uChar c) {
    return (c.writeSize() > 2) ? true : 
                    (!isWhitespace(c) && 
                    !isOperatorChar(c) && 
                    !isNumber(c) &&
                    !isReservedChar(c) &&
                    isntDoubleQuotes(c)) &&
                    isntTag(c);
}

/**
 * @brief a templated macro that is inspired by rust's iter take_while which takes a predicate
 * and takes from an iter. It is used to search through the given input, under a given predicate
 * @returns a size_t with the position of the first charachter for which the predicate is false
 */
template <typename Predicate>
static constexpr size_t countTakeWhile(const Utf8String& text, size_t curPos, Predicate takeFunction) {
    size_t advance = curPos;
    const size_t maxCount = text.getCharCount();
    while (advance < maxCount) {
        bool keepTaking = takeFunction(text[advance]);
        if (keepTaking == false) {
            return advance;
        } else {
            advance++;
        }
    }
    return advance;
}

/*======================================================================================================*/
/*                                       Tokenizer                                                      */
/*======================================================================================================*/

/**
 * @brief tokenizes a given input as a Utf8String. This is a state machine approach that builds
 * views over the original text to minimize copies. It is also implemented to be easy to add
 * single char special charachters, and defined keywords by putting them in a map which is then
 * checked against
 * @todo check error handling
 */
Result<std::vector<Token>, Utf8String> tokenize(const Utf8String& text) {
    std::vector<Token> tokens;

    size_t curPos = 0;
    size_t lastPos = 0;
    size_t lineCount = 1;
    size_t charCount = 1;

    const std::map<uChar, TokenType> singleCharTokenMap = {
        {";"_u, TokenType::EOL}, {"@"_u, TokenType::Prepocessor}, 
        {"("_u, TokenType::OpenParen}, {")"_u, TokenType::CloseParen},
        {"["_u, TokenType::OpenSquare}, {"]"_u, TokenType::CloseSquare},
        {"{"_u, TokenType::OpenCurly}, {";"_u, TokenType::CloseCurly},
        {","_u, TokenType::Comma},
    };

    const std::map<Utf8String, TokenType> keywordMap = {
        {"func"_utf8, TokenType::Func}, {"if"_utf8, TokenType::If},
        {"elif"_utf8, TokenType::Elif}, {"else"_utf8, TokenType::Else},
        {"then"_utf8, TokenType::Then}, {"do"_utf8, TokenType::Do},
        {"while"_utf8, TokenType::While}, {"for"_utf8, TokenType::For},
        {"import"_utf8, TokenType::Import}, {"returns"_utf8, TokenType::Returns},
        {"let"_utf8, TokenType::Let}, {"end"_utf8, TokenType::End}
    };

    const std::map<Utf8String, TokenType> validOperators = {
        {"++"_utf8, TokenType::PostInc}, {"--"_utf8, TokenType::PostDec}, 
        {"."_utf8, TokenType::Period},
        {"!"_utf8, TokenType::LogNot},
        {"*"_utf8, TokenType::Mul}, {"/"_utf8, TokenType::Div}, {"%"_utf8, TokenType::Mod},
        {"+"_utf8, TokenType::Add}, {"-"_utf8, TokenType::Sub},
        {"<"_utf8, TokenType::LessThan}, {"<="_utf8, TokenType::LessEqual},
        {">"_utf8, TokenType::GreaterThan}, {">="_utf8, TokenType::GreaterEqual},
        {"=="_utf8, TokenType::Equals}, {"!="_utf8, TokenType::NotEquals},
        {"="_utf8, TokenType::Assign},
        {"+="_utf8, TokenType::AddAssign}, {"-="_utf8, TokenType::SubAssign},
        {"*="_utf8, TokenType::MulAssign}, {"/="_utf8, TokenType::DivAssign}
    };


    const size_t maxCharCount = text.getCharCount();
    while (curPos < maxCharCount) {
        uChar curChar = text[curPos];
        TokenType newType = TokenType::Undefined;
        if (isWhitespace(curChar)) {
            curPos++;
            charCount++;
            lastPos++;
            if (curChar == "\n"_u) {
                lineCount++;
                charCount = 1;
            }
            continue;
        } else if (curChar == "#"_u) {
            size_t savedPos = curPos; //Utilize this later for erros
            curPos = countTakeWhile(text, curPos + 1, isntTag) + 1;
            if (curPos >= maxCharCount) {
                return Result<std::vector<Token>, Utf8String>::Err("Comment was left unclosed!"_utf8);
            }
            //Even on comments, ensure that chars are advanced
            charCount += (curPos - lastPos);
            lastPos = curPos;
            continue;
        } else if (isOperatorChar(curChar)) {
            curPos = countTakeWhile(text, curPos, isOperatorChar);
            newType = TokenType::Operator;

            //Test to see if operator is valid
            auto testView = Utf8StringView(text, lastPos, curPos);
            for (const auto& pair : validOperators) {
                if (testView == pair.first) {
                    newType = pair.second;
                    break;
                }
            }

            if (newType == TokenType::Operator) {
                return Result<std::vector<Token>, Utf8String>::Err("Illegal Operator!"_utf8);
            }

        } else if (isNumber(curChar)) {

            curPos = countTakeWhile(text, curPos, isNumber);
            //Check for decimal numbers
            if ((text.getCharCount() > (curPos + 1)) && (text[curPos] == "."_u)) {
                curPos++; //Skip past the period;
                curPos = countTakeWhile(text, curPos, isNumber);
            }
            newType = TokenType::Number;

        } else if (curChar == "\""_u) {
            size_t savedPos = curPos; //remmeber this for erros later
            curPos = countTakeWhile(text, curPos + 1, isntDoubleQuotes) + 1;
            if (curPos >= maxCharCount) {
                return Result<std::vector<Token>, Utf8String>::Err("String literal left unclosed!"_utf8);
            }
            newType = TokenType::StringLit;
        } else if (singleCharTokenMap.contains(curChar)) {
            curPos++;
            //Conditionally change identifiers to function calls
            if ((tokens.size() > 0) && (curChar == "("_u) && (tokens.back().type == TokenType::Identifier)) {
                tokens.back().type = TokenType::FuncCall;
            }

            newType = singleCharTokenMap.at(curChar);
        } else {
            curPos = countTakeWhile(text, curPos, isIdentifier);

            //Set the base type to identifer, and create a test view to look over
            //This view is compared against the keyword map to assign it to all of our keywords
            newType = TokenType::Identifier;
            auto testView = Utf8StringView(text, lastPos, curPos);
            for (const auto& pair : keywordMap) {
                if (testView == pair.first) {
                    newType = pair.second;
                    break;
                }
            }
        }

        //Push back our new token
        tokens.push_back(Token{
            .type = newType,
            .text = Utf8StringView(text, lastPos, curPos),
            .lineCount = lineCount,
            .charCount = charCount
        });

        //Advance current charachter by the number of charachters advanced
        charCount += (curPos - lastPos);
        lastPos = curPos;
    }

    //After everything we can return our tokens
    return Result<std::vector<Token>, Utf8String>::Ok(tokens);
}

} //end namespace fl