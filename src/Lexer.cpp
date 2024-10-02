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

#include "Lexer.h"
#include "Logger.h"

#include <sstream>
#include <string>
#include <unordered_map>
namespace
{

bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

bool isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool isAlphaNumeric(char c)
{
    return isAlpha(c) || isDigit(c);
}

const std::unordered_map<std::string_view, lox::TokenType> KeywordsMap{
    { "and", lox::TokenType::And },     { "class", lox::TokenType::Class },   { "else", lox::TokenType::Else },
    { "false", lox::TokenType::False }, { "for", lox::TokenType::For },       { "fun", lox::TokenType::Fun },
    { "if", lox::TokenType::If },       { "nil", lox::TokenType::Nil },       { "or", lox::TokenType::Or },
    { "print", lox::TokenType::Print }, { "return", lox::TokenType::Return }, { "super", lox::TokenType::Super },
    { "this", lox::TokenType::This },   { "true", lox::TokenType::True },     { "var", lox::TokenType::Var },
    { "while", lox::TokenType::While }
};

} // namespace

namespace lox
{

Lexer::Lexer(std::string_view source)
    : m_source(std::move(source))
{
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens{};

    while (true)
    {
        auto tok = getNextToken();
        m_start = m_current;
        tokens.emplace_back(tok);
        if (tok.type == TokenType::Eof)
        {
            break;
        }
    }
    for (auto tok : tokens)
    {
        std::stringstream ss;
        ss << tok;
        Logger::debug(std::format("Got Token: {}", ss.str()));
    }
    return tokens;
}

char Lexer::advance()
{
    return m_source.at(m_current++);
}

Token Lexer::getNextToken()
{
    if (isAtEnd())
    {
        return Token{ Eof, std::monostate{}, "", m_line };
    }

    using enum TokenType;
    char c = advance();
    // Logger::debug(std::format("Evalutating {}", c));
    switch (c)
    {
    case '(':
        return Token{ LeftParen, std::monostate{}, "", m_line };
    case ')':
        return Token{ RightParen, std::monostate{}, "", m_line };
    case '{':
        return Token{ LeftBrace, std::monostate{}, "", m_line };
    case '}':
        return Token{ RightBrace, std::monostate{}, "", m_line };
    case ',':
        return Token{ Comma, std::monostate{}, "", m_line };
    case '.':
        return Token{ Dot, std::monostate{}, "", m_line };
    case '-':
        return Token{ Minus, std::monostate{}, "", m_line };
    case '+':
        return Token{ Plus, std::monostate{}, "", m_line };
    case ';':
        return Token{ Semicolon, std::monostate{}, "", m_line };
    case '*':
        return Token{ Star, std::monostate{}, "", m_line };
    case '!':
        if (match('='))
        {
            advance();
            return Token{ BangEqual, std::monostate{}, "", m_line };
        }
        return Token{ Bang, std::monostate{}, "", m_line };
    case '=':
        if (match('='))
        {
            advance();
            return Token{ EqualEqual, std::monostate{}, "", m_line };
        }
        return Token{ Equal, std::monostate{}, "", m_line };
    case '<':
        if (match('='))
        {
            advance();
            return Token{ LessEqual, std::monostate{}, "", m_line };
        }
        return Token{ Less, std::monostate{}, "", m_line };
    case '>':
        if (match('='))
        {
            advance();
            return Token{ GreaterEqual, std::monostate{}, "", m_line };
        }
        return Token{ Greater, std::monostate{}, "", m_line };
    case '/':
        if (match('/')) // Comments
        {
            advanceUntilEndOfLine();
            return rerun();
        }
        else if (match('*')) // Multiline comments
        {
            advanceUntilEndOfComment();
            return rerun();
        }
        return Token{ Slash, std::monostate{}, "", m_line };
    case ' ':
    case '\r':
    case '\t':
        // Ignore whitespace.
        return rerun();
    case '\n':
        m_line++;
        return rerun();

    // Literals
    case '"':
        return getStringToken();
    default:
        if (isDigit(c))
        {
            return getNumberToken();
        }
        else if (isAlpha(c))
        {
            return getIdentifierToken();
        }
    }
    Logger::error(std::format("[Line {}] Unexpected character -> {}.", m_line, c));
    return Token{ Error, std::monostate{}, std::string{ c }, m_line }; // Maybe throw here???
}

Token Lexer::rerun()
{
    m_start = m_current;
    return getNextToken();
}

Token Lexer::getStringToken()
{
    // Get to end of string
    while (peek() != '"')
    {
        if (isAtEnd())
        {
            return Token{ Error, std::monostate{}, "Unterminated string.", m_line }; // Maybe throw here???
        }
        if (peek() == '\n')
        {
            m_line++;
        }
        advance();
    }

    if (advance() != '"') // The closing '"'
    {
        return Token{ Error, std::monostate{}, "Unterminated string.", m_line }; // Maybe throw here???
    }

    // Logger::debug(std::format(
    //     "Building token with value {}, from index {} to index {}",
    //     std::string{ m_source.substr(m_start + 1, m_current - (m_start + 1) - 1) },
    //     m_start + 1,
    //     m_current - (m_start + 1) - 1));
    return Token{
        String, std::string{ m_source.substr(m_start + 1, m_current - (m_start + 1) - 1) }, "", m_line
    }; // Maybe throw here???
}

Token Lexer::getNumberToken()
{
    // In Lox every number is double!
    while (isDigit(peek()))
    {
        advance();
    }

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext()))
    {
        // Consume the "."
        advance();

        while (isDigit(peek()))
        {
            advance();
        }
    }

    auto str = std::string{ m_source.substr(m_start, m_current - m_start) };
    try
    {
        std::size_t index;
        double value = std::stod(str.data(), &index);
        return Token{ Number, std::move(value), "", m_line }; // Maybe throw here???
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return Token{ Error, std::monostate{}, str, m_line }; // Maybe throw here???
    }
    catch (const std::out_of_range& e)
    {
        std::cerr << "Out of range: " << e.what() << std::endl;
        return Token{ Error, std::monostate{}, str, m_line }; // Maybe throw here???
    }
}

Token Lexer::getIdentifierToken()
{
    while (isAlphaNumeric(peek()))
    {
        advance();
    }

    auto text = std::string{ m_source.substr(m_start, m_current - m_start) };
    auto type = TokenType::Identifier;
    if (KeywordsMap.contains(text))
    {
        type = KeywordsMap.at(text);
        return Token{ type, std::monostate{} };
    }
    return Token{ type, text };
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

void Lexer::advanceUntilEndOfLine()
{
    constexpr char NewLine = '\n';
    while (peek() != NewLine && !isAtEnd())
    {
        advance();
    }
}

void Lexer::advanceUntilEndOfComment()
{
    advance(); // consume the / in /*
    advance(); // consume the * in /*
    while (peek() != '*' && !isAtEnd() && peekNext() != '/')
    {
        if ('\n' == advance())
        {
            m_line++;
        }
    }
    advance(); // consume the * in */
    advance(); // consume the / in */
}

char Lexer::peek()
{
    constexpr char NullTerminator = '\0';
    if (isAtEnd())
    {
        return NullTerminator;
    }
    return m_source.at(m_current);
}

char Lexer::peekNext()
{
    constexpr char NullTerminator = '\0';
    if (m_current + 1 >= m_source.size())
    {
        return NullTerminator;
    }
    return m_source.at(m_current + 1);
}

} // namespace lox