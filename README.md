# Simple Compiler Project

A minimalist compiler/tokenizer that can parse basic code syntax and generate assembly.

## Description

This project implements a simple tokenizer and code generator that can parse basic syntax and convert it to x86-64 assembly. Currently, it supports basic "exit" statements with integer values.

## Features

- Tokenization of basic syntax elements:
  - Keywords (currently only "exit")
  - Integer literals
  - Identifiers (variable names)
  - Semicolons
- Assembly generation for simple exit statements

## How It Works

The compiler operates in two main stages:
1. **Tokenization**: The input string is broken down into tokens
2. **Code Generation**: The tokens are processed to generate assembly code

## Building the Project

The project uses CMake for building:

```bash
# Create a build directory
mkdir -p build
cd build

# Configure and build with CMake
cmake ..
make
```

## Usage

```bash
./compiler
```

This will parse the code "exit 42;" and generate assembly code that exits with status code 42.

## Output

The compiler generates NASM-compatible x86-64 assembly code like:

```asm
global _start
_start:
    mov rax, 60
    mov rdi, 42
    syscall
```

## Testing

The project includes a test script that compiles, runs, and displays the results automatically:

```bash
# Run the test script
./test.sh
```

The script will:
1. Pass your code to the compiler
2. Generate the assembly output
3. Assemble and link the executable
4. Run the program
5. Display the results

## Current Limitations

- Limited language features (see implemented features below)
- Basic error handling and reporting
- No optimization passes
- Single-file compilation only

## Project Roadmap

### Implemented Features
- ✅ Variables and assignment operations
- ✅ Arithmetic expressions and operators
- ✅ Code blocks and scoping
- ✅ Basic control flow (if/else, if/else if/else statements)

### In Progress
- 🔄 Additional control structures (loops, switch statements)

### Planned Features
- ⏳ Function definitions and calls
- ⏳ Arrays and complex data structures
- ⏳ String manipulation
- ⏳ Standard library implementation
- ⏳ Error recovery and better diagnostics
- ⏳ Object-oriented programming features
- ⏳ Basic optimizations

