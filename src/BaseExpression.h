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

#include "token.h"

#include <iostream>
#include <memory>
#include <type_traits>
#include <variant>

namespace lox
{

class BinaryExpression;
class LiteralExpression;
struct NullLiteral
{
};
std::ostream& operator<<(std::ostream& os, NullLiteral /*nl*/);
using LiteralValues = std::variant<std::string, double, bool, NullLiteral>;
std::string print(const LiteralValues& values);
class UnaryExpression;
class GroupingExpression;

class ExpressionVisitor
{
public:
    virtual ~ExpressionVisitor() = default;

    virtual LiteralValues visit(const BinaryExpression& expr) = 0;
    virtual LiteralValues visit(const LiteralExpression& expr) = 0;
    virtual LiteralValues visit(const UnaryExpression& expr) = 0;
    virtual LiteralValues visit(const GroupingExpression& expr) = 0;
};

class Expression
{
public:
    virtual ~Expression() = default;
    // Accept method for the Visitor pattern
    virtual LiteralValues accept(ExpressionVisitor& visitor) const = 0;
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

    LiteralValues accept(ExpressionVisitor& visitor) const override { return visitor.visit(*this); }

    std::unique_ptr<Expression> left;
    Token op;
    std::unique_ptr<Expression> right;
};

class LiteralExpression : public Expression
{

public:
    LiteralExpression(LiteralValues value)
        : value(std::move(value))
    {
    }

    LiteralValues accept(ExpressionVisitor& visitor) const override { return visitor.visit(*this); }

    LiteralValues value;
};

class UnaryExpression : public Expression
{
public:
    UnaryExpression(Token op, std::unique_ptr<Expression> right)
        : op(std::move(op))
        , right(std::move(right))
    {
    }

    LiteralValues accept(ExpressionVisitor& visitor) const override { return visitor.visit(*this); }

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

    LiteralValues accept(ExpressionVisitor& visitor) const override { return visitor.visit(*this); }

    std::unique_ptr<Expression> expression;
};

} // namespace lox
