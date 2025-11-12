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
    // Utf8String::setLocale();
    // std::string filePath = "/mnt/c/Users/Moose/Desktop/Programming/FlowLang/test.fl";
    // Utf8String fileContent = Utf8String::fromFile(filePath.c_str());
    // Tokenizer tokenizer = Tokenizer(fileContent);
    // // for (auto t : tokenizer.getTokens()) {
    // //     std::cout << t << std::endl;
    // // }
    // auto tokens = tokenizer.getTokens();
    // ASTNodePtr head = parseGlobal(std::span<Token>(tokens.begin(), tokens.end())).value();

    // auto res = fl::Result<int, double>::Ok(4);

    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto subspan = fl::Span<int>(nums.data() + 2, 3);
    for (const auto& num : subspan) {
        std::cout << "Span over element: " << num << std::endl;
    }

    return 0;
}