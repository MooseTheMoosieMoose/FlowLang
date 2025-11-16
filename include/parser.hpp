/*
 __                __                                           
|_| _    .   /\   (_  _ _. _ |_. _  _   |   _  _  _     _  _  _ 
| |(_)\)/.  /--\  __)(_| ||_)|_|| )(_)  |__(_|| )(_)|_|(_|(_)(- 
                          |        _/            _/       _/    

Copyright (c) 2025 Moose Abou-Harb All rights reserved.
This software is licensed under the BSD 3-Clause License, which can be found in the accompanying LICENSE file.
*/

#include "ast_node.hpp"
#include "fl_util.hpp"
#include <optional>

namespace fl {

/*======================================================================================================*/
/*                                          Parsers                                                     */
/*======================================================================================================*/

/**
 * @brief a type alias to make writing the parsers cleaner
 */
using ParseResult = Result<ASTNode&, Utf8String>;

/**
 * @brief an object to parse the entire contents of a token stream
 * @note the resulting AST, and any nodes within it returned from this function, are bound to the lifetime
 * of this object!
 */
class FlowParser {
public:
    FlowParser() noexcept {}

    /**
     * @brief parseGlobal is meant to be called on the global scope of a file,
     * and implement a more traditional recursive descent approach
     * @returns a result covering either the whole global AST or an error message
     */
    ParseResult parseGlobal(const Span<Token>& tokens);

private:
    //This is built up as a flat tree for cache locality
    std::vector<ASTNode> ast;

    /**
     * @brief inserts a new child into the parser AST
     * @note uses emplace so that hopefully each AST node is only constructed once
     */
    ASTNode& addAstNode(const Token& body = Token{}, std::vector<ASTNode&> children = std::vector<ASTNode&>{});
};


} //end namespace fl