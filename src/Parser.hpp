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
    INTEGER,  // Littéral entier (ex: 42)
    VARIABLE, // Référence à une variable (ex: count)
    BINARY    // Expression binaire (ex: a + b)
};

/**
 * @brief Types d'instructions supportées
 */
enum class StmtType
{
    EXIT, // Instruction exit(expr)
    LET,  // Affectation let var = expr
    BLOCK // Bloc d'instructions
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
    MOD
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
            std::cerr << "Erreur: Un { est attendu" << std::endl;
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
        return parseAddition();
    }

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

    std::optional<std::shared_ptr<Expr>> parseSemiParenth()
    {
        if (m_position < m_tokens.size())
        {
            if (m_tokens[m_position].type == TokenType::INT_LITERAL)
            {
                auto intExpr = std::make_shared<IntExpr>(m_tokens[m_position]);
                m_position++;
                return intExpr;
            }
            else if (m_tokens[m_position].type == TokenType::IDENTIFIER)
            {
                auto varExpr = std::make_shared<VarExpr>(m_tokens[m_position]);
                m_position++;
                return varExpr;
            }
            else if (m_tokens[m_position].type == TokenType::LPARENTHESIS)
            {
                m_position++; //  je skip '('
                auto expr = parseExpression();
                if (!expr)
                    return std::nullopt;

                if (m_position >= m_tokens.size() || m_tokens[m_position].type != TokenType::RPARENTHESIS)
                {
                    std::cerr << "Erreur: Un ) est attendu" << std::endl;
                    return std::nullopt;
                }
                m_position++; // je skip ')'
                return expr;
            }
        }

        std::cerr << "Erreur: Expression attendue" << std::endl;
        return std::nullopt;
    }
    std::vector<Token> m_tokens; ///< Vecteur des tokens à analyser
    size_t m_position;           ///< Position actuelle dans le flux de tokens
};