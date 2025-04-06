#pragma once

#include "Parser.hpp"

/**
 * @brief Classe responsable de la génération de code assembleur
 *
 * Cette classe prend un arbre syntaxique abstrait (AST) construit par le Parser
 * et le convertit en code assembleur exécutable.
 */
class Generator
{
public:
    /**
     * @brief Constructeur de la classe Generator
     * @param root Nœud représentant la racine de l'AST
     */
    Generator(const NodeExit &root) : m_node(root) {}

    /**
     * @brief Génère le code assembleur à partir de l'AST
     * @return std::string Le code assembleur généré
     */
    std::string generateAssembly() const
    {
        std::stringstream assembly;
        assembly << "global _start\n";
        assembly << "_start:\n";
        // Vérifier si la valeur existe avant de l'utiliser
        if (m_node.expression.INT_LITERAL.value)
        {
            assembly << "    mov rax, 60\n";
            assembly << "    mov rdi, " << *m_node.expression.INT_LITERAL.value << "\n";
            assembly << "    syscall\n";
        }
        else
        {
            assembly << "    mov rax, 60\n";
            assembly << "    mov rdi, 1\n";
            assembly << "    syscall\n";
        }
        return assembly.str();
    }

private:
    /**
     * @brief Nœud racine de l'AST représentant une instruction de sortie
     */
    const NodeExit m_node;
};