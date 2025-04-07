#pragma once

#include "Tokenizer.hpp"
#include <memory>
#include <vector>
#include <optional>
#include <iostream>

/**
 * @brief Classe représentant un programme contenant des instructions.
 *
 * Ce programme est construit à partir de tokens (des morceaux de texte analysés, comme des mots-clés ou des nombres),
 * et il est composé de trois éléments principaux :
 * - **Les expressions** : Ce sont les valeurs que nous utilisons dans les instructions. Elles peuvent être soit un nombre entier (`INT_LITERAL`), soit une référence à une variable (`IDENTIFIER`).
 * - **Les instructions** : Ce sont des actions que le programme doit exécuter. Actuellement, nous avons deux types d'instructions :
 *   - **ExitStmt** : Correspond à une instruction `exit(expr)` qui termine l'exécution d'un programme avec une expression donnée.
 *   - **LetStmt** : Correspond à une instruction `let var = expr` qui permet d'affecter une valeur à une variable.
 * - **Le programme** : Un programme est une collection d'instructions qui sont exécutées les unes après les autres. Nous créons ce programme en analysant une liste de tokens.
 *
 * Le **Parser** est responsable de la lecture des tokens un par un et de la construction de ce programme.
 * Lorsqu'il rencontre un token `EXIT`, il va chercher l'expression associée (qui peut être un `INT_LITERAL` ou une `IDENTIFIER`) et l'ajouter à une instruction `ExitStmt`.
 * Ensuite, cette instruction est ajoutée au programme. Si un autre token correspond à `LET`, il fera de même, mais pour l'instruction `LetStmt`.
 *
 * À la fin, le **Parser** retourne un programme complet qui peut être exécuté, et ce programme contient toutes les instructions analysées à partir des tokens.
 *
 * ### Exemple : `exit(7);`
 *
 * Voici comment cela se passe pour l'instruction `exit(7);` :
 *
 * 1. Le **Parser** commence avec la liste de tokens obtenue à partir du code source. Dans ce cas, la liste pourrait ressembler à :
 *    - `TokenType::EXIT`, `TokenType::LPARENTHESIS`, `TokenType::INT_LITERAL(7)`, `TokenType::RPARENTHESIS`, `TokenType::SEMICOLON`
 *
 * 2. Il commence à analyser le premier token. Ici, il rencontre `TokenType::EXIT`, ce qui lui indique qu'il s'agit d'une instruction `exit`.
 *    Il passe donc à l'analyse de l'expression qui suit.
 *
 * 3. Ensuite, le **Parser** s'attend à une parenthèse ouvrante `(` (token `TokenType::LPARENTHESIS`). Lorsqu'il la trouve, il avance au token suivant.
 *
 * 4. Le **Parser** rencontre ensuite un **token `INT_LITERAL`** (valeur `7`). Cela signifie que l'expression à l'intérieur de la parenthèse est un nombre entier.
 *
 * 5. Le **Parser** crée alors une instance de `IntExpr` (une expression entière) avec la valeur `7` et passe au token suivant.
 *
 * 6. Le **Parser** vérifie ensuite qu'il y a une parenthèse fermante `)` (token `TokenType::RPARENTHESIS`) et continue si elle est présente.
 *
 * 7. Il s'attend enfin à un point-virgule `;` pour terminer l'instruction, et lorsqu'il le trouve, il termine l'analyse de cette instruction.
 *
 * 8. Le **Parser** crée une instruction `ExitStmt` avec l'expression entière (7), puis l'ajoute au programme.
 *
 * En résumé, le **Parser** a transformé `exit(7);` en une instruction de type `ExitStmt` contenant une expression de type `IntExpr` avec la valeur `7`.
 *
 * Ce modèle permet de construire un programme à partir de simples instructions écrites sous forme de texte,
 * et de les analyser de manière structurée pour les exécuter ensuite.
 */

/**
 * @brief Types d'expressions supportées
 */
enum class ExprType
{
    INTEGER,      // Littéral entier (ex: 42)
    VARIABLE,     // Référence à une variable (ex: count)
    BINARY,       // Expression binaire (ex: a + b)
    ARRAY,        // Expression de tableau (ex: array[0])
    ARRAY_ACCESS, // Accès à un élément de tableau (ex: array[0])
    LENGTH,

};

/**
 * @brief Types d'instructions supportées
 */
