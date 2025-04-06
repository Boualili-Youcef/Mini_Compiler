#pragma once

#include <string>
#include <sstream>
#include <optional>
#include <vector>
#include <cctype>
#include <iostream>
#include <unordered_map>

// ici c'est simple pour le moment 
// c'est soit :
enum class TokenType {
    EXIT, // mot cle exit
    INT_LITERAL, // nombre entier
    SEMICOLON, // ;
    IDENTIFIER, // un mot style (ma_var2_a__toto)
    UNKNOWN // un token inconnu
};

struct Token {
    TokenType type;
    std::optional<std::string> value;
};

class Tokenizer {
public:
    Tokenizer(const std::string &input) : m_input(input) {}

    // Ici on va utiliser les cours d'informatique theorique
    // Cette methode doit renvoyer un vecteur de token
    std::vector<Token> tokenize() const {
        std::vector<Token> tokens;
        int position = 0;
        while (position < m_input.size()) {
            //  ici on doit ignorer les espaces avant de chaque token 
            if (std::isspace(m_input[position])) {
                position++;
                continue;
            }
            
            // si on rencontre un caractere alphabetique ou underscore
            if (std::isalpha(m_input[position]) || m_input[position] == '_') {
                std::string identifier = consumeIdentifier(position);
                
                if (identifier == "exit") {
                    tokens.push_back({TokenType::EXIT, identifier});
                } else {
                    tokens.push_back({TokenType::IDENTIFIER, identifier});
                }
                continue;
            }
            
            // si on trouve un nombre
            if (std::isdigit(m_input[position])) {
                Token token = StartNumToken(position);
                tokens.push_back(token);
                continue;
            }
            
            // ici c'est ;
            if (m_input[position] == ';') {
                tokens.push_back({TokenType::SEMICOLON, ";"});
                position++;
                break;
            }
            
            // Les OVNI 
            std::string unknown;
            unknown += m_input[position++];
            tokens.push_back({TokenType::UNKNOWN, unknown});
        }
        
        return tokens;
    }

   

private:
    std::string m_input;
    
    // Methode pour consommer un identifiant:
    // donc il acceptera les lettres, les chiffres et le underscore
    // et il s'arretera quand il rencontrera un caractere qui n'est pas valide espace ou autre l'idée
    std::string consumeIdentifier(int &position) const{
        std::string result;
        while (position < m_input.size() && 
               (std::isalnum(m_input[position]) || m_input[position] == '_')) {
            result += m_input[position++];
        }
        return result;
    }
    
    // Methode pour consommer un nombre
    // donc il acceptera les chiffres et il s'arretera quand il rencontrera un caractere qui n'est pas valide espace ou autrre 
    std::string consumeNumber(int &position) const {
        std::string result;
        while (position < m_input.size() && std::isdigit(m_input[position])) {
            result += m_input[position++];
        }
        return result;
    }

    // Methode pour gérer les tokens qui commencent par un chiffre
    Token StartNumToken(int &position) const {
        
        // Consommer tous les chiffres
        std::string number = consumeNumber(position);
        
        // Verifier si le prochain caractère est une lettre ou un underscore
        if (position < m_input.size() && 
            (std::isalpha(m_input[position]) || m_input[position] == '_')) {
            
            std::string identifier = number;
            
            while (position < m_input.size() && 
                   (std::isalnum(m_input[position]) || m_input[position] == '_')) {
                identifier += m_input[position++];
            }
            
            return {TokenType::IDENTIFIER, identifier};
        } else {
            // C'est un simple nombre
            return {TokenType::INT_LITERAL, number};
        }
    }
};

