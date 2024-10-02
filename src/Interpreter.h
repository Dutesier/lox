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

#include "Environment.h"
#include "Lexer.h"
#include "Logger.h"
#include "Parser.h"

#include "BaseExpression.h"
#include "BaseStatement.h"
#include "Object.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace lox
{

class Interpreter
    : public ExpressionVisitor
    , public StatementVisitor
{
public:
    Interpreter();
    explicit Interpreter(std::filesystem::path file);

    int run();
    Object visit(const BinaryExpression& expr) override;
    Object visit(const LiteralExpression& expr) override;
    Object visit(const GroupingExpression& expr) override;
    Object visit(const UnaryExpression& expr) override;
    Object visit(const VariableExpression& expr) override;
    Object visit(const AssignmentExpression& expr) override;
    Object visit(const LogicalExpression& expr) override;
    Object visit(const CallExpression& expr) override;

    void visit(const PrintStatement& stmt) override;
    void visit(const ExpressionStatement& stmt) override;
    void visit(const VarStatement& stmt) override;
    void visit(const BlockStatement& stmt) override;
    void visit(const IfStatement& stmt) override;
    void visit(const WhileStatement& stmt) override;
    void visit(const FunctionStatement& statement) override;
    void visit(const ReturnStatement& statement) override;

    void executeCodeBlock(const std::vector<StatementUPTR>& body, std::shared_ptr<Environment> env);
    std::shared_ptr<Environment> m_globalEnvironment = std::make_shared<Environment>();

private:
    int interpretFile();
    int interpretStdin();
    int interpret(const std::string& content);

    void defineNativeFunctions();

    Object evaluate(const Expression& expr);

    void logError(unsigned int line, std::string_view location, std::string_view message);

    std::optional<std::filesystem::path> m_path;
    std::unique_ptr<Lexer> m_lexer;
    std::unique_ptr<Parser> m_parser;
    std::shared_ptr<Environment> m_env;
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

    class ReturnException : public std::exception
    {
        std::string message;

    public:
        // Constructor to initialize the exception with an object and custom message
        ReturnException(const Object& obj)
            : message("Interpreter Error: Unhandled return.")
            , obj(obj)
        {
        }

        // Override the what() function to return the error message
        const char* what() const noexcept override { return message.c_str(); }
        Object obj;
    };
};

} // namespace lox
