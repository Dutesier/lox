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

#include "Token.h"

#include "Object.h"

#include <memory>
#include <vector>

namespace lox
{

class BinaryExpression;
class LiteralExpression;
class UnaryExpression;
class GroupingExpression;
class VariableExpression;
class AssignmentExpression;
class LogicalExpression;
class CallExpression;

class ExpressionVisitor
{
public:
    virtual ~ExpressionVisitor() = default;

    virtual Object visit(const BinaryExpression& expr) = 0;
    virtual Object visit(const LiteralExpression& expr) = 0;
    virtual Object visit(const UnaryExpression& expr) = 0;
    virtual Object visit(const GroupingExpression& expr) = 0;
    virtual Object visit(const VariableExpression& expr) = 0;
    virtual Object visit(const AssignmentExpression& expr) = 0;
    virtual Object visit(const LogicalExpression& expr) = 0;
    virtual Object visit(const CallExpression& expr) = 0;
};

class Expression
{
public:
    virtual ~Expression() = default;
    // Accept method for the Visitor pattern
    virtual Object accept(ExpressionVisitor& visitor) const = 0;
};

class BinaryExpression : public Expression
{
public:
    BinaryExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right)
        : left(std::move(left))
        , op(std::move(op))
        , right(std::move(right))
    {
    }

    Object accept(ExpressionVisitor& visitor) const override { return visitor.visit(*this); }

    std::unique_ptr<Expression> left;
    Token op;
    std::unique_ptr<Expression> right;
};

class LiteralExpression : public Expression
{

public:
    LiteralExpression(Object value)
        : value(std::move(value))
    {
    }

    Object accept(ExpressionVisitor& visitor) const override { return visitor.visit(*this); }

    Object value;
};

class UnaryExpression : public Expression
{
public:
    UnaryExpression(Token op, std::unique_ptr<Expression> right)
        : op(std::move(op))
        , right(std::move(right))
    {
    }

    Object accept(ExpressionVisitor& visitor) const override { return visitor.visit(*this); }

    Token op;
    std::unique_ptr<Expression> right;
};

class GroupingExpression : public Expression
{
public:
    GroupingExpression(std::unique_ptr<Expression> expression)
        : expression(std::move(expression))
    {
    }

    Object accept(ExpressionVisitor& visitor) const override { return visitor.visit(*this); }

    std::unique_ptr<Expression> expression;
};

class VariableExpression : public Expression
{
public:
    VariableExpression(Token name)
        : name(std::move(name))
    {
    }

    Object accept(ExpressionVisitor& visitor) const override { return visitor.visit(*this); }

    Token name;
};

class AssignmentExpression : public Expression
{
public:
    AssignmentExpression(Token name, std::unique_ptr<Expression> expr)
        : name(std::move(name))
        , expr(std::move(expr))
    {
    }

    Object accept(ExpressionVisitor& visitor) const override { return visitor.visit(*this); }

    Token name;
    std::unique_ptr<Expression> expr;
};

class LogicalExpression : public Expression
{
public:
    LogicalExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right)
        : left(std::move(left))
        , op(std::move(op))
        , right(std::move(right))
    {
    }

    Object accept(ExpressionVisitor& visitor) const override { return visitor.visit(*this); }

    std::unique_ptr<Expression> left;
    Token op;
    std::unique_ptr<Expression> right;
};

class CallExpression : public Expression
{
public:
    CallExpression(std::unique_ptr<Expression> callee, Token paren, std::vector<std::unique_ptr<Expression>> arguments)
        : callee(std::move(callee))
        , paren(std::move(paren))
        , arguments(std::move(arguments))
    {
    }

    Object accept(ExpressionVisitor& visitor) const override { return visitor.visit(*this); }

    std::unique_ptr<Expression> callee;
    Token paren;
    std::vector<std::unique_ptr<Expression>> arguments;
};

} // namespace lox
