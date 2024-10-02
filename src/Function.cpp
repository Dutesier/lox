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

#include "Function.h"
#include "Environment.h"
#include "Interpreter.h"
#include "Logger.h"
#include "Object.h"
#include <string>
#include <variant>

namespace lox
{

Function::Function(const FunctionStatement& declaration, std::shared_ptr<Environment> clo)
    : declaration(declaration)
    , closure(std::move(clo))
{
    if (name == "Callable" || name.empty())
    {
        if (std::holds_alternative<std::string>(declaration.name.literal))
        {
            name = std::get<std::string>(declaration.name.literal);
        }
    }
    arity = declaration.params.size();
    Logger::debug(std::format("Closure environment on '{}' creation", name));
    closure->debug();
    call = [&declaration, name = this->name, closure = this->closure](
               Interpreter& interpreter, const std::vector<Object>& arguments) -> Object
    {
        auto env = std::make_shared<Environment>(closure);
        Logger::debug(std::format("Closure environment on '{}' invocation", name));
        closure->debug();
        for (std::size_t i = 0; i < declaration.params.size(); ++i)
        {
            const auto& name = declaration.params.at(i).literal;
            if (std::holds_alternative<std::string>(name))
            {
                env->define(std::get<std::string>(name), arguments.at(i));
            }
        }

        try
        {
            interpreter.executeCodeBlock(declaration.body, env);
        }
        catch (const Interpreter::ReturnException& re)
        {
            return re.obj;
        }
        return NullLiteral{};
    };
}

} // namespace lox