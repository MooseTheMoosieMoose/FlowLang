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
    Tokenizer tokenizer = Tokenizer(fileContent);

    for (auto t : tokenizer.getTokens()) {
        std::cout << t << std::endl;
    }
    

    // auto tokens = tokenizer.getTokens();
    // auto head = parseGlobal(Span<Token>(tokens.data(), tokens.size()));

    // std::cout << "Parser finished!" << std::endl;
    // if (head.isOk()) {
    //     auto topLevel = head.okValue();
    //     topLevel->log(); //Get a display of the final syntax tree
    // } else {
    //     std::cout << "Error: " << head.errValue() << std::endl;
    // }

    return 0;
}