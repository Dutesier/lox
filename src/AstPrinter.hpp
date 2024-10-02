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

#include "BaseExpression.h"

#include <iostream>

namespace lox
{

class AstPrinter : public ExpressionVisitor
{
public:
    void print(const Expression& expr)
    {
        expr.accept(*this);
        std::cout << std::endl;
    }

    Object visit(const BinaryExpression& expr) override
    {
        std::cout << "Binary(";
        std::cout << "OP: " << expr.op;
        std::cout << ", Left: ";
        expr.left->accept(*this);
        std::cout << ", Right: ";
        expr.right->accept(*this);
        std::cout << ")";
        return NullLiteral{};
    }

    Object visit(const LiteralExpression& expr) override
    {
        std::visit([](auto&& value) { std::cout << value; }, expr.value);
        return NullLiteral{};
    }

    Object visit(const UnaryExpression& expr) override
    {
        std::cout << "Unary( " << expr.op << " ";
        expr.right->accept(*this);
        std::cout << ")";
        return NullLiteral{};
    }

    Object visit(const GroupingExpression& expr) override
    {
        std::cout << "Grouping(";
        expr.expression->accept(*this);
        std::cout << ")";
        return NullLiteral{};
    }

    Object visit(const VariableExpression& expr) override
    {
        std::cout << "Variable("
                  << " Name: " << expr.name;
        std::cout << ")";
        return NullLiteral{};
    }

    Object visit(const AssignmentExpression& expr) override
    {
        std::cout << "Assignment("
                  << " Name: " << expr.name;
        expr.expr->accept(*this);
        std::cout << ")";
        return NullLiteral{};
    }

    Object visit(const LogicalExpression& expr) override
    {
        std::cout << "Logical(";
        std::cout << "OP: " << expr.op;
        std::cout << ", Left: ";
        expr.left->accept(*this);
        std::cout << ", Right: ";
        expr.right->accept(*this);
        std::cout << ")";
        return NullLiteral{};
    }

    Object visit(const CallExpression& expr) override
    {
        std::cout << "Call(";
        std::cout << "Callee: ";
        expr.callee->accept(*this);
        std::cout << ", Arguments: ";
        for (const auto& arg : expr.arguments)
        {
            arg->accept(*this);
        }
        std::cout << ")";
        return NullLiteral{};
    };
};

} // namespace lox