#pragma once

#include "Parser.hpp"
#include <sstream>
#include <unordered_map>
#include <optional>
#include <vector>

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
        std::vector<std::unordered_map<std::string, int>> symbolTables;
        symbolTables.push_back({}); // Scope global
        
        int stackOffset = 0;      // Offset de la pile pour les variables
        bool hasExitStmt = false; // Indique si une instruction exit a été trouvée

        assembly << "global _start\n";
        assembly << "section .text\n";
        assembly << "_start:\n";

        // Initialisation de la base de pile
        assembly << "    push rbp\n";
        assembly << "    mov rbp, rsp\n";

        // Parcourir toutes les instructions du programme
        for (const auto &stmt : m_program.statements)
        {
            switch (stmt->getType())
            {
            case StmtType::EXIT:
                generateExitCode(dynamic_cast<ExitStmt *>(stmt.get()), assembly, symbolTables);
                hasExitStmt = true;
                break;
            case StmtType::LET:
                generateLetCode(dynamic_cast<LetStmt *>(stmt.get()), assembly, symbolTables, stackOffset);
                break;
            case StmtType::BLOCK:
                generateBlockCode(dynamic_cast<BlockStmt *>(stmt.get()), assembly, symbolTables, stackOffset);
                break;
            default:
                assembly << "    ; Instruction non supportée\n";
                break;
            }
        }

        // Ajouter une sortie par défaut seulement si aucun exit n'est présent
        if (!hasExitStmt)
        {
            assembly << "    mov rax, 60\n";
            assembly << "    mov rdi, 0\n";
            assembly << "    syscall\n";
        }

        return assembly.str();
    }

