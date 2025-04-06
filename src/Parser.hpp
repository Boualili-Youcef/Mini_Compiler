#pragma once

#include "Tokenizer.hpp"

struct NodeExpression
{
    Token INT_LITERAL;
};
struct NodeExit
{
    NodeExpression expression;
};

class Parser
{
public:
    Parser(const std::vector<Token> &token) : m_tokens(token) {}

    // Idée ici c'est que le Parser c'est presque le meme que le Tokenizer
    // mais au lieu de passer caractere par caractere
    // on va passer token par token

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
    std::vector<Token> m_tokens;
};