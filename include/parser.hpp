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
#include <map>
#include <optional>

/**
 * @brief REMOVE AFTER DEBUGGING
 */
#include <iostream>

namespace fl {

/*======================================================================================================*/
/*                                          Parsers                                                     */
/*======================================================================================================*/

/**
 * @brief a type alias to make writing the parsers cleaner
 */
using ParseResult = Result<size_t, Utf8String>;

/**
 * @brief an object to parse the entire contents of a token stream
 * @note the resulting AST, and any nodes within it returned from this function, are bound to the lifetime
 * of this object!
 */
class FlowParser {
public:
    FlowParser() noexcept {}

    /**
     * @brief a saftey wrapper over the internal parseGlobal to ensure that any upwards propogated
     * errors results in clearing the internal data of the parser
     */
    inline Result<ASTNode*, Utf8String> parse(std::vector<Token>& tokens) {
        auto result = parseGlobal(Span<Token>(tokens.data(), tokens.size()));
        if (!result.isOk()) {
            ast.clear();
        }
        return Result<ASTNode*, Utf8String>::Ok(&ast[0]);
    }

    /**
     * @brief displays an AST
     */
    void log(int depth = 0, int index = 0) {
        for (int i = 0; i < depth; i++) {
            std::cout << "-";
        }
        std::cout << "> " << ast[index].body.text << std::endl;

        for (auto& c : ast[index].children) {
            log(depth + 1, c);
        }
    }

private:
    //This is built up as a flat tree for cache locality
    std::vector<ASTNode> ast;

    //A map which takes in a given functions name and returns its parse tree
    std::map<Utf8StringView, size_t> functionDecs;

    /**
     * @brief performs all the heavy lifting over actually parsing anything
     */
    ParseResult parseGlobal(const Span<Token>& tokens);

    /**
     * @brief provides the initial structural parsing of a top level function block
     */
    ParseResult parseFunc(const Span<Token>& tokens);

    /**
     * @brief parses lines of blocks / expressions into children of a given node
     */
    std::optional<Utf8String> parseExprs(size_t parent, const Span<Token>& tokens);

    /**
     * @brief parses a normal expression line, like `let val foo = 4 + 5;`
     * @note this expects to not see the closing EOL token at the end
     */
    ParseResult parseExpr(const Span<Token>& tokens);

    /**
     * @brief inserts a new child into the parser AST
     * @note uses emplace so that hopefully each AST node is only constructed once
     */
    size_t addAstNode(const Token* newNodeBody = nullptr, int64_t newParent = -1);
};


} //end namespace fl