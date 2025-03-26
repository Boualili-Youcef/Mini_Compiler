#pragma once

#include "Parser.hpp"

class Generator
{
public:
    Generator(const NodeExit &root) : m_node(root) {}
    std::string generateAssembly() const
    {
        std::stringstream assembly;
        assembly << "global _start\n";
        assembly << "_start:\n";
         // Vérifier si la valeur existe avant de l'utiliser
         if (m_node.expression.INT_LITERAL.value) {
            assembly << "    mov rax, 60\n";
            assembly << "    mov rdi, " << *m_node.expression.INT_LITERAL.value << "\n";
            assembly << "    syscall\n";
        } else {
            assembly << "    mov rax, 60\n";
            assembly << "    mov rdi, 1\n"; 
            assembly << "    syscall\n";
        }
        return assembly.str();
    }

private:
    const NodeExit m_node;
};