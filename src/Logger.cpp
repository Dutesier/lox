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

#include "Logger.h"

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

enum Color
{
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    Reset
};

const std::unordered_map<Color, std::string> colorMap = {
    { Color::Red, "\033[31m" },   { Color::Green, "\033[32m" },   { Color::Yellow, "\033[33m" },
    { Color::Blue, "\033[34m" },  { Color::Magenta, "\033[35m" }, { Color::Cyan, "\033[36m" },
    { Color::White, "\033[37m" }, { Color::Reset, "\033[0m" } // Reset to default color
};
} // namespace

void Logger::log(LogLevel level, std::string_view data)
{
    static std::unordered_map<Logger::LogLevel, std::string_view> levelToString{ { LogLevel::Debug, "DEBUG" },
                                                                                 { LogLevel::Info, "INFO" },
                                                                                 { LogLevel::Warn, "WARN" },
                                                                                 { LogLevel::Error, "ERROR" } };

    static std::unordered_map<Logger::LogLevel, std::string> levelToColorString{
        { LogLevel::Debug, colorMap.at(Color::Green) },
        { LogLevel::Info, colorMap.at(Color::Blue) },
        { LogLevel::Warn, colorMap.at(Color::Yellow) },
        { LogLevel::Error, colorMap.at(Color::Red) }
    };
    const auto levelStr = levelToString[level];
    std::cout << levelToColorString[level] << "[" << centerString(levelStr, 7) << "]\t" << data
              << colorMap.at(Color::Reset) << std::endl;
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

void Logger::terminal(const std::string& data)
{
    constexpr std::string_view term = ">\t";
    std::cout << term << (data.data() ? data.data() : "");
}

} // namespace lox