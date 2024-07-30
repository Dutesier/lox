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

#include "lexer.h"
#include "logger.h"

namespace lox
{

Lexer::Lexer(std::string_view source)
    : m_source(std::move(source))
{
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens{};

    while (!isAtEnd())
    {
        m_start = m_current;
        tokens.emplace_back(getNextToken());
    }
    tokens.emplace_back(Token{ TokenType::Eof, "", m_line });
    return tokens;
}

char Lexer::advance()
{
    return m_source.at(m_current++);
}

Token Lexer::getNextToken()
{
    using enum TokenType;
    char c = advance();
    switch (c)
    {
    case '(':
        return Token{ LeftParen };
    case ')':
        return Token{ RightParen };
    case '{':
        return Token{ LeftParen };
    case '}':
        return Token{ RightBrace };
    case ',':
        return Token{ Comma };
    case '.':
        return Token{ Dot };
    case '-':
        return Token{ Minus };
    case '+':
        return Token{ Plus };
    case ';':
        return Token{ Semicolon };
    case '*':
        return Token{ Star };
    case '!':
        if (match('='))
        {
            advance();
            return Token{ BangEqual };
        }
        return Token{ Bang };
        break;
    case '=':
        if (match('='))
        {
            advance();
            return Token{ EqualEqual };
        }
        return Token{ Equal };
        break;
    case '<':
        if (match('='))
        {
            advance();
            return Token{ LessEqual };
        }
        return Token{ Less };
        break;
    case '>':
        if (match('='))
        {
            advance();
            return Token{ GreaterEqual };
        }
        return Token{ Greater };
        break;
    }
    Logger::error(std::format("[Line {}] Unexpected character.", m_line));
    return Token{ Eof };
}

bool Lexer::isAtEnd()
{
    return m_current >= m_source.size();
}

bool Lexer::match(char next)
{
    if (isAtEnd())
    {
        return false;
    }
    if (m_source.at(m_current) != next)
    {
        return false;
    }
    return true;
}

} // namespace lox