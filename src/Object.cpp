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

#include "Object.h"
#include "Logger.h"

namespace lox
{

std::ostream& operator<<(std::ostream& os, NullLiteral /*nl*/)
{
    return os << "null";
}

std::ostream& operator<<(std::ostream& os, Callable /*nl*/)
{
    return os << "callable";
}

std::string print(const Object& values)
{
    if (std::holds_alternative<double>(values))
    {
        return std::to_string(std::get<double>(values));
    }
    if (std::holds_alternative<bool>(values))
    {
        return std::get<bool>(values) ? "true" : "false";
    }
    if (std::holds_alternative<std::string>(values))
    {
        return std::get<std::string>(values);
    }
    if (std::holds_alternative<Callable>(values))
    {
        return "callable";
    }
    return "null";
}

// auto Callable::call(Interpreter& interpreter, const std::vector<Object>& arguments) -> Object
// {
//     Logger::debug("Callable call called instead");
//     (void)interpreter;
//     (void)arguments;
//     return NullLiteral{};
// }

} // namespace lox
