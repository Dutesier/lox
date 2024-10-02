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

#include "Resolver.h"
#include "BaseStatement.h"
#include "Logger.h"
#include "Token.h"
#include <gtest/gtest.h>
#include <string>
#include <variant>

namespace lox
{

Resolver::Resolver(Interpreter& interpreter)
    : m_interpreter(interpreter)
{
}

void Resolver::resolve(const std::vector<std::unique_ptr<Statement>>& statements)
{
    for (const auto& statement : statements)
    {
        resolve(*statement);
    }
    return;
}

void Resolver::resolve(const Statement& statement)
{
    statement.accept(*this);
    return;
}

void Resolver::resolve(const Expression& expression)
{
    expression.accept(*this);
    return;
}

void Resolver::beginScope()
{
    scopes.emplace_back();
    return;
}

void Resolver::endScope()
{
    scopes.pop();
    return;
}

void Resolver::visit(const BlockStatement& stmt)
{
    beginScope();
    resolve(stmt.statements);
    endScope();
    return;
}

void Resolver::visit(const VarStatement& stmt)
{
    std::string name = "";
    if (std::holds_alternative<std::string>(stmt.name.literal))
    {
        name = std::get<std::string>(stmt.name.literal);
    }
    declare(name);

    if (stmt.expr != nullptr)
    {
        resolve(*stmt.expr);
    }
    define(name);
    return;
}

void Resolver::declare(const std::string& name)
{
    if (scopes.empty())
    {
        return;
    }
    auto& scope = scopes.top();
    scope[name] = false;
}

void Resolver::define(const std::string& name)
{
    if (scopes.empty())
    {
        return;
    }
    auto& scope = scopes.top();
    scope[name] = true;
}

Object Resolver::visit(const VariableExpression& expr)
{
    std::string name = std::holds_alternative<std::string>(expr.name.literal) ? std::get<std::string>(expr.name.literal)
                                                                              : "";
    if (!scopes.empty() && scopes.top().contains(name) && !scopes.top()[name])
    {
        Logger::error(name + " - Can't read local variable in its own initializer.");
    }

    resolveLocal(expr, name);
    return NullLiteral{};
}

void Resolver::resolveLocal(const Expression& expr, const std::string& name)
{
    for (int i = scopes.size() - 1; i >= 0; --i)
    {
        if (scopes[i].contains(name))
        {
            m_interpreter.resolve(expr, scopes.size() - 1 - i);
            return;
        }
    }
}

Object Resolver::visit(const AssignmentExpression& expr)
{
    std::string name = std::holds_alternative<std::string>(expr.name.literal) ? std::get<std::string>(expr.name.literal)
                                                                              : "";
    resolve(*expr.expr);
    resolveLocal(expr, name);
    return NullLiteral{};
}

void Resolver::visit(const FunctionStatement& stmt)
{
    std::string name = std::holds_alternative<std::string>(stmt.name.literal) ? std::get<std::string>(expr.name.literal)
                                                                              : "";
    declare(name);
    define(name);

    resolveFunction(stmt);

    return;
}

void Resolver::resolveFunction(const FunctionStatement& function)
{
    beginScope();
    for (Token param : function.params)
    {
        auto parameterName = std::holds_alternative<std::string>(param.literal) ? std::get<std::string>(param.literal)
                                                                                : "";
        declare(parameterName);
        define(parameterName);
    }
    resolve(function.body);
    endScope();
}

void Resolver::visit(const ExpressionStatement& stmt)
{
    resolve(*stmt.expr);
}

void Resolver::visit(const IfStatement& stmt)
{
    resolve(*stmt.condition);
    resolve(*stmt.thenBranch);
    if (stmt.elseBranch.has_value() && stmt.elseBranch.value())
    {
        resolve(*stmt.elseBranch.value());
    }
}

void Resolver::visit(const PrintStatement& stmt)
{
    resolve(*stmt.expr);
}

void Resolver::visit(const ReturnStatement& stmt)
{
    if (stmt.value)
    {
        resolve(*stmt.value.value());
    }
}

void Resolver::visit(const WhileStatement& stmt)
{
    resolve(*stmt.condition);
    resolve(*stmt.body);
}

Object Resolver::visit(const BinaryExpression& expr)
{
    resolve(*expr.left);
    resolve(*expr.right);
    return NullLiteral{};
}

Object Resolver::visit(const CallExpression& expr)
{
    resolve(*expr.callee);
    for (const auto& argument : expr.arguments)
    {
        resolve(*argument);
    }
    return NullLiteral{};
}

Object Resolver::visit(const GroupingExpression& expr)
{
    resolve(*expr.expression);
    return NullLiteral{};
}

Object Resolver::visit(const LiteralExpression&)
{
    return NullLiteral{};
}

Object Resolver::visit(const LogicalExpression& expr)
{
    resolve(*expr.left);
    resolve(*expr.right);
    return NullLiteral{};
}

Object Resolver::visit(const UnaryExpression& expr)
{
    resolve(*expr.right);
    return NullLiteral{};
}

} // namespace lox
