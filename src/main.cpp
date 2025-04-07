#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <optional>
#include <vector>
#include <cctype>
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "Generator.hpp"

// TODO : a deleter
std::string toString(TokenType type)
{
    switch (type)
    {
    case TokenType::EXIT:
        return "EXIT";
    case TokenType::LET:
        return "LET";
    case TokenType::INT_LITERAL:
        return "INT_LITERAL";
    case TokenType::SEMICOLON:
        return "SEMICOLON";
    case TokenType::IDENTIFIER:
        return "IDENTIFIER";
    case TokenType::LPARENTHESIS:
        return "LPARENTHESIS";
    case TokenType::RPARENTHESIS:
        return "RPARENTHESIS";
    case TokenType::EQUAL:
        return "EQUAL";
    case TokenType::PLUS:
        return "PLUS";
    case TokenType::STAR:
        return "MULTIPLY";
    case TokenType::MINUS:
        return "MINUS";
    case TokenType::DIVIDE:
        return "DIVIDE";
    case TokenType::MODULO:
        return "MODULO";
    case TokenType::LBRACE:
        return "LBRACE";
    case TokenType::RBRACE:
        return "RBRACE";
    case TokenType::IF:
        return "IF";
    case TokenType::EGAL:
        return "Egalité";
    case TokenType::GREAT:
        return "GREAT";
    case TokenType::LESS:
        return "LESS";
    case TokenType::GREAT_EQUAL:
        return "GREAT_EQUAL";
    case TokenType::LESS_EQUAL:
        return "LESS_EQUAL";
    case TokenType::ELSE:
        return "ELSE";
    case TokenType::AND:
        return "AND";
    case TokenType::OR:
        return "OR";
    case TokenType::NEGAL:
        return "NEGAL";
    case TokenType::LBRACKET:
        return "LBRACKET";
    case TokenType::RBRACKET:
        return "RBRACKET";
    case TokenType::COMMA:
        return "COMMA";
    case TokenType::WHILE:
        return "WHILE";
    case TokenType::PRINT:
        return "PRINT";
    case TokenType::LENGTH:
        return "LENGTH";
    default:
        return "UNKNOWN";
    }
}

/**
 * @brief Point d'entrée principal du compilateur
 *
 * Ce programme implémente un compilateur simple qui:
 * 1. Lit le code source d'un fichier
 * 2. Analyse lexicalement le code (tokenization)
 * 3. Effectue l'analyse syntaxique (parsing)
 * 4. Génère du code assembleur correspondant
 *
 * @param argc Nombre d'arguments passés au programme
 * @param argv Tableau des arguments passés au programme
 * @return int Code de retour (EXIT_SUCCESS en cas de succès, EXIT_FAILURE sinon)
 */
int main(int argc, char *argv[])
{

    std::string filePath;

    if (argc >= 2)
    {
        filePath = argv[1];
        std::cout << "Lecture du fichier: " << filePath << std::endl;
    }
    else
    {
        filePath = "../exemples/test.yb";
        std::cout << "Pas de fichier spécifier, utilisation du path par defaut: " << filePath << std::endl;
    }
    // Ouverture du fichier
    std::ifstream file(filePath);

    if (!file)
    {
        std::cerr << "Erreur ouverture du fichier: " << filePath << std::endl;
        return EXIT_FAILURE;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();
    std::string content = ss.str();

    // ETape 01: On fait l'annalyse lexicale
    Tokenizer tokenizer(content);
    std::vector<Token> tokens = tokenizer.tokenize();

    std::cout << "Tokens:\n";
    for (const auto &token : tokens)
    {
        std::cout << "Type: " << toString(token.type) << "\t\t" << ", Value: " << (token.value ? *token.value : "null") << std::endl;
    }

    // ETape 02: On fait l'analyse syntaxique
    Parser parser(tokens);
    auto program = parser.parse();
    if (!program)
    {
        std::cerr << "Erreur: Impossible d'analyser le programme" << std::endl;
        return EXIT_FAILURE;
    }

    // ETape 03: On fait la generation du code assembleur
    Generator generator(program.value());
    std::string asm_code = generator.generateAssembly();

    std::cout << "Le code asm:\n"
              << asm_code << std::endl;

    std::ofstream asm_file("../build_asm/asm/org.asm");
    if (!asm_file)
    {
        std::cerr << "erreur de creation du fichier " << std::endl;
        return EXIT_FAILURE;
    }
    asm_file << asm_code;
    asm_file.close();
    std::cout << "ecriture du code assembleur fini" << std::endl;

    return EXIT_SUCCESS;
}