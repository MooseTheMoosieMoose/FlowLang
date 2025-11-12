# FlowLang
### What is FlowLang
FlowLang or Flow is a general purpose very-high-level programming language with a heavy emphasis on scripting capabilities and integration into existing projects, and strong localization support through native Utf8 support. It is currently in development, and being implemented in C++23. Once I get things more ironed out I plan to roll it back to C++17 to massivly increase the number of projects and targets that Flow can run on. Generally here is the state of things that have been done and things that need to be done:

I dont usually post any of my projects onto public repositories for a few reasons, but as graduation comes around the corner, I started this project at the beginning of November, 2025 to get something concrete I can put on my resume 

**Bytecode Compiler**
* [x] implement a utf8 string and string view system
* [x] implement a tokenizer
* [ ] create alternatives to std::expected and std::span (in progress)
* [ ] create a parser (in progress)
* [ ] create a standard for the bytecode
* [ ] create an AST walker to convert the AST to bytecode
