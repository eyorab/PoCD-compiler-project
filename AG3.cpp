//operators are included inside symbol table
#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <unordered_map>

enum TokenType {
    KEYWORD,
    SPECIAL_CHARACTER,
    IDENTIFIER,
    OPERATOR,
    CONSTANT,
    LITERAL,
    PUNCTUATION_SYMBOL,
    INVALID,
    END
};

struct Token {
    TokenType type;
    std::string value;
    size_t lineNumber;

 
    Token() : type(INVALID), value(""), lineNumber(0) {}

    
    Token(TokenType t, const std::string& v = "", size_t ln = 0) : type(t), value(v), lineNumber(ln) {}
};


std::string getTokenTypeName(TokenType type);

class Lexer {
private:
    std::ifstream inputFile;
    size_t lineNumber;
    std::unordered_map<std::string, Token> symbolTable;

public:
    Lexer(const std::string& filename) : inputFile(appendExtension(filename)), lineNumber(1) {
        if (!inputFile.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    Token getNextToken() {
        char currentChar;

        while (inputFile.get(currentChar)) {
            if (currentChar == '\n') {
                ++lineNumber;
            }

            if (std::isalpha(currentChar) || currentChar == '_') {
                return parseIdentifier(currentChar);
            } else if (std::isdigit(currentChar)) {
                return parseConstant(currentChar);
            } else if (currentChar == '"' || currentChar == '\'') {
                return parseLiteral(currentChar);
            } else if (isOperator(currentChar)) {
                return insertIntoSymbolTable(OPERATOR, std::string(1, currentChar), lineNumber);
            } else if (isPunctuation(currentChar)) {
                return insertIntoSymbolTable(PUNCTUATION_SYMBOL, std::string(1, currentChar), lineNumber);
            } else if (std::isspace(currentChar)) {
                
                continue;
            } else {
                return Token(INVALID, std::string(1, currentChar));
            }
        }

        return Token(END, "");
    }

    void printSymbolTable() {
        std::cout << "\nSymbol Table:\n";
        for (const auto& entry : symbolTable) {
            const Token& token = entry.second;
            std::cout << "Name: " << entry.first << ", Type: " << getTokenTypeName(token.type)
                      << ", Value: " << token.value << ", Line Number: " << token.lineNumber << std::endl;
        }
    }

private:
    Token parseIdentifier(char firstChar) {
        std::string result(1, firstChar);

        while (inputFile.get(firstChar) && (std::isalnum(firstChar) || firstChar == '_')) {
            result += firstChar;
        }

        
        if (inputFile) inputFile.unget();

        
        if (result == "if" || result == "else" || result == "while" || result == "int" || result == "float") {
            return insertIntoSymbolTable(KEYWORD, result, lineNumber);
        }

        return insertIntoSymbolTable(IDENTIFIER, result, lineNumber);
    }

    Token parseConstant(char firstDigit) {
        std::string result(1, firstDigit);

        while (inputFile.get(firstDigit) && std::isdigit(firstDigit)) {
            result += firstDigit;
        }

        
        if (inputFile) inputFile.unget();

        return insertIntoSymbolTable(CONSTANT, result, lineNumber);
    }

    Token parseLiteral(char quoteChar) {
        std::string result;
        char currentChar;

        while (inputFile.get(currentChar) && currentChar != quoteChar) {
            result += currentChar;
        }

        return insertIntoSymbolTable(LITERAL, result, lineNumber);
    }

    bool isOperator(char c) {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '=';
    }

    bool isPunctuation(char c) {
        return c == ',' || c == ';' || c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == '.';
    }

    Token insertIntoSymbolTable(TokenType type, const std::string& value, size_t lineNum) {
        Token token(type, value, lineNum);
        symbolTable[value] = token;
        return token;
    }

    std::string appendExtension(const std::string& filename) {
        
        if (filename.find_last_of(".") == std::string::npos) {
            return filename + ".txt";
        }
        return filename;
    }
};


std::string getTokenTypeName(TokenType type) {
    switch (type) {
        case KEYWORD:
            return "Keyword";
        case SPECIAL_CHARACTER:
            return "Special Character";
        case IDENTIFIER:
            return "Identifier";
        case OPERATOR:
            return "Operator";
        case CONSTANT:
            return "Constant";
        case LITERAL:
            return "Literal";
        case PUNCTUATION_SYMBOL:
            return "Punctuation Symbol";
        case INVALID:
            return "Invalid";
        case END:
            return "End";
        default:
            return "Unknown";
    }
}

int main() {
    std::string filename;
    std::cout << "Enter the filename (without extension): ";
    std::cin >> filename;

    Lexer lexer(filename);

    Token token;
    do {
        token = lexer.getNextToken();
        if (token.type != END) {
            std::cout << "Type: " << getTokenTypeName(token.type) << ", Value: " << token.value
                      << ", Line Number: " << token.lineNumber << std::endl;
        }
    } while (token.type != END);

    lexer.printSymbolTable();

    return 0;
}
