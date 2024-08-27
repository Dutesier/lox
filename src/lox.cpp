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

#include "lox.h"
#include "Interpreter.h"

#include <iostream>

namespace lox
{

int run()
{
    Interpreter interpreter;
    return interpreter.run();
}

int run(const std::string& filepath)
{
    Interpreter interpreter(filepath);
    return interpreter.run();
}

} // namespace lox