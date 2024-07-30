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

#include "logger.h"

#include <unordered_map>

namespace lox
{

namespace
{
std::string centerString(std::string_view str, size_t width)
{
    if (str.size() >= width)
    {
        return std::string{ str }; // If the string is already longer than the width, return it as is
    }
    size_t leftPadding = (width - str.size()) / 2;
    size_t rightPadding = width - str.size() - leftPadding;
    return std::string(leftPadding, ' ') + std::string{ str } + std::string(rightPadding, ' ');
}
} // namespace

void Logger::log(LogLevel level, std::string_view data)
{
    static std::unordered_map<Logger::LogLevel, std::string_view> levelToString{ { LogLevel::Debug, "DEBUG" },
                                                                                 { LogLevel::Info, "INFO" },
                                                                                 { LogLevel::Warn, "WARN" },
                                                                                 { LogLevel::Error, "ERROR" } };
    const auto levelStr = levelToString[level];
    std::cout << "[" << centerString(levelStr, 7) << "]\t" << data << std::endl;
}

void Logger::debug(const std::string& data)
{
    return log(LogLevel::Debug, data);
}

void Logger::info(const std::string& data)
{
    return log(LogLevel::Info, data);
}

void Logger::warn(const std::string& data)
{
    return log(LogLevel::Warn, data);
}

void Logger::error(const std::string& data)
{
    return log(LogLevel::Error, data);
}

} // namespace lox