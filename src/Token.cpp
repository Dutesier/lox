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

#include "Token.h"

#include <assert.h>
#include <format>
#include <iostream>
#include <sstream>

namespace lox
{

namespace
{

std::string tokenTypeToString(TokenType type)
{
    using enum TokenType;
    switch (type)
    {
    case TokenType::LeftParen:
        return "LeftParen";
    case TokenType::RightParen:
        return "RightParen";
    case TokenType::LeftBrace:
        return "LeftBrace";
    case TokenType::RightBrace:
        return "RightBrace";
    case TokenType::Comma:
        return "Comma";
    case TokenType::Dot:
        return "Dot";
    case TokenType::Minus:
        return "Minus";
    case TokenType::Plus:
        return "Plus";
    case TokenType::Semicolon:
        return "Semicolon";
    case TokenType::Slash:
        return "Slash";
    case TokenType::Star:
        return "Star";
    case TokenType::Bang:
        return "Bang";
    case TokenType::BangEqual:
        return "BangEqual";
    case TokenType::Equal:
        return "Equal";
    case TokenType::EqualEqual:
        return "EqualEqual";
    case TokenType::Greater:
        return "Greater";
    case TokenType::GreaterEqual:
        return "GreaterEqual";
    case TokenType::Less:
        return "Less";
    case TokenType::LessEqual:
        return "LessEqual";
    case TokenType::Identifier:
        return "Identifier";
    case TokenType::String:
        return "String";
    case TokenType::Number:
        return "Number";
    case TokenType::And:
        return "And";
    case TokenType::Class:
        return "Class";
    case TokenType::Else:
        return "Else";
    case TokenType::False:
        return "False";
    case TokenType::Fun:
        return "Fun";
    case TokenType::For:
        return "For";
    case TokenType::If:
        return "If";
    case TokenType::Nil:
        return "Nil";
    case TokenType::Or:
        return "Or";
    case TokenType::Print:
        return "Print";
    case TokenType::Return:
        return "Return";
    case TokenType::Super:
        return "Super";
    case TokenType::This:
        return "This";
    case TokenType::True:
        return "True";
    case TokenType::Var:
        return "Var";
    case TokenType::While:
        return "While";
    case TokenType::Eof:
        return "Eof";
    case TokenType::Error:
        return "ERROR";
    default:
        assert(false);
        return "";
    }
}

std::string literalToString(const Token::LiteralValues& tok)
{
    if (std::holds_alternative<std::string>(tok))
    {
        return std::get<std::string>(tok);
    }
    else if (std::holds_alternative<double>(tok))
    {
        std::stringstream ss;
        ss << std::get<double>(tok);
        return ss.str();
    }
    else
    {
        return "no-literal-value";
    }
}

bool literalEquals(const Token::LiteralValues& lhs, const Token::LiteralValues& rhs)
{
    if (std::holds_alternative<std::monostate>(lhs) && std::holds_alternative<std::monostate>(rhs))
    {
        return true;
    }

    if (std::holds_alternative<std::string>(lhs) && std::holds_alternative<std::string>(rhs))
    {
        return std::get<std::string>(lhs) == std::get<std::string>(rhs);
    }

    if (std::holds_alternative<double>(lhs) && std::holds_alternative<double>(rhs))
    {
        return std::get<double>(lhs) == std::get<double>(rhs);
    }

    return false;
}

} // namespace

std::string Token::print() const
{
    return "(Token){\"type\": \"" + tokenTypeToString(type) + "\",\"literal\": \"" + literalToString(literal) +
           "\",\"location\": \"" + (location.data() ? location.data() : "") +
           "\",\"lineNo\": " + std::to_string(lineNo) + "}";
}

std::ostream& operator<<(std::ostream& os, const Token& me)
{
    return os << me.print();
}

bool operator==(const Token& lhs, const Token& rhs)
{
    return lhs.type == rhs.type && lhs.lineNo == rhs.lineNo && lhs.location == rhs.location &&
           literalEquals(lhs.literal, rhs.literal);
}

} // namespace lox
