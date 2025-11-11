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
        case "."_u.n: {
            return true;
        }
        default: {
            return false;
        }
    }
}

static constexpr bool isNumber(uChar c) {
    return ((c.n >= "0"_u.n) && (c.n <= "9"_u.n));
}

static constexpr bool isNumberOrPeriod(uChar c) {
    return (isNumber(c) || c == "."_u);
}

static constexpr bool isWhitespace(uChar c) {
    switch (c.n) {
        case " "_u.n:
        case "\n"_u.n:
        case "\r"_u.n: {
            return true;
        }
        default: {
            return false;
        }
    }
}

static constexpr bool isntDoubleQuotes(uChar c) {
    return (c != "\""_u);
}

static constexpr bool isntTag(uChar c) {
    return (c != "#"_u);
}

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
        case "\""_u.n:
        {
            return true;
        }
        default: {
            return false;
        }
    }
}

static constexpr bool isIdentifier(uChar c) {
    return (!isWhitespace(c) && 
            !isOperatorChar(c) && 
            !isNumberOrPeriod(c) &&
            !isReservedChar(c) &&
            isntDoubleQuotes(c)) &&
            isntTag(c);
}

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

Tokenizer::Tokenizer(const Utf8String& text) {
    size_t curPos = 0;
    size_t lastPos = 0;

    const std::map<uChar, TokenType> singleCharTokenMap = {
        {";"_u, TokenType::EOL}, {"@"_u, TokenType::Prepocessor},
        {"("_u, TokenType::OpenParen}, {")"_u, TokenType::CloseParen},
        {"["_u, TokenType::OpenSquare}, {"]"_u, TokenType::CloseSquare},
        {"{"_u, TokenType::OpenCurly}, {";"_u, TokenType::CloseCurly},
    };

    const std::map<Utf8String, TokenType> keywordMap = {
        {"func"_utf8, TokenType::Func}, {"if"_utf8, TokenType::If},
        {"then"_utf8, TokenType::Then}, {"do"_utf8, TokenType::Do},
        {"while"_utf8, TokenType::While}, {"for"_utf8, TokenType::For},
        {"import"_utf8, TokenType::Import}, {"returns"_utf8, TokenType::Returns},
        {"let"_utf8, TokenType::Let}, {"end"_utf8, TokenType::End}
    };

    const size_t maxCharCount = text.getCharCount();
    while (curPos < maxCharCount) {
        uChar curChar = text[curPos];
        TokenType newType = TokenType::Undefined;
        if (isWhitespace(curChar)) {
            curPos++;
            lastPos++;
            continue;
        } else if (curChar == "#"_u) {
            size_t savedPos = curPos;
            curPos = countTakeWhile(text, curPos + 1, isntTag) + 1;
            if (curPos >= maxCharCount) {
                throw std::runtime_error("Failed to find closing tag for comment beginning with charachter: " + savedPos);
            }
            lastPos = curPos;
            continue;
        } else if (isOperatorChar(curChar)) {
            curPos = countTakeWhile(text, curPos, isOperatorChar);
            newType = TokenType::Operator;
        } else if (isNumber(curChar)) {
            curPos = countTakeWhile(text, curPos, isNumberOrPeriod);
            newType = TokenType::Number;
        } else if (curChar == "\""_u) {
            size_t savedPos = curPos;
            curPos = countTakeWhile(text, curPos + 1, isntDoubleQuotes) + 1;
            if (curPos >= maxCharCount) {
                throw std::runtime_error("Failed to find closing quotes for string literal beginning with charachter: " + savedPos);
            }
            newType = TokenType::StringLit;
        } else if (singleCharTokenMap.contains(curChar)) {
            curPos++;
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

        //std::cout << "New Token! " << newType << std::endl;
        tokens.push_back(Token{
            .type = newType,
            .text = Utf8StringView(text, lastPos, curPos),
        });
        lastPos = curPos;
    }
}

const std::vector<Token>& Tokenizer::getTokens() const {
    return tokens;
}