#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <optional>
#include <vector>
#include <cctype>

enum class TokenType
{
    _return,
    int_lit,
    semi,
    unknown
};

struct Token
{
    TokenType type;
    std::optional<std::string> value;
};

// TODO: ecrire un lexer
std::vector<Token> lexer(const std::string &input)
{
    std::vector<Token> tokens;
    std::string mots, numbre, symbol;
    for (char c : input)
    {
        if (std::isspace(c))
            continue;
        if (std::isalpha(c))
            mots += c;
        else if (std::isdigit(c))
            numbre += c;
        else
            symbol += c;
    }

    tokens.push_back({TokenType::_return, mots});
    tokens.push_back({TokenType::int_lit, numbre});
    tokens.push_back({TokenType::semi, symbol});

    return tokens;
}

// TODO: Vers l'assembleur
std::string tokens_to_asm(const std::vector<Token> &tokens)
{
    std::stringstream asm_code;
    asm_code << "global _start" << std::endl;
    asm_code << "_start:" << std::endl;

    if (tokens.size() < 3)
    {
        std::cerr << "Erreur: verifier le nombre de trucs" << std::endl;
        return "";
    }

    for (int i = 0; i < tokens.size(); i++)
    {
        if (tokens.at(i).type == TokenType::_return)
        {
            if (i + 1 < tokens.size() && i + 2 < tokens.size() &&
                tokens.at(i + 1).type == TokenType::int_lit &&
                tokens.at(i + 2).type == TokenType::semi)
            {
                asm_code << "    mov rax, 60" << std::endl;
                asm_code << "    mov rdi, " << *tokens.at(i + 1).value << std::endl;
                asm_code << "    syscall" << std::endl;

                return asm_code.str();
            }
            else
            {
                std::cerr << "Erreur: flemme ici" << std::endl;
                return "";
            }
        }
    }

    std::cerr << "Error: je veux dormir" << std::endl;
    return "";
}

int main(int argc, char *argv[])
{

    // TODO: ecrire un lecteur de fichier
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Reading file: " << argv[1] << std::endl;

    // Ouverture du fichier
    std::ifstream file(argv[1]);
    if (!file)
    {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();
    std::string content = ss.str();

    std::vector<Token> tokens = lexer(content);
    std::cout << "Tokens:\n";
    for (const auto &token : tokens)
    {
        std::cout << "Type: " << static_cast<int>(token.type) << ", Value: " << (token.value ? *token.value : "null") << std::endl;
    }

    std::string asm_code = tokens_to_asm(tokens);
    if (asm_code.empty())
    {
        std::cerr << "erreur lors de la generation du code asm" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Le code asm:\n"
              << asm_code << std::endl;

    std::ofstream asm_file("../org2.asm");
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