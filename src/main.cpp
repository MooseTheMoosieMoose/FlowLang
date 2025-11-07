#include <iostream>
#include <string>
#include "utf8string.hpp"
#include "tokenizer.hpp"
int main() {
    Utf8String::setLocale();
    std::string filePath = "/mnt/c/Users/Moose/Desktop/Programming/FlowLang/test.fl";
    Utf8String fileContent = Utf8String::fromFile(filePath.c_str());
    Tokenizer tokenizer = Tokenizer(fileContent);
    for (auto t : tokenizer.getTokens()) {
        std::cout << t << std::endl;
    }
    return 0;
}