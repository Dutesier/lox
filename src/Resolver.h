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
#include "BaseStatement.h"
#include "Interpreter.h"
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>

namespace lox
{

class Resolver
    : public ExpressionVisitor
    , public StatementVisitor
{
public:
    Resolver(Interpreter& interpreter);

    void resolve(const std::vector<std::unique_ptr<Statement>>& statements);
    void resolve(const Statement& stmt);
    void resolve(const Expression& expr);

    // ExpressionVisitor methods
    Object visit(const AssignmentExpression& expr) override;
    Object visit(const BinaryExpression& expr) override;
    Object visit(const CallExpression& expr) override;
    Object visit(const GroupingExpression& expr) override;
    Object visit(const LiteralExpression& expr) override;
    Object visit(const LogicalExpression& expr) override;
    Object visit(const UnaryExpression& expr) override;
    Object visit(const VariableExpression& expr) override;

    // StatementVisitor methods
    void visit(const BlockStatement& stmt) override;
    void visit(const ExpressionStatement& stmt) override;
    void visit(const FunctionStatement& stmt) override;
    void visit(const IfStatement& stmt) override;
    void visit(const PrintStatement& stmt) override;
    void visit(const ReturnStatement& stmt) override;
    void visit(const VarStatement& stmt) override;
    void visit(const WhileStatement& stmt) override;

private:
    void beginScope();
    void endScope();
    void declare(const std::string& name);
    void define(const std::string& name);
    void resolveLocal(const Expression& expr, const std::string& name);
    void resolveFunction(const FunctionStatement& function);

    Interpreter& m_interpreter;
    std::stack<std::unordered_map<std::string, bool>> scopes;
};

} // namespace lox
