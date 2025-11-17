# Rational for AST design

The following document contains notes on the rational behind specific AST design choices, as well as serving as a reference for AST patterns for documentation

## Functions

Functions take in a large number of required tokens, yet have to be flexible in their construction to facillitate all the different types of functions possible. Each function currently should only exist in the global scope, and are denoted by a token with the `func` identifier.

The first token in the children of a parsed function is going to be its `identifier` token, which will be used for symbol matching. This node will have children consisting of all the individual block and expression trees produced inside the parsed function, and actual bytecode generation occurs by walking the AST tree from this point. From an outside perspective, the next child of the `func` token is the return type, followed by pairs of `type` and `identifier` that form the parameters of the function. **Every function is uniquely defined by its identifier, which unfortunatly means that function overloading is not yet supported**

[FunctionToASTDiagram](./function.drawio.svg)
<img src="./function.drawio.svg">

## If-Elif-Else

Basic control flow is acheived through `if`, `elif` and `else` blocks, which have the following AST structure. In this way, a left to right traversal of the `if` blocks children yeilds each succsessive condition that must be met for the block expressions to run. If the child is `else` instead, failure to satisfy any of the left conditions results in the `else` child expressions being run

[IfToASTDiagram](./if.drawio.svg)
<img src="./if.drawio.svg">

## While

While blocks are the simplest block structure, and just imply a loop over a set of expressions while some condition evaluates to boolean true. Left to right the blocks can be evaluated as the stopping condition, followed by the block expressions, making it more natural for the bytecode generator to line up all the required operations

[WhileToASTDiagram](./while.drawio.svg)
<img src="./while.drawio.svg">

# For

For blocks are similar to while blocks but with more requirements. Left to right, you declare the initial value of your loop variable, then you define the stopping and advance expressions, followed by the block expressions. Functionally the bytecode generator, can run the initial loop variable declaration once then create a loop block with the stop condition at the start, and the advance condition at the end

[WhileToASTDiagram](./for.drawio.svg)
<img src="./for.drawio.svg">