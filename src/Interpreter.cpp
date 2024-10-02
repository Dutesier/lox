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

#include "Interpreter.h"

#include "AstPrinter.hpp" // Debugging
#include "BaseExpression.h"
#include "BaseStatement.h"
#include "Environment.h"
#include "Function.h"
#include "Logger.h"
#include "Object.h"
#include "Token.h"

#include <assert.h>
#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <typeinfo>
#include <unistd.h>
#include <variant>
#include <vector>

namespace lox
{

namespace
{

// Follows Lox (Ruby)s convention
bool isTruthy(const Object& value)
{
    if (std::holds_alternative<bool>(value))
    {
        return std::get<bool>(value);
    }
    if (std::holds_alternative<NullLiteral>(value))
    {
        return false;
    }
    return true;
}

bool isEqual(const Object& a, const Object& b)
{
    if (std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b))
    {
        return std::get<bool>(a) == std::get<bool>(b);
    }
    if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b))
    {
        return std::get<std::string>(a) == std::get<std::string>(b);
    }

    if (std::holds_alternative<double>(a) && std::holds_alternative<double>(b))
    {
        return std::get<double>(a) == std::get<double>(b);
    }

    if (std::holds_alternative<NullLiteral>(a) || std::holds_alternative<NullLiteral>(b))
    {
        if (std::holds_alternative<NullLiteral>(a) && std::holds_alternative<NullLiteral>(b))
        {
            return true;
        }
        return false;
    }
    return false;
}

template <typename T> void assertBothAreType(const Token& tok, const Object& a, const Object& b)
{
    if (std::holds_alternative<T>(a) && std::holds_alternative<T>(b))
    {
        return;
    }

    T debug;
    throw Interpreter::InterpreterException{
        tok, "variables do not hold the same type " + std::string{ typeid(debug).name() }
    };
}

} // namespace

Interpreter::Interpreter()
{
    defineNativeFunctions();
}

Interpreter::Interpreter(std::filesystem::path path)
    : m_path(std::move(path))
{
    if (m_path->empty())
    {
        throw std::domain_error("Invalid file path.");
    }
    defineNativeFunctions();
}

