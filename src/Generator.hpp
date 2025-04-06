#pragma once

#include "Parser.hpp"
#include <sstream>
#include <unordered_map>

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
        std::unordered_map<std::string, int> symbolTable;
        int stackOffset = 0;

        assembly << "global _start\n";
        assembly << "section .text\n";
        assembly << "_start:\n";
        
        // Initialisation de la base de pile
        assembly << "    push rbp\n";
        assembly << "    mov rbp, rsp\n";

        // Parcourir toutes les instructions du programme
        for (const auto &stmt : m_program.statements)
        {
            // Traiter chaque type d'instruction
            switch (stmt->getType())
            {
            case StmtType::EXIT:
                generateExitCode(dynamic_cast<ExitStmt *>(stmt.get()), assembly, symbolTable);
                break;
            case StmtType::LET:
                generateLetCode(dynamic_cast<LetStmt *>(stmt.get()), assembly, symbolTable, stackOffset);
                break;
            default:
                // Erreur: type d'instruction non supporté
                assembly << "    ; Instruction non supportée\n";
                break;
            }
        }

        // En cas d'absence d'instruction exit, ajouter une sortie par défaut
        assembly << "    mov rax, 60\n";
        assembly << "    mov rdi, 0\n";
        assembly << "    syscall\n";

        return assembly.str();
    }

private:
    /**
     * @brief Génère le code assembleur pour une instruction exit
     * @param exitStmt Instruction exit à traiter
     * @param assembly Flux où écrire le code assembleur
     * @param symbolTable Table des symboles contenant les variables et leurs offsets dans la pile
     */
    void generateExitCode(const ExitStmt *exitStmt, std::stringstream &assembly, 
                          const std::unordered_map<std::string, int> &symbolTable) const
    {
        assembly << "    mov rax, 60\n"; // syscall exit

        // Traiter l'expression
        if (exitStmt && exitStmt->expr)
        { 
            if (exitStmt->expr->getType() == ExprType::INTEGER)
            { 
                // Cas d'un littéral entier
                auto intExpr = dynamic_cast<IntExpr *>(exitStmt->expr.get()); 
                if (intExpr && intExpr->token.value)
                { 
                    assembly << "    mov rdi, " << *intExpr->token.value << "\n";
                }
                else
                {
                    assembly << "    mov rdi, 0\n"; // Valeur par défaut si problème
                }
            }
            else if (exitStmt->expr->getType() == ExprType::VARIABLE)
            { 
                auto varExpr = dynamic_cast<VarExpr *>(exitStmt->expr.get());
                if (varExpr && varExpr->token.value)
                { 
                    std::string varName = *varExpr->token.value;
                    if (symbolTable.find(varName) != symbolTable.end())
                    {
                        // Récupérer la variable depuis la pile
                        int offset = symbolTable.at(varName);
                        assembly << "    mov rdi, [rbp-" << offset << "]\n";
                    }
                    else
                    {
                        assembly << "    ; Variable non définie: " << varName << "\n";
                        assembly << "    mov rdi, 0\n";
                    }
                }
                else
                {
                    assembly << "    mov rdi, 0\n"; // Valeur par défaut si problème
                }
            }
        }
        else
        {
            assembly << "    mov rdi, 1\n"; // Code d'erreur par défaut
        }

        assembly << "    syscall\n";
    }

    /**
     * @brief Génère le code assembleur pour une instruction let
     * @param letStmt Instruction let à traiter
     * @param assembly Flux où écrire le code assembleur
     * @param symbolTable Table des symboles à mettre à jour
     * @param stackOffset Offset actuel de la pile, à mettre à jour
     */
    void generateLetCode(const LetStmt *letStmt, std::stringstream &assembly,
                        std::unordered_map<std::string, int> &symbolTable, int &stackOffset) const
    {
        if (letStmt && letStmt->expr && letStmt->var.value)
        {
            std::string varName = *letStmt->var.value;
            
            // Évaluer l'expression et mettre le résultat dans rax
            if (letStmt->expr->getType() == ExprType::INTEGER)
            {
                auto intExpr = dynamic_cast<IntExpr *>(letStmt->expr.get());
                if (intExpr && intExpr->token.value)
                {
                    assembly << "    mov rax, " << *intExpr->token.value << "\n";
                }
            }
            else if (letStmt->expr->getType() == ExprType::VARIABLE)
            {
                auto varExpr = dynamic_cast<VarExpr *>(letStmt->expr.get());
                if (varExpr && varExpr->token.value)
                {
                    std::string srcVarName = *varExpr->token.value;
                    if (symbolTable.find(srcVarName) != symbolTable.end())
                    {
                        int srcOffset = symbolTable.at(srcVarName);
                        assembly << "    mov rax, [rbp-" << srcOffset << "]\n";
                    }
                    else
                    {
                        assembly << "    ; Variable source non définie: " << srcVarName << "\n";
                        assembly << "    mov rax, 0\n";
                    }
                }
            }

            // Allouer de l'espace sur la pile si c'est une nouvelle variable
            if (symbolTable.find(varName) == symbolTable.end())
            {
                stackOffset += 8; // Utiliser 8 octets (64 bits) pour chaque variable
                symbolTable[varName] = stackOffset;
                assembly << "    sub rsp, 8\n";
            }

            // Stocker la valeur sur la pile
            int offset = symbolTable[varName];
            assembly << "    mov [rbp-" << offset << "], rax\n";
        }
    }

    /**
     * @brief Programme à compiler
     */
    const Program m_program;
};