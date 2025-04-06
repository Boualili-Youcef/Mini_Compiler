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
        std::cout << "Type: " << static_cast<int>(token.type) << ", Value: " << (token.value ? *token.value : "null") << std::endl;
    }

    // ETape 02: On fait l'analyse syntaxique
    Parser parser(tokens);
    std::optional<NodeExit> nodeExit = parser.parseExit();
    if (!nodeExit)
    {
        std::cerr << "Erreur lors de l'analyse syntaxique" << std::endl;
        return EXIT_FAILURE;
    }

    // ETape 03: On fait la generation du code assembleur
    Generator generator(*nodeExit);
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