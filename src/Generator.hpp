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
            case StmtType::IF:
                generateIfCode(dynamic_cast<IfStmt *>(stmt.get()), assembly, symbolTables, stackOffset);
                break;
            case StmtType::WHILE:
                generateWhileCode(dynamic_cast<WhileStmt *>(stmt.get()), assembly, symbolTables, stackOffset);
                break;
            case StmtType::PRINT:
                generatePrintCode(dynamic_cast<PrintStmt *>(stmt.get()), assembly, symbolTables);
                break;
            case StmtType::ARRAY_ASSIGN:
                generateAssignCode(dynamic_cast<AssignStmt *>(stmt.get()), assembly, symbolTables);
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
                case BinaryOpType::EQUAL:
                    assembly << "    cmp rax, rbx\n";  // Comparer les deux valeurs
                    assembly << "    sete al\n";       // Mettre 1 si égal, sinon 0
                    assembly << "    movzx rax, al\n"; // Étendre le résultat à 64 bits
                    break;
                case BinaryOpType::GREAT:
                    assembly << "    cmp rax, rbx\n";  // Comparer les deux valeurs
                    assembly << "    setg al\n";       // Mettre 1 si rax > rbx, sinon 0
                    assembly << "    movzx rax, al\n"; // Étendre le résultat à 64 bits
                    break;
                case BinaryOpType::LESS:
                    assembly << "    cmp rax, rbx\n";  // Comparer les deux valeurs
                    assembly << "    setl al\n";       // Mettre 1 si rax < rbx, sinon 0
                    assembly << "    movzx rax, al\n"; // Étendre le résultat à 64 bits
                    break;
                case BinaryOpType::GREAT_EQUAL:
                    assembly << "    cmp rax, rbx\n";  // Comparer les deux valeurs
                    assembly << "    setge al\n";      // Mettre 1 si rax >= rbx, sinon 0
                    assembly << "    movzx rax, al\n"; // Étendre le résultat à 64 bits
                    break;
                case BinaryOpType::LESS_EQUAL:
                    assembly << "    cmp rax, rbx\n";  // Comparer les deux valeurs
                    assembly << "    setle al\n";      // Mettre 1 si rax <= rbx, sinon 0
                    assembly << "    movzx rax, al\n"; // Étendre le résultat à 64 bits
                    break;
                case BinaryOpType::AND:
                    assembly << "    and rax, rbx\n"; // Effectuer un ET logique
                    break;
                case BinaryOpType::OR:
                    assembly << "    or rax, rbx\n"; // Effectuer un OU logique
                    break;
                case BinaryOpType::NOT_EQUAL:
                    assembly << "    cmp rax, rbx\n";  // Comparer les deux valeurs
                    assembly << "    setne al\n";      // Mettre 1 si différent, sinon 0
                    assembly << "    movzx rax, al\n"; // Étendre le résultat à 64 bits
                    break;
                }
            }
            break;
        }
        case ExprType::ARRAY:
        {
            const ArrayExpr *arrayExpr = static_cast<const ArrayExpr *>(expr.get());
            size_t size = arrayExpr->elements.size();

            // Allouer mémoire pour (taille + éléments)
            assembly << "    mov rax, 9\n";
            assembly << "    mov rdi, 0\n";
            assembly << "    mov rsi, " << (size + 1) * 8 << "\n"; // +1 pour la taille
            assembly << "    mov rdx, 3\n";
            assembly << "    mov r10, 34\n";
            assembly << "    mov r8, -1\n";
            assembly << "    mov r9, 0\n";
            assembly << "    syscall\n";

            assembly << "    push rax\n";

            // Stocker la taille en premier
            assembly << "    mov rbx, [rsp]\n";
            assembly << "    mov qword [rbx], " << size << "\n";

            // Initialiser les éléments, en commençant à l'offset +8
            for (size_t i = 0; i < size; i++)
            {
                assembly << "    mov rbx, [rsp]\n";
                assembly << "    push rbx\n";
                generateExpressionCode(arrayExpr->elements[i], assembly, symbolTables);
                assembly << "    pop rbx\n";
                assembly << "    mov [rbx + " << (i + 1) * 8 << "], rax\n"; // Notez le i+1
            }

            assembly << "    pop rax\n";
            break;
        }

        case ExprType::ARRAY_ACCESS:
        {
            const ArrayAccessExpr *accessExpr = static_cast<const ArrayAccessExpr *>(expr.get());

            // Générer le code pour l'expression du tableau (adresse dans rax)
            generateExpressionCode(accessExpr->array, assembly, symbolTables);
            assembly << "    push rax\n";

            generateExpressionCode(accessExpr->index, assembly, symbolTables);

            assembly << "    add rax, 1\n";

            assembly << "    imul rax, 8\n";

            assembly << "    pop rbx\n";        // Récupérer l'adresse du tableau
            assembly << "    add rbx, rax\n";   // Calculer l'adresse de l'élément
            assembly << "    mov rax, [rbx]\n"; // Charger la valeur
            break;
        }
        case ExprType::LENGTH:
        {
            const LengthExpr *lengthExpr = static_cast<const LengthExpr *>(expr.get());

            // Générer le code pour obtenir l'adresse du tableau
            generateExpressionCode(lengthExpr->array, assembly, symbolTables);

            // La taille est stockée dans le premier mot
            assembly << "    mov rax, [rax]\n";
            break;
        }
        }
    }

    /**
     * @brief Génère le code pour une instruction exit
     */
    void
    generateExitCode(const ExitStmt *exitStmt, std::stringstream &assembly,
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

        // Générer le code pour chaque instruction dans le bloc ici Copier coller du parcours dans le programme
        for (const auto &stmt : blockStmt->statements)
        {
            switch (stmt->getType())
            {
            case StmtType::EXIT:
                generateExitCode(dynamic_cast<ExitStmt *>(stmt.get()), assembly, symbolTables);
                break;
            case StmtType::LET:
                generateLetCode(dynamic_cast<LetStmt *>(stmt.get()), assembly, symbolTables, stackOffset);
                break;
            case StmtType::BLOCK:
                generateBlockCode(dynamic_cast<BlockStmt *>(stmt.get()), assembly, symbolTables, stackOffset);
                break;
            case StmtType::IF:
                generateIfCode(dynamic_cast<IfStmt *>(stmt.get()), assembly, symbolTables, stackOffset);
                break;
            case StmtType::WHILE:
                generateWhileCode(dynamic_cast<WhileStmt *>(stmt.get()), assembly, symbolTables, stackOffset);
                break;
            case StmtType::ASSIGN:
                generateAssignCode(dynamic_cast<AssignStmt *>(stmt.get()), assembly, symbolTables);
                break;
            case StmtType::PRINT:
                generatePrintCode(dynamic_cast<PrintStmt *>(stmt.get()), assembly, symbolTables);
                break;
            case StmtType::ARRAY_ASSIGN:
                generateArrayAssignCode(static_cast<const ArrayAssignStmt *>(stmt.get()), assembly, symbolTables);
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
     * @brief Génère le code pour une instruction if
     */
    void generateIfCode(const IfStmt *ifStmt, std::stringstream &assembly,
                        std::vector<std::unordered_map<std::string, int>> &symbolTables,
                        int &stackOffset) const
    {
        if (!ifStmt || !ifStmt->condition || !ifStmt->thenBranch)
            return;

        // Générer un label unique pour le saut
        static int labelCounter = 0;
        std::string elseLabel = ".if_else_" + std::to_string(labelCounter);
        std::string endLabel = ".if_end_" + std::to_string(labelCounter++);

        assembly << "    ; Début du if\n";

        // Évaluer la condition
        generateExpressionCode(ifStmt->condition, assembly, symbolTables);

        assembly << "    cmp rax, 0\n";
        if (ifStmt->elseBranch)
        {
            assembly << "    je " << elseLabel << "\n";
        }
        else
        {
            assembly << "    je " << endLabel << "\n";
        }

        generateBlockCode(ifStmt->thenBranch.get(), assembly, symbolTables, stackOffset);

        if (ifStmt->elseBranch)
        {
            assembly << "    jmp " << elseLabel << "\n";
        }
        // Le Bloc else
        if (ifStmt->elseBranch)
        {
            assembly << elseLabel << ":\n";
            generateBlockCode(ifStmt->elseBranch.get(), assembly, symbolTables, stackOffset);
        }

        // Label pour la fin du if
        assembly << endLabel << ":\n";
        assembly << "    ; Fin du if\n";
    }

    /**
     * @brief Génère le code pour une instruction while
     */
    void generateWhileCode(const WhileStmt *whileStmt, std::stringstream &assembly,
                           std::vector<std::unordered_map<std::string, int>> &symbolTables,
                           int &stackOffset) const
    {
        if (!whileStmt || !whileStmt->condition || !whileStmt->body)
            return;

        // Générer un label unique pour le début et la fin de la boucle
        static int labelCounter = 0;
        std::string startLabel = ".while_start_" + std::to_string(labelCounter);
        std::string endLabel = ".while_end_" + std::to_string(labelCounter++);

        assembly << startLabel << ":\n";

        // Évaluer la condition
        generateExpressionCode(whileStmt->condition, assembly, symbolTables);

        assembly << "    cmp rax, 0\n";
        assembly << "    je " << endLabel << "\n"; // un jump de kungoru si la condition est fausse

        // Générer le corps de la boucle
        generateBlockCode(whileStmt->body.get(), assembly, symbolTables, stackOffset);

        // Retourner au début de la boucle
        assembly << "    jmp " << startLabel << "\n";

        // Label pour la fin de la boucle
        assembly << endLabel << ":\n";
    }

    /**
     * @brief Génère le code assembleur pour une assignation de variable
     */
    void generateAssignCode(const AssignStmt *assignStmt, std::stringstream &assembly,
                            const std::vector<std::unordered_map<std::string, int>> &symbolTables) const
    {
        if (!assignStmt || !assignStmt->expr)
            return;

        // Aller dora ou est la variable
        std::string varName = *assignStmt->var.value; // le nom de la variable
        int offset = -1;

        // Chercher la variable dans toutes les tables de symboles (de la plus récente à la plus ancienne)
        for (int i = symbolTables.size() - 1; i >= 0; i--)
        {
            if (symbolTables[i].find(varName) != symbolTables[i].end())
            {
                offset = symbolTables[i].at(varName);
                break;
            }
        }

        if (offset == -1)
        {
            std::cerr << "Erreur: Variable non déclarée PTN FAIT GAFFE NE REFFAIT PLUS JAMAIS CA !!!! : " << varName << std::endl;
            return;
        }

        // Générer le code pour l'expression
        generateExpressionCode(assignStmt->expr, assembly, symbolTables);

        // Stocker le résultat dans la variable
        assembly << "    mov [rbp-" << offset << "], rax\n";
    }

    /**
     * @brief Génère le code pour une instruction print
     */

    // CETTE partie j'ai rien fait par moi meme yoo l'assembleur c'est chaud ca GM
    void generatePrintCode(const PrintStmt *printStmt, std::stringstream &assembly,
                           const std::vector<std::unordered_map<std::string, int>> &symbolTables) const
    {
        if (!printStmt || !printStmt->expr)
            return;

        // Compteur statique pour garantir l'unicité des labels
        static int printCounter = 0;
        std::string positiveLabel = ".print_positive_" + std::to_string(printCounter);
        std::string convertLabel = ".convert_loop_" + std::to_string(printCounter);
        printCounter++; // Incrémenter pour le prochain appel

        // Générer le code pour l'expression (résultat dans rax)
        generateExpressionCode(printStmt->expr, assembly, symbolTables);

        // Ajouter le code pour convertir un entier en chaîne et l'afficher
        assembly << "    ; Convertir et afficher l'entier\n";

        // Allouer de l'espace sur la pile pour stocker la chaîne (21 octets pour un int64 max)
        assembly << "    sub rsp, 32\n";          // Réserver 32 octets
        assembly << "    mov rcx, rsp\n";         // Pointeur vers le buffer
        assembly << "    add rcx, 31\n";          // Pointer à la fin du buffer
        assembly << "    mov byte [rcx], 0x0A\n"; // Ajouter un saut de ligne
        assembly << "    dec rcx\n";              // Déplacer le pointeur en arrière

        // Sauvegarder rax (la valeur à afficher)
        assembly << "    mov r10, rax\n"; // Copier la valeur à afficher

        // Vérifier si le nombre est négatif
        assembly << "    test r10, r10\n";               // Tester si r10 < 0
        assembly << "    jns " << positiveLabel << "\n"; // Si non négatif, sauter
        assembly << "    neg r10\n";                     // Rendre r10 positif
        assembly << "    mov byte [rcx], 0x2D\n";        // Ajouter '-' (ASCII 45)
        assembly << "    dec rcx\n";                     // Déplacer le pointeur en arrière

        // Convertir la valeur absolue
        assembly << positiveLabel << ":\n";
        assembly << "    mov rax, r10\n"; // rax = valeur absolue
        assembly << "    mov r9, 10\n";   // Diviseur = 10

        // Début de la boucle de conversion
        assembly << convertLabel << ":\n";
        assembly << "    xor rdx, rdx\n";               // rdx = 0 (reste)
        assembly << "    div r9\n";                     // rax = quotient, rdx = reste
        assembly << "    add dl, '0'\n";                // Convertir en caractère ASCII
        assembly << "    mov [rcx], dl\n";              // Stocker dans le buffer
        assembly << "    dec rcx\n";                    // Déplacer le pointeur en arrière
        assembly << "    test rax, rax\n";              // Vérifier si on a terminé
        assembly << "    jnz " << convertLabel << "\n"; // Si quotient != 0, continuer

        // Calculer la longueur de la chaîne
        assembly << "    lea rsi, [rcx+1]\n"; // Adresse du premier caractère
        assembly << "    mov rdx, rsp\n";
        assembly << "    add rdx, 31\n";  // Adresse du dernier caractère
        assembly << "    sub rdx, rcx\n"; // Calculer la longueur

        // Afficher la chaîne avec l'appel système write
        assembly << "    mov rax, 1\n"; // syscall write
        assembly << "    mov rdi, 1\n"; // stdout
        assembly << "    syscall\n";

        // Libérer l'espace sur la pile
        assembly << "    add rsp, 32\n";
    }
    // Je suis fatigué mais je dois au moins finir ca travaille chatGPT sur cette partie hh
    /**
     * @brief Génère le code pour une assignation de tableau
     */
    void generateArrayAssignCode(const ArrayAssignStmt *stmt, std::stringstream &assembly,
                                 const std::vector<std::unordered_map<std::string, int>> &symbolTables) const
    {
        // Générer le code pour la valeur à assigner
        generateExpressionCode(stmt->value, assembly, symbolTables);
        assembly << "    push rax\n"; // Sauvegarder la valeur

        // Générer le code pour l'accès au tableau
        generateExpressionCode(stmt->array, assembly, symbolTables);
        assembly << "    push rax\n"; // Sauvegarder l'adresse du tableau

        // Générer le code pour l'indice
        generateExpressionCode(stmt->index, assembly, symbolTables);

        // Ajouter 1 à l'indice pour le décalage de la taille
        assembly << "    add rax, 1\n";
        assembly << "    imul rax, 8\n"; // Multiplier par 8 octets

        // Calculer l'adresse cible
        assembly << "    pop rbx\n";      // Récupérer l'adresse du tableau
        assembly << "    add rbx, rax\n"; // Calculer l'adresse de l'élément

        // Stocker la valeur
        assembly << "    pop rax\n";        // Récupérer la valeur
        assembly << "    mov [rbx], rax\n"; // Stocker la valeur
    }
    /**
     * @brief Programme à compiler
     */
    const Program m_program;
};