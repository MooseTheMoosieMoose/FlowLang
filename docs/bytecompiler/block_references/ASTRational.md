# Rational for AST design

The following document contains notes on the rational behind specific AST design choices, as well as serving as a reference for AST patterns for documentation

## Functions

Functions take in a large number of required tokens, yet have to be flexible in their construction to facillitate all the different types of functions possible. Each function currently should only exist in the global scope, and are denoted by a token with the `func` identifier.

The first token in the children of a parsed function is going to be its `identifier` token, which will be used for symbol matching. This node will have children consisting of all the individual block and expression trees produced inside the parsed function, and actual bytecode generation occurs by walking the AST tree from this point. From an outside perspective, the next child of the `func` token is the return type, followed by pairs of `type` and `identifier` that form the parameters of the function. **All of the immediate children of a `func` token determine its function signature together, to allow for function overloading**

[Alt text](./function.drawio.svg)
<img src="./function.drawio.svg">