#pragma once

#include <string>
#include <sstream>
#include <optional>
#include <vector>
#include <cctype>
#include <iostream>
#include <unordered_map>

/**
 * @file Tokenizer.hpp
 * @brief Définit les structures et classes nécessaires pour l'analyse lexicale.
 *
 * Ce fichier implémente un tokenizer qui transforme le code source en une séquence
 * de tokens utilisables par le parser. Il utilise les principes d'informatique théorique
 * pour réaliser l'analyse lexicale.
 */

/**
 * @enum TokenType
 * @brief Énumère les différents types de tokens reconnus par le tokenizer.
 */
enum class TokenType
{
    EXIT,         /**< Mot clé 'exit' */
    LET,          /**< Mot clé 'let' */
    INT_LITERAL,  /**< Nombre entier littéral */
    SEMICOLON,    /**< Point-virgule ';' */
    IDENTIFIER,   /**< Identifiant (variable, fonction, etc.) */
    LPARENTHESIS, /**< Parenthèse gauche '(' */
    RPARENTHESIS, /**< Parenthèse droite ')' */
    EQUAL,        /**< Signe égal '=' */
    PLUS,         /**< Signe plus '+' */
    STAR,         /**< Signe étoile '*' */
    MINUS,        /**< Signe moins '-' */
    DIVIDE,       /**< Signe division '/' */
    MODULO,       /**< Signe modulo '%' */
    LBRACE,       /**< Accolade gauche '{' */
    RBRACE,       /**< Accolade droite '}' */
    IF,           /**< Mot clé 'if' */
    EGAL,         /**< Signe égal '==' */
    NEGAL,        /**< Signe différent '!=' */
    GREAT,        /**< Signe supérieur '>' */
    LESS,         /**< Signe inférieur '<' */
    GREAT_EQUAL,  /**< Signe supérieur ou égal '>=' */
    LESS_EQUAL,   /**< Signe inférieur ou égal '<=' */
    ELSE,         /**< Mot clé 'else' */
    AND,          /**< Signe && */
    OR,           /**< Signe || */
    LBRACKET,     /**< Accolade gauche '[' */
    RBRACKET,     /**< Accolade droite ']' */
    COMMA,        /**< Virgule ',' */
    WHILE,        /**< Mot clé 'while' */
    PRINT,        /**< Mot clé 'print' */
    LENGTH,       /**< Mot clé 'length' */
    UNKNOWN       /**< Token non reconnu */
};

/**
 * @struct Token
 * @brief Représente un token dans le code source.
 *
 * Chaque token possède un type et une valeur optionnelle.
 */
struct Token
{
    TokenType type;                   /**< Type du token */
    std::optional<std::string> value; /**< Valeur du token (si applicable) */
};

/**
 * @class Tokenizer
 * @brief Classe responsable de l'analyse lexicale du code source.
 *
 * Cette classe analyse une chaîne de caractères représentant le code source
 * et la convertit en une séquence de tokens.
 */
class Tokenizer
{
public:
    /**
     * @brief Constructeur qui initialise le tokenizer avec le code source.
     * @param input La chaîne de caractères contenant le code source à analyser.
     */
    Tokenizer(const std::string &input) : m_input(input) {}

