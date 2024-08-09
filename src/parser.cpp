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

#include "parser.h"

#include "logger.h"

namespace lox
{

Parser::Parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens))
{
}

std::optional<ExpressionUPTR> Parser::parse()
{
    try
    {
        return expression();
    }
    catch (ParserException& error)
    {
        Logger::error("Failed parsing. " + std::string{ error.what() });
        return std::nullopt;
    }
}

ExpressionUPTR Parser::expression()
{
    // Logger::debug("expression");
    return comma();
}

ExpressionUPTR Parser::buildBinaryExpression(ExpressionProducingFn lowerPrecedenceFn, MatchingFn matchFn)
{
    auto lowerPrecedence = lowerPrecedenceFn();
    while (matchFn())
    {
        Token op = previous();
        auto right = lowerPrecedenceFn();
        lowerPrecedence = std::make_unique<BinaryExpression>(std::move(lowerPrecedence), op, std::move(right));
    }
    return lowerPrecedence;
}

ExpressionUPTR Parser::comma()
{
    Logger::debug("comma");
    ExpressionProducingFn lowerPrecedenceFn = [this]() { return equality(); };
    MatchingFn matchingFn = [this]()
    {
        using enum TokenType;
        return match(Comma);
    };

    return buildBinaryExpression(std::move(lowerPrecedenceFn), std::move(matchingFn));
}

ExpressionUPTR Parser::equality()
{
    // Logger::debug("equality");
    ExpressionProducingFn lowerPrecedenceFn = [this]() { return comparison(); };
    MatchingFn matchingFn = [this]()
    {
        using enum TokenType;
        return match(BangEqual) || match(EqualEqual);
    };

    return buildBinaryExpression(std::move(lowerPrecedenceFn), std::move(matchingFn));
}

ExpressionUPTR Parser::comparison()
{
    // Logger::debug("comparison");
    using enum TokenType;
    ExpressionProducingFn lowerPrecedenceFn = [this]() { return term(); };
    MatchingFn matchingFn = [this]()
    { return match(Greater) || match(Less) || match(GreaterEqual) || match(LessEqual); };

    return buildBinaryExpression(std::move(lowerPrecedenceFn), std::move(matchingFn));
}

ExpressionUPTR Parser::term()
{
    // Logger::debug("term");
    using enum TokenType;
    ExpressionProducingFn lowerPrecedenceFn = [this]() { return factor(); };
    MatchingFn matchingFn = [this]() { return match(Plus) || match(Minus); };

    return buildBinaryExpression(std::move(lowerPrecedenceFn), std::move(matchingFn));
}

ExpressionUPTR Parser::factor()
{
    // Logger::debug("factor");
    using enum TokenType;
    ExpressionProducingFn lowerPrecedenceFn = [this]() { return unary(); };
    MatchingFn matchingFn = [this]() { return match(Slash) || match(Star); };

    return buildBinaryExpression(std::move(lowerPrecedenceFn), std::move(matchingFn));
}

ExpressionUPTR Parser::unary()
{
    // Logger::debug("unary");
    using enum TokenType;
    while (match(Bang) || match(Minus))
    {
        Token op = previous();
        return std::make_unique<UnaryExpression>(op, std::move(unary()));
    }
    return primary();
}

namespace
{

ExpressionUPTR literalExpressionFromLiteralToken(const Token& tok)
{
    if (std::holds_alternative<double>(tok.literal))
    {
        return std::make_unique<LiteralExpression>(std::get<double>(tok.literal));
    }
    else if (std::holds_alternative<std::string_view>(tok.literal))
    {
        auto str = std::get<std::string_view>(tok.literal);
        return std::make_unique<LiteralExpression>(str.data() ? std::string{ str.data() } : std::string{});
    }
    else
    {
        return std::make_unique<LiteralExpression>(NullLiteral{});
    }
}

} // namespace

ExpressionUPTR Parser::primary()
{
    // Logger::debug("primary");
    using enum TokenType;
    if (match(False))
    {
        return std::make_unique<LiteralExpression>(false);
    }
    if (match(True))
    {
        return std::make_unique<LiteralExpression>(true);
    }
    if (match(Nil))
    {
        return std::make_unique<LiteralExpression>(NullLiteral{});
    }

    if (match(Number) || match(String))
    {
        return literalExpressionFromLiteralToken(previous());
    }

    if (match(LeftParen))
    {
        auto expr = expression();
        consumeOrThrow(RightParen, "Expected ')' after expression.");
        return std::make_unique<GroupingExpression>(std::move(expr));
    }

    // If nothing matched so far this is not a valid expression
    throw error(peek(), "Expected expression.");
}

Token Parser::consumeOrThrow(TokenType type, const std::string& error_msg)
{
    if (checkCurrentToken(type))
    {
        return advance();
    }
    throw error(peek(), error_msg);
}

auto Parser::error(const Token& token, const std::string& msg) -> ParserException
{
    Logger::error(token.print());
    return { token, msg };
}

Token Parser::peek()
{
    return m_tokens.at(m_current);
}

Token Parser::previous()
{
    if (!m_current)
    {
        throw std::domain_error{ "Invalid range" };
    }
    return m_tokens.at(m_current - 1);
}

bool Parser::isAtEnd()
{
    return peek().type == TokenType::Eof;
}

bool Parser::checkCurrentToken(TokenType type)
{
    if (!isAtEnd() && peek().type == type)
    {
        return true;
    }
    return false;
}

bool Parser::match(TokenType type)
{
    if (checkCurrentToken(type))
    {
        advance();
        return true;
    }
    return false;
}

Token Parser::advance()
{
    // Logger::debug("advance");
    if (!isAtEnd())
    {
        m_current++;
    }
    return previous();
}

void Parser::synchronize()
{
    using enum TokenType;
    advance();

    while (!isAtEnd())
    {
        if (previous().type == Semicolon)
        {
            return;
        }

        switch (peek().type)
        {
        case Class:
        case Fun:
        case Var:
        case For:
        case If:
        case While:
        case Print:
        case Return:
            return;
        default:
            break;
        }

        advance();
    }
}

} // namespace lox