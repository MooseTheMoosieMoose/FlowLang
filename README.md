# FlowLang
### What is FlowLang
FlowLang or Flow is a general purpose very-high-level programming language with a heavy emphasis on scripting capabilities and integration into existing projects, and strong localization support through native Utf8 support. It is currently in development, and being implemented in C++23. Once I get things more ironed out I plan to roll it back to C++17 to massivly increase the number of projects and targets that Flow can run on. Generally here is the state of things that have been done and things that need to be done:

I dont usually post any of my projects onto public repositories for a few reasons, but as graduation comes around the corner, I started this project at the beginning of November, 2025 to get something concrete I can put on my resume 

**Project wide Documentation**
* [ ] add docs and rationale for the bytecode compiler
    * [ ] create docs and rationale for AST structures
        * [x] create d & r for functions in the AST

**Bytecode Compiler**
* [x] implement a utf8 string and string view system
* [x] implement a tokenizer
    * [ ] add line and charachter counts to tokens
* [x] create useful types for C++17 backroll
    * [x] created an alterative to std::expected for better error propogation without exceptions
    * [x] create an alternative to std::span
    * [x] created a super simple custom iterator for the project in use by the custom span
* [ ] create a parser (in progress)
    * [x] built out initial seeking utilities to find matching elements in a token stream
    * [x] make sure all parsers use the new fl::Result type in error handling
    * [ ] develop a more robust testing framework for parser results
    * [ ] propogate error messages up the parser chain
* [ ] create a standard for the bytecode
* [ ] create an AST walker to convert the AST to bytecode
* [ ] explore techniques to speed up AST generation and memory saftey
    * [ ] look at converting tokens to owned copies instead of views
    * [ ] utilize better error handling in the project
    * [ ] implement more move semantics to optimize data flow
