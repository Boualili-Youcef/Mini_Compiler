#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <optional>
#include <vector>
#include <cctype>
#include "Tokenizer.hpp"

std::vector<Token> lexer(const Tokenizer &tokenizer, const std::string &input)
{
    return tokenizer.tokenize();
}

std::string tokens_to_asm(const Tokenizer &tokenizer, const std::vector<Token> &tokens)
{
    return tokenizer.generateAssembly(tokens);
}

int main(int argc, char *argv[])
{

    std::string filePath;

    if (argc >= 2)
    {
        filePath = argv[1];
        std::cout << "Reading file: " << filePath << std::endl;
    }
    else
    {
        filePath = "./test.yb";
        std::cout << "No file specified, using default: " << filePath << std::endl;
    }
    // Ouverture du fichier
    std::ifstream file(filePath);

    if (!file)
    {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return EXIT_FAILURE;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();
    std::string content = ss.str();

    Tokenizer tokenizer(content);

    std::vector<Token> tokens = lexer(tokenizer, content);

    std::cout << "Tokens:\n";
    for (const auto &token : tokens)
    {
        std::cout << "Type: " << static_cast<int>(token.type) << ", Value: " << (token.value ? *token.value : "null") << std::endl;
    }

    std::string asm_code = tokens_to_asm(tokenizer, tokens);

    if (asm_code.empty())
    {
        std::cerr << "erreur lors de la generation du code asm" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Le code asm:\n"
              << asm_code << std::endl;

    std::ofstream asm_file("./org.asm");
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