enum class StmtType
{
    EXIT,         // Instruction exit(expr)
    LET,          // Affectation let var = expr
    BLOCK,        // Bloc d'instructions
    IF,           // Instruction conditionnelle if(condition) { ... }
    ELES,         // Instruction else
    WHILE,        // Instruction de boucle while(condition) { ... }
    ASSIGN,       // Affectation var = expr
    PRINT,        // Instruction d'affichage print(expr)
    ARRAY_ASSIGN, // Affectation d'un élément de tableau array[index] = expr
};

/**
 * @brief Types d'opérations binaires supportées
 */
enum class BinaryOpType
{
    ADD,
    MUL,
    SUB,
    DIV,
    MOD,
    EQUAL,
    GREAT,
    LESS,
    GREAT_EQUAL,
    LESS_EQUAL,
    AND,
    OR,
    NOT_EQUAL,
};

/**
 * @brief Classe de base pour toutes les expressions
 */
struct Expr
{
    virtual ~Expr() = default;
    virtual ExprType getType() const = 0;
};

/**
 * @brief Expression de type littéral entier (ex: 42)
 */
struct IntExpr : public Expr
{
    Token token;

    IntExpr(Token token) : token(token) {}
    ExprType getType() const override { return ExprType::INTEGER; }
};

/**
 * @brief Expression de type variable (ex: count)
 */
struct VarExpr : public Expr
{
    Token token;

    VarExpr(Token token) : token(token) {}
    ExprType getType() const override { return ExprType::VARIABLE; }
};

/**
 * @brief Expression binaire (ex: a + b)
 */
struct BinaryExpr : public Expr
{
    std::shared_ptr<Expr> gauche;
    std::shared_ptr<Expr> droite;
    BinaryOpType op;

    BinaryExpr(std::shared_ptr<Expr> gauche, BinaryOpType op, std::shared_ptr<Expr> droite)
        : gauche(gauche), droite(droite), op(op) {}

    ExprType getType() const override { return ExprType::BINARY; }
};

/**
 * @brief Expression de type tableau (ex: array[0])
 */
struct ArrayExpr : public Expr
{
    std::vector<std::shared_ptr<Expr>> elements;
    // Explication :
    // elements est un vecteur de pointeurs partagés vers des expressions.
    // Cela signifie que chaque élément du tableau peut être une expression entier, variable, et et oui meme une expression binaire
    ArrayExpr(std::vector<std::shared_ptr<Expr>> elements) : elements(elements) {}
    ExprType getType() const override { return ExprType::ARRAY; }
};

/**
 * @brief Expression d'accès à un élément de tableau (ex: array[0])
 */
struct ArrayAccessExpr : public Expr
{
    std::shared_ptr<Expr> array; // pointeur vers le tableau
    std::shared_ptr<Expr> index; // le nom le dit non

    ArrayAccessExpr(std::shared_ptr<Expr> array, std::shared_ptr<Expr> index)
        : array(array), index(index) {}

    ExprType getType() const override { return ExprType::ARRAY_ACCESS; }
};

// len fait une action donc c'est une expr
struct LengthExpr : public Expr
{
    std::shared_ptr<Expr> array;

    LengthExpr(std::shared_ptr<Expr> array) : array(array) {}

    ExprType getType() const override { return ExprType::LENGTH; }
};

/**
 * @brief Classe de base pour toutes les instructions
 */
struct Stmt
{
    virtual ~Stmt() = default;
    virtual StmtType getType() const = 0;
};

/**
 * @brief Instruction exit(expr)
 */
// Vive le polymorphisme
struct ExitStmt : public Stmt
{
    std::shared_ptr<Expr> expr;

    ExitStmt(std::shared_ptr<Expr> expr) : expr(expr) {}
    StmtType getType() const override { return StmtType::EXIT; }
};

/**
 * @brief Instruction let var = expr
 */
// Vive le polymorphisme
struct LetStmt : public Stmt
{
    Token var;
    std::shared_ptr<Expr> expr;

    LetStmt(Token var, std::shared_ptr<Expr> expr) : var(var), expr(expr) {}
    StmtType getType() const override { return StmtType::LET; }
};

/**
 * @brief Instruction de type bloc
 */

struct BlockStmt : public Stmt
{
    // C'est un vecteur d'instructions qui vont etre dans le bloc
    std::vector<std::shared_ptr<Stmt>> statements;

    BlockStmt(std::vector<std::shared_ptr<Stmt>> statements) : statements(statements) {}
    StmtType getType() const override { return StmtType::BLOCK; }
};