    /**
     * @brief Analyse le code source pour produire une séquence de tokens.
     * @return Un vecteur contenant les tokens identifiés dans le code source.
     */
    std::vector<Token> tokenize() const
    {
        const std::unordered_map<std::string, TokenType> keywords = {
            {"exit", TokenType::EXIT},
            {"let", TokenType::LET},
            {"if", TokenType::IF},
            {"else", TokenType::ELSE},
            {"while", TokenType::WHILE},
            {"print", TokenType::PRINT},
            {"len", TokenType::LENGTH}
            
        };
        std::vector<Token> tokens;
        int position = 0;
        while (position < m_input.size())
        {
            //  ici on doit ignorer les espaces avant de chaque token
            if (std::isspace(m_input[position]))
            {
                position++;
                continue;
            }

            // ici &&
            if (m_input[position] == '&' && position + 1 < m_input.size() && m_input[position + 1] == '&')
            {
                tokens.push_back({TokenType::AND, "&&"});
                position += 2;
                continue;
            }
            // ici ||
            if (m_input[position] == '|' && position + 1 < m_input.size() && m_input[position + 1] == '|')
            {
                tokens.push_back({TokenType::OR, "||"});
                position += 2;
                continue;
            }

            // ici on doit ignorer les commentaires sur une ligne
            if (m_input[position] == '/' && position + 1 < m_input.size() && m_input[position + 1] == '/')
            {
                while (position < m_input.size() && m_input[position] != '\n')
                {
                    position++;
                }
                continue;
            }

            // ici on doit ignorer les commentaires sur plusieurs lignes
            if (m_input[position] == '/' && position + 1 < m_input.size() && m_input[position + 1] == '*')
            {
                bool closed = false;
                position += 2;
                while (position < m_input.size() && !(m_input[position] == '*' && position + 1 < m_input.size() && m_input[position + 1] == '/'))
                {
                    position++;
                }
                if (position < m_input.size())
                {
                    position += 2;
                    closed = true;
                }
                if (!closed)
                {
                    std::cerr << "Erreur: YOOO WESH ferme le multiligne toi la avec ta grosse clavicie la TSSS !!!!" << std::endl;
                    position++;
                }

                continue;
            }

            // si on rencontre un caractere alphabetique ou underscore
            if (std::isalpha(m_input[position]) || m_input[position] == '_')
            {
                std::string identifier = consumeIdentifier(position);

                auto it = keywords.find(identifier);
                if (it != keywords.end())
                {
                    tokens.push_back({it->second, identifier});
                }
                else
                {
                    tokens.push_back({TokenType::IDENTIFIER, identifier});
                }
                continue;
            }

            // si on trouve un nombre
            if (std::isdigit(m_input[position]))
            {
                Token token = StartNumToken(position);
                tokens.push_back(token);
                continue;
            }

            // ici c'est (
            if (m_input[position] == '(')
            {
                tokens.push_back({TokenType::LPARENTHESIS, "("});
                position++;
                continue;
            }

            // ici c'est )
            if (m_input[position] == ')')
            {
                tokens.push_back({TokenType::RPARENTHESIS, ")"});
                position++;
                continue;
            }

            // ici c'est ==
            if (m_input[position] == '=' && position + 1 < m_input.size() && m_input[position + 1] == '=')
            {
                tokens.push_back({TokenType::EGAL, "=="});
                position += 2;
                continue;
            }

            // ici c'est !=
            if (m_input[position] == '!' && position + 1 < m_input.size() && m_input[position + 1] == '=')
            {
                tokens.push_back({TokenType::NEGAL, "!="});
                position += 2;
                continue;
            }

            // ici c'est >=
            if (m_input[position] == '>' && position + 1 < m_input.size() && m_input[position + 1] == '=')
            {
                tokens.push_back({TokenType::GREAT_EQUAL, ">="});
                position += 2;
                continue;
            }

            // ici c'est <=
            if (m_input[position] == '<' && position + 1 < m_input.size() && m_input[position + 1] == '=')
            {
                tokens.push_back({TokenType::LESS_EQUAL, "<="});
                position += 2;
                continue;
            }

            // ici c'est >
            if (m_input[position] == '>')
            {
                tokens.push_back({TokenType::GREAT, ">"});
                position++;
                continue;
            }

            // ici c'est <
            if (m_input[position] == '<')
            {
                tokens.push_back({TokenType::LESS, "<"});
                position++;
                continue;
            }
            // ici c'est =
            if (m_input[position] == '=')
            {
                tokens.push_back({TokenType::EQUAL, "="});
                position++;
                continue;
            }

            // ici c'est -
            if (m_input[position] == '-')
            {
                tokens.push_back({TokenType::MINUS, "-"});
                position++;
                continue;
            }

            // ici c'est +
            if (m_input[position] == '+')
            {
                tokens.push_back({TokenType::PLUS, "+"});
                position++;
                continue;
            }

            // ici c'est /
            if (m_input[position] == '/')
            {
                tokens.push_back({TokenType::DIVIDE, "/"});
                position++;
                continue;
            }

            // ici c'est %
            if (m_input[position] == '%')
            {
                tokens.push_back({TokenType::MODULO, "%"});
                position++;
                continue;
            }

            // ici c'est *
            if (m_input[position] == '*')
            {
                tokens.push_back({TokenType::STAR, "*"});
                position++;
                continue;
            }

            // ici c'est {
            if (m_input[position] == '{')
            {
                tokens.push_back({TokenType::LBRACE, "{"});
                position++;
                continue;
            }

            // ici c'est }
            if (m_input[position] == '}')
            {
                tokens.push_back({TokenType::RBRACE, "}"});
                position++;
                continue;
            }

            // ici  c'est [
            if (m_input[position] == '[')
            {
                tokens.push_back({TokenType::LBRACKET, "["});
                position++;
                continue;
            }
            // ici c'est ]
            if (m_input[position] == ']')
            {
                tokens.push_back({TokenType::RBRACKET, "]"});
                position++;
                continue;
            }

            // ici c'est ;
            if (m_input[position] == ';')
            {
                tokens.push_back({TokenType::SEMICOLON, ";"});
                position++;
                continue;
            }

            // ici c'est ,
            if (m_input[position] == ',')
            {
                tokens.push_back({TokenType::COMMA, ","});
                position++;
                continue;
            }

            // Les OVNI
            std::string unknown;
            unknown += m_input[position++];
            tokens.push_back({TokenType::UNKNOWN, unknown});
        }

        return tokens;
    }

private:
    std::string m_input; /**< Code source à analyser */

