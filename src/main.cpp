#include <iostream>
#include <string>
#include "utf8string.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"
#include "fl_util.hpp"

/**
 * @brief THIS IS A TESTING FILE ONLY
 */

using namespace fl;

int main() {
    Utf8String::setLocale();
    std::string filePath = "/mnt/c/Users/Moose/Desktop/Programming/FlowLang/test.fl";
    Utf8String fileContent = Utf8String::fromFile(filePath.c_str());

    auto tokensRes = tokenize(fileContent);
    std::vector<Token> tokens = {};
    if (tokensRes.isOk()) {
        tokens = tokensRes.okValue();
    } else {
        std::cout << "Tokenizer error: " << tokensRes.errValue() << std::endl;
        return 1;
    }

    // for (auto t : tokens) {
    //     std::cout << t << std::endl;
    // }

    auto parser = FlowParser();
    auto head = parser.parse(tokens);

    std::cout << "Parser finished!" << std::endl;
    if (head.isOk()) {
        //parser.log();
    } else {
        std::cout << "Parser Failure: " << head.errValue() << std::endl;
        return 1;
    }

    return 0;
}