void Interpreter::defineNativeFunctions()
{
    Callable clock{ .call = [](Interpreter&, const std::vector<Object>&) -> Object {
                       return static_cast<double>(
                           std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
                   },
                    .name = "clock" };

    Callable sleep{ .call = [](Interpreter&, const std::vector<Object>& args) -> Object
                    {
                        if (args.empty())
                        {
                            throw InterpreterException{ Token{},
                                                        "Sleep requires one argument" }; // This should never be called
                        }

                        if (!std::holds_alternative<double>(args[0]))
                        {
                            throw InterpreterException{ Token{}, "Sleep requires a double as argument" };
                        }
                        ::sleep(std::get<double>(args[0]));
                        return args[0];
                    },
                    .arity = 1,
                    .name = "sleep" };

    m_globalEnvironment->define(clock.name, clock);
    m_globalEnvironment->define(sleep.name, sleep);
}

int Interpreter::run()
{
    if (m_path)
    {
        return interpretFile();
    }
    return interpretStdin();
}

int Interpreter::interpretFile()
{
    assert(m_path.has_value());
    std::ifstream file(m_path.value());
    if (!file.is_open())
    {
        m_logger.error(std::format("[interpretFile]: Failed to open file at {}.", m_path.value().string()));
        return EXIT_FAILURE;
    }

    std::string lineBuffer, wholeFile;
    while (getline(file, lineBuffer))
    {
        wholeFile += lineBuffer + '\n';
    }

    file.close();
    return interpret(wholeFile);
}

int Interpreter::interpretStdin()
{
    auto exitCode = EXIT_FAILURE;
    std::string lineBuffer{};
    Logger::terminal("");
    while (getline(std::cin, lineBuffer))
    {
        exitCode = interpret(lineBuffer);
        Logger::terminal("");
    }

    return exitCode;
}

int Interpreter::interpret(const std::string& content)
{
    if (content.empty())
    {
        m_logger.info("[interpret]: No content to interpret.");
        return EXIT_FAILURE;
    }

    m_logger.debug(std::format("[interpret]: Content: {}", content));
    std::string_view content_view{ content };
    m_lexer = std::make_unique<Lexer>(std::move(content_view));
    m_parser = std::make_unique<Parser>(m_lexer->tokenize());
    auto statements = m_parser->parse();
    if (statements.empty())
    {
        return EXIT_FAILURE;
    }

    AstPrinter printer;

    m_env = m_globalEnvironment;
    for (const auto& stmt : statements)
    {
        try
        {
            stmt->accept(*this);
        }
        catch (InterpreterException& e)
        {
            Logger::error("Runtime error: " + std::string{ e.what() });
            return EXIT_FAILURE;
        }
        catch (Environment::EnvironmentException& e)
        {
            Logger::error("Runtime error: " + std::string{ e.what() });
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

void Interpreter::visit(const PrintStatement& stmt)
{
    auto value = evaluate(*stmt.expr);
    Logger::terminal(print(value) + '\n');
}

void Interpreter::visit(const ExpressionStatement& stmt)
{
    Logger::error("Visiting ExpressionStatement");
    evaluate(*stmt.expr);
}

void Interpreter::visit(const VarStatement& stmt)
{
    Logger::error("Visiting VarStatement");
    assert(std::holds_alternative<std::string>(stmt.name.literal));
    Object value = NullLiteral{};

    if (stmt.expr)
    {
        value = evaluate(*stmt.expr);
    }
    m_env->define(std::get<std::string>(stmt.name.literal), value);
}

void Interpreter::visit(const IfStatement& stmt)
{
    Logger::error("Visiting IfStatement");
    if (isTruthy(evaluate(*stmt.condition)))
    {
        stmt.thenBranch->accept(*this);
    }
    else if (stmt.elseBranch.has_value())
    {
        stmt.elseBranch.value()->accept(*this);
    }
}

void Interpreter::visit(const WhileStatement& stmt)
{
    Logger::error("Visiting WhileStatement");
    while (isTruthy(evaluate(*stmt.condition)))
    {
        stmt.body->accept(*this);
    }
}

void Interpreter::visit(const FunctionStatement& statement)
{
    Logger::error("Visiting FunctionStatement");
    if (m_env == nullptr)
    {
        Logger::warn("Nullptr env");
    }
    Function fun(statement, m_env);
    if (!std::holds_alternative<std::string>(statement.name.literal))
    {
        return;
    }
    fun.name = std::get<std::string>(statement.name.literal);
    m_env->define(fun.name, fun);
    return;
}

void Interpreter::visit(const ReturnStatement& statement)
{
    Logger::error("Visiting ReturnStatement");
    Object value = NullLiteral{};
    if (statement.value.has_value())
    {
        value = evaluate(*statement.value.value());
    }
    throw ReturnException{ value };
}

void Interpreter::logError(unsigned int line, std::string_view location, std::string_view message)
{
    m_logger.error(std::format("[line {}] {}: {}", line, location, message));
}

Object Interpreter::evaluate(const Expression& expr)
{
    return expr.accept(*this);
}

Object Interpreter::visit(const LiteralExpression& expr)
{
    // Temporary
    return expr.value;
}

Object Interpreter::visit(const GroupingExpression& expr)
{
    return evaluate(*expr.expression);
}

Object Interpreter::visit(const BinaryExpression& expr)
{
    using enum TokenType;
    Object left = evaluate(*(expr.left));
    Object right = evaluate(*(expr.right));

    switch (expr.op.type)
    {
        // TODO: type check here
    case Minus:
        assertBothAreType<double>(expr.op, left, right);
        return std::get<double>(left) - std::get<double>(right);
    case Slash:
        assertBothAreType<double>(expr.op, left, right);
        return std::get<double>(left) / std::get<double>(right);
    case Star:
        assertBothAreType<double>(expr.op, left, right);
        return std::get<double>(left) * std::get<double>(right);
    case Plus:
        if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
        {
            return std::get<std::string>(left) + std::get<std::string>(right);
        }
        else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
        {
            return std::get<double>(left) + std::get<double>(right);
        }
        else
        {
            throw InterpreterException{ expr.op,
                                        "Addition on something other than two doubles or two strings not allowed." };
        }

    case Greater:
        assertBothAreType<double>(expr.op, left, right);
        return std::get<double>(left) > std::get<double>(right);
    case GreaterEqual:
        assertBothAreType<double>(expr.op, left, right);
        return std::get<double>(left) >= std::get<double>(right);
    case Less:
        assertBothAreType<double>(expr.op, left, right);
        return std::get<double>(left) < std::get<double>(right);
    case LessEqual:
        assertBothAreType<double>(expr.op, left, right);
        return std::get<double>(left) <= std::get<double>(right);

    case BangEqual:
        return !isEqual(left, right);
    case EqualEqual:
        return isEqual(left, right);

    default:
        break;
    }
    // Temporary
    return NullLiteral{};
}

Object Interpreter::visit(const UnaryExpression& expr)
{
    Object right = evaluate(*expr.right);
    if (expr.op.type == TokenType::Minus)
    {
        assert(std::holds_alternative<double>(right));
        // Note: mind the overflow  (MAX_DOUBLE vs MIN_DOUBLE), probably in the scannser
        return -std::get<double>(right); // Pay attention to the minus
    }

    return !isTruthy(right);
}

Object Interpreter::visit(const VariableExpression& expr)
{
    assert(std::holds_alternative<std::string>(expr.name.literal));
    return m_env->get(std::get<std::string>(expr.name.literal));
}

Object Interpreter::visit(const AssignmentExpression& expr)
{
    assert(std::holds_alternative<std::string>(expr.name.literal));
    auto key = std::get<std::string>(expr.name.literal);
    auto value = evaluate(*expr.expr);
    m_env->assign(key, value);
    return value;
}

Object Interpreter::visit(const LogicalExpression& expr)
{
    // Logger::debug("Visiting logical expression");
    auto left = evaluate(*expr.left);

    if (expr.op.type == TokenType::Or)
    {
        if (isTruthy(left))
        {
            return left;
        }
    }
    else
    {
        if (!isTruthy(left))
        {
            return left;
        }
    }

    return evaluate(*expr.right);
}

Object Interpreter::visit(const CallExpression& expr)
{
    Object callee = evaluate(*expr.callee);

    std::vector<Object> arguments;
    for (const auto& arg : expr.arguments)
    {
        arguments.emplace_back(evaluate(*arg));
    }

    if (!std::holds_alternative<Callable>(callee))
    {
        throw InterpreterException{ expr.paren, "Can only call functions and classes." };
    }
    Callable function = std::get<Callable>(callee);

    if (arguments.size() != function.arity)
    {
        throw InterpreterException{
            expr.paren, std::format("Expected {} arguments but got {}.", function.arity, arguments.size())
        };
    }
    return function.call(*this, arguments);
}

void Interpreter::visit(const BlockStatement& stmt)
{
    if (m_env == nullptr)
    {
        throw std::domain_error{ "Environment points to null adress" };
    }
    auto env = std::make_shared<Environment>(m_env);
    executeCodeBlock(stmt.statements, env);
}

void Interpreter::executeCodeBlock(const std::vector<StatementUPTR>& body, std::shared_ptr<Environment> env)
{
    Logger::debug("Executing code block. Received env:");
    env->debug();
    // Store the outer environment
    std::shared_ptr<Environment> previous = m_env;
    try
    {
        // Create a new environment for the current block
        auto blockEnvironment = std::make_shared<Environment>(env);
        // m_env = std::make_unique<Environment>(&env);
        m_env = blockEnvironment;
        Logger::debug("Created env for code block. Current env: ");
        m_env->debug();
        for (const auto& statement : body)
        {
            statement->accept(*this);
        }
        Logger::debug("Finished statements in body");
    }
    catch (InterpreterException& e)
    {
        Logger::error(e.what());
    }
    // Reset the environment (a la stack)
    Logger::debug("Created env for block no longer needed");
    m_env = previous;
    Logger::debug("Restored interpreter current env to: ");
    m_env->debug();
}

} // namespace lox
