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

#include "Parser.h"

#include "BaseExpression.h"
#include "BaseStatement.h"
#include "Logger.h"
#include "Token.h"
#include <memory>
#include <optional>
#include <vector>

namespace lox
{

Parser::Parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens))
{
}

std::vector<StatementUPTR> Parser::parse()
{
    std::vector<StatementUPTR> statements;
    while (!isAtEnd())
    {
        try
        {
            statements.emplace_back(declaration());
        }
        catch (ParserException& error)
        {
            Logger::error("Failed parsing. " + std::string{ error.what() });
            synchronize();
            break;
        }
    }

    return statements;
}

StatementUPTR Parser::declaration()
{
    if (match(Fun))
    {
        return function("function");
    }
    if (match(Var))
    {
        return varDeclaration();
    }
    return statement();
}

StatementUPTR Parser::function(std::string kind)
{
    Token name = consumeOrThrow(Identifier, "Expected " + kind + " name.");
    consumeOrThrow(LeftParen, "Expect '(' after " + kind + " name.");
    std::vector<Token> parameters;
    if (!checkCurrentToken(RightParen))
    {
        do
        {
            if (parameters.size() >= 255)
            {
                error(peek(), "Can't have more than 255 parameters.");
            }
            parameters.emplace_back(consumeOrThrow(Identifier, "Expect parameter name."));
        } while (match(Comma));
    }
    consumeOrThrow(RightParen, "Expect ')' after parameters.");
    consumeOrThrow(LeftBrace, "Expect '{' before " + kind + " body.");
    if (auto&& body = dynamic_cast<BlockStatement*>(block().release()); body)
    {
        return std::make_unique<FunctionStatement>(std::move(name), std::move(parameters), std::move(body->statements));
    }
    throw error(peek(), "Couldn't create a block statement");
}

StatementUPTR Parser::varDeclaration()
{
    Token name = consumeOrThrow(Identifier, "Expected an identifier.");
    std::unique_ptr<Expression> expr;
    if (match(Equal))
    {
        expr = expression();
    }
    consumeOrThrow(Semicolon, "Expected semicolon after variable declaration.");

    return std::make_unique<VarStatement>(name, std::move(expr));
}

StatementUPTR Parser::statement()
{
    if (match(For))
    {
        return forStatement();
    }
    if (match(TokenType::If))
    {
        return ifStatement();
    }
    if (match(TokenType::Print))
    {
        return printStatement();
    }
    if (match(Return))
    {
        return returnStatement();
    }
    if (match(TokenType::While))
    {
        return whileStatement();
    }
    if (match(TokenType::LeftBrace))
    {
        return block();
    }
    return expressionStatement();
}

StatementUPTR Parser::forStatement()
{
    consumeOrThrow(LeftParen, "Expected '(' after 'for'.");

    // Initializer
    StatementUPTR initializer;
    if (match(Semicolon))
    {
        initializer = nullptr;
    }
    else if (match(Var))
    {
        initializer = varDeclaration();
    }
    else
    {
        initializer = expressionStatement();
    }

    ExpressionUPTR condition;
    if (!checkCurrentToken(Semicolon))
    {
        condition = expression();
    }
    consumeOrThrow(Semicolon, "Expect ';' after loop condition.");

    ExpressionUPTR increment;
    if (!checkCurrentToken(RightParen))
    {
        increment = expression();
    }
    consumeOrThrow(RightParen, "Expect ')' after for clauses.");

    StatementUPTR body = statement();

    if (increment)
    {
        StatementUPTR incrementStatement = std::make_unique<ExpressionStatement>(std::move(increment));
        std::vector<StatementUPTR> statements;
        statements.emplace_back(std::move(body));
        statements.emplace_back(std::move(incrementStatement));
        body = std::make_unique<BlockStatement>(std::move(statements));
    }

    if (!condition)
    {
        condition = std::make_unique<LiteralExpression>(true);
    }

    body = std::make_unique<WhileStatement>(std::move(condition), std::move(body));

    if (!initializer)
    {
        std::vector<StatementUPTR> statements;
        statements.emplace_back(std::move(initializer));
        statements.emplace_back(std::move(body));
        body = std::make_unique<BlockStatement>(std::move(statements));
    }

    return body;
}

StatementUPTR Parser::printStatement()
{
    // Logger::debug("printStatement");
    auto value = expression();
    consumeOrThrow(TokenType::Semicolon, "Expect ';' after value.");
    return std::make_unique<PrintStatement>(std::move(value));
}

StatementUPTR Parser::expressionStatement()
{
    // Logger::debug("expressionStatement");
    auto value = expression();
    consumeOrThrow(TokenType::Semicolon, "Expect ';' after value.");
    return std::make_unique<ExpressionStatement>(std::move(value));
}

