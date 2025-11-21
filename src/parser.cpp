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

/**
 * @brief gets the prescedence of an operator, highly optimized hopefully
 */
constexpr int64_t getPrescedence(const Token& token) {
    switch (token.type) {
        case TokenType::Let:
        case TokenType::FuncCall:
        case TokenType::PostInc:
        case TokenType::PostDec: {
            return 1;
        }
        case TokenType::Period: {
            return 2;
        }
        case TokenType::LogNot: {
            return 3;
        }
        case TokenType::Mul:
        case TokenType::Div:
        case TokenType::Mod: {
            return 4;
        }
        case TokenType::Add:
        case TokenType::Sub: {
            return 5;
        }
        case TokenType::LessThan:
        case TokenType::LessEqual: {
            return 6;
        }
        case TokenType::GreaterThan:
        case TokenType::GreaterEqual: {
            return 7;
        }
        case TokenType::Equals:
        case TokenType::NotEquals: {
            return 8;
        }
        case TokenType::Assign: {
            return 9;
        }
        case TokenType::AddAssign:
        case TokenType::SubAssign: {
            return 10;
        }
        case TokenType::MulAssign:
        case TokenType::DivAssign: {
            return 11;
        }
        default: {
            return -1;
        }
    }
}

constexpr BindingType getBindingType(const Token& t) {
    switch (t.type) {
        case TokenType::FuncCall: {
            return BindingType::Functional;
        }
        case TokenType::PostInc:
        case TokenType::PostDec: {
            return BindingType::LeftUnary;
        }
        case TokenType::Let:
        case TokenType::LogNot: {
            return BindingType::RightUnary;
        }
        case TokenType::Add:
        case TokenType::Sub:
        case TokenType::Mul:
        case TokenType::Div:
        case TokenType::Mod:
        case TokenType::LessThan:
        case TokenType::LessEqual:
        case TokenType::GreaterThan:
        case TokenType::GreaterEqual:
        case TokenType::Assign:
        case TokenType::Equals:
        case TokenType::NotEquals:
        case TokenType::AddAssign:
        case TokenType::SubAssign:
        case TokenType::MulAssign:
        case TokenType::DivAssign:
        case TokenType::Period: {
            return BindingType::BinaryInfix;
        }
        default: {
            return BindingType::Unbound;
        }
    }
}

/**
 * @brief loops over a set of tokens which is expected to be just constants and parenthesis
 */
Result<int64_t, Utf8String> extractSingle(const Span<Token>& tokens) {
    int64_t nextConst = -1;
    for (int i = 0; i < tokens.size(); i++) {
        if ((tokens[i].type != TokenType::OpenParen) && (tokens[i].type != TokenType::CloseParen)) {
            if (nextConst == -1) {
                nextConst = i;
            } else {
                return Result<int64_t, Utf8String>::Err("Expected to see a single operand!"_utf8);
            }
        }
    }
    return Result<int64_t, Utf8String>::Ok(nextConst);
}

/*======================================================================================================*/
/*                                        Seekers                                                       */
/*======================================================================================================*/

Result<int64_t, Utf8String> findNextOp(const Span<Token>& tokens) {
    int64_t nextOpPres = 0;    //The raw pres of the next op
    int64_t nextOpPPower = INT64_MAX;  //The number of parenthesis around next op
    int64_t nextOpIndx = -1;   //The index of the next op
    int64_t currentPPower = 0; //The current pLevel
    for (int i = 0; i < tokens.size(); i++) {
        const auto& t = tokens[i];
        if (t.type == TokenType::OpenParen) {
            currentPPower++;
            continue;
        } else if (t.type == TokenType::CloseParen) {
            currentPPower--;
            continue;
        } else {
            int64_t thisPres = getPrescedence(t);
            //Check to see if its a non-op
            if (thisPres == -1) {
                continue;
            }
            //If there is strictly less parenthesis it should always be run first
            if (currentPPower < nextOpPPower) {
                nextOpPPower = currentPPower;
                nextOpPres = thisPres;
                nextOpIndx = i;
                
            //If they are equal in parenthetical power, then we care about operator prescedence
            } else if ((currentPPower == nextOpPPower) && (thisPres > nextOpPres)) {
                nextOpPPower = currentPPower;
                nextOpPres = thisPres;
                nextOpIndx = i;
            }
        }
    }

    
    return Result<int64_t, Utf8String>::Ok(nextOpIndx);
}

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
            curTokenIndx += end + 1; //Advance past this section
        }
        else {
            std::cout << "Illegal top level token: " << tokens[curTokenIndx] << std::endl;
            curTokenIndx++;
        }
    }

    //All parsing is aOk, we can return
    return ParseResult::Ok(globalHead);
}

