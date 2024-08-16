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

#include <optional>
#include <unordered_map>

namespace lox
{

class Environment
{
public:
    explicit Environment(Environment* parent = nullptr);

    void define(std::string key, LiteralValues value);
    LiteralValues get(const std::string& key);
    void assign(std::string key, LiteralValues value);

private:
    std::unordered_map<std::string, LiteralValues> m_variables;

    // This would be the ideal architecture but it's not supported:
    // std::optional<Environment&> m_enclosing;
    Environment* m_enclosing = nullptr;
    const bool m_isRootNode;

public:
    // Custom exception class
    class EnvironmentException : public std::exception
    {
    private:
        std::string message;

    public:
        EnvironmentException(const std::string& msg)
            : message("Error: Tried to access undeclared variable " + msg + ".")
        {
        }

        // Override the what() function to return the error message
        const char* what() const noexcept override { return message.c_str(); }
    };
};

} // namespace lox