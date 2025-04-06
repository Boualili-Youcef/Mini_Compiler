#pragma once

#include "Tokenizer.hpp"

/**
 * @brief Structure représentant une expression avec un littéral entier
 */
struct NodeExpression
{
    Token INT_LITERAL;
};

/**
 * @brief Structure représentant un nœud de type "exit" dans l'AST
 */
struct NodeExit
{
    NodeExpression expression;
};

/**
 * @brief Classe responsable de l'analyse syntaxique
 * 
 * Cette classe prend une séquence de tokens générée par le Tokenizer
 * et construit un arbre syntaxique abstrait (AST).
 */
class Parser
{
public:
    /**
     * @brief Constructeur du Parser
     * @param token Vecteur de tokens à analyser
     */
    Parser(const std::vector<Token> &token) : m_tokens(token) {}

    // Idée ici c'est que le Parser c'est presque le meme que le Tokenizer
    // mais au lieu de passer caractere par caractere
    // on va passer token par token

    /**
     * @brief Analyse les tokens pour produire un nœud de type exit
     * @return std::optional<NodeExit> Le nœud exit ou nullopt en cas d'erreur
     */
    std::optional<NodeExit> parseExit() const
    {
        NodeExit node;
        int position = 0;
        // On va parser le token EXIT
        // et on va s'assurer qu'il est bien suivi d'un INT_LITERAL
        // et d'un ;
        if (position < m_tokens.size() && m_tokens[position].type == TokenType::EXIT)
        {
            position++;
            if (position < m_tokens.size() && m_tokens[position].type == TokenType::INT_LITERAL)
            {
                node.expression.INT_LITERAL = m_tokens[position];
                position++;
                if (position < m_tokens.size() && m_tokens[position].type == TokenType::SEMICOLON)
                {
                    position++;
                    return node;
                }
                else
                {
                    std::cerr << "Erreur: Un ; est attendu aprés le INT_LITTERAL " << position << std::endl;
                    return std::nullopt;
                }
            }
            else
            {
                std::cerr << "Erreur: Un INT_LITERAL et attendu aprés le EXIT " << position << std::endl;
                return std::nullopt;
            }
        }
        else
        {
            std::cerr << "Erreur: Un EXIT est attendu " << position << std::endl;
            return std::nullopt;
        }
    }

    /**
     * @brief Analyse les tokens pour produire un nœud d'expression
     * @param position Position actuelle dans le flux de tokens (modifiée par la fonction)
     * @return std::optional<NodeExpression> Le nœud d'expression ou nullopt en cas d'erreur
     */
    std::optional<NodeExpression> parseExpression(int &position) const
    {
        NodeExpression node;
        if (position + 1 < m_tokens.size() && m_tokens[position + 1].type == TokenType::INT_LITERAL)
        {
            node.INT_LITERAL = m_tokens[position];
            position++;
            return node;
        }
        else
        {
            std::cerr << "Erreur: Un EXIT est attendu " << position << std::endl;
            return std::nullopt;
        }
    }

private:
    /**
     * @brief Vecteur des tokens à analyser
     */
    std::vector<Token> m_tokens;
};