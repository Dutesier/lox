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
#include <memory>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>

namespace lox
{

class ExpressionStatement;
class PrintStatement;
class VarStatement;
class BlockStatement;
class IfStatement;
class WhileStatement;
class FunctionStatement;
class ReturnStatement;

class StatementVisitor
{
public:
    virtual ~StatementVisitor() = default;

    virtual void visit(const ExpressionStatement& statement) = 0;
    virtual void visit(const PrintStatement& statement) = 0;
    virtual void visit(const VarStatement& statement) = 0;
    virtual void visit(const BlockStatement& statement) = 0;
    virtual void visit(const IfStatement& statement) = 0;
    virtual void visit(const WhileStatement& statement) = 0;
    virtual void visit(const FunctionStatement& statement) = 0;
    virtual void visit(const ReturnStatement& statement) = 0;
};

class Statement
{
public:
    virtual ~Statement() = default;
    // Accept method for the Visitor pattern
    virtual void accept(StatementVisitor& visitor) const = 0;
};

class ExpressionStatement : public Statement
{
public:
    ExpressionStatement(std::unique_ptr<Expression> expr)
        : expr(std::move(expr))
    {
    }

    void accept(StatementVisitor& visitor) const { visitor.visit(*this); };

    std::unique_ptr<Expression> expr;
};

class PrintStatement : public Statement
{
public:
    PrintStatement(std::unique_ptr<Expression> expr)
        : expr(std::move(expr))
    {
    }

    void accept(StatementVisitor& visitor) const { visitor.visit(*this); };

    std::unique_ptr<Expression> expr;
};

class VarStatement : public Statement
{
public:
    VarStatement(Token name, std::unique_ptr<Expression> expr)
        : name(std::move(name))
        , expr(std::move(expr))
    {
    }

    void accept(StatementVisitor& visitor) const { visitor.visit(*this); };

    Token name;
    std::unique_ptr<Expression> expr;
};

class BlockStatement : public Statement
{
public:
    BlockStatement(std::vector<std::unique_ptr<Statement>> statements)
        : statements(std::move(statements))
    {
    }

    void accept(StatementVisitor& visitor) const { visitor.visit(*this); };

    std::vector<std::unique_ptr<Statement>> statements;
};

class IfStatement : public Statement
{
public:
    IfStatement(
        std::unique_ptr<Expression> condition,
        std::unique_ptr<Statement> thenBranch,
        std::optional<std::unique_ptr<Statement>> elseBranch = std::nullopt)
        : condition(std::move(condition))
        , thenBranch(std::move(thenBranch))
        , elseBranch(std::move(elseBranch))
    {
    }

    void accept(StatementVisitor& visitor) const { visitor.visit(*this); };

    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::optional<std::unique_ptr<Statement>> elseBranch;
};

class WhileStatement : public Statement {
public:
    WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
        : condition(std::move(condition))
        , body(std::move(body))
    {
    }

    void accept(StatementVisitor& visitor) const { visitor.visit(*this); };

    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;
};

class FunctionStatement : public Statement
{
public:
    FunctionStatement(Token name, std::vector<Token> params, std::vector<std::unique_ptr<Statement>> body)
        : name(std::move(name))
        , params(std::move(params))
        , body(std::move(body))
    {
    }

    void accept(StatementVisitor& visitor) const { visitor.visit(*this); };

    Token name;
    std::vector<Token> params;
    std::vector<std::unique_ptr<Statement>> body;
};

class ReturnStatement : public Statement
{
public:
    ReturnStatement(Token keyword, std::optional<std::unique_ptr<Expression>> value)
        : keyword(std::move(keyword))
        , value(std::move(value))
    {
    }

    void accept(StatementVisitor& visitor) const { visitor.visit(*this); };

    Token keyword;
    std::optional<std::unique_ptr<Expression>> value;
};

} // namespace lox
