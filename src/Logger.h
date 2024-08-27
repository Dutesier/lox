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

#include <format>
#include <iostream>
#include <string_view>

namespace lox
{

struct Logger
{
public:
    static void info(const std::string& data);
    static void debug(const std::string& data);
    static void warn(const std::string& data);
    static void error(const std::string& data);
    static void terminal(const std::string& data);

private:
    enum LogLevel
    {
        Debug,
        Info,
        Warn,
        Error
    };
    static void log(LogLevel level, std::string_view data);
};

} // namespace lox