    /**
     * @brief Consomme et retourne un identifiant à partir de la position courante.
     *
     * Un identifiant peut contenir des lettres, des chiffres et des underscores.
     * La position est mise à jour pour pointer après l'identifiant.
     *
     * @param position Référence à la position courante dans le code source.
     * @return L'identifiant consommé sous forme de chaîne de caractères.
     */
    std::string consumeIdentifier(int &position) const
    {
        std::string result;
        while (position < m_input.size() &&
               (std::isalnum(m_input[position]) || m_input[position] == '_'))
        {
            result += m_input[position++];
        }
        return result;
    }

    /**
     * @brief Consomme et retourne un nombre à partir de la position courante.
     *
     * Accepte uniquement les chiffres et s'arrête au premier caractère non-chiffre.
     * La position est mise à jour pour pointer après le nombre.
     *
     * @param position Référence à la position courante dans le code source.
     * @return Le nombre consommé sous forme de chaîne de caractères.
     */
    std::string consumeNumber(int &position) const
    {
        std::string result;
        while (position < m_input.size() && std::isdigit(m_input[position]))
        {
            result += m_input[position++];
        }
        return result;
    }

    /**
     * @brief Traite les tokens qui commencent par un chiffre.
     *
     * Ce peut être soit un littéral entier, soit un identifiant commençant par un chiffre.
     * La position est mise à jour pour pointer après le token.
     *
     * @param position Référence à la position courante dans le code source.
     * @return Un token de type INT_LITERAL ou IDENTIFIER.
     */
    Token StartNumToken(int &position) const
    {
        // Consommer tous les chiffres
        std::string number = consumeNumber(position);

        // Verifier si le prochain caractère est une lettre ou un underscore
        if (position < m_input.size() &&
            (std::isalpha(m_input[position]) || m_input[position] == '_'))
        {

            std::string identifier = number;

            while (position < m_input.size() &&
                   (std::isalnum(m_input[position]) || m_input[position] == '_'))
            {
                identifier += m_input[position++];
            }

            return {TokenType::IDENTIFIER, identifier};
        }
        else
        {
            // C'est un simple nombre
            return {TokenType::INT_LITERAL, number};
        }
    }
};
