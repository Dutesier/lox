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

#include "interpreter.h"

#include <assert.h>
#include <exception>
#include <fstream>
#include <iostream>

namespace lox
{

Interpreter::Interpreter() {}

Interpreter::Interpreter(std::filesystem::path path)
    : m_path(std::move(path))
{
    if (m_path->empty())
    {
        throw std::domain_error("Invalid file path.");
    }
}

int Interpreter::run()
{
    if (m_path)
    {
        return interpretFile();
    }
    return interpretStdin();
}

int Interpreter::interpretFile()
{
    assert(m_path.has_value());
    std::ifstream file(m_path.value());
    if (!file.is_open())
    {
        m_logger.error(std::format("[interpretFile]: Failed to open file at {}.", m_path.value().string()));
        return EXIT_FAILURE;
    }

    std::string lineBuffer, wholeFile;
    while (getline(file, lineBuffer))
    {
        wholeFile += lineBuffer;
    }

    file.close();
    return interpret(wholeFile);
}

int Interpreter::interpretStdin()
{
    auto exitCode = EXIT_FAILURE;
    std::string lineBuffer{};
    std::cout << ">\t";
    while (getline(std::cin, lineBuffer))
    {
        exitCode = interpret(lineBuffer);
        std::cout << ">\t";
    }

    return exitCode;
}

int Interpreter::interpret(const std::string& content)
{
    if (content.empty())
    {
        m_logger.info("[interpret]: No content to interpret.");
        return EXIT_FAILURE;
    }

    m_logger.debug(std::format("[interpret]: Content: {}", content));
    std::string_view content_view{ content };
    m_lexer = std::make_unique<Lexer>(std::move(content_view));
    m_lexer->tokenize();
    return EXIT_SUCCESS;
}

void Interpreter::logError(unsigned int line, std::string_view location, std::string_view message)
{
    m_logger.error(std::format("[line {}] {}: {}", line, location, message));
}

} // namespace lox