ParseResult FlowParser::parseFunc(const Span<Token>& tokens) {
    size_t tokenCount = tokens.size();
    //Tokens contains the entire contents of a function body, so the node we want to return is at the top level,
    //a func token, which is at 0, which we know exists becuase it came down from the top level parser
    size_t funcHead = addAstNode(&tokens[0]);

    //The first child must then be the function name, which is the next token
    if ((tokenCount < 2) || (tokens[1].type != TokenType::FuncCall)) {
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
    std::optional<Utf8String> exprError = parseExprs(funcName, tokens.subspan(closeParen + 3));
    if (exprError.has_value()) {
        return ParseResult::Err(exprError.value());
    }
    
    //Everything was good, return the new tree, and add this node to our top level registry of functions
    functionDecs.insert({ast[funcName].body.text, funcHead});
    return ParseResult::Ok(funcHead);
}

std::optional<Utf8String> FlowParser::parseExprs(size_t parent, const Span<Token>& tokens) {
    size_t curTokenIndx = 0;
    while (curTokenIndx < tokens.size()) {
        const Token& nextExprStart = tokens[curTokenIndx];
        if (nextExprStart.type == TokenType::If) {

        } else if (nextExprStart.type == TokenType::For) {

        } else if (nextExprStart.type == TokenType::While) {

        } else {
            //Our next line is simply an expression
            int64_t endOfLine = seekNext(tokens.subspan(curTokenIndx), TokenType::EOL);
            if (endOfLine == -1) {

                return std::optional("Unbounded expression, are you missing an end of line?"_utf8);
            }

            //Parse the expression tree
            std::cout << "Parsing new Expression for tokens: " << std::endl;
            for (const auto& t : tokens.subspan(curTokenIndx, endOfLine)) {
                std::cout << t << std::endl;
            }
            std::cout << "end expression tokens" << std::endl;
            auto exprTree = parseExpr(tokens.subspan(curTokenIndx, endOfLine));
            if (!exprTree.isOk()) {
                return std::optional(exprTree.errValue());
            }

            //Everything went fine, add it as a child to parent
            ast[parent].addChild(exprTree.okValue());
            curTokenIndx += endOfLine + 1;
        }
    }

    //Everything went fine
    return std::nullopt;
}

ParseResult FlowParser::parseExpr(const Span<Token>& tokens) {
    //Check to see if tokens is just a constant, or there is an error in the parsing code
    size_t tokenCount = tokens.size();
    if (tokenCount == 0) {
        //Todo this might actually be dead code, will need to experiment more
        return ParseResult::Err("Attempted to parse a zero token expression!"_utf8);
    } else if (tokenCount == 1) {
        std::cout << "Constant Node established: " << tokens[0] << std::endl;
        size_t tNode = addAstNode(&tokens[0]);
        return ParseResult::Ok(tNode);
    }

    //Scan for the operator to act upon
    auto nextOp = findNextOp(tokens);
    if (!nextOp.isOk()) {
        return ParseResult::Err(nextOp.errValue());
    }

    //Unwrap the next op and see what we are dealing with
    int64_t nextOpIndx = nextOp.okValue();

    //Check to see if the next side is just some parenthesis and a single non-parenthesis item
    if (nextOpIndx == -1) {

        //If no extra non-parenthesis item is found, err
        auto constRes = extractSingle(tokens);
        if (!constRes.isOk()) {
            return ParseResult::Err(constRes.errValue());
        }

        //If all good, return our new constant node
        size_t tNode = addAstNode(&tokens[constRes.okValue()]);
        return ParseResult::Ok(tNode);
    }

    std::cout << "Parse expression has identified the next op: " << tokens[nextOpIndx] << std::endl;

    //Lets dispatch these based on binding direction
    BindingType bindType = getBindingType(tokens[nextOpIndx]);
    switch (bindType) {
        case BindingType::BinaryInfix: {
            //If we have a binary operator next, then we have to parse that
            auto binaryRes = parseBinaryExpr(nextOpIndx, tokens);
            return binaryRes;
        }
    }


    return ParseResult::Err("Invalid Expression: Generic Flavour!"_utf8);
}

ParseResult FlowParser::parseBinaryExpr(size_t nextOp, const Span<Token>& tokens) {
    //Since this is binary we create a new head representing the op
    size_t newHead = addAstNode(&tokens[nextOp]);

    //And parse everything to the left of it
    if (nextOp == 0) {
        return ParseResult::Err("Expected to see an operand to the left of binary infix operator!"_utf8);
    }
    auto lhs = parseExpr(tokens.subspan(0, nextOp));
    if (!lhs.isOk()) {
        return lhs;
    }
    ast[newHead].addChild(lhs.okValue());

    //And parse everything to the right of it
    if (nextOp == (tokens.size() - 1)) {
        return ParseResult::Err("Expected to see an operand to the right of binary infix operator!"_utf8);
    }
    auto rhs = parseExpr(tokens.subspan(nextOp + 1));
    if (!rhs.isOk()) {
        return rhs;
    }
    ast[newHead].addChild(rhs.okValue());

    //Full binary expression parsed succsessfully
    return ParseResult::Ok(newHead);
}

} //end namespace fl