struct IfStmt : public Stmt
{
    std::shared_ptr<Expr> condition;
    std::shared_ptr<BlockStmt> thenBranch;
    std::shared_ptr<BlockStmt> elseBranch;

    IfStmt(std::shared_ptr<Expr> condition, std::shared_ptr<BlockStmt> thenBranch, std::shared_ptr<BlockStmt> elseBranch = nullptr)
        : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}

    StmtType getType() const override { return StmtType::IF; }
};

struct ElseStmt : public Stmt
{
    std::shared_ptr<BlockStmt> elseBranch;

    ElseStmt(std::shared_ptr<BlockStmt> elseBranch) : elseBranch(elseBranch) {}

    StmtType getType() const override { return StmtType::ELES; }
};

struct WhileStmt : public Stmt
{
    std::shared_ptr<Expr> condition;
    std::shared_ptr<BlockStmt> body;

    WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<BlockStmt> body)
        : condition(condition), body(body) {}

    StmtType getType() const override { return StmtType::WHILE; }
};

struct AssignStmt : public Stmt
{
    Token var;
    std::shared_ptr<Expr> expr;

    AssignStmt(Token var, std::shared_ptr<Expr> expr) : var(var), expr(expr) {}
    StmtType getType() const override { return StmtType::ASSIGN; }
};

struct PrintStmt : public Stmt
{
    std::shared_ptr<Expr> expr;

    PrintStmt(std::shared_ptr<Expr> expr) : expr(expr) {}
    StmtType getType() const override { return StmtType::PRINT; }
};

struct ArrayAssignStmt : public Stmt
{
    std::shared_ptr<Expr> array;
    std::shared_ptr<Expr> index;
    std::shared_ptr<Expr> value;

    ArrayAssignStmt(std::shared_ptr<Expr> array, std::shared_ptr<Expr> index, std::shared_ptr<Expr> value)
        : array(array), index(index), value(value) {}

    StmtType getType() const override { return StmtType::ARRAY_ASSIGN; }
};

/**
 * @brief Programme complet c'est une liste d'instructions donc vecteur de statements
 */
struct Program
{
    std::vector<std::shared_ptr<Stmt>> statements;

    void addStatement(std::shared_ptr<Stmt> stmt)
    {
        statements.push_back(stmt);
    }
};

/**
 * @brief Classe responsable de l'analyse syntaxique
 */
class Parser
{
public:
    /**
     * @brief Constructeur du Parser
     * @param tokens Vecteur de tokens à analyser
     */
    Parser(const std::vector<Token> &tokens) : m_tokens(tokens), m_position(0) {}

    /**
     * @brief Analyse tous les tokens pour produire un programme complet
     * @return std::optional<Program> Le programme ou nullopt en cas d'erreur
     */
    std::optional<Program> parse()
    {
        Program program;
        m_position = 0;

        while (m_position < m_tokens.size())
        {
            auto stmt = parseStatement();
            if (!stmt)
                return std::nullopt;
            program.addStatement(stmt.value());
        }

        return program;
    }

private:
    // TODO: a deleter
    std::string toString(ExprType type)
    {
        switch (type)
        {
        case ExprType::INTEGER:
            return "INTEGER";
        case ExprType::VARIABLE:
            return "VARIABLE";
        default:
            return "UNKNOWN";
        }
    }

    /**
     * @brief Analyse une seule instruction
     * @return std::optional<std::shared_ptr<Stmt>> L'instruction ou nullopt en cas d'erreur
     */
    std::optional<std::shared_ptr<Stmt>> parseStatement()
    {
        // TODO : Commence a grossir donc vaut mieux un switch
        // Analyser le type d'instruction
        if (m_position < m_tokens.size() && m_tokens[m_position].type == TokenType::EXIT)
        {
            return parseExitStmt();
        }
        else if (m_position < m_tokens.size() && m_tokens[m_position].type == TokenType::LET)
        {
            return parseLetStmt();
        }
        else if (m_position < m_tokens.size() && m_tokens[m_position].type == TokenType::LBRACE)
        {
            return parseBlockStmt();
        }
        else if (m_position < m_tokens.size() && m_tokens[m_position].type == TokenType::IF)
        {
            return parseIfStmt();
        }
        else if (m_position < m_tokens.size() && m_tokens[m_position].type == TokenType::WHILE)
        {
            return parseWhileStmt();
        }
        else if (m_position < m_tokens.size() && m_tokens[m_position].type == TokenType::IDENTIFIER && m_position + 1 < m_tokens.size() && m_tokens[m_position + 1].type == TokenType::EQUAL)
        {
            return parseAssignStmt();
        }
        else if (m_position < m_tokens.size() && m_tokens[m_position].type == TokenType::PRINT)
        {
            return parsePrintStmt();
        }
        else if(m_position < m_tokens.size() && m_tokens[m_position].type == TokenType::IDENTIFIER){
            return parseArrayAssignStmt();
        
        }
        else
        {
            std::cerr << "Erreur: Instruction non reconnue" << std::endl;
            return std::nullopt;
        }
    }

