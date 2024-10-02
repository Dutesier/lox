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

#include "Logger.h" // TODO: remove
#include <format>
#include <memory>
#include <string>

namespace lox
{

unsigned int Environment::globalId = 1;
Environment::Environment(std::shared_ptr<Environment> parent)
    : m_enclosing(std::move(parent))
    , m_isRootNode(false)
{

    id = Environment::globalId++;
    Logger::info(std::format("Instantiated new env with id {} and enclosing id {}", id, m_enclosing->id));
    // Environment::globalId = Environment::globalId + 1;
}

Environment::Environment()
    : m_enclosing(nullptr) // This shall never be used
    , m_isRootNode(true)
{
    id = Environment::globalId++;
    Logger::info(std::format("Instantiated new env with id {} and NO enclosing env", id));
}

Environment::~Environment()
{
    Logger::warn(std::format("Destroyed env {}", id));
}

void Environment::define(std::string key, Object value)
{
    Logger::warn(std::format(
        "Defining {} on env-{} [{}]", key, id, m_isRootNode ? "ROOT" : "LEAF->" + std::to_string(m_enclosing->id)));
    m_variables[std::move(key)] = std::move(value);
}

void Environment::debug() const
{
    std::string envString = std::format("Environment {} {{", id);
    for (auto i = m_variables.begin(); i != m_variables.end(); ++i)
    {
        envString += std::format(R"("{}": {},)", i->first, print(i->second));
    }
    if (!m_variables.empty())
    {
        envString.pop_back();
    }
    envString += "}";
    Logger::info(envString);
}

Object Environment::get(const std::string& key)
{
    Logger::warn(std::format(
        "Trying to get {} on env-{} [{}]",
        key,
        id,
        m_isRootNode ? "ROOT" : "LEAF->" + std::to_string(m_enclosing->id)));
    if (!m_variables.contains(key))
    {
        if (m_isRootNode)
        {
            throw EnvironmentException{ key };
        }
        return m_enclosing->get(key);
    }
    return m_variables[key];
}

void Environment::assign(std::string key, Object value)
{
    Logger::warn(std::format(
        "Assigning {} on env-{} [{}]", key, id, m_isRootNode ? "ROOT" : "LEAF->" + std::to_string(m_enclosing->id)));

    if (!m_variables.contains(key))
    {
        if (m_isRootNode)
        {
            throw EnvironmentException{ key };
        }
        return m_enclosing->assign(key, value);
    }
    m_variables[key] = value;
}

} // namespace lox