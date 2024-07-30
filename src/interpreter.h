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

#include "logger.h"

#include <filesystem>
#include <optional>
#include <string>

namespace lox
{

class Interpreter
{
public:
    Interpreter();
    explicit Interpreter(std::filesystem::path file);

    int run();

private:
    int interpretFile();
    int interpretStdin();
    int interpret(const std::string& content);

    void logError(unsigned int line, std::string_view location, std::string_view message);

    std::optional<std::filesystem::path> m_path;
    Logger m_logger;
};

} // namespace lox