    /**
     * @brief Analyse les tokens pour produire une instruction exit
     * @return std::optional<std::shared_ptr<ExitStmt>> L'instruction exit ou nullopt en cas d'erreur
     */

    std::optional<std::shared_ptr<ExitStmt>> parseExitStmt()
    {
        // Vérifier le token 'exit'
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::EXIT)
        {
            std::cerr << "Erreur: Un EXIT est attendu" << std::endl;
            return std::nullopt;
        }
        m_position++;

        // Vérifier la parenthèse ouvrante
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::LPARENTHESIS)
        {
            std::cerr << "Erreur: Un ( est attendu après le EXIT" << std::endl;
            return std::nullopt;
        }
        m_position++;

        // Analyser l'expression
        auto expr = parseExpression();
        if (!expr)
        {
            return std::nullopt;
        }

        // Vérifier la parenthèse fermante
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::RPARENTHESIS)
        {
            std::cerr << "Erreur: Un ) est attendu après l'expression" << std::endl;
            return std::nullopt;
        }
        m_position++;

        // Vérifier le point-virgule
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::SEMICOLON)
        {
            std::cerr << "Erreur: Un ; est attendu à la fin de l'instruction" << std::endl;
            return std::nullopt;
        }
        m_position++;
        std::cout << toString(expr.value()->getType()) << std::endl;
        return std::make_shared<ExitStmt>(expr.value());
    }

    /**
     * @brief Analyse les tokens pour produire une instruction if
     * @return std::optional<std::shared_ptr<IfStmt>> L'instruction if ou nullopt en cas d'erreur
     */
    std::optional<std::shared_ptr<IfStmt>> parseIfStmt()
    {
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::IF)
        {
            std::cerr << "Erreur: Un IF est attendu" << std::endl;
            return std::nullopt;
        }
        m_position++;
        // Vérifier la parenthèse ouvrante
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::LPARENTHESIS)
        {
            std::cerr << "Erreur: Un ( est attendu après le IF" << std::endl;
            return std::nullopt;
        }
        m_position++;
        // Analyser l'expression
        auto expr = parseExpression();
        if (!expr)
        {
            return std::nullopt;
        }
        // Vérifier la parenthèse fermante
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::RPARENTHESIS)
        {
            std::cerr << "Erreur: Un ) est attendu après l'expression" << std::endl;
            return std::nullopt;
        }
        m_position++;
        // Vérifier le bloc
        auto block = parseBlockStmt();
        if (!block)
        {
            return std::nullopt;
        }

        // Je verifie si on a un else
        if (m_position < m_tokens.size() && m_tokens[m_position].type == TokenType::ELSE)
        {
            auto elseStmt = parseElseStmt();
            if (!elseStmt)
            {
                return std::nullopt;
            }
            return std::make_shared<IfStmt>(expr.value(), block.value(), elseStmt.value());
        }

        return std::make_shared<IfStmt>(expr.value(), block.value());
    }

    /**
     * @brief Analyse les tokens pour produire une instruction else
     * @return std::optional<std::shared_ptr<ElseStmt>> L'instruction else ou nullopt en cas d'erreur
     */
    std::optional<std::shared_ptr<BlockStmt>> parseElseStmt()
    {
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::ELSE)
        {
            std::cerr << "Erreur: Un ELSE est attendu" << std::endl;
            return std::nullopt;
        }
        m_position++;
        if (m_position < m_tokens.size() && m_tokens[m_position].type == TokenType::IF)
        {
            auto ifStmt = parseIfStmt();
            if (!ifStmt)
            {
                return std::nullopt;
            }
            std::vector<std::shared_ptr<Stmt>> statements;
            statements.push_back(ifStmt.value());
            return std::make_shared<BlockStmt>(statements);
        }
        // Vérifier le bloc
        auto block = parseBlockStmt();
        if (!block)
        {
            return std::nullopt;
        }
        return block;
    }

    std::optional<std::shared_ptr<WhileStmt>> parseWhileStmt()
    {
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::WHILE)
        {
            std::cerr << "Erreur: Un WHILE est attendu" << std::endl;
            return std::nullopt;
        }
        m_position++;
        // Je verifie si on a une parenthese ouvrante
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::LPARENTHESIS)
        {
            std::cerr << "Erreur: Un ( est attendu apres le WHILE" << std::endl;
            return std::nullopt;
        }
        m_position++;
        // Analyser l'expression
        auto expr = parseExpression();
        if (!expr)
        {
            return std::nullopt;
        }
        // Je vérifie la parenthèse fermante
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::RPARENTHESIS)
        {
            std::cerr << "Erreur: Un ) est attendu après l'expression" << std::endl;
            return std::nullopt;
        }
        m_position++;
        // LE bloc
        auto block = parseBlockStmt();
        if (!block)
        {
            return std::nullopt;
        }
        return std::make_shared<WhileStmt>(expr.value(), block.value());
    }

    /**
     * @brief Analyse les tokens pour produire une instruction let
     * @return std::optional<std::shared_ptr<LetStmt>> L'instruction let ou nullopt en cas d'erreur
     */
    std::optional<std::shared_ptr<LetStmt>> parseLetStmt()
    {
        // Vérifier le token 'let'
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::LET)
        {
            std::cerr << "Erreur: Un LET est attendu" << std::endl;
            return std::nullopt;
        }
        m_position++;

        // Vérifier l'identifiant de la variable
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::IDENTIFIER)
        {
            std::cerr << "Erreur: Un IDENTIFIER est attendu après le LET" << std::endl;
            return std::nullopt;
        }
        auto var = m_tokens[m_position];
        m_position++;

        // Vérifier le signe égal
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::EQUAL)
        {
            std::cerr << "Erreur: Un = est attendu après le LET" << std::endl;
            return std::nullopt;
        }
        m_position++;

        // Analyser l'expression
        auto expr = parseExpression();
        if (!expr)
        {
            return std::nullopt;
        }

        // Vérifier le point-virgule
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::SEMICOLON)
        {
            std::cerr << "Erreur: Un ; est attendu à la fin de l'instruction" << std::endl;
            return std::nullopt;
        }
        m_position++;

        return std::make_shared<LetStmt>(var, expr.value());
    }

    std::optional<std::shared_ptr<BlockStmt>> parseBlockStmt()
    {
        // On vérifie si on a un '{'
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::LBRACE)
        {
            std::cerr << "Erreur: Un { est attendu a " << m_position << std::endl;
            return std::nullopt;
        }
        // hop on récupere toutes les instructions entre les accolades
        m_position++;
        std::vector<std::shared_ptr<Stmt>> statements;
        while (m_position < m_tokens.size() && m_tokens[m_position].type != TokenType::RBRACE)
        {
            auto stmt = parseStatement();
            if (!stmt)
                return std::nullopt;
            statements.push_back(stmt.value());
        }
        // On vérifie si on a un '}'
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::RBRACE)
        {
            std::cerr << "Erreur: Un } est attendu" << std::endl;
            return std::nullopt;
        }
        m_position++;
        return std::make_shared<BlockStmt>(statements);
    }

    /**
     * @brief Analyse les tokens pour produire une expression
     * @return std::optional<std::shared_ptr<Expr>> L'expression ou nullopt en cas d'erreur
     */

    // 2 + 5 * 3 + 7

    std::optional<std::shared_ptr<Expr>> parseExpression()
    {
        return parseLogicalConOR();
    }

    std::optional<std::shared_ptr<Expr>> parseLogicalConOR()
    {
        auto left = parseLogicalConAND();
        if (!left)
            return std::nullopt;

        while (m_position < m_tokens.size() && (m_tokens[m_position].type == TokenType::OR))
        {
            TokenType operatorType = m_tokens[m_position].type;
            m_position++;
            auto right = parseLogicalConAND();
            if (!right)
                return std::nullopt;
            left = std::make_shared<BinaryExpr>(left.value(), BinaryOpType::OR, right.value());
        }

        return left;
    }

    /**
     * @brief Analyse les tokens pour produire une expression de comparaison
     * @return std::optional<std::shared_ptr<Expr>> L'expression de comparaison ou nullopt en cas d'erreur
     * @note Actuellement, seules les égalités sont gérées
     *       (ex: a == b).
     */
    std::optional<std::shared_ptr<Expr>> parseComparison()
    {
        auto left = parseAddition();
        if (!left)
            return std::nullopt;

        while (m_position < m_tokens.size() && (m_tokens[m_position].type == TokenType::EGAL || m_tokens[m_position].type == TokenType::GREAT || m_tokens[m_position].type == TokenType::LESS || m_tokens[m_position].type == TokenType::GREAT_EQUAL || m_tokens[m_position].type == TokenType::LESS_EQUAL || m_tokens[m_position].type == TokenType::NEGAL))
        {
            TokenType operatorType = m_tokens[m_position].type;
            m_position++;
            BinaryOpType binaryOpType;
            if (operatorType == TokenType::EGAL)
                binaryOpType = BinaryOpType::EQUAL;
            else if (operatorType == TokenType::GREAT)
                binaryOpType = BinaryOpType::GREAT;
            else if (operatorType == TokenType::LESS)
                binaryOpType = BinaryOpType::LESS;
            else if (operatorType == TokenType::GREAT_EQUAL)
                binaryOpType = BinaryOpType::GREAT_EQUAL;
            else if (operatorType == TokenType::LESS_EQUAL)
                binaryOpType = BinaryOpType::LESS_EQUAL;
            else if (operatorType == TokenType::NEGAL)
                binaryOpType = BinaryOpType::NOT_EQUAL;
            else
            {
                std::cerr << "Erreur: Opérateur de comparaison non reconnu" << std::endl;
                return std::nullopt;
            }
            auto right = parseAddition();
            if (!right)
                return std::nullopt;

            left = std::make_shared<BinaryExpr>(left.value(), binaryOpType, right.value());
        }

        return left;
    }

    std::optional<std::shared_ptr<Expr>> parseLogicalConAND()
    {
        auto left = parseComparison();
        if (!left)
            return std::nullopt;

        while (m_position < m_tokens.size() && (m_tokens[m_position].type == TokenType::AND))
        {
            TokenType operatorType = m_tokens[m_position].type;
            m_position++;
            auto right = parseComparison();
            if (!right)
                return std::nullopt;
            left = std::make_shared<BinaryExpr>(left.value(), BinaryOpType::AND, right.value());
        }

        return left;
    }

    /**
     * @brief Analyse les tokens pour produire une expression d'addition
     * @return std::optional<std::shared_ptr<Expr>> L'expression d'addition ou nullopt en cas d'erreur
     */
    std::optional<std::shared_ptr<Expr>> parseAddition()
    {
        auto left = parseMultiplication();
        if (!left)
            return std::nullopt;

        while (m_position < m_tokens.size() && m_tokens[m_position].type == TokenType::PLUS || m_tokens[m_position].type == TokenType::MINUS)
        {
            // Y au Bug marrant ici je le comprend pas tres bien la ca marche mais
            // si n'utilise pas le operatorType et je met m_position + 1 a la fin ca merde
            TokenType operatorType = m_tokens[m_position].type;
            m_position++;
            auto right = parseMultiplication();
            if (!right)
                return std::nullopt;

            if (operatorType == TokenType::MINUS)
                left = std::make_shared<BinaryExpr>(left.value(), BinaryOpType::SUB, right.value());
            else if (operatorType == TokenType::PLUS)
                left = std::make_shared<BinaryExpr>(left.value(), BinaryOpType::ADD, right.value());
        }

        return left;
    }

    /**
     * @brief Analyse les tokens pour produire une expression de multiplication
     * @return std::optional<std::shared_ptr<Expr>> L'expression de multiplication ou nullopt en cas d'erreur
     */

    std::optional<std::shared_ptr<Expr>> parseMultiplication()
    {
        auto left = parseSemiParenth();
        if (!left)
            return std::nullopt;
        while (m_position < m_tokens.size() && (m_tokens[m_position].type == TokenType::STAR || m_tokens[m_position].type == TokenType::DIVIDE || m_tokens[m_position].type == TokenType::STAR || m_tokens[m_position].type == TokenType::MODULO))
        {
            TokenType operatorType = m_tokens[m_position].type;
            m_position++;
            auto right = parseSemiParenth();
            if (!right)
                return std::nullopt;

            if (operatorType == TokenType::DIVIDE)
                left = std::make_shared<BinaryExpr>(left.value(), BinaryOpType::DIV, right.value());
            else if (operatorType == TokenType::MODULO)
                left = std::make_shared<BinaryExpr>(left.value(), BinaryOpType::MOD, right.value());
            else if (operatorType == TokenType::STAR)
                left = std::make_shared<BinaryExpr>(left.value(), BinaryOpType::MUL, right.value());
        }
        return left;
    }

    /**
     * @brief Analyse les tokens pour produire une expression entre parenthèses
     * @return std::optional<std::shared_ptr<Expr>> L'expression ou nullopt en cas d'erreur
     * exemple: (a + b) ou [1, 2, 3]
     */
    std::optional<std::shared_ptr<Expr>> parseSemiParenth()
    {
        if (m_position < m_tokens.size())
        {
            if (m_tokens[m_position].type == TokenType::LBRACKET)
            {
                return parseArray();
            }
            // Cas d'un entier
            else if (m_tokens[m_position].type == TokenType::INT_LITERAL)
            {
                auto intExpr = std::make_shared<IntExpr>(m_tokens[m_position]);
                m_position++;
                return intExpr;
            }
            // Cas d'une variable ou accès à un tableau
            else if (m_tokens[m_position].type == TokenType::IDENTIFIER)
            {
                auto varExpr = std::make_shared<VarExpr>(m_tokens[m_position]);
                m_position++;

                // Vérifier si on a un accès à un tableau: arr[index]
                if (m_position < m_tokens.size() && m_tokens[m_position].type == TokenType::LBRACKET)
                {
                    m_position++; // Consommer le '['

                    auto indexExpr = parseExpression();
                    if (!indexExpr)
                        return std::nullopt;

                    if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::RBRACKET)
                    {
                        std::cerr << "Erreur: Un ']' est attendu" << std::endl;
                        return std::nullopt;
                    }

                    m_position++; // Consommer le ']'
                    return std::make_shared<ArrayAccessExpr>(varExpr, indexExpr.value());
                }

                return varExpr;
            }
            else if (m_tokens[m_position].type == TokenType::LENGTH)
            {
                m_position++; // Consommer 'len'

                // Vérifier la parenthèse ouvrante
                if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::LPARENTHESIS)
                {
                    std::cerr << "Erreur: Un ( est attendu après len" << std::endl;
                    return std::nullopt;
                }
                m_position++; // Consommer '('

                // Parser l'expression du tableau
                auto arrayExpr = parseExpression();
                if (!arrayExpr)
                    return std::nullopt;

                // Vérifier la parenthèse fermante
                if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::RPARENTHESIS)
                {
                    std::cerr << "Erreur: Un ) est attendu après l'argument de len()" << std::endl;
                    return std::nullopt;
                }
                m_position++; // Consommer ')'

                return std::make_shared<LengthExpr>(arrayExpr.value());
            }
            // Cas d'une expression entre parenthèses
            else if (m_tokens[m_position].type == TokenType::LPARENTHESIS)
            {
                m_position++; // Consommer '('
                auto expr = parseExpression();
                if (!expr)
                    return std::nullopt;

                if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::RPARENTHESIS)
                {
                    std::cerr << "Erreur: Un ) est attendu" << std::endl;
                    return std::nullopt;
                }
                m_position++; // Consommer ')'
                return expr;
            }
        }

        std::cerr << "Erreur: Expression attendue" << std::endl;
        return std::nullopt;
    }

    std::optional<std::shared_ptr<Expr>> parseArray()
    {
        // Vérifier et consommer le crochet ouvrant '['
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::LBRACKET)
        {
            std::cerr << "Erreur: Un [ est attendu" << std::endl;
            return std::nullopt;
        }
        m_position++; // Consommer '['

        std::vector<std::shared_ptr<Expr>> elements;

        // Cas du tableau vide []
        if (m_position < m_tokens.size() && m_tokens[m_position].type == TokenType::RBRACKET)
        {
            m_position++; // Consommer ']'
            return std::make_shared<ArrayExpr>(elements);
        }

        // Parser les éléments du tableau
        while (true)
        {
            // Parser un élément
            auto expr = parseExpression();
            if (!expr)
                return std::nullopt;

            elements.push_back(expr.value());

            // Vérifier si on a atteint la fin du tableau
            if (m_position >= m_tokens.size())
            {
                std::cerr << "Erreur: Fin de fichier inattendue dans la déclaration du tableau" << std::endl;
                return std::nullopt;
            }

            // Si on trouve ']', c'est la fin du tableau
            if (m_tokens[m_position].type == TokenType::RBRACKET)
            {
                m_position++; // Consommer ']'
                break;
            }

            // Sinon, on doit avoir une virgule
            if (m_tokens[m_position].type != TokenType::COMMA)
            {
                std::cerr << "Erreur: Une virgule est attendue entre les éléments du tableau" << std::endl;
                return std::nullopt;
            }
            m_position++; // Consommer ','
        }

        return std::make_shared<ArrayExpr>(elements);
    }

    std::optional<std::shared_ptr<AssignStmt>> parseAssignStmt()
    {
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::IDENTIFIER)
        {
            std::cerr << "Erreur: Un IDENTIFIER est attendu" << std::endl;
            return std::nullopt;
        }
        auto var = m_tokens[m_position];
        m_position++;
        // Vérifier le signe égal
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::EQUAL)
        {
            std::cerr << "Erreur: Un = est attendu avant " << std::endl;
            return std::nullopt;
        }
        m_position++;
        // Analyser l'expression
        auto expr = parseExpression();
        if (!expr)
        {
            return std::nullopt;
        }
        // Vérifier le point-virgule
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::SEMICOLON)
        {
            std::cerr << "Erreur: Un ; est attendu à la fin de l'instruction" << std::endl;
            return std::nullopt;
        }
        m_position++;
        return std::make_shared<AssignStmt>(var, expr.value());
    }

    std::optional<std::shared_ptr<PrintStmt>> parsePrintStmt()
    {
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::PRINT)
        {
            std::cerr << "Erreur: Un PRINT est attendu" << std::endl;
            return std::nullopt;
        }
        m_position++;
        // parenthese ouvrante (
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::LPARENTHESIS)
        {
            std::cerr << "Erreur: Un ( est attendu après le PRINT" << std::endl;
            return std::nullopt;
        }
        m_position++;
        // Analyser l'expression
        auto expr = parseExpression();
        if (!expr)
        {
            return std::nullopt;
        }
        // parenthese fermante )
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::RPARENTHESIS)
        {
            std::cerr << "Erreur: Un ) est attendu après l'expression" << std::endl;
            return std::nullopt;
        }
        m_position++;
        // Vérifier le point-virgule
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::SEMICOLON)
        {
            std::cerr << "Erreur: Un ; est attendu à la fin de l'instruction" << std::endl;
            return std::nullopt;
        }
        m_position++;
        return std::make_shared<PrintStmt>(expr.value());
    }

    std::optional<std::shared_ptr<Expr>> parseLengthExpr()
    {
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::LENGTH)
            return std::nullopt;
        m_position++;

        // Vérifier (
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::LPARENTHESIS)
        {
            std::cerr << "Erreur: Un ( est attendu après len" << std::endl;
            return std::nullopt;
        }
        m_position++;

        auto arrayExpr = parseExpression();
        if (!arrayExpr)
            return std::nullopt;

        // Vérifier )
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::RPARENTHESIS)
        {
            std::cerr << "Erreur: Un ) est attendu" << std::endl;
            return std::nullopt;
        }
        m_position++;

        return std::make_shared<LengthExpr>(arrayExpr.value());
    }

    std::optional<std::shared_ptr<ArrayAssignStmt>> parseArrayAssignStmt()
    {
        // Sauvegarder la position pour pouvoir revenir en back back
        size_t startPos = m_position;

        // Parser l'identifiant du tableau
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::IDENTIFIER)
            return std::nullopt;

        Token arrayToken = m_tokens[m_position];
        auto array = std::make_shared<VarExpr>(arrayToken);
        m_position++;

        // Vérifier le crochet ouvrant
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::LBRACKET)
        {
            m_position = startPos;
            return std::nullopt;
        }
        m_position++;

        // Parser l'indice
        auto index = parseExpression();
        if (!index)
        {
            m_position = startPos;
            return std::nullopt;
        }

        // Vérifier le crochet fermant ] je suis pas sûr du nom
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::RBRACKET)
        {
            m_position = startPos;
            return std::nullopt;
        }
        m_position++;

        // Vérifier le signe égal
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::EQUAL)
        {
            m_position = startPos;
            return std::nullopt;
        }
        m_position++;

        // Parser la valeur à assigner
        auto value = parseExpression();
        if (!value)
        {
            m_position = startPos;
            return std::nullopt;
        }

        // Vérifier le point-virgule
        if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::SEMICOLON)
        {
            std::cerr << "Erreur: Un ; est attendu après l'assignation" << std::endl;
            return std::nullopt;
        }
        m_position++;

        return std::make_shared<ArrayAssignStmt>(array, index.value(), value.value());
    }

    std::vector<Token> m_tokens; ///< Vecteur des tokens à analyser
    size_t m_position;           ///< Position actuelle dans le flux de tokens
};