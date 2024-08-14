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

#include "Environment.h"

namespace lox
{

void Environment::define(std::string key, LiteralValues value)
{
    m_variables[std::move(key)] = std::move(value);
}

LiteralValues Environment::get(const std::string& key)
{
    if (!m_variables.contains(key))
    {
        throw EnvironmentException{ key };
    }
    return m_variables[key];
}

void Environment::assign(std::string key, LiteralValues value)
{
    if (!m_variables.contains(key))
    {
        throw EnvironmentException{ key };
    }
    m_variables[key] = value;
}

} // namespace lox