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

#include <functional>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

namespace lox
{

struct NullLiteral
{
};
std::ostream& operator<<(std::ostream& os, NullLiteral /*nl*/);
// Forward declaration
class Interpreter;

struct Callable
{
    using Object = std::variant<std::string, double, bool, Callable, NullLiteral>;
    using CallableFn = std::function<Object(Interpreter&, const std::vector<Object>& arguments)>;

    // virtual Object call(
    //     Interpreter& interpreter,
    //     const std::vector<Object>& arguments); // TODO: Explore concepts or std::function here

    CallableFn call;
    unsigned int arity{};
    std::string name{ "Callable" };
};
using Object = Callable::Object;
std::string print(const Object& values);
std::ostream& operator<<(std::ostream& os, Callable /*nl*/);

} // namespace lox