StatementUPTR Parser::ifStatement()
{
    consumeOrThrow(LeftParen, "Expected '(' after 'if'.");
    auto expr = expression();
    consumeOrThrow(RightParen, "Expected ')' after if condition.");

    auto thenBranch = statement();
    std::optional<StatementUPTR> elseBranch;
    if (match(Else))
    {
        elseBranch = statement();
    }
    return std::make_unique<IfStatement>(std::move(expr), std::move(thenBranch), std::move(elseBranch));
}

StatementUPTR Parser::whileStatement()
{
    consumeOrThrow(LeftParen, "Expected '(' after 'while'.");
    auto condition = expression();
    consumeOrThrow(RightParen, "Expected ')' after condition.");
    auto body = statement();

    return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
}

StatementUPTR Parser::returnStatement()
{
    auto keyword = previous();
    std::optional<ExpressionUPTR> value;
    if (!checkCurrentToken(Semicolon))
    {
        value.emplace(expression());
    }
    consumeOrThrow(Semicolon, "Expected ';' after return value.");

    return std::make_unique<ReturnStatement>(std::move(keyword), std::move(value));
}

StatementUPTR Parser::block()
{
    std::vector<StatementUPTR> statements;

    while (!checkCurrentToken(RightBrace) && !isAtEnd())
    {
        statements.emplace_back(declaration());
    }
    consumeOrThrow(RightBrace, "Expected '}' after block.");
    return std::make_unique<BlockStatement>(std::move(statements));
}

ExpressionUPTR Parser::expression()
{
    // Logger::debug("expression");
    return assignment();
}

ExpressionUPTR Parser::buildBinaryExpression(ExpressionProducingFn lowerPrecedenceFn, MatchingFn matchFn)
{
    // Logger::debug("building binary expression");
    auto lowerPrecedence = lowerPrecedenceFn();
    while (matchFn())
    {
        Token op = previous();
        auto right = lowerPrecedenceFn();
        lowerPrecedence = std::make_unique<BinaryExpression>(std::move(lowerPrecedence), op, std::move(right));
    }
    return lowerPrecedence;
}

ExpressionUPTR Parser::assignment()
{
    // Logger::debug("assignment");
    std::unique_ptr<Expression> expr = comma();

    if (match(Equal))
    {
        Token equals = previous();
        auto value = assignment();
        if (auto&& var = dynamic_cast<VariableExpression*>(expr.get()))
        {
            return std::make_unique<AssignmentExpression>(var->name, std::move(value));
        }
        error(equals, "Invalid assignment target");
    }
    return expr;
}

ExpressionUPTR Parser::comma()
{
    // Logger::debug("comma");
    ExpressionProducingFn lowerPrecedenceFn = [this]() { return logicOr(); };
    MatchingFn matchingFn = [this]()
    {
        using enum TokenType;
        return match(Comma);
    };

    return buildBinaryExpression(std::move(lowerPrecedenceFn), std::move(matchingFn));
}

ExpressionUPTR Parser::logicOr()
{
    // Logger::debug("logicOr");
    ExpressionUPTR expr = logicAnd();
    while (match(Or))
    {
        Token op = previous();
        ExpressionUPTR right = logicAnd();
        expr = std::make_unique<LogicalExpression>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExpressionUPTR Parser::logicAnd()
{
    // Logger::debug("logicAnd");
    ExpressionUPTR expr = equality();
    while (match(And))
    {
        Token op = previous();
        ExpressionUPTR right = equality();
        expr = std::make_unique<LogicalExpression>(std::move(expr), op, std::move(right));
    }
    return expr;
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
    return call();
}

ExpressionUPTR Parser::call()
{
    auto expr = primary();

    for (;;)
    {
        if (!match(LeftParen))
        {
            break;
        }
        expr = finishCall(std::move(expr));
    }
    return expr;
}

ExpressionUPTR Parser::finishCall(ExpressionUPTR callee)
{
    std::vector<ExpressionUPTR> arguments;
    if (!checkCurrentToken(RightParen))
    {
        do
        {
            if (arguments.size() >= 255)
            {
                error(peek(), "Can't have more than 255 arguments.");
            }
            arguments.emplace_back(expression());
        } while (match(Comma));
    }

    Token paren = consumeOrThrow(RightParen, "Expect ')' after arguments.");
    return std::make_unique<CallExpression>(std::move(callee), paren, std::move(arguments));
}

namespace
{

ExpressionUPTR literalExpressionFromLiteralToken(const Token& tok)
{
    if (std::holds_alternative<double>(tok.literal))
    {
        return std::make_unique<LiteralExpression>(std::get<double>(tok.literal));
    }
    else if (std::holds_alternative<std::string>(tok.literal))
    {
        return std::make_unique<LiteralExpression>(std::get<std::string>(tok.literal));
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

    if (match(Identifier))
    {
        return std::make_unique<VariableExpression>(previous());
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