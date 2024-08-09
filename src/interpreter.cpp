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

#include "interpreter.h"

#include "AstPrinter.hpp" // Debugging

#include <assert.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <typeinfo>

namespace lox
{

namespace
{

// Follows Lox (Ruby)s convention
bool isTruthy(const LiteralValues& value)
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

bool isEqual(const LiteralValues& a, const LiteralValues& b)
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

template <typename T> void assertBothAreType(const Token& tok, const LiteralValues& a, const LiteralValues& b)
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

Interpreter::Interpreter() {}

Interpreter::Interpreter(std::filesystem::path path)
    : m_path(std::move(path))
{
    if (m_path->empty())
    {
        throw std::domain_error("Invalid file path.");
    }
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
        wholeFile += lineBuffer;
    }

    file.close();
    return interpret(wholeFile);
}

int Interpreter::interpretStdin()
{
    auto exitCode = EXIT_FAILURE;
    std::string lineBuffer{};
    std::cout << ">\t";
    while (getline(std::cin, lineBuffer))
    {
        exitCode = interpret(lineBuffer);
        std::cout << ">\t";
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
    auto expr = m_parser->parse();
    if (!expr)
    {
        return EXIT_FAILURE;
    }
    AstPrinter printer;
    printer.print(*(expr.value())); // Refactor!!!!!!!!

    try
    {
        auto value = evaluate(*(expr.value()));
        Logger::info(print(value));
    }
    catch (InterpreterException& e)
    {
        Logger::error("Runtime error: " + std::string{ e.what() });
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void Interpreter::logError(unsigned int line, std::string_view location, std::string_view message)
{
    m_logger.error(std::format("[line {}] {}: {}", line, location, message));
}

LiteralValues Interpreter::evaluate(const Expression& expr)
{
    return expr.accept(*this);
}

LiteralValues Interpreter::visit(const LiteralExpression& expr)
{
    // Temporary
    return expr.value;
}

LiteralValues Interpreter::visit(const GroupingExpression& expr)
{
    return evaluate(*expr.expression);
}

LiteralValues Interpreter::visit(const BinaryExpression& expr)
{
    using enum TokenType;
    LiteralValues left = evaluate(*(expr.left));
    LiteralValues right = evaluate(*(expr.right));

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

LiteralValues Interpreter::visit(const UnaryExpression& expr)
{
    LiteralValues right = evaluate(*expr.right);
    if (expr.op.type == TokenType::Minus)
    {
        assert(std::holds_alternative<double>(right));
        // Note: mind the overflow  (MAX_DOUBLE vs MIN_DOUBLE), probably in the scannser
        return -std::get<double>(right); // Pay attention to the minus
    }

    return !isTruthy(right);
}

} // namespace lox
