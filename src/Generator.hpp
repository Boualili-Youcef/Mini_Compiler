#pragma once

#include "Parser.hpp"
#include <sstream>

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
     * @param program Programme à compiler (racine de l'AST)
     */
    Generator(const Program &program) : m_program(program) {}

    /**
     * @brief Génère le code assembleur à partir de l'AST
     * @return std::string Le code assembleur généré
     */
    std::string generateAssembly() const
    {
        std::stringstream assembly;
        assembly << "global _start\n";
        assembly << "_start:\n";

        // Parcourir toutes les instructions du programme
        for (const auto &stmt : m_program.statements)
        {
            // Traiter chaque type d'instruction
            switch (stmt->getType())
            {
            case StmtType::EXIT:
                generateExitCode(dynamic_cast<ExitStmt *>(stmt.get()), assembly);
                break;
            case StmtType::LET:
                // Pour l'instant, les instructions 'let' ne génèrent pas de code
                // On pourrait les utiliser pour les variables plus tard
                break;
            default:
                // Erreur: type d'instruction non supporté
                assembly << "    ; Instruction non supportée\n";
                break;
            }
        }

        return assembly.str();
    }

private:
    /**
     * @brief Génère le code assembleur pour une instruction exit
     * @param exitStmt Instruction exit à traiter
     * @param assembly Flux où écrire le code assembleur
     */
    void generateExitCode(const ExitStmt *exitStmt, std::stringstream &assembly) const
    {
        assembly << "    mov rax, 60\n"; // syscall exit

        // Traiter l'expression
        if (exitStmt && exitStmt->expr)
        { // Changé 'value' en 'expr'
            if (exitStmt->expr->getType() == ExprType::INTEGER)
            { // Changé 'value' en 'expr'
                // Cas d'un littéral entier
                // Je ne vais pas pouvoir utiliser le polymorphisme ici habituelle car je veux faire une separation des taches entre 
                // le parser et le generator c'est pour ça que je fais un cast ici pas sexy a mon gout mais je pense ca va faire l'affaire
                auto intExpr = dynamic_cast<IntExpr *>(exitStmt->expr.get()); // Changé 'value' en 'expr'
                if (intExpr && intExpr->token.value)
                { // Changé 'value.value' en 'token.value'
                    assembly << "    mov rdi, " << *intExpr->token.value << "\n";
                }
                else
                {
                    assembly << "    mov rdi, 0\n"; // Valeur par défaut si problème
                }
            }
            else if (exitStmt->expr->getType() == ExprType::VARIABLE)
            { // Changé 'value' en 'expr'
                // Cas d'une variable (non implémenté pour l'instant)
                assembly << "    mov rdi, 0\n"; // Pour l'instant, valeur par défaut
            }
        }
        else
        {
            assembly << "    mov rdi, 1\n"; // Code d'erreur par défaut
        }

        assembly << "    syscall\n";
    }
    /**
     * @brief Programme à compiler
     */
    const Program m_program;
};