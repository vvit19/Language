# Programming language

## Overview
This project is an implementation of my own programming language. Code on my language is translated into code in my own assembler, whis is executed on [my virtual spu](https://github.com/vvit19/Processor).

## Compiling process
### 1) Frontend
Parsing code into AST (format for AST was invented by me). The recursive descent algorithm is selected for the parser.
### 2) Middleend
At middleend stage, AST is optimizing: some subtrees are made simpler to raise code performance. This project only implements optimizations of arithmetic operations (constant folding and dead-code removing).
### 3) Backend
Received from the middleend stage AST is translated into instructions for my own assembler. Code on my assembler is translated into instructions for my virtual spu, then these instructions are executed.

## Code example
This program calculates factorial (other examples are [here](https://github.com/vvit19/Language/tree/master/examples)):

```
func main ()
{
    var n = 0;
    in (n);

    var res = call factorial (n);

    out (res);

    return 0;
}

func factorial (n)
{
    if (n == 1)
    {
        return 1;
    }

    var res = call factorial (n - 1);

    return n * res;
}
```
