/******************************************************************************
 * Project:  Lox
 * Brief:    A C++ Lox interpreter.
 *
 * This software is provided "as is," without warranty of any kind, express
 * or implied, including but not limited to the warranties of merchantability,
 * fitness for a particular purpose, and noninfringement. In no event shall
 * the authors or copyright holders be liable for any claim, damages, or
 * other liability, whether in an action of contract, tort, or otherwise,
 * arising from, out of, or in connection with the software or the use or
 * other dealings in the software.
 *
 * Author:   Dutesier
 *
 ******************************************************************************/

#pragma once

#include "lexer.h"
#include "logger.h"
#include "parser.h"

#include "BaseExpression.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <string>

namespace lox
{

class Interpreter : public ExpressionVisitor
{
public:
    Interpreter();
    explicit Interpreter(std::filesystem::path file);

    int run();
    LiteralValues visit(const BinaryExpression& expr) override;
    LiteralValues visit(const LiteralExpression& expr) override;
    LiteralValues visit(const GroupingExpression& expr) override;
    LiteralValues visit(const UnaryExpression& expr) override;

private:
    int interpretFile();
    int interpretStdin();
    int interpret(const std::string& content);

    LiteralValues evaluate(const Expression& expr);

    void logError(unsigned int line, std::string_view location, std::string_view message);

    std::optional<std::filesystem::path> m_path;
    std::unique_ptr<Lexer> m_lexer;
    std::unique_ptr<Parser> m_parser;
    Logger m_logger;

public:
    // Custom exception class
    class InterpreterException : public std::exception
    {
    private:
        Token token;
        std::string message;

    public:
        // Constructor to initialize the exception with a Token and custom message
        InterpreterException(const Token& token, const std::string& msg)
            : token(token)
            , message("Interpreter Error: Operator: " + token.print() + msg + ".")
        {
        }

        // Override the what() function to return the error message
        const char* what() const noexcept override { return message.c_str(); }
    };
};

} // namespace lox