private:
    /**
     * @brief Recherche une variable dans tous les scopes disponibles
     * @param varName Nom de la variable à rechercher
     * @param symbolTables Pile des tables de symboles (scopes)
     * @return L'offset de la variable ou std::nullopt si non trouvée
     */
    std::optional<int> findVariableOffset(const std::string &varName,
                                         const std::vector<std::unordered_map<std::string, int>> &symbolTables) const
    {
        // Parcourir du scope le plus interne au plus externe
        for (auto it = symbolTables.rbegin(); it != symbolTables.rend(); ++it)
        {
            auto found = it->find(varName);
            if (found != it->end())
            {
                return found->second;
            }
        }
        return std::nullopt;
    }

    /**
     * @brief Génère le code assembleur pour évaluer une expression
     * @param expr Expression à évaluer
     * @param assembly Flux où écrire le code assembleur
     * @param symbolTables Tables des symboles contenant les variables
     * @note Le résultat est placé dans le registre rax
     */
    void generateExpressionCode(const std::shared_ptr<Expr> &expr, std::stringstream &assembly,
                               const std::vector<std::unordered_map<std::string, int>> &symbolTables) const
    {
        if (!expr)
            return;

        switch (expr->getType())
        {
        case ExprType::INTEGER:
        {
            auto intExpr = dynamic_cast<IntExpr *>(expr.get());
            if (intExpr && intExpr->token.value)
            {
                assembly << "    mov rax, " << *intExpr->token.value << "\n";
            }
            else
            {
                assembly << "    mov rax, 0\n";
            }
            break;
        }
        case ExprType::VARIABLE:
        {
            auto varExpr = dynamic_cast<VarExpr *>(expr.get());
            if (varExpr && varExpr->token.value)
            {
                std::string varName = *varExpr->token.value;
                auto offsetOpt = findVariableOffset(varName, symbolTables);
                
                if (offsetOpt.has_value())
                {
                    int offset = offsetOpt.value();
                    assembly << "    mov rax, [rbp-" << offset << "]\n";
                }
                else
                {
                    assembly << "    ; Variable non définie: " << varName << "\n";
                    assembly << "    mov rax, 0\n";
                }
            }
            break;
        }
        case ExprType::BINARY:
        {
            auto binExpr = dynamic_cast<BinaryExpr *>(expr.get());
            if (binExpr)
            {
                // Parcour de l'arbre
                generateExpressionCode(binExpr->droite, assembly, symbolTables);
                assembly << "    push rax\n"; // Sauvegarder le résultat

                generateExpressionCode(binExpr->gauche, assembly, symbolTables);

                // rax = gauche, récupérer droite dans rbx
                assembly << "    pop rbx\n";

                switch (binExpr->op)
                {
                case BinaryOpType::ADD:
                    assembly << "    add rax, rbx\n";
                    break;
                case BinaryOpType::MUL:
                    assembly << "    imul rax, rbx\n";
                    break;
                case BinaryOpType::SUB:
                    assembly << "    sub rax, rbx\n";
                    break;
                case BinaryOpType::DIV:
                    assembly << "    mov rcx, rbx\n"; // Sauvegarder le diviseur dans rcx
                    assembly << "    xor rdx, rdx\n"; // Mettre à zéro la partie haute du dividende
                    assembly << "    div rcx\n";      // Diviser par rcx, résultat dans rax
                    break;
                case BinaryOpType::MOD:
                    assembly << "    mov rcx, rbx\n"; // Sauvegarder le diviseur dans rcx
                    assembly << "    xor rdx, rdx\n"; // Mettre à zéro la partie haute du dividende
                    assembly << "    div rcx\n";      // Diviser par rcx, quotient dans rax, reste dans rdx
                    assembly << "    mov rax, rdx\n"; // Copier le reste (modulo) dans rax
                    break;
                }
            }
            break;
        }
        }
    }

    /**
     * @brief Génère le code pour une instruction exit
     */
    void generateExitCode(const ExitStmt *exitStmt, std::stringstream &assembly,
                         const std::vector<std::unordered_map<std::string, int>> &symbolTables) const
    {
        // Traiter l'expression
        if (exitStmt && exitStmt->expr)
        {
            generateExpressionCode(exitStmt->expr, assembly, symbolTables);
            assembly << "    mov rdi, rax\n";
        }
        else
        {
            assembly << "    mov rdi, 0\n"; // Code d'erreur par défaut
        }

        assembly << "    mov rax, 60\n"; // syscall exit
        assembly << "    syscall\n";
    }

    /**
     * @brief Génère le code pour une instruction let
     */
    void generateLetCode(const LetStmt *letStmt, std::stringstream &assembly,
                        std::vector<std::unordered_map<std::string, int>> &symbolTables, 
                        int &stackOffset) const
    {
        if (letStmt && letStmt->expr && letStmt->var.value)
        {
            std::string varName = *letStmt->var.value;

            // Évaluer l'expression et mettre le résultat dans rax
            generateExpressionCode(letStmt->expr, assembly, symbolTables);

            // Référence au scope actuel (dernier élément du vecteur)
            auto &currentScope = symbolTables.back();
            
            // Allouer de l'espace sur la pile si c'est une nouvelle variable
            if (currentScope.find(varName) == currentScope.end())
            {
                stackOffset += 8; // Utiliser 8 octets (64 bits) pour chaque variable
                currentScope[varName] = stackOffset;
                assembly << "    sub rsp, 8\n";
            }

            // Stocker la valeur sur la pile
            int offset = currentScope[varName];
            assembly << "    mov [rbp-" << offset << "], rax\n";
        }
    }

    /**
     * @brief Génère le code pour un bloc d'instructions
     */
    void generateBlockCode(const BlockStmt *blockStmt, std::stringstream &assembly,
                          std::vector<std::unordered_map<std::string, int>> &symbolTables,
                          int &stackOffset) const
    {
        // Marquer le début du bloc avec un commentaire
        assembly << "    ; Début de bloc\n";
        
        // Créer un nouveau scope (table de symboles pour ce bloc)
        symbolTables.push_back({});
        int initialStackOffset = stackOffset;
        
        // Générer le code pour chaque instruction dans le bloc
        for (const auto &stmt : blockStmt->statements)
        {
            switch (stmt->getType())
            {
            case StmtType::EXIT:
                generateExitCode(dynamic_cast<ExitStmt*>(stmt.get()), assembly, symbolTables);
                break;
            case StmtType::LET:
                generateLetCode(dynamic_cast<LetStmt*>(stmt.get()), assembly, symbolTables, stackOffset);
                break;
            case StmtType::BLOCK:
                generateBlockCode(dynamic_cast<BlockStmt*>(stmt.get()), assembly, symbolTables, stackOffset);
                break;
            }
        }
        
        // Restaurer la pile en sortant du bloc (libérer les variables locales)
        if (stackOffset > initialStackOffset)
        {
            int difference = stackOffset - initialStackOffset;
            assembly << "    add rsp, " << difference << "\n";
            stackOffset = initialStackOffset;
        }
        
        // Fermer ce scope
        symbolTables.pop_back();
        
        assembly << "    ; Fin de bloc\n";
    }

    /**
     * @brief Programme à compiler
     */
    const Program m_